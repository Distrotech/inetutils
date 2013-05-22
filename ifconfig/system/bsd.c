/* bsd.c -- BSD specific code for ifconfig
  Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009,
  2010, 2011, 2012, 2013 Free Software Foundation, Inc.

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

#include <config.h>

#include <unistd.h>
#include "../ifconfig.h"
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_types.h>
#include <net/if_dl.h>
#include <netinet/if_ether.h>
#include <ifaddrs.h>

#include <unused-parameter.h>


/* Output format stuff.  */

const char *system_default_format = "unix";


/* Argument parsing stuff.  */

const char *system_help = "\
NAME [ADDR [DSTADDR]] [broadcast BRDADDR] [netmask MASK] "
"[metric N] [mtu N] [up|down]";

struct argp_child system_argp_child;

int
system_parse_opt (struct ifconfig **ifp _GL_UNUSED_PARAMETER,
		  char option _GL_UNUSED_PARAMETER,
		  char *optarg _GL_UNUSED_PARAMETER)
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
      else if (!strcmp (argv[i], "broadcast"))
	expect = EXPECT_BROADCAST;
      else if (!strcmp (argv[i], "netmask"))
	expect = EXPECT_NETMASK;
      else if (!strcmp (argv[i], "metric"))
	expect = EXPECT_METRIC;
      else if (!strcmp (argv[i], "mtu"))
	expect = EXPECT_MTU;
      else if (!strcmp (argv[i], "up"))
	parse_opt_set_flag (*ifp, IFF_UP | IFF_RUNNING, 0);
      else if (!strcmp (argv[i], "down"))
	parse_opt_set_flag (*ifp, IFF_UP, 1);
      else
	{
	  /* Recognize AF here.  */
	  /* Also alias, -alias, promisc, -promisc,
	     create, destroy, monitor, -monitor.  */
	  if (!((*ifp)->valid & IF_VALID_ADDR))
	    parse_opt_set_address (*ifp, argv[i]);
	  else if (!((*ifp)->valid & IF_VALID_DSTADDR))
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

    case EXPECT_MTU:
      error (0, 0, "option `mtu' requires an argument");
      break;

    case EXPECT_NOTHING:
      break;
    }
  return expect == EXPECT_NOTHING;
}

int
system_configure (int sfd _GL_UNUSED_PARAMETER,
		  struct ifreq *ifr _GL_UNUSED_PARAMETER,
		  struct system_ifconfig *ifs _GL_UNUSED_PARAMETER)
{
  return 0;
}


/* System hooks. */
static struct ifaddrs *ifp = NULL;

#define ESTABLISH_IFADDRS \
  if (!ifp) \
    getifaddrs (&ifp);

struct if_nameindex* (*system_if_nameindex) (void) = if_nameindex;

void
system_fh_hwaddr_query (format_data_t form, int argc, char *argv[])
{
  ESTABLISH_IFADDRS
  if (!ifp)
    select_arg (form, argc, argv, 1);
  else
    {
      int missing = 1;
      struct ifaddrs *fp;

      for (fp = ifp; fp; fp = fp->ifa_next)
	{
	  struct sockaddr_dl *dl;

	  if (fp->ifa_addr->sa_family != AF_LINK ||
	      strcmp (fp->ifa_name, form->ifr->ifr_name))
	    continue;

	  dl = (struct sockaddr_dl *) fp->ifa_addr;
	  if (dl && (dl->sdl_len > 0) &&
	      dl->sdl_type == IFT_ETHER)	/* XXX: More cases?  */
	    missing = 0;
	  break;
	}
      select_arg (form, argc, argv, missing);
    }
}

void
system_fh_hwaddr (format_data_t form, int argc _GL_UNUSED_PARAMETER,
		  char *argv[] _GL_UNUSED_PARAMETER)
{
  ESTABLISH_IFADDRS
  if (!ifp)
    put_string (form, "(hwaddr unknown)");
  else
    {
      int missing = 1;
      struct ifaddrs *fp;

      for (fp = ifp; fp; fp = fp->ifa_next)
	{
	  struct sockaddr_dl *dl;

	  if (fp->ifa_addr->sa_family != AF_LINK ||
	      strcmp (fp->ifa_name, form->ifr->ifr_name))
	    continue;

	  dl = (struct sockaddr_dl *) fp->ifa_addr;
	  if (dl && (dl->sdl_len > 0) &&
	      dl->sdl_type == IFT_ETHER)	/* XXX: More cases?  */
	    {
	      missing = 0;
	      put_string (form, ether_ntoa ((struct ether_addr *) LLADDR (dl)));
	    }
	  break;
	}
      if (missing)
	put_string (form, "(hwaddr unknown)");
    }
}

void
system_fh_hwtype_query (format_data_t form, int argc, char *argv[])
{
  system_fh_hwaddr_query (form, argc, argv);
}

void
system_fh_hwtype (format_data_t form, int argc _GL_UNUSED_PARAMETER,
		  char *argv[] _GL_UNUSED_PARAMETER)
{
  ESTABLISH_IFADDRS
  if (!ifp)
    put_string (form, "(hwtype unknown)");
  else
    {
      int found = 0;
      struct ifaddrs *fp;
      struct sockaddr_dl *dl = NULL;

      for (fp = ifp; fp; fp = fp->ifa_next)
	{
	  if (fp->ifa_addr->sa_family != AF_LINK ||
	      strcmp (fp->ifa_name, form->ifr->ifr_name))
	    continue;

	  dl = (struct sockaddr_dl *) fp->ifa_addr;
	  if (dl && (dl->sdl_len > 0) &&
	      dl->sdl_type == IFT_ETHER)	/* XXX: More cases?  */
	    {
	      found = 1;
	      put_string (form, ETHERNAME);
	    }
	  break;
	}
      if (!found)
	put_string (form, "(unknown hwtype)");
    }
}
