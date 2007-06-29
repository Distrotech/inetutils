/* generic.c -- generic system code for ifconfig

   Copyright (C) 2001, 2002, 2007 Free Software Foundation, Inc.

   Written by Marcus Brinkmann.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 3
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdio.h>
#include <errno.h>

#if HAVE_UNISTD_H
# include <unistd.h>
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
