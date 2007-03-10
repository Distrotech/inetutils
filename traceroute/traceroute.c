/* Traceroute
   
   Copyright (C) 2007 Free Software Foundation, Inc.
   
   This file is part of GNU Inetutils.
   
   Written by Elian Gidoni.
   
   GNU Inetutils is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   GNU Inetutils is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301 USA.
*/

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
#include <limits.h>
#include <assert.h>
#include <getopt.h>
#include <icmp.h>

#include "traceroute.h"

#define TIME_INTERVAL 3

void show_usage (void);
void do_try (trace_t * trace, const int hop,
	     const int max_hops, const int max_tries);

char *get_hostname (struct in_addr *addr);

int stop = 0;
int pid = 0;
struct sockaddr_in dest;

char *program_name;

int opt_port = 33434;
int opt_max_hops = 64;
int opt_max_tries = 3;
int opt_resolve_hostnames = 0;

#define OPT_VERSION 1
#define OPT_HELP 2
#define OPT_RESOLVE_HOSTNAMES 3

char short_options[] = "p:";
struct option long_options[] = {
  {"port", required_argument, NULL, 'p'},
  {"resolve-hostnames", no_argument, NULL, OPT_RESOLVE_HOSTNAMES},
  {"version", no_argument, NULL, OPT_VERSION},
  {"help", no_argument, NULL, OPT_HELP},
};


int
main (int argc, char **argv)
{
  int c;
  struct hostent *host;
  trace_t trace;

  program_name = argv[0];

  while ((c = getopt_long (argc, argv, short_options, long_options, NULL))
	 != EOF)
    {
      switch (c)
	{
	case OPT_VERSION:
	  printf ("traceroute (%s) %s\n", PACKAGE_NAME, PACKAGE_VERSION);
	  printf ("Copyright (C) 2007 Free Software Foundation, Inc.\n");
	  printf
	    ("This is free software.  You may redistribute copies of it under the terms of\n");
	  printf
	    ("the GNU General Public License <http://www.gnu.org/licenses/gpl.html>.\n");
	  printf
	    ("There is NO WARRANTY, to the extent permitted by law.\n\n");
	  printf ("Written by Elian Gidoni.\n");
	  exit (0);
	  break;

	case OPT_HELP:
	  show_usage ();
	  exit (0);
	  break;

	case OPT_RESOLVE_HOSTNAMES:
	  opt_resolve_hostnames = 1;
	  break;

	case 'p':
	  {
	    char *p;
	    opt_port = strtoul (optarg, &p, 0);
	    if (*p || opt_port == 0 || opt_port > 65536)
	      error (1, 0, "invalid port number `%s'\n", optarg);
	  }
	  break;

	default:
	  fprintf (stderr,
		   "Try `%s --help' for more information.\n", program_name);
	  exit (1);
	  break;
	}
    }

  if (optind < argc)
    {
      host = gethostbyname (argv[optind]);
      if (host == NULL)
	error (1, 0, "%s: %s", argv[optind], hstrerror (h_errno));
    }

  argc -= optind;
  argv += optind;
  if (argc == 0)
    {
      error (0, 0, "insufficient arguments");
      fprintf (stderr, "Try `%s --help' for more information.\n",
	       program_name);
      exit (1);
    }

  if (getuid () != 0)
    {
      error (0, 0, "insufficient permissions");
      fprintf (stderr, "Try `%s --help' for more information.\n",
	       program_name);
      exit (1);
    }

  dest.sin_addr = *(struct in_addr *) host->h_addr;
  dest.sin_family = AF_INET;
  dest.sin_port = htons (opt_port);

  printf ("traceroute to %s (%s), %d hops max\n",
	  host->h_name, inet_ntoa (dest.sin_addr), opt_max_hops);

  trace_init (&trace, dest, TRACE_ICMP);

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
show_usage (void)
{
  printf
    ("Usage: traceroute [OPTION]... HOST\n"
     "\n"
     "   -p, --port=PORT           (default: 33434)\n"
     "       --resolve-hostnames   resolve hostnames\n"
     "       --help                display this help and exit\n"
     "       --version             output version information and exit\n"
     "\n"
     "Report bugs to <" PACKAGE_BUGREPORT ">.\n");
}

void
do_try (trace_t * trace, const int hop,
	const int max_hops, const int max_tries)
{
  fd_set readset;
  int ret, tries, readonly = 0;
  struct timeval now, time;
  struct hostent *host;
  double triptime = 0.0;
  trace_t *prev_trace = NULL;

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
	      error (1, 1, "select failed");
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
		  /* FIXME: Instead of printing this (only where the
		     previous IP numbers are the same):

		     22   64.233.167.99 (64.233.167.99) 130.000 ms 64.233.167.99 (64.233.167.99) 130.000 ms  64.233.167.99 (64.233.167.99) 130.000 ms

		     print this:

		     22   64.233.167.99 (64.233.167.99) 130.000 ms 130.000 ms  130.000 ms */
		  printf (" %s (%s) ",
			  inet_ntoa (trace->from.sin_addr),
			  get_hostname (&trace->from.sin_addr));
		  printf ("%.3fms ", triptime);
		}
	      prev_trace = trace;
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
	{
	  perror ("socket");
	  exit (EXIT_FAILURE);
	}

      if (setsockopt (t->udpfd, IPPROTO_IP, IP_TTL, ttlp, 
		      sizeof (t->ttl)) < 0)
	{
	  perror ("setsockopt");
	  exit (EXIT_FAILURE);
	}
    }

  if (t->type == TRACE_ICMP || t->type == TRACE_UDP)
    {
      struct protoent *protocol = getprotobyname ("icmp");
      if (protocol)
	{
	  t->icmpfd = socket (PF_INET, SOCK_RAW, protocol->p_proto);
	  if (t->icmpfd < 0)
	    {
	      perror ("socket");
	      exit (EXIT_FAILURE);
	    }

	  if (setsockopt (t->icmpfd, IPPROTO_IP, IP_TTL,
			  ttlp, sizeof (t->ttl)) < 0)
	    {
	      perror ("setsockopt");
	      exit (EXIT_FAILURE);
	    }
	}
      else
	{
	  /* FIXME: Should we error out? */
	  fprintf (stderr, "can't find supplied protocol 'icmp'.\n");
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
  char data[56];		/* For a TIME_EXCEEDED datagram. */
  struct ip *ip;
  icmphdr_t *ic;
  int siz;

  assert (t);

  siz = sizeof (t->from);

  len = recvfrom (t->icmpfd, (char *) data, 56, 0,
		  (struct sockaddr *) &t->from, &siz);
  if (len < 0)
    {
      perror ("recvfrom");
      exit (EXIT_FAILURE);
    }

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
		perror ("sendto");
		exit (EXIT_FAILURE);
	      }
	  }
	
	if (gettimeofday (&t->tsent, NULL) < 0)
	  {
	    perror ("gettimeofday");
	    exit (EXIT_FAILURE);
	  }
      }
      break;
      
    case TRACE_ICMP:
      {
	icmphdr_t hdr;
	/* FIXME: We could use the pid as the icmp seqno/ident. */
	if (icmp_echo_encode ((char *) &hdr, sizeof (hdr), pid, pid))
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
		perror ("sendto");
		exit (EXIT_FAILURE);
	      }
	  }
	
	if (gettimeofday (&t->tsent, NULL) < 0)
	  {
	    perror ("gettimeofday");
	    exit (EXIT_FAILURE);
	  }
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
    {
      perror ("setsockopt");
      exit (EXIT_FAILURE);
    }
}

void
trace_inc_port (trace_t * t)
{
  assert (t);
  if (t->type == TRACE_UDP)
    t->to.sin_port = htons (ntohs (t->to.sin_port) + 1);
}
