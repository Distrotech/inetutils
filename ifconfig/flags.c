/* flags.c -- network interface flag handling
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

#include <stdio.h>

#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <stdlib.h>
#include "ifconfig.h"
#include "xalloc.h"

/* Conversion table for interface flag names.
   The mask must be a power of 2.  */
struct if_flag
{
  const char *name;
  int mask;
  int rev;
} if_flags[] =
  {
    /* Available on all systems which derive the network interface from
       BSD. Verified for GNU, Linux 2.4, FreeBSD, Solaris 2.7, HP-UX
       10.20 and OSF 4.0g.  */

#ifdef IFF_UP			/* Interface is up.  */
    {"UP", IFF_UP},
#endif
#ifdef IFF_BROADCAST		/* Broadcast address is valid.  */
    {"BROADCAST", IFF_BROADCAST},
#endif
#ifdef IFF_DEBUG		/* Debugging is turned on.  */
    {"DEBUG", IFF_DEBUG},
#endif
#ifdef IFF_LOOPBACK		/* Is a loopback net.  */
    {"LOOPBACK", IFF_LOOPBACK},
#endif
#ifdef IFF_POINTOPOINT		/* Interface is a point-to-point link.  */
    {"POINTOPOINT", IFF_POINTOPOINT},
#endif
#ifdef IFF_RUNNING		/* Resources allocated.  */
    {"RUNNING", IFF_RUNNING},
#endif
#ifdef IFF_NOARP		/* No address resolution protocol.  */
    {"NOARP", IFF_NOARP},
    {"ARP", IFF_NOARP, 1},
#endif
#ifdef IFF_PROMISC		/* Receive all packets.  */
    {"PROMISC", IFF_PROMISC},
#endif
#ifdef IFF_ALLMULTI		/* Receive all multicast packets.  */
    {"ALLMULTI", IFF_ALLMULTI},
#endif
#ifdef IFF_MULTICAST		/* Supports multicast.  */
    {"MULTICAST", IFF_MULTICAST},
#endif
    /* Usually available on all systems which derive the network
       interface from BSD (see above), but with exceptions noted.  */
#ifdef IFF_NOTRAILERS		/* Avoid use of trailers.  */
    /* Obsoleted on FreeBSD systems.  */
    {"NOTRAILERS", IFF_NOTRAILERS},
    {"TRAILERS", IFF_NOTRAILERS, 1},
#endif
    /* Available on GNU and Linux systems.  */
#ifdef IFF_MASTER		/* Master of a load balancer.  */
    {"MASTER", IFF_MASTER},
#endif
#ifdef IFF_SLAVE		/* Slave of a load balancer.  */
    {"SLAVE", IFF_SLAVE},
#endif
#ifdef IFF_PORTSEL		/* Can set media type.  */
    {"PORTSEL", IFF_PORTSEL},
#endif
#ifdef IFF_AUTOMEDIA		/* Auto media select is active.  */
    {"AUTOMEDIA", IFF_AUTOMEDIA},
#endif
    /* Available on Linux 2.4 systems (not glibc <= 2.2.1).  */
#ifdef IFF_DYNAMIC		/* Dialup service with hanging addresses.  */
    {"DYNAMIC", IFF_DYNAMIC},
#endif
    /* Available on FreeBSD and OSF 4.0g systems.  */
#ifdef IFF_OACTIVE		/* Transmission is in progress.  */
    {"OACTIVE", IFF_OACTIVE},
#endif
#ifdef IFF_SIMPLEX		/* Can't hear own transmissions.  */
    {"SIMPLEX", IFF_SIMPLEX},
#endif
    /* Available on FreeBSD systems.  */
#ifdef IFF_LINK0		/* Per link layer defined bit.  */
    {"LINK0", IFF_LINK0},
#endif
#ifdef IFF_LINK1		/* Per link layer defined bit.  */
    {"LINK1", IFF_LINK1},
#endif
#if defined(IFF_LINK2) && defined(IFF_ALTPHYS)
# if IFF_LINK2 == IFF_ALTPHYS
    /* IFF_ALTPHYS == IFF_LINK2 on FreeBSD.  This entry is used as a
       fallback for if_flagtoname conversion, if no relevant EXPECT_
       macro is specified to figure out which one is meant.  */
    {"LINK2/ALTPHYS", IFF_LINK2},
# endif
#endif
#ifdef IFF_LINK2		/* Per link layer defined bit.  */
    {"LINK2", IFF_LINK2},
#endif
#ifdef IFF_ALTPHYS		/* Use alternate physical connection.  */
    {"ALTPHYS", IFF_ALTPHYS},
#endif
    /* Available on Solaris 2.7 systems.  */
#ifdef IFF_INTELLIGENT		/* Protocol code on board.  */
    {"INTELLIGENT", IFF_INTELLIGENT},
#endif
#ifdef IFF_MULTI_BCAST		/* Multicast using broadcast address.  */
    {"MULTI_BCAST", IFF_MULTI_BCAST},
#endif
#ifdef IFF_UNNUMBERED		/* Address is not unique.  */
    {"UNNUMBERED", IFF_UNNUMBERED},
#endif
#ifdef IFF_DHCPRUNNING		/* Interface is under control of DHCP.  */
    {"DHCPRUNNING", IFF_DHCPRUNNING},
#endif
#ifdef IFF_PRIVATE		/* Do not advertise.  */
    {"PRIVATE", IFF_PRIVATE},
#endif
    /* Available on HP-UX 10.20 systems.  */
#ifdef IFF_NOTRAILERS		/* Avoid use of trailers.  */
    {"NOTRAILERS", IFF_NOTRAILERS},
    {"TRAILERS", IFF_NOTRAILERS, 1},
#endif
#ifdef IFF_LOCALSUBNETS		/* Subnets of this net are local.  */
    {"LOCALSUBNETS", IFF_LOCALSUBNETS},
#endif
#ifdef IFF_CKO			/* Interface supports header checksum.  */
    {"CKO", IFF_CKO},
#endif
#ifdef IFF_NOACC		/* No data access on outbound.  */
    {"NOACC", IFF_NOACC},
    {"ACC", IFF_NOACC, 1},
#endif
#ifdef IFF_OACTIVE		/* Transmission in progress.  */
    {"OACTIVE", IFF_OACTIVE},
#endif
#ifdef IFF_NOSR8025		/* No source route 802.5.  */
    {"NOSR8025", IFF_NOSR8025},
    {"SR8025", IFF_NOSR8025, 1},
#endif
#ifdef IFF_CKO_ETC		/* Interface supports trailer checksum.  */
    {"CKO_ETC", IFF_CKO_ETC},
#endif
#ifdef IFF_AR_SR8025		/* All routes broadcast for ARP 8025.  */
    {"AR_SR8025", IFF_AR_SR8025},
#endif
#ifdef IFF_ALT_SR8025		/* Alternating no rif, rif for ARP on.  */
    {"ALT_SR8025", IFF_ALT_SR8025},
#endif
    /* Defined on OSF 4.0g systems.  */
#ifdef IFF_PFCOPYALL		/* PFILT gets packets to this host.  */
    {"PFCOPYALL", IFF_PFCOPYALL},
#endif
#ifdef IFF_UIOMOVE		/* DART.  */
    {"UIOMOVE", IFF_UIOMOVE},
#endif
#ifdef IFF_PKTOK		/* DART.  */
    {"PKTOK", IFF_PKTOK},
#endif
#ifdef IFF_SOCKBUF		/* DART.  */
    {"SOCKBUF", IFF_SOCKBUF},
#endif
#ifdef IFF_VAR_MTU		/* Interface supports variable MTUs.  */
    {"VAR_MTU", IFF_VAR_MTU},
#endif
#ifdef IFF_NOCHECKSUM		/* No checksums needed (reliable media).  */
    {"NOCHECKSUM", IFF_NOCHECKSUM},
    {"CHECKSUM", IFF_NOCHECKSUM, 1},
#endif
#ifdef IFF_MULTINET		/* Multiple networks on interface.  */
    {"MULTINET", IFF_MULTINET},
#endif
#ifdef IFF_VMIFNET		/* Used to identify a virtual MAC address.  */
    {"VMIFNET", IFF_VMIFNET},
#endif
#if defined(IFF_D1) && defined(IFF_SNAP)
# if IFF_D1 == IFF_SNAP
    /* IFF_SNAP == IFF_D1 on OSF 4.0g systems.  This entry is used as a
       fallback for if_flagtoname conversion, if no relevant EXPECT_
       macro is specified to figure out which one is meant.  */
    {"D1/SNAP", IFF_D2},
# endif
#endif
#ifdef IFF_D2			/* Flag is specific to device.  */
    {"D2", IFF_D2},
#endif
#ifdef IFF_SNAP			/* Ethernet driver outputs SNAP header.  */
    {"SNAP", IFF_SNAP},
#endif
#ifdef IFF_D2			/* Flag is specific to device.  */
    {"D2", IFF_D2},
#endif
    { NULL }
  };

static int
cmpname (const void *a, const void *b)
{
  return strcmp (*(const char**)a, *(const char**)b);
}

char *
if_list_flags (const char *prefix)
{
  size_t len = 0;
  struct if_flag *fp;
  char **fnames;
  size_t i, fcount;
  char *str, *p;

  for (fp = if_flags, len = 0, fcount = 0; fp->name; fp++)
    if (!fp->rev)
      {
	fcount++;
	len += strlen (fp->name) + 1;
      }

  fcount = sizeof (if_flags) / sizeof (if_flags[0]) - 1;
  fnames = xmalloc (fcount * sizeof (fnames[0]) + len);
  p = (char*)(fnames + fcount);

  for (fp = if_flags, i = 0; fp->name; fp++)
    if (!fp->rev)
      {
	const char *q;

	fnames[i++] = p;
	q = fp->name;
	if (strncmp (q, "NO", 2) == 0)
	  q += 2;
	for (; *q; q++)
	  *p++ = tolower (*q);
	*p++ = 0;
      }
  fcount = i;
  qsort (fnames, fcount, sizeof (fnames[0]), cmpname);

  len += 2 * fcount;

  if (prefix)
    len += strlen (prefix);

  str = xmalloc (len + 1);
  p = str;
  if (prefix)
    {
      strcpy (p, prefix);
      p += strlen (prefix);
    }

  for (i = 0; i < fcount; i++)
    {
      if (i && strcmp (fnames[i - 1], fnames[i]) == 0)
	continue;
      strcpy (p, fnames[i]);
      p += strlen (fnames[i]);
      if (++i < fcount)
	{
	  *p++ = ',';
	  *p++ = ' ';
	}
      else
	break;
    }
  *p = 0;
  free (fnames);
  return str;
}

/* Return the name corresponding to the interface flag FLAG.
   If FLAG is unknown, return NULL.
   AVOID contains a ':' surrounded and separated list of flag names
   that should be avoided if alternative names with the same flag value
   exists.  The first unavoided match is returned, or the first avoided
   match if no better is available.  */
const char *
if_flagtoname (int flag, const char *avoid)
{
  struct if_flag *fp;
  const char *first_match = NULL;
  char *start;

  for (fp = if_flags; ; fp++)
    {
      if (!fp->name)
	return NULL;
      if (flag == fp->mask && !fp->rev)
	break;
    }

  first_match = fp->name;

  /* We now have found the first match.  Look for a better one.  */
  if (avoid)
    do
      {
	start = strstr (avoid, fp->name);
	if (!start || *(start - 1) != ':'
	    || *(start + strlen (fp->name)) != ':')
	  break;
	fp++;
      }
    while (fp->name);

  if (fp->name)
    return fp->name;
  else
    return first_match;
}

int
if_nametoflag (const char *name, size_t len, int *prev)
{
  struct if_flag *fp;
  int rev = 0;

  if (len > 1 && name[0] == '-')
    {
      name++;
      len--;
      rev = 1;
    }
  else if (len > 2 && strncasecmp (name, "NO", 2) == 0)
    {
      name += 2;
      len -= 2;
      rev = 1;
    }

  for (fp = if_flags; fp->name; fp++)
    {
      if (strncasecmp (fp->name, name, len) == 0)
	{
	  *prev = fp->rev ^ rev;
	  return fp->mask;
	}
    }
  return 0;
}

int
if_nameztoflag (const char *name, int *prev)
{
  return if_nametoflag (name, strlen (name), prev);
}

struct if_flag_char
{
  int mask;
  int ch;
};

/* Interface flag bits and the corresponding letters for short output.
   Notice that the entries must be ordered alphabetically, by the letter name.
   There are two lamentable exceptions:

   1. The 'd' is misplaced.
   2. The 'P' letter is ambiguous. Depending on its position in the output
      line it may stand for IFF_PROMISC or IFF_POINTOPOINT.

   That's the way netstat does it.
*/
static struct if_flag_char flag_char_tab[] = {
  { IFF_ALLMULTI,    'A' },
  { IFF_BROADCAST,   'B' },
  { IFF_DEBUG,       'D' },
  { IFF_LOOPBACK,    'L' },
  { IFF_MULTICAST,   'M' },
#ifdef HAVE_DYNAMIC
  { IFF_DYNAMIC,     'd' },
#endif
  { IFF_PROMISC,     'P' },
  { IFF_NOTRAILERS,  'N' },
  { IFF_NOARP,       'O' },
  { IFF_POINTOPOINT, 'P' },
  { IFF_SLAVE,       's' },
  { IFF_MASTER,      'm' },
  { IFF_RUNNING,     'R' },
  { IFF_UP,          'U' },
  { 0 }
};

void
if_format_flags (int flags, char *buf, size_t size)
{
  struct if_flag_char *fp;
  size--;
  for (fp = flag_char_tab; size && fp->mask; fp++)
    if (fp->mask & flags)
      {
	*buf++ = fp->ch;
	size--;
      }
  *buf = 0;
}


/* Print the flags in FLAGS, using AVOID as in if_flagtoname, and
   SEPARATOR between individual flags.  Returns the number of
   characters printed.  */
int
print_if_flags (int flags, const char *avoid, char separator)
{
  int f = 1;
  const char *name;
  int first = 1;
  int length = 0;

  while (flags && f)
    {
      if (f & flags)
	{
	  name = if_flagtoname (f, avoid);
	  if (name)
	    {
	      if (!first)
		{
		  putchar (separator);
		  length++;
		}
	      length += printf ("%s", name);
	      flags &= ~f;
	      first = 0;
	    }
	}
      f = f << 1;
    }
  if (flags)
    {
      if (!first)
	{
	  putchar (separator);
	  length++;
	}
      length += printf ("%#x", flags);
    }
  return length;
}
