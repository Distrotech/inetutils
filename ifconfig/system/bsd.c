/* generic.c -- generic system code for ifconfig
  Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009,
  2010, 2011, 2012 Free Software Foundation, Inc.

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


/* Output format stuff.  */

const char *system_default_format = "unix";


/* Argument parsing stuff.  */

const char *system_help = "\
NAME [ADDR [DSTADDR]] [broadcast BRDADDR] [netmask MASK] [metric N] [mtu N]";

struct argp_child system_argp_child;

int
system_parse_opt (struct ifconfig **ifp, char option, char *optarg)
{
  return 0;
}

int
system_parse_opt_rest (struct ifconfig **ifp, int argc, char *argv[])
{
  return 0;
}

int
system_configure (int sfd, struct ifreq *ifr, struct system_ifconfig *ifs)
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
system_fh_hwaddr (format_data_t form, int argc, char *argv[])
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
system_fh_hwtype (format_data_t form, int argc, char *argv[])
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

void
system_fh_metric (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFMETRIC
  if (ioctl (form->sfd, SIOCGIFMETRIC, form->ifr) < 0)
    error (EXIT_FAILURE, errno,
	   "SIOCGIFMETRIC failed for interface `%s'",
	   form->ifr->ifr_name);
  else
    put_int (form, argc, argv, form->ifr->ifr_metric);
#else
  *column += printf ("(not available)");
  had_output = 1;
#endif
}
