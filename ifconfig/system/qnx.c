/* generic.c -- generic system code for ifconfig

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
#include <stdio.h>
#include <errno.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if STDC_HEADERS
# include <stdlib.h>
#endif

#include <sys/ioctl.h>
#include <net/if.h>

#include "../ifconfig.h"


/* Output format stuff.  */

const char *system_default_format = "unix";


/* Argument parsing stuff.  */

const char *system_help;

const char *system_help_options;

void
qfh_brdaddr (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFBRDADDR
  if (ioctl (form->sfd, SIOCGIFBRDADDR, form->ifr) < 0)
    {
      fprintf (stderr, "%s: SIOCGIFBRDADDR failed for interface `%s': %s\n",
               __progname, form->ifr->ifr_name, strerror (errno));
      exit (EXIT_FAILURE);
    }
  else
    put_addr (form, argc, argv, &form->ifr->ifr_addr);
#else
  *column += printf ("(not available)");
  had_output = 1;
#endif
}

void
qfh_netmask (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFNETMASK
  if (ioctl (form->sfd, SIOCGIFNETMASK, (caddr_t *)form->ifr) < 0)
    {
      fprintf (stderr, "%s: SIOCGIFNETMASK failed for interface `%s': %s\n",
	       __progname, form->ifr->ifr_name, strerror (errno));
    }
  else
    put_addr (form, argc, argv, &form->ifr->ifr_addr);
#else
  *column += printf ("(not available)");
  had_output = 1;
#endif
}

int
system_parse_opt(struct ifconfig **ifp, char option, char *optarg)
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
