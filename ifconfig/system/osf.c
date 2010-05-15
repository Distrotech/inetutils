/* osf.c -- OSF specific code for ifconfig
  Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009,
  2010 Free Software Foundation, Inc.

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

/* Written by Marcus Brinkmann.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

#include <unistd.h>

#include <string.h>

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

const char *system_default_format "osf"/* Argument parsing stuff.  */
const char *system_help = "\
NAME [AF]\n\
NAME AF [ADDR [DSTADDR]] [broadcast BRDADDR] [netmask MASK] [metric N]";

struct argp_child system_argp_child;

int
system_parse_opt (struct ifconfig **ifp, char option, char *optarg)
{
  return 0;
}

int
system_parse_opt_rest (struct ifconfig **ifp, int argc, char *argv[])
{
  int i = -1;
  enum
  {
    EXPECT_NOTHING,
    EXPECT_AF,
    EXPECT_NAME,
    EXPECT_BROADCAST,
    EXPECT_NETMASK,
    EXPECT_METRIC,
  } expect = EXPECT_NAME;

  while (++i < argc)
    {
      switch (expect)
	{
	case EXPECT_NAME:
	  *ifp = parse_opt_new_ifs (argv[i]);
	  break;

	case EXPECT_AF:
	  parse_opt_set_af (*ifp, argv[i]);
	  break;

	case EXPECT_BROADCAST:
	  parse_opt_set_brdaddr (*ifp, argv[i]);
	  break;

	case EXPECT_NETMASK:
	  parse_opt_set_netmask (*ifp, argv[i]);
	  break;

	case EXPECT_METRIC:
	  parse_opt_set_metric (*ifp, argv[i]);
	  break;

	case EXPECT_NOTHING:
	  break;
	}

      if (expect != EXPECT_NOTHING)
	expect = EXPECT_NOTHING;
      else if (!strcmp (argv[i], "broadcast"))
	expect = EXPECT_BROADCAST;
      else if (!strcmp (argv[i], "netmask"))
	expect = EXPECT_NETMASK;
      else if (!strcmp (argv[i], "metric"))
	expect = EXPECT_METRIC;
      else
	{
	  /* Recognize up/down.  */
	  /* Also debug, -debug, trailers, -trailers,
	     ipdst.  */
	  if (!(*ifp->valid & IF_VALID_ADDR))
	    {
	      parse_opt_set_address (*ifp, argv[i]);
	      expect = EXPECT_AF;
	    }
	  else if (!(*ifp->valid & IF_VALID_DSTADDR))
	    parse_opt_set_dstaddr (*ifp, argv[i]);
	}
    }

  switch (expect)
    {
    case EXPECT_BROADCAST:
      error (0, 0, "option `broadcast' requires an argument");
      break;

    case EXPECT_NETMASK:
      error (0, 0, "option `netmask' requires an argument");
      break;

    case EXPECT_METRIC:
      error (0, 0, "option `metric' requires an argument");
      break;

    case EXPECT_NAME:
    case EXPECT_AF:
    case EXPECT_NOTHING:
      expect = EXPECT_NOTHING;
      break;
    }
  return expect == EXPECT_NOTHING;
}

int
system_configure (int sfd, struct ifreq *ifr, struct system_ifconfig *ifs)
{
  return 0;
}
