/* solaris.c -- Solaris specific code for ifconfig

   Copyright (C) 2001 Free Software Foundation, Inc.

   Written by Marcus Brinkmann.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <version.h>

#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#if STDC_HEADERS
# include <stdlib.h>
#else
# ifndef HAVE_STRCHR
#  define strchr index
#  define strrchr rindex
# endif
#endif

#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <linux/if_ether.h>

#include "../ifconfig.h"


/* Output format stuff.  */

char *system_default_format "unix"


/* Argument parsing stuff.  */

char *system_help = "\
  NAME [ADDR [DSTADDR]] [broadcast BRDADDR]\n\
  [netmask MASK] [metric N] [mtu N]";

char *system_help_options;

int
system_parse_opt(struct ifconfig **ifp, char option, char *optarg)
{
  return 0;
}

int
system_parse_opt_rest (struct ifconfig **ifp, int argc, char *argv[])
{
  int i = 0;
  enum
  {
    EXPECT_NOTHING,
    EXPECT_BROADCAST,
    EXPECT_NETMASK,
    EXPECT_METRIC,
    EXPECT_MTU
  } expect = EXPECT_NOTHING;

  *ifp = parse_opt_new_ifs (argv[0]);

  while (++i < argc)
    {
      switch (expect)
	{
	case EXPECT_BROADCAST:
	  parse_opt_set_brdaddr (*ifp, argv[i]);
	  break;

	case EXPECT_NETMASK:
	  parse_opt_set_netmask (*ifp, argv[i]);
	  break;

	case EXPECT_MTU:
	  parse_opt_set_mtu (*ifp, argv[i]);
	  break;

	case EXPECT_METRIC:
	  parse_opt_set_metric (*ifp, argv[i]);
	  break;

	case EXPECT_NOTHING:
	  break;
	}

      if (expect != EXPECT_NOTHING)
	expect = EXPECT_NOTHING;
      else if (! strcmp (argv[i], "broadcast"))
	expect = EXPECT_BROADCAST;
      else if (! strcmp (argv[i], "netmask"))
	expect = EXPECT_NETMASK;
      else if (! strcmp (argv[i], "metric"))
	expect = EXPECT_METRIC;
      else if (! strcmp (argv[i], "mtu"))
	expect = EXPECT_MTU;
      else
	{
	  /* Recognize AF here.  */
	  /* Recognize up/down.  */
	  /* Also auto-revarp, trailers, -trailers,
	     private, -private, arp, -arp, plumb, unplumb.  */
	  if (! (*ifp->valid & IF_VALID_ADDR))
	    parse_opt_set_address (*ifp, argv[i]);
	  else if (! (*ifp->valid & IF_VALID_DSTADDR))
	    parse_opt_set_dstaddr (*ifp, argv[i]);
	}
    }

  switch (expect)
    {
    case EXPECT_BROADCAST:
      fprintf (stderr, "%s: option `broadcast' requires an argument\n",
	       __progname);
      break;

    case EXPECT_NETMASK:
      fprintf (stderr, "%s: option `netmask' requires an argument\n",
	       __progname);
      break;

    case EXPECT_METRIC:
      fprintf (stderr, "%s: option `metric' requires an argument\n",
	       __progname);
      break;

    case EXPECT_MTU:
      fprintf (stderr, "%s: option `mtu' requires an argument\n",
	       __progname);
      break;

    case EXPECT_NOTHING:
      break;
    }
  if (expect != EXPECT_NOTHING)
    usage (EXIT_FAILURE);

  return 1;
}
