/* if_index.c -- an implementation of the if_* functions using SIOCGIFCONF

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


#ifndef HAVE_IF_NAMEINDEX
extern char *xrealloc __P ((void *p, size_t s));

struct ifmap
{
  char name[IFNAMSIZ];
  int index;
};

static struct ifmap *iflist;
static int iflist_size;

static int
map_interfaces()
{
  int sfd;
  struct ifmap *newlist;
  struct ifconf ifc = { 0, NULL };
  int last_nr = -1, nr = 0;
  int bufsize, i;
  struct ifreq *ifr;

  sfd = socket (AF_INET, SOCK_STREAM, 0);
  if (sfd < 0)
    return 0;

#ifdef SIOCGIFNUM
  if (ioctl (sfd, SIOCGIFNUM, &bufsize) < 0)
#endif
#ifdef SIOCGIFCOUNT
    if (ioctl (sfd, SIOCGIFCOUNT, &bufsize) < 0)
#endif
      bufsize = 128;	/* Initial guess.  */

  bufsize *= sizeof (struct ifreq);

  /* Loop over SIOCGIFCONF, calling it as often with increasing buffer
     size until number of returned interfaces stabilizes.  This is
     necessary because some systems silently truncate the return
     buffer.  */
  while (last_nr != nr)
    {
      int buflen;

      ifc.ifc_buf = xrealloc (ifc.ifc_buf, bufsize);
      if (! ifc.ifc_buf)
	goto fail;

      ifc.ifc_len = bufsize;
      if (ioctl (sfd, SIOCGIFCONF, &ifc) < 0)
	goto fail; /* XXX: Retry at certain errors.  */

      last_nr = nr;
      nr = 0;
      buflen = ifc.ifc_len;
      ifr = (struct ifreq *) ifc.ifc_buf;
      while (buflen > 0)
        {
	  nr++;
	  buflen -= IFNAMSIZ + SA_LEN(&ifr->ifr_addr);
	}
    }

  newlist = malloc (nr * sizeof (struct ifmap));
  if (!newlist)
    goto fail;

  i = 0;
  ifr = (struct ifreq *) ifc.ifc_buf;
  while (ifc.ifc_len > 0)
    {
      strncpy (newlist[i].name, ifr->ifr_name, IFNAMSIZ);
      newlist[i].name[IFNAMSIZ-1] = '\0';
#ifdef SIOCGIFINDEX
      if (ioctl (sfd, SIOCGIFINDEX, ifr) < 0)
	goto fail;
      newlist[i].index = ifr->ifr_index;
#else
      newlist[i].index = i + 1;
#endif
      i++;
      ifc.ifc_len -= IFNAMSIZ + SA_LEN(&ifr->ifr_addr);
    }
  assert (i == nr - 1);

  if (iflist)
    free (iflist);
  iflist = newlist;
  iflist_size = nr;
  goto ok;

 fail:
  if (newlist)
    free (newlist);
 ok:
  if (!ifc.ifc_buf)
    free (ifc.ifc_buf);
  close (sfd);
  return 1;
}

unsigned int
if_nametoindex (const char *ifname)
{
  

#ifdef SIOCGIFNAME
  
#endif /* HAVE_IF_NAMEINDEX



