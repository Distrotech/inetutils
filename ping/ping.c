/* Copyright (C) 1998 Free Software Foundation, Inc.

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
   along with GNU Inetutils; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA. */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/signal.h>

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
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <limits.h>

#include "getopt.h"
#include "version.h"
#include <icmp.h>
#include <ping.h>
#include <ping_impl.h>

#define PACKAGE "ping"
#define GNU_PACKAGE "GNU ping"

static char short_options[] = "VLhc:dfi:l:np:qRrs:t:v";
static struct option long_options[] = 
{
  {"version", no_argument, NULL, 'V'},
  {"license", no_argument, NULL, 'L'},
  {"help",    no_argument, NULL, 'h'},
  {"count",   required_argument, NULL, 'c'},
  {"debug",   no_argument, NULL, 'd'},
  {"flood",   no_argument, NULL, 'f'},
  {"interval",required_argument, NULL, 'i'},
  {"preload", required_argument, NULL, 'l'},
  {"numeric", no_argument, NULL, 'n'},
  {"pattern", required_argument, NULL, 'p'},
  {"quiet",   no_argument, NULL, 'q'},
  {"route",   no_argument, NULL, 'R'},
  {"ignore-routing", no_argument, NULL, 'r'},
  {"size",    required_argument, NULL, 's'},
  {"type",    required_argument, NULL, 't'},
  {"verbose", no_argument, NULL, 'v'},
  {NULL,      no_argument, NULL, 0}
};

extern int ping_echo __P((int argc, char **argv));
extern int ping_timestamp __P((int argc, char **argv));
extern int ping_address __P((int argc, char **argv));
extern int ping_router __P((int argc, char **argv));

PING *ping;
int is_root;        /* were we started with root privileges */
unsigned options;
unsigned long preload = 0;
u_char pattern[16];
int pattern_len = 16;
int (*ping_type) __P((int argc, char **argv)) = ping_echo;


static void show_usage (void);
static void show_license (void);
static void decode_pattern(char *text, int *pattern_len, u_char *pattern_data);
static void decode_type(char *optarg);

int
main(int argc, char **argv)
{
  int c;
  char *p;
  int one = 1;
  
  is_root = getuid() == 0;

  if ((ping = ping_init(getpid())) == NULL)
    {
      fprintf(stderr, "can't init ping: %s\n", strerror(errno));
      exit (1);
    }

  /* Reset root privileges */
  setuid(getuid());

  /* Parse command line */
  while ((c = getopt_long (argc, argv, short_options, long_options, NULL))
	 != EOF)
    {
      switch (c)
	{
	case 'V':
	  printf ("ping - GNU inetutils %s\n", inetutils_version);
	  printf ("Copyright (C) 1998,2001 Free Software Foundation, Inc.\n");
	  printf ("%s comes with ABSOLUTELY NO WARRANTY.\n", GNU_PACKAGE);
	  printf ("You may redistribute copies of %s\n", PACKAGE);
	  printf ("under the terms of the GNU General Public License.\n");
	  printf ("For more information about these matters, ");
	  printf ("see the files named COPYING.\n");
	  exit (0);
	  break;
	case 'L':
	  show_license();
	  exit(0);
	case 'h':
	  show_usage ();
	  exit (0);
	  break;
	case 'c':
	  ping_set_count(ping, atoi(optarg));
	  break;
	case 'd':
	  ping_set_sockopt(ping, SO_DEBUG, &one, sizeof(one));
	  break;
	case 'r':
	  ping_set_sockopt(ping, SO_DONTROUTE, &one, sizeof(one));
	  break;
	case 'i':
	  options |= OPT_INTERVAL;
	  ping_set_interval(ping, atoi(optarg));
	  break;
	case 'p':
	  decode_pattern(optarg, &pattern_len, pattern);
	  break;
 	case 's':
	  ping_set_packetsize(ping, atoi(optarg));
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
	      fprintf(stderr, "ping: option not allowed: --preload\n");
	      exit(1);
	    }
	  preload = strtoul(optarg, &p, 0);
	  if (*p || preload > INT_MAX)
	    {
	      fprintf(stderr, "ping: invalid preload value (%s)\n", optarg);
	      exit(1);
	    }
	  break;

	case 'f':
	  if (!is_root)
	    {
	      fprintf(stderr, "ping: option not allowed: --flood\n");
	      exit(1);
	    }
	  options |= OPT_FLOOD;
	  setbuf(stdout, (char *)NULL);
	  break;

	case 't':
	  decode_type(optarg);
	  break;
	  
	default:
	  fprintf(stderr, "%c: not implemented\n", c);
	  exit(1);
	}
    }

  argc -= optind;
  argv += optind;
  if (argc == 0) 
    {
      show_usage ();
      exit (0);
    }

  return (*ping_type)(argc, argv);
}

void
decode_type(char *optarg)
{
  if (strcasecmp(optarg, "echo") == 0)
    ping_type = ping_echo;
#if 0  
  else if (strcasecmp(optarg, "timestamp") == 0)
    ping_type = ping_timestamp;
  else if (strcasecmp(optarg, "address") == 0)
    ping_type = ping_address;
  else if (strcasecmp(optarg, "router") == 0)
    ping_type = ping_router;
#endif  
  else
    {
      fprintf(stderr, "unsupported packet type: %s\n", optarg);
      exit(1);
    }
}

void
decode_pattern(char *text, int *pattern_len, u_char *pattern_data)
{
  int i, c, off;

  for (i = 0; *text && i < *pattern_len; i++)
    {
      if (sscanf(text, "%2x%n", &c, &off) != 1)
	{
	  fprintf(stderr, "ping: error in pattern near %s\n", text);
	  exit(1);
	}
      text += off;
    }
  *pattern_len = i;
}

void
show_usage (void)
{
  printf("\
Usage: ping [OPTION]... [ADDRESS]...\n\
\n\
  -h, --help         display this help and exit\n\
  -V, --version      output version information and exit\n\
  -c, --count N      stop after sending N ECHO_RESPONSE packets\n\
  -d, --debug        set the SO_DEBUG option\n\
* -f, --flood        flood ping \n\
  -i, --interval N   wait N seconds between sending each packet\n\
* -l, --preload N    send N packets as fast as possible before falling into\n\
                     normal mode of behavior\n\
  -n, --numeric      do not resolve host addresses\n\
  -p, --pattern PAT  fill ICMP packet with given pattern (hex)\n\
  -q, --quiet        quiet output\n\
  -R, --route        record route\n\
  -r, --ignore-routing  send directly to a host on an attached network\n\
  -s, --size N       set number of data octets to send\n\
  -v, --verbose      verbose output\n\
\n\
Options marked with an * are available only to super-user\n\
\n\
report bugs to " inetutils_bugaddr ".\n\
");
}

void
show_license (void)
{
  static char license_text[] =
"   This program is free software; you can redistribute it and/or modify\n"
"   it under the terms of the GNU General Public License as published by\n"
"   the Free Software Foundation; either version 2, or (at your option)\n"
"   any later version.\n"
"\n"
"   This program is distributed in the hope that it will be useful,\n"
"   but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"   GNU General Public License for more details.\n"
"\n"
"   You should have received a copy of the GNU General Public License\n"
"   along with this program; if not, write to the Free Software\n"
"   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n";
  printf("%s", license_text);
}

