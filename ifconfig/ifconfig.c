/* ifconfig.c -- network interface configuration utility

   Copyright (C) 2001, 2002 Free Software Foundation, Inc.

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

#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "ifconfig.h"

int
main(int argc, char *argv[])
{
  int err = 0;
  int sfd;
  struct ifconfig *ifp;

  parse_opt (argc, argv);

  sfd = socket (AF_INET, SOCK_STREAM, 0);
  if (sfd < 0)
    {
      fprintf (stderr, "%s: socket error: %s\n",
		   __progname, strerror (errno));
      exit (1);
    }

  ifp = ifs;
  while (ifp - ifs < nifs)
    {
      err = configure_if (sfd, ifp);
      if (err)
	break;
      ifp++;
    }

  close (sfd);
  return (err);
}
