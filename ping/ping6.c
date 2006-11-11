/* Copyright (C) 1998, 2001, 2002, 2004, 2005, 2006 Free Software Foundation, Inc.

   This file is part of GNU Inetutils.

   GNU Inetutils is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GNU Inetutils is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GNU Inetutils; see the file COPYING.  If not, write
   to the Free Software Foundation, Inc., 51 Franklin Street,
   Fifth Floor, Boston, MA 02110-1301 USA. */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/socket.h>
#include <sys/time.h>
#include <signal.h>

#include <netinet/in.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>

#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#include <getopt.h>
#include <xalloc.h>
#include "ping_common.h"
#include "ping6.h"

static char short_options[] = "VLhc:dfi:l:np:qRrs:t:v";
static struct option long_options[] = {
  /* Help options */
  {"version", no_argument, NULL, 'V'},
  {"license", no_argument, NULL, 'L'},
  {"help", no_argument, NULL, 'h'},
  /* Common options */
  {"count", required_argument, NULL, 'c'},
  {"debug", no_argument, NULL, 'd'},
  {"ignore-routing", no_argument, NULL, 'r'},
  {"size", required_argument, NULL, 's'},
  {"interval", required_argument, NULL, 'i'},
  {"numeric", no_argument, NULL, 'n'},
  /* echo-specific options */
  {"flood", no_argument, NULL, 'f'},
  {"preload", required_argument, NULL, 'l'},
  {"pattern", required_argument, NULL, 'p'},
  {"quiet", no_argument, NULL, 'q'},
  {NULL, no_argument, NULL, 0}
};

static PING *ping;
unsigned char *data_buffer;
size_t data_length = PING_DATALEN;
static unsigned int options;
static unsigned long preload = 0;

static int ping_echo (int argc, char **argv);

static void show_usage (void);
static int send_echo (PING * ping);

char *program_name;

int
main (int argc, char **argv)
{
  int c;
  char *p;
  int one = 1;
  u_char pattern[16];
  int pattern_len = 16;
  u_char *patptr = NULL;
  int is_root = getuid () == 0;

  program_name = argv[0];
  if ((ping = ping_init (0, getpid ())) == NULL)
    {
      fprintf (stderr, "can't init ping: %s\n", strerror (errno));
      exit (1);
    }
  setsockopt (ping->ping_fd, SOL_SOCKET, SO_BROADCAST, (char *) &one,
	      sizeof (one));

  /* Reset root privileges */
  setuid (getuid ());

  /* Parse command line */
  while ((c = getopt_long (argc, argv, short_options, long_options, NULL))
	 != EOF)
    {
      switch (c)
	{
	case 'V':
	  printf ("ping - %s %s\n", PACKAGE_NAME, PACKAGE_VERSION);
	  printf ("Copyright (C) 2005 Free Software Foundation, Inc.\n");
	  printf ("%s comes with ABSOLUTELY NO WARRANTY.\n", PACKAGE_NAME);
	  printf ("You may redistribute copies of %s\n", PACKAGE_NAME);
	  printf ("under the terms of the GNU General Public License.\n");
	  printf ("For more information about these matters, ");
	  printf ("see the files named COPYING.\n");
	  exit (0);
	  break;

	case 'L':
	  show_license ();
	  exit (0);

	case 'h':
	  show_usage ();
	  exit (0);
	  break;

	case 'c':
	  ping->ping_count = ping_cvt_number (optarg, 0, 0);
	  break;

	case 'd':
	  setsockopt (ping->ping_fd, SOL_SOCKET, SO_DEBUG, &one,
		      sizeof (one));
	  break;

	case 'r':
	  setsockopt (ping->ping_fd, SOL_SOCKET, SO_DONTROUTE, &one,
		      sizeof (one));
	  break;

	case 'i':
	  options |= OPT_INTERVAL;
	  ping->ping_interval = ping_cvt_number (optarg, 0, 0);
	  break;

	case 'p':
	  decode_pattern (optarg, &pattern_len, pattern);
	  patptr = pattern;
	  break;

	case 's':
	  data_length = ping_cvt_number (optarg, PING_MAX_DATALEN, 1);
	  break;

	case 'n':
	  options |= OPT_NUMERIC;
	  break;

	case 'q':
	  options |= OPT_QUIET;
	  break;

	case 'l':
	  if (!is_root)
	    {
	      fprintf (stderr, "ping: option not allowed: --preload\n");
	      exit (1);
	    }
	  preload = strtoul (optarg, &p, 0);
	  if (*p || preload > INT_MAX)
	    {
	      fprintf (stderr, "ping: invalid preload value (%s)\n", optarg);
	      exit (1);
	    }
	  break;

	case 'f':
	  if (!is_root)
	    {
	      fprintf (stderr, "ping: option not allowed: --flood\n");
	      exit (1);
	    }
	  options |= OPT_FLOOD;
	  setbuf (stdout, (char *) NULL);
	  break;

	default:
	  fprintf (stderr, "%c: not implemented\n", c);
	  exit (1);
	}
    }

  argc -= optind;
  argv += optind;
  if (argc == 0)
    {
      show_usage ();
      exit (0);
    }

  init_data_buffer (patptr, pattern_len);

  return ping_echo (argc, argv);
}

static char *
ipaddr2str (struct sockaddr_in6 *from)
{
  int err;
  size_t len;
  char *buf, ipstr[256], hoststr[256];

  err = getnameinfo ((struct sockaddr *) from, sizeof (*from), ipstr,
		     sizeof (ipstr), NULL, 0, NI_NUMERICHOST);
  if (err)
    {
      const char *errmsg;

      if (err == EAI_SYSTEM)
	errmsg = strerror (errno);
      else
	errmsg = gai_strerror (err);

      fprintf (stderr, "ping: getnameinfo: %s\n", errmsg);
      return xstrdup ("unknown");
    }

  if (options & OPT_NUMERIC)
    return xstrdup (ipstr);

  err = getnameinfo ((struct sockaddr *) from, sizeof (*from), hoststr,
		     sizeof (hoststr), NULL, 0, NI_NAMEREQD);
  if (err)
    return xstrdup (ipstr);

  len = strlen (ipstr) + strlen (hoststr) + 4;	/* Pair of parentheses, a space
						   and a NUL. */
  buf = xmalloc (len);
  sprintf (buf, "%s (%s)", hoststr, ipstr);

  return buf;
}

static volatile int stop = 0;

static RETSIGTYPE
sig_int (int signal)
{
  stop = 1;
}

static int
ping_run (PING * ping, int (*finish) ())
{
  fd_set fdset;
  int fdmax;
  struct timeval timeout;
  struct timeval last, intvl, now;
  struct timeval *t = NULL;
  int finishing = 0;
  int nresp = 0;

  signal (SIGINT, sig_int);

  fdmax = ping->ping_fd + 1;

  while (preload--)
    send_echo (ping);

  if (options & OPT_FLOOD)
    {
      intvl.tv_sec = 0;
      intvl.tv_usec = 10000;
    }
  else
    {
      intvl.tv_sec = ping->ping_interval;
      intvl.tv_usec = 0;
    }

  gettimeofday (&last, NULL);
  send_echo (ping);

  while (!stop)
    {
      int n;

      FD_ZERO (&fdset);
      FD_SET (ping->ping_fd, &fdset);
      gettimeofday (&now, NULL);
      timeout.tv_sec = last.tv_sec + intvl.tv_sec - now.tv_sec;
      timeout.tv_usec = last.tv_usec + intvl.tv_usec - now.tv_usec;

      while (timeout.tv_usec < 0)
	{
	  timeout.tv_usec += 1000000;
	  timeout.tv_sec--;
	}
      while (timeout.tv_usec >= 1000000)
	{
	  timeout.tv_usec -= 1000000;
	  timeout.tv_sec++;
	}

      if (timeout.tv_sec < 0)
	timeout.tv_sec = timeout.tv_usec = 0;

      n = select (fdmax, &fdset, NULL, NULL, &timeout);
      if (n < 0)
	{
	  if (errno != EINTR)
	    perror ("select");
	  continue;
	}
      else if (n == 1)
	{
	  if (ping_recv (ping) == 0)
	    nresp++;
	  if (t == 0)
	    {
	      gettimeofday (&now, NULL);
	      t = &now;
	    }
	  if (ping->ping_count && nresp >= ping->ping_count)
	    break;
	}
      else
	{
	  if (!ping->ping_count || ping->ping_num_xmit < ping->ping_count)
	    {
	      send_echo (ping);
	      if (!(options & OPT_QUIET) && options & OPT_FLOOD)
		{
		  putchar ('.');
		}
	    }
	  else if (finishing)
	    break;
	  else
	    {
	      finishing = 1;

	      intvl.tv_sec = MAXWAIT;
	    }
	  gettimeofday (&last, NULL);
	}
    }
  if (finish)
    return (*finish) ();
  return 0;
}

static int
send_echo (PING * ping)
{
  int off = 0;

  if (PING_TIMING (data_length))
    {
      struct timeval tv;
      gettimeofday (&tv, NULL);
      ping_set_data (ping, &tv, 0, sizeof (tv));
      off += sizeof (tv);
    }
  if (data_buffer)
    ping_set_data (ping, data_buffer, off,
		   data_length > PING_HEADER_LEN ?
		   data_length - PING_HEADER_LEN : data_length);
  return ping_xmit (ping);
}

static int
ping_finish ()
{
  fflush (stdout);
  printf ("--- %s ping statistics ---\n", ping->ping_hostname);
  printf ("%ld packets transmitted, ", ping->ping_num_xmit);
  printf ("%ld packets received, ", ping->ping_num_recv);
  if (ping->ping_num_rept)
    printf ("+%ld duplicates, ", ping->ping_num_rept);
  if (ping->ping_num_xmit)
    {
      if (ping->ping_num_recv > ping->ping_num_xmit)
	printf ("-- somebody's printing up packets!");
      else
	printf ("%d%% packet loss",
		(int) (((ping->ping_num_xmit - ping->ping_num_recv) * 100) /
		       ping->ping_num_xmit));

    }
  printf ("\n");
  return 0;
}

static int print_echo (int dup, int hops, struct ping_stat *stat,
		       struct sockaddr_in6 *dest, struct sockaddr_in6 *from,
		       struct icmp6_hdr *icmp6, int datalen);
static void print_icmp_error (struct sockaddr_in6 *from,
			      struct icmp6_hdr *icmp6, int len);

static int echo_finish (void);

static int
ping_echo (int argc, char **argv)
{
  int err;
  char buffer[256];
  struct ping_stat ping_stat;

  if (options & OPT_FLOOD && options & OPT_INTERVAL)
    {
      fprintf (stderr, "ping: -f and -i incompatible options.\n");
      return 2;
    }

  memset (&ping_stat, 0, sizeof (ping_stat));
  ping_stat.tmin = 999999999.0;

  ping->ping_datalen = data_length;
  ping->ping_closure = &ping_stat;

  if (ping_set_dest (ping, *argv))
    {
      fprintf (stderr, "ping: unknown host\n");
      exit (1);
    }

  err = getnameinfo ((struct sockaddr *) &ping->ping_dest,
		     sizeof (ping->ping_dest), buffer,
		     sizeof (buffer), NULL, 0, NI_NUMERICHOST);
  if (err)
    {
      const char *errmsg;

      if (err == EAI_SYSTEM)
	errmsg = strerror (errno);
      else
	errmsg = gai_strerror (err);

      fprintf (stderr, "ping: getnameinfo: %s\n", errmsg);

      exit (1);
    }

  printf ("PING %s (%s): %d data bytes\n",
	  ping->ping_hostname, buffer, data_length);

  return ping_run (ping, echo_finish);
}

static int
print_echo (int dupflag, int hops, struct ping_stat *ping_stat,
	    struct sockaddr_in6 *dest, struct sockaddr_in6 *from,
	    struct icmp6_hdr *icmp6, int datalen)
{
  int err;
  char buf[256];
  struct timeval tv;
  int timing = 0;
  double triptime = 0.0;

  gettimeofday (&tv, NULL);

  /* Do timing */
  if (PING_TIMING (datalen - sizeof (struct icmp6_hdr)))
    {
      struct timeval tv1, *tp;

      timing++;
      tp = (struct timeval *) icmp6 + 1;

      /* Avoid unaligned data: */
      memcpy (&tv1, tp, sizeof (tv1));
      tvsub (&tv, &tv1);

      triptime = ((double) tv.tv_sec) * 1000.0 +
	((double) tv.tv_usec) / 1000.0;
      ping_stat->tsum += triptime;
      ping_stat->tsumsq += triptime * triptime;
      if (triptime < ping_stat->tmin)
	ping_stat->tmin = triptime;
      if (triptime > ping_stat->tmax)
	ping_stat->tmax = triptime;
    }

  if (options & OPT_QUIET)
    return 0;
  if (options & OPT_FLOOD)
    {
      putchar ('\b');
      return 0;
    }

  err = getnameinfo ((struct sockaddr *) from, sizeof (*from), buf,
		     sizeof (buf), NULL, 0, 0);
  if (err)
    {
      const char *errmsg;

      if (err == EAI_SYSTEM)
	errmsg = strerror (errno);
      else
	errmsg = gai_strerror (err);

      fprintf (stderr, "ping: getnameinfo: %s\n", errmsg);

      strcpy (buf, "unknown");
    }

  printf ("%d bytes from %s: icmp_seq=%u", datalen, buf,
	  htons (icmp6->icmp6_seq));
  if (hops >= 0)
    printf (" ttl=%d", hops);
  if (timing)
    printf (" time=%.3f ms", triptime);
  if (dupflag)
    printf (" (DUP!)");

  putchar ('\n');

  return 0;
}

#define NITEMS(a) sizeof(a)/sizeof((a)[0])

struct icmp_code_descr
{
  int code;
  char *diag;
};

static struct icmp_code_descr icmp_dest_unreach_desc[] = {
  {ICMP6_DST_UNREACH_NOROUTE, "No route to destination"},
  {ICMP6_DST_UNREACH_ADMIN, "Destination administratively prohibited"},
  {ICMP6_DST_UNREACH_BEYONDSCOPE, "Beyond scope of source address"},
  {ICMP6_DST_UNREACH_ADDR, "Address unreachable"},
  {ICMP6_DST_UNREACH_NOPORT, "Port unreachable"},
};

static void
print_dst_unreach (struct icmp6_hdr *icmp6)
{
  struct icmp_code_descr *p;

  printf ("Destination unreachable: ");
  for (p = icmp_dest_unreach_desc;
       p < icmp_dest_unreach_desc + NITEMS (icmp_dest_unreach_desc); p++)
    {
      if (p->code == icmp6->icmp6_code)
	{
	  puts (p->diag);
	  return;
	}
    }

  printf ("Unknown code %d\n", icmp6->icmp6_code);
}

static void
print_packet_too_big (struct icmp6_hdr *icmp6)
{
  printf ("Packet too big, mtu=%d\n", icmp6->icmp6_mtu);
}

static struct icmp_code_descr icmp_time_exceeded_desc[] = {
  {ICMP6_TIME_EXCEED_TRANSIT, "Hop limit exceeded"},
  {ICMP6_TIME_EXCEED_REASSEMBLY, "Fragment reassembly timeout"},
};

static void
print_time_exceeded (struct icmp6_hdr *icmp6)
{
  struct icmp_code_descr *p;

  printf ("Time exceeded: ");
  for (p = icmp_time_exceeded_desc;
       p < icmp_time_exceeded_desc + NITEMS (icmp_time_exceeded_desc); p++)
    {
      if (p->code == icmp6->icmp6_code)
	{
	  puts (p->diag);
	  return;
	}
    }

  printf ("Unknown code %d\n", icmp6->icmp6_code);
}

static struct icmp_code_descr icmp_param_prob_desc[] = {
  {ICMP6_PARAMPROB_HEADER, "Erroneous header field"},
  {ICMP6_PARAMPROB_NEXTHEADER, "Unrecognized Next Header type"},
  {ICMP6_PARAMPROB_OPTION, "Unrecognized IPv6 option"},
};

static void
print_param_prob (struct icmp6_hdr *icmp6)
{
  struct icmp_code_descr *p;

  printf ("Parameter problem: ");
  for (p = icmp_param_prob_desc;
       p < icmp_param_prob_desc + NITEMS (icmp_param_prob_desc); p++)
    {
      if (p->code == icmp6->icmp6_code)
	{
	  puts (p->diag);
	  return;
	}
    }

  printf ("Unknown code %d\n", icmp6->icmp6_code);
}

static struct icmp_diag
{
  int type;
  void (*func) (struct icmp6_hdr *);
} icmp_diag[] =
{
  {
  ICMP6_DST_UNREACH, print_dst_unreach},
  {
  ICMP6_PACKET_TOO_BIG, print_packet_too_big},
  {
  ICMP6_TIME_EXCEEDED, print_time_exceeded},
  {
ICMP6_PARAM_PROB, print_param_prob},};

static void
print_icmp_error (struct sockaddr_in6 *from, struct icmp6_hdr *icmp6, int len)
{
  char *s;
  struct icmp_diag *p;

  s = ipaddr2str (from);
  printf ("%d bytes from %s: ", len, s);
  free (s);

  for (p = icmp_diag; p < icmp_diag + NITEMS (icmp_diag); p++)
    {
      if (p->type == icmp6->icmp6_type)
	{
	  p->func (icmp6);
	  return;
	}
    }

  /* This should never happen because of the ICMP6_FILTER set in
     ping_init(). */
  printf ("Unknown ICMP type: %d\n", icmp6->icmp6_type);
}

static int
echo_finish ()
{
  ping_finish ();
  if (ping->ping_num_recv && PING_TIMING (data_length))
    {
      struct ping_stat *ping_stat = (struct ping_stat *) ping->ping_closure;
      double total = ping->ping_num_recv + ping->ping_num_rept;
      double avg = ping_stat->tsum / total;
      double vari = ping_stat->tsumsq / total - avg * avg;

      printf ("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
	      ping_stat->tmin, avg, ping_stat->tmax, nsqrt (vari, 0.0005));
    }
  exit (ping->ping_num_recv == 0);
}

static void
show_usage (void)
{
  printf ("\
Usage: ping6 [OPTION]... [ADDRESS]...\n\
\n\
Informational options:\n\
  -h, --help         display this help and exit\n\
  -L, --license      display license and exit\n\
  -V, --version      output version information and exit\n\
Options valid for all request types:\n\
  -c, --count N      stop after sending N packets (default: %d)\n\
  -d, --debug        set the SO_DEBUG option\n\
  -i, --interval N   wait N seconds between sending each packet\n\
  -n, --numeric      do not resolve host addresses\n\
  -r, --ignore-routing  send directly to a host on an attached network\n\
Options valid for --echo requests:\n\
* -f, --flood        flood ping \n\
* -l, --preload N    send N packets as fast as possible before falling into\n\
                     normal mode of behavior\n\
  -p, --pattern PAT  fill ICMP packet with given pattern (hex)\n\
  -q, --quiet        quiet output\n\
  -s, --size N       set number of data octets to send\n\
\n\
Options marked with an * are available only to super-user\n\
\n\
report bugs to " PACKAGE_BUGREPORT ".\n\
", DEFAULT_PING_COUNT);
}

static PING *
ping_init (int type, int ident)
{
  int fd, err;
  const int on = 1;
  PING *p;
  struct icmp6_filter filter;

  /* Initialize raw ICMPv6 socket */
  fd = socket (PF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
  if (fd < 0)
    {
      if (errno == EPERM)
	{
	  fprintf (stderr, "ping: ping must run as root\n");
	}
      return NULL;
    }

  /* Tell which ICMPs we are interested in.  */
  ICMP6_FILTER_SETBLOCKALL (&filter);
  ICMP6_FILTER_SETPASS (ICMP6_ECHO_REPLY, &filter);
  ICMP6_FILTER_SETPASS (ICMP6_DST_UNREACH, &filter);
  ICMP6_FILTER_SETPASS (ICMP6_PACKET_TOO_BIG, &filter);
  ICMP6_FILTER_SETPASS (ICMP6_TIME_EXCEEDED, &filter);
  ICMP6_FILTER_SETPASS (ICMP6_PARAM_PROB, &filter);

  err =
    setsockopt (fd, IPPROTO_ICMPV6, ICMP6_FILTER, &filter, sizeof (filter));
  if (err)
    {
      close (fd);
      return NULL;
    }

  err = setsockopt (fd, IPPROTO_IPV6, IPV6_RECVHOPLIMIT, &on, sizeof (on));
  if (err)
    {
      close (fd);
      return NULL;
    }

  /* Allocate PING structure and initialize it to default values */
  p = malloc (sizeof (*p));
  if (!p)
    {
      close (fd);
      return NULL;
    }

  memset (p, 0, sizeof (*p));

  p->ping_fd = fd;
  p->ping_count = DEFAULT_PING_COUNT;
  p->ping_interval = PING_INTERVAL;
  p->ping_datalen = sizeof (struct icmp6_hdr);
  /* Make sure we use only 16 bits in this field, id for icmp is a u_short.  */
  p->ping_ident = ident & 0xFFFF;
  p->ping_cktab_size = PING_CKTABSIZE;
  return p;
}

static int
_ping_setbuf (PING * p)
{
  if (!p->ping_buffer)
    {
      p->ping_buffer = malloc (_PING_BUFLEN (p));
      if (!p->ping_buffer)
	return -1;
    }
  if (!p->ping_cktab)
    {
      p->ping_cktab = malloc (p->ping_cktab_size);
      if (!p->ping_cktab)
	return -1;
      memset (p->ping_cktab, 0, p->ping_cktab_size);
    }
  return 0;
}

static int
ping_set_data (PING * p, void *data, size_t off, size_t len)
{
  if (_ping_setbuf (p))
    return -1;
  if (p->ping_datalen < off + len)
    return -1;
  memcpy (p->ping_buffer + sizeof (struct icmp6_hdr) + off, data, len);

  return 0;
}

static int
ping_xmit (PING * p)
{
  int i, buflen;
  struct icmp6_hdr *icmp6;

  if (_ping_setbuf (p))
    return -1;

  buflen = p->ping_datalen + sizeof (struct icmp6_hdr);

  /* Mark sequence number as sent */
  _PING_CLR (p, p->ping_num_xmit % p->ping_cktab_size);

  icmp6 = (struct icmp6_hdr *) p->ping_buffer;
  icmp6->icmp6_type = ICMP6_ECHO_REQUEST;
  icmp6->icmp6_code = 0;
  /* The checksum will be calculated by the TCP/IP stack.  */
  icmp6->icmp6_cksum = 0;
  icmp6->icmp6_id = htons (p->ping_ident);
  icmp6->icmp6_seq = htons (p->ping_num_xmit);

  i = sendto (p->ping_fd, (char *) p->ping_buffer, buflen, 0,
	      (struct sockaddr *) &p->ping_dest, sizeof (p->ping_dest));
  if (i < 0)
    perror ("ping: sendto");
  else
    {
      p->ping_num_xmit++;
      if (i != buflen)
	printf ("ping: wrote %s %d chars, ret=%d\n",
		p->ping_hostname, buflen, i);
    }

  return 0;
}

static int
my_echo_reply (PING * p, struct icmp6_hdr *icmp6)
{
  struct ip6_hdr *orig_ip = (struct ip6_hdr *) (icmp6 + 1);
  struct icmp6_hdr *orig_icmp = (struct icmp6_hdr *) (orig_ip + 1);

  return IN6_ARE_ADDR_EQUAL (&orig_ip->ip6_dst, &ping->ping_dest.sin6_addr)
    && orig_ip->ip6_nxt == IPPROTO_ICMPV6
    && orig_icmp->icmp6_type == ICMP6_ECHO_REQUEST
    && orig_icmp->icmp6_id == htons (p->ping_ident);
}

static int
ping_recv (PING * p)
{
  int dupflag, n;
  int hops = -1;
  struct msghdr msg;
  struct iovec iov;
  struct icmp6_hdr *icmp6;
  struct cmsghdr *cmsg;
  char cmsg_data[1024];

  iov.iov_base = p->ping_buffer;
  iov.iov_len = _PING_BUFLEN (p);
  msg.msg_name = &p->ping_from;
  msg.msg_namelen = sizeof (p->ping_from);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = cmsg_data;
  msg.msg_controllen = sizeof (cmsg_data);
  msg.msg_flags = 0;

  n = recvmsg (p->ping_fd, &msg, 0);
  if (n < 0)
    return -1;

  for (cmsg = CMSG_FIRSTHDR (&msg); cmsg; cmsg = CMSG_NXTHDR (&msg, cmsg))
    {
      if (cmsg->cmsg_level == IPPROTO_IPV6
	  && cmsg->cmsg_type == IPV6_HOPLIMIT)
	{
	  hops = *(int *) CMSG_DATA (cmsg);
	  break;
	}
    }

  icmp6 = (struct icmp6_hdr *) p->ping_buffer;
  if (icmp6->icmp6_type == ICMP6_ECHO_REPLY)
    {
      /* We got an echo reply.  */
      if (ntohs (icmp6->icmp6_id) != p->ping_ident)
	return -1;		/* It's not a response to us.  */

      if (_PING_TST (p, ntohs (icmp6->icmp6_seq) % p->ping_cktab_size))
	{
	  /* We already got the reply for this echo request.  */
	  p->ping_num_rept++;
	  dupflag = 1;
	}
      else
	{
	  _PING_SET (p, ntohs (icmp6->icmp6_seq) % p->ping_cktab_size);
	  p->ping_num_recv++;
	  dupflag = 0;
	}

      print_echo (dupflag, hops, p->ping_closure, &p->ping_dest,
		  &p->ping_from, icmp6, n);

    }
  else
    {
      /* We got an error reply.  */
      if (!my_echo_reply (p, icmp6))
	return -1;		/* It's not for us.  */

      print_icmp_error (&p->ping_from, icmp6, n);
    }

  return 0;
}

static int
ping_set_dest (PING * ping, char *host)
{
  int err;
  struct addrinfo *result, hints;

  memset (&hints, 0, sizeof (hints));

  hints.ai_family = AF_INET6;

  err = getaddrinfo (host, NULL, &hints, &result);
  if (err)
    return 1;

  memcpy (&ping->ping_dest, result->ai_addr, result->ai_addrlen);

  freeaddrinfo (result);

  ping->ping_hostname = strdup (host);
  if (!ping->ping_hostname)
    return 1;

  return 0;
}
