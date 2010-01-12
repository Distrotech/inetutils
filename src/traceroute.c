/*
  Copyright (C) 2007, 2008, 2009, 2010 Free Software Foundation, Inc.

  This file is part of GNU Inetutils.

  GNU Inetutils is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or (at
  your option) any later version.

  GNU Inetutils is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see `http://www.gnu.org/licenses/'. */

/* Written by Elian Gidoni.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/time.h>
#include <signal.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
/* #include <netinet/ip_icmp.h> -- Deliberately not including this
   since the definitions are are using are pulled in by libicmp. */

#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <error.h>
#include <progname.h>
#include <limits.h>
#include <assert.h>
#include <argp.h>
#include <icmp.h>

#include "libinetutils.h"

#define TRACE_UDP_PORT 33434
#define TRACE_TTL 1

enum trace_type
{
  TRACE_UDP,			/* UDP datagrams.  */
  TRACE_ICMP,			/* ICMP echo requests.  */
  TRACE_1393			/* RFC 1393 requests. */
};

typedef struct trace
{
  int icmpfd, udpfd;
  enum trace_type type;
  struct sockaddr_in to, from;
  unsigned char ttl;
  struct timeval tsent;
} trace_t;

void trace_init (trace_t * t, const struct sockaddr_in to,
		 const enum trace_type type);
void trace_inc_ttl (trace_t * t);
void trace_inc_port (trace_t * t);
void trace_port (trace_t * t, const unsigned short port);
int trace_read (trace_t * t);
int trace_write (trace_t * t);
int trace_udp_sock (trace_t * t);
int trace_icmp_sock (trace_t * t);

#define TIME_INTERVAL 3

void do_try (trace_t * trace, const int hop,
	     const int max_hops, const int max_tries);

char *get_hostname (struct in_addr *addr);

int stop = 0;
int pid = 0;
struct hostent *host;
struct sockaddr_in dest;

static enum trace_type opt_type = TRACE_ICMP;
int opt_port = 33434;
int opt_max_hops = 64;
static int opt_max_tries = 3;
int opt_resolve_hostnames = 0;

const char args_doc[] = "HOST";
const char doc[] = "Print the route packets trace to network host.";
const char *program_authors[] = {
	"Elian Gidoni",
	NULL
};

/* Define keys for long options that do not have short counterparts. */
enum {
  OPT_RESOLVE = 256
};

static struct argp_option argp_options[] = {
#define GRP 0
  {"port", 'p', "PORT", 0, "use destination PORT port (default: 33434)",
   GRP+1},
  {"resolve-hostnames", OPT_RESOLVE, NULL, 0, "resolve hostnames", GRP+1},
  {"tries", 'q', "NUM", 0, "send NUM probe packets per hop (default: 3)",
   GRP+1},
  {"type", 'M', "METHOD", 0, "use METHOD (`icmp' or `udp') for traceroute "
   "operations", GRP+1},
#undef GRP
  {NULL}
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  char *p;
  static bool host_is_given = false;

  switch (key)
    {
    case 'p':
      opt_port = strtoul (arg, &p, 0);
      if (*p || opt_port == 0 || opt_port > 65536)
        error (EXIT_FAILURE, 0, "invalid port number `%s'", arg);
      break;

    case OPT_RESOLVE:
      opt_resolve_hostnames = 1;
      break;

    case 'q':
      opt_max_tries = (int) strtol (arg, &p, 10);
      if (*p)
        argp_error (state, "invalid value (`%s' near `%s')", arg, p);
      if (opt_max_tries < 1 || opt_max_tries > 10)
        error (EXIT_FAILURE, 0, "number of tries should be between 1 and 10");
      break;

    case 'M':
      if (strcmp (arg, "icmp") == 0)
        opt_type = TRACE_ICMP;
      else if (strcmp (arg, "udp") == 0)
        opt_type = TRACE_UDP;
      else
        argp_error (state, "invalid method");
      break;

    case ARGP_KEY_ARG:
      host_is_given = true;
      host = gethostbyname (arg);
      if (host == NULL)
        error (EXIT_FAILURE, 0, "unknown host");
      break;

    case ARGP_KEY_SUCCESS:
      if (!host_is_given)
        argp_error (state, "missing host operand");
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }

  return 0;
}

static struct argp argp = {argp_options, parse_opt, args_doc, doc};

int
main (int argc, char **argv)
{
  trace_t trace;

  set_program_name (argv[0]);

  /* Parse command line */
  iu_argp_init ("traceroute", program_authors);
  argp_parse (&argp, argc, argv, 0, NULL, NULL);

  if (geteuid () != 0)
    error (EXIT_FAILURE, EPERM, "insufficient permissions");

  dest.sin_addr = *(struct in_addr *) host->h_addr;
  dest.sin_family = AF_INET;
  dest.sin_port = htons (opt_port);

  printf ("traceroute to %s (%s), %d hops max\n",
	  host->h_name, inet_ntoa (dest.sin_addr), opt_max_hops);

  trace_init (&trace, dest, opt_type);

  int hop = 1;
  while (!stop)
    {
      if (hop > opt_max_hops)
	exit (0);
      do_try (&trace, hop, opt_max_hops, opt_max_tries);
      trace_inc_ttl (&trace);
      trace_inc_port (&trace);
      hop++;
    }

  exit (0);
}

void
do_try (trace_t * trace, const int hop,
	const int max_hops, const int max_tries)
{
  fd_set readset;
  int ret, tries, readonly = 0;
  struct timeval now, time;
  double triptime = 0.0;
  uint32_t prev_addr = 0;

  printf (" %d  ", hop);

  for (tries = 0; tries < max_tries; tries++)
    {
      FD_ZERO (&readset);
      FD_SET (trace_icmp_sock (trace), &readset);

      time.tv_sec = TIME_INTERVAL;
      time.tv_usec = 0;

      if (!readonly)
	trace_write (trace);

      ret = select (FD_SETSIZE, &readset, NULL, NULL, &time);

      gettimeofday (&now, NULL);

      now.tv_usec -= trace->tsent.tv_usec;
      now.tv_sec -= trace->tsent.tv_sec;

      if (ret < 0)
	{
	  switch (errno)
	    {
	    case EINTR:
	      /* was interrupted */
	      break;
	    default:
              error (EXIT_FAILURE, EPERM, "select failed");
	      break;
	    }
	}
      else if (ret == 0)
	{
	  /* time expired */
	  printf (" * ");
	  fflush (stdout);
	}
      else
	{
	  if (FD_ISSET (trace_icmp_sock (trace), &readset))
	    {
	      triptime = ((double) now.tv_sec) * 1000.0 +
		((double) now.tv_usec) / 1000.0;

	      if (trace_read (trace))
		{
		  /* FIXME: printf ("Some error ocurred\n"); */
		  tries--;
		  readonly = 1;
		  continue;
		}
	      else
		{
		  if (tries == 0 || prev_addr != trace->from.sin_addr.s_addr)
		    printf (" %s (%s) ",
			    inet_ntoa (trace->from.sin_addr),
			    get_hostname (&trace->from.sin_addr));
		  printf ("%.3fms ", triptime);

		}
	      prev_addr = trace->from.sin_addr.s_addr;
	    }
	}
      readonly = 0;
      fflush (stdout);
    }
  printf ("\n");
}

char *
get_hostname (struct in_addr *addr)
{
  if (opt_resolve_hostnames)
    {
      struct hostent *info =
	gethostbyaddr ((char *) addr, sizeof (*addr), AF_INET);
      if (info != NULL)
	return info->h_name;
    }
  return inet_ntoa (*addr);
}

void
trace_init (trace_t * t, const struct sockaddr_in to,
	    const enum trace_type type)
{
  const unsigned char *ttlp;
  assert (t);
  ttlp = &t->ttl;

  t->type = type;
  t->to = to;
  t->ttl = TRACE_TTL;

  if (t->type == TRACE_UDP)
    {
      t->udpfd = socket (PF_INET, SOCK_DGRAM, 0);
      if (t->udpfd < 0)
        error (EXIT_FAILURE, errno, "socket");

      if (setsockopt (t->udpfd, IPPROTO_IP, IP_TTL, ttlp,
		      sizeof (t->ttl)) < 0)
        error (EXIT_FAILURE, errno, "setsockopt");
    }

  if (t->type == TRACE_ICMP || t->type == TRACE_UDP)
    {
      struct protoent *protocol = getprotobyname ("icmp");
      if (protocol)
	{
	  t->icmpfd = socket (PF_INET, SOCK_RAW, protocol->p_proto);
	  if (t->icmpfd < 0)
            error (EXIT_FAILURE, errno, "socket");

	  if (setsockopt (t->icmpfd, IPPROTO_IP, IP_TTL,
			  ttlp, sizeof (t->ttl)) < 0)
            error (EXIT_FAILURE, errno, "setsockopt");
	}
      else
	{
	  /* FIXME: Should we error out? */
          error (EXIT_FAILURE, 0, "can't find supplied protocol 'icmp'");
	}

      /* free (protocol); ??? */
      /* FIXME: ... */
    }
  else
    {
      /* FIXME: type according to RFC 1393 */
    }
}

void
trace_port (trace_t * t, const unsigned short int port)
{
  assert (t);
  if (port < IPPORT_RESERVED)
    t->to.sin_port = TRACE_UDP_PORT;
  else
    t->to.sin_port = port;
}

int
trace_read (trace_t * t)
{
  int len;
  u_char data[56];		/* For a TIME_EXCEEDED datagram. */
  struct ip *ip;
  icmphdr_t *ic;
  socklen_t siz;

  assert (t);

  siz = sizeof (t->from);

  len = recvfrom (t->icmpfd, (char *) data, 56, 0,
		  (struct sockaddr *) &t->from, &siz);
  if (len < 0)
    error (EXIT_FAILURE, errno, "recvfrom");

  icmp_generic_decode (data, 56, &ip, &ic);

  switch (t->type)
    {
    case TRACE_UDP:
      {
	unsigned short *port;
	if ((ic->icmp_type != ICMP_TIME_EXCEEDED
	     && ic->icmp_type != ICMP_DEST_UNREACH)
	    || (ic->icmp_type == ICMP_DEST_UNREACH
		&& ic->icmp_code != ICMP_PORT_UNREACH))
	  return -1;

	/* check whether it's for us */
        port = (unsigned short *) &ic->icmp_ip + 11;
	if (*port != t->to.sin_port)
	  return -1;

	if (ic->icmp_code == ICMP_PORT_UNREACH)
	  /* FIXME: Ugly hack. */
	  stop = 1;
      }
      break;

    case TRACE_ICMP:
      if (ic->icmp_type != ICMP_TIME_EXCEEDED
	  && ic->icmp_type != ICMP_ECHOREPLY)
	return -1;

      if (ic->icmp_type == ICMP_ECHOREPLY
	  && (ic->icmp_seq != pid || ic->icmp_id != pid))
	return -1;
      else if (ic->icmp_type == ICMP_TIME_EXCEEDED)
	{
	  unsigned short *seq = (unsigned short *) &ic->icmp_ip + 12;
	  unsigned short *ident = (unsigned short *) &ic->icmp_ip + 13;
	  if (*seq != pid || *ident != pid)
	    return -1;
	}

      if (ip->ip_src.s_addr == dest.sin_addr.s_addr)
	/* FIXME: Ugly hack. */
	stop = 1;
      break;

      /* FIXME: Type according to RFC 1393. */

    default:
      break;
    }

  return 0;
}

int
trace_write (trace_t * t)
{
  int len;

  assert (t);

  switch (t->type)
    {
    case TRACE_UDP:
      {
	char data[] = "SUPERMAN";
	len = sendto (t->udpfd, (char *) data, sizeof (data),
		      0, (struct sockaddr *) &t->to, sizeof (t->to));
	if (len < 0)
	  {
	    switch (errno)
	      {
	      case ECONNRESET:
		break;
	      default:
                error (EXIT_FAILURE, errno, "sendto");
	      }
	  }

	if (gettimeofday (&t->tsent, NULL) < 0)
          error (EXIT_FAILURE, errno, "gettimeofday");
      }
      break;

    case TRACE_ICMP:
      {
	icmphdr_t hdr;
	/* FIXME: We could use the pid as the icmp seqno/ident. */
	if (icmp_echo_encode ((u_char *) &hdr, sizeof (hdr), pid, pid))
	  return -1;

	len = sendto (t->icmpfd, (char *) &hdr, sizeof (hdr),
		      0, (struct sockaddr *) &t->to, sizeof (t->to));
	if (len < 0)
	  {
	    switch (errno)
	      {
	      case ECONNRESET:
		break;
	      default:
                error (EXIT_FAILURE, errno, "sendto");
	      }
	  }

	if (gettimeofday (&t->tsent, NULL) < 0)
          error (EXIT_FAILURE, errno, "gettimeofday");
      }
      break;

      /* FIXME: type according to RFC 1393 */

    default:
      break;
    }

  return 0;
}

int
trace_udp_sock (trace_t * t)
{
  return (t != NULL ? t->udpfd : -1);
}

int
trace_icmp_sock (trace_t * t)
{
  return (t != NULL ? t->icmpfd : -1);
}

void
trace_inc_ttl (trace_t * t)
{
  int fd;
  const unsigned char *ttlp;

  assert (t);

  ttlp = &t->ttl;
  t->ttl++;
  fd = (t->type == TRACE_UDP ? t->udpfd : t->icmpfd);
  if (setsockopt (fd, IPPROTO_IP, IP_TTL, ttlp, sizeof (t->ttl)) < 0)
    error (EXIT_FAILURE, errno, "setsockopt");
}

void
trace_inc_port (trace_t * t)
{
  assert (t);
  if (t->type == TRACE_UDP)
    t->to.sin_port = htons (ntohs (t->to.sin_port) + 1);
}
