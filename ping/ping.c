/* Copyright (C) 1998,2001, 2002, 2005 Free Software Foundation, Inc.

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

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/time.h>
#include <signal.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
/*#include <netinet/ip_icmp.h>  -- deliberately not including this */
#ifdef HAVE_NETINET_IP_VAR_H
# include <netinet/ip_var.h>
#endif

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

#include <getopt.h>
#include <icmp.h>
#include <ping.h>
#include "ping_common.h"
#include "ping_impl.h"

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
  {"verbose", no_argument, NULL, 'v'},
  /* Packet types */
  {"type", required_argument, NULL, 't'},
  {"echo", no_argument, NULL, ICMP_ECHO},
  {"timestamp", no_argument, NULL, ICMP_TIMESTAMP},
  {"address", no_argument, NULL, ICMP_ADDRESS},
  {"router", no_argument, NULL, ICMP_ROUTERDISCOVERY},
  /* echo-specific options */
  {"flood", no_argument, NULL, 'f'},
  {"preload", required_argument, NULL, 'l'},
  {"pattern", required_argument, NULL, 'p'},
  {"quiet", no_argument, NULL, 'q'},
  {"route", no_argument, NULL, 'R'},
  {NULL, no_argument, NULL, 0}
};

extern int ping_echo (int argc, char **argv);
extern int ping_timestamp (int argc, char **argv);
extern int ping_address (int argc, char **argv);
extern int ping_router (int argc, char **argv);

PING *ping;
u_char *data_buffer;
size_t data_length = PING_DATALEN;
unsigned options;
unsigned long preload = 0;
int (*ping_type) (int argc, char **argv) = ping_echo;


static void show_usage (void);
static void decode_type (const char *optarg);
static int send_echo (PING * ping);

#define MIN_USER_INTERVAL (200000/PING_PRECISION)

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
  bool is_root = false;

  size_t count = 0;
  int socket_type = 0;
  size_t interval = 0;

  program_name = argv[0];

  if (getuid () == 0)
    is_root = true;

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
	  count = ping_cvt_number (optarg, 0, 0);
	  break;

	case 'd':
	  socket_type = SO_DEBUG;
	  break;

	case 'r':
	  socket_type = SO_DONTROUTE;
	  break;

	case 'i':
	  {
	    double v;

	    v = strtod (optarg, &p);
	    if (*p)
	      {
		fprintf (stderr, "Invalid value (`%s' near `%s')\n",
			 optarg, p);
		exit (1);
	      }

	    options |= OPT_INTERVAL;
	    interval = v * PING_PRECISION;
	    if (!is_root && interval < MIN_USER_INTERVAL)
	      {
		fprintf (stderr, "Option value too small: %s\n", optarg);
		exit (1);
	      }
	  }
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

	case 'R':
	  options |= OPT_RROUTE;
	  break;

	case 'v':
	  options |= OPT_VERBOSE;
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
	  if (is_root == false)
	    {
	      fprintf (stderr, "ping: option not allowed: --flood\n");
	      exit (1);
	    }
	  options |= OPT_FLOOD;
	  setbuf (stdout, (char *) NULL);
	  break;

	case 't':
	  decode_type (optarg);
	  break;

	case ICMP_ECHO:
	  decode_type ("echo");
	  break;

	case ICMP_TIMESTAMP:
	  decode_type ("timestamp");
	  break;

	case ICMP_ADDRESS:
	  if (!is_root)
	    {
	      fprintf (stderr, "ping: option not allowed: --address\n");
	      exit (1);
	    }
	  decode_type ("address");
	  break;

	case ICMP_ROUTERDISCOVERY:
	  if (!is_root)
	    {
	      fprintf (stderr, "ping: option not allowed: --router\n");
	      exit (1);
	    }
	  decode_type ("router");
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

  ping = ping_init (ICMP_ECHO, getpid ());
  if (ping == NULL)
    {
      fprintf (stderr, "can't init ping: %s\n", strerror (errno));
      exit (1);
    }
  ping_set_sockopt (ping, SO_BROADCAST, (char *) &one, sizeof (one));

  /* Reset root privileges */
  setuid (getuid ());

  if (count != 0)
    ping_set_count (ping, count);

  if (socket_type != 0)
    ping_set_sockopt (ping, socket_type, &one, sizeof (one));

  if (options & OPT_INTERVAL)
    ping_set_interval (ping, interval);

  init_data_buffer (patptr, pattern_len);

  return (*ping_type) (argc, argv);
}



void
decode_type (const char *optarg)
{
  if (strcasecmp (optarg, "echo") == 0)
    ping_type = ping_echo;
  else if (strcasecmp (optarg, "timestamp") == 0)
    ping_type = ping_timestamp;
  else if (strcasecmp (optarg, "address") == 0)
    ping_type = ping_address;
#if 0
  else if (strcasecmp (optarg, "router") == 0)
    ping_type = ping_router;
#endif
  else
    {
      fprintf (stderr, "unsupported packet type: %s\n", optarg);
      exit (1);
    }
}

int volatile stop = 0;

RETSIGTYPE
sig_int (int signal)
{
  stop = 1;
}

int
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
    PING_SET_INTERVAL (intvl, ping->ping_interval);

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
		putchar ('.');
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

int
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

int
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

void
show_usage (void)
{
  printf ("\
Usage: ping [OPTION]... [ADDRESS]...\n\
\n\
Informational options:\n\
  -h, --help         display this help and exit\n\
  -L, --license      display license and exit\n\
  -V, --version      output version information and exit\n\
Options controlling ICMP request types:\n\
  --echo             Send ICMP_ECHO requests (default)\n\
* --address          Send ICMP_ADDRESS packets\n\
  --timestamp        Send ICMP_TIMESTAMP packets\n\
* --router           Send ICMP_ROUTERDISCOVERY packets\n\
Options valid for all request types:\n\
  -c, --count N      stop after sending N packets\n\
  -d, --debug        set the SO_DEBUG option\n\
  -i, --interval N   wait N seconds between sending each packet\n\
  -n, --numeric      do not resolve host addresses\n\
  -r, --ignore-routing  send directly to a host on an attached network\n\
  -v, --verbose      verbose output\n\
Options valid for --echo requests:\n\
* -f, --flood        flood ping \n\
* -l, --preload N    send N packets as fast as possible before falling into\n\
                     normal mode of behavior\n\
  -p, --pattern PAT  fill ICMP packet with given pattern (hex)\n\
  -q, --quiet        quiet output\n\
  -R, --route        record route\n\
  -s, --size N       set number of data octets to send\n\
\n\
Options marked with an * are available only to super-user\n\
\n\
Report bugs to <" PACKAGE_BUGREPORT ">.\n\
");
}
