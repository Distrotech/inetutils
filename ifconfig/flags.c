/* flags.c -- network interface flag handling

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

#include <stdio.h>

#if HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#include <sys/socket.h>
#include <net/if.h>
#include "ifconfig.h"

/* Conversion table for interface flag names.
   The mask must be a power of 2.  */
struct if_flag
{
  const char *name;
  int mask;
} if_flags[] =
{
  /* Available on all systems which derive the network interface from
     BSD. Verified for GNU, Linux 2.4, FreeBSD, Solaris 2.7, HP-UX
     10.20 and OSF 4.0g.  */

#ifdef IFF_UP		/* Interface is up.  */
  { "UP", IFF_UP },
#endif
#ifdef IFF_BROADCAST	/* Broadcast address is valid.  */
  { "BROADCAST", IFF_BROADCAST },
#endif
#ifdef IFF_DEBUG	/* Debugging is turned on.  */
  { "DEBUG", IFF_DEBUG },
#endif
#ifdef IFF_LOOPBACK	/* Is a loopback net.  */
  { "LOOPBACK", IFF_LOOPBACK },
#endif
#ifdef IFF_POINTOPOINT	/* Interface is a point-to-point link.  */
  { "POINTOPOINT", IFF_POINTOPOINT },
#endif
#ifdef IFF_RUNNING	/* Resources allocated.  */
  { "RUNNING", IFF_RUNNING },
#endif
#ifdef IFF_NOARP	/* No address resolution protocol.  */
  { "NOARP", IFF_NOARP },
#endif
#ifdef IFF_PROMISC	/* Receive all packets.  */
  { "PROMISC", IFF_PROMISC },
#endif
#ifdef IFF_ALLMULTI	/* Receive all multicast packets.  */
  { "ALLMULTI", IFF_ALLMULTI },
#endif
#ifdef IFF_MULTICAST	/* Supports multicast.  */
  { "MULTICAST", IFF_MULTICAST },
#endif

  /* Usually available on all systems which derive the network
     interface from BSD (see above), but with exceptions noted.  */

#ifdef IFF_NOTRAILERS	/* Avoid use of trailers.  */
  /* Obsoleted on FreeBSD systems.  */
  { "NOTRAILERS", IFF_NOTRAILERS },
#endif

  /* Available on GNU and Linux systems.  */

#ifdef IFF_MASTER	/* Master of a load balancer.  */
  { "MASTER", IFF_MASTER },
#endif
#ifdef IFF_SLAVE	/* Slave of a load balancer.  */
  { "SLAVE", IFF_SLAVE },
#endif
#ifdef IFF_PORTSEL	/* Can set media type.  */
  { "PORTSEL", IFF_PORTSEL },
#endif
#ifdef IFF_AUTOMEDIA	/* Auto media select is active.  */
  { "AUTOMEDIA", IFF_AUTOMEDIA },
#endif

  /* Available on Linux 2.4 systems (not glibc <= 2.2.1).  */
#ifdef IFF_DYNAMIC	/* Dialup service with hanging addresses.  */
  { "DYNAMIC", IFF_DYNAMIC },
#endif

  /* Available on FreeBSD and OSF 4.0g systems.  */

#ifdef IFF_OACTIVE	/* Transmission is in progress.  */
  { "OACTIVE", IFF_OACTIVE },
#endif
#ifdef IFF_SIMPLEX	/* Can't hear own transmissions.  */
  { "SIMPLEX", IFF_SIMPLEX },
#endif

  /* Available on FreeBSD systems.  */

#ifdef IFF_LINK0	/* Per link layer defined bit.  */
  { "LINK0", IFF_LINK0 },
#endif
#ifdef IFF_LINK1	/* Per link layer defined bit.  */
  { "LINK1", IFF_LINK1 },
#endif
#if defined(IFF_LINK2) && defined(IFF_ALTPHYS)
#  if IFF_LINK2 == IFF_ALTPHYS
  /* IFF_ALTPHYS == IFF_LINK2 on FreeBSD.  This entry is used as a
     fallback for if_flagtoname conversion, if no relevant EXPECT_
     macro is specified to figure out which one is meant.  */
  { "LINK2/ALTPHYS", IFF_LINK2 },
#  endif
#endif
#ifdef IFF_LINK2	/* Per link layer defined bit.  */
  { "LINK2", IFF_LINK2 },
#endif
#ifdef IFF_ALTPHYS	/* Use alternate physical connection.  */
  { "ALTPHYS", IFF_ALTPHYS },
#endif

  /* Available on Solaris 2.7 systems.  */

#ifdef IFF_INTELLIGENT	/* Protocol code on board.  */
  { "INTELLIGENT", IFF_INTELLIGENT },
#endif
#ifdef IFF_MULTI_BCAST	/* Multicast using broadcast address.  */
  { "MULTI_BCAST", IFF_MULTI_BCAST },
#endif
#ifdef IFF_UNNUMBERED	/* Address is not unique.  */
  { "UNNUMBERED", IFF_UNNUMBERED },
#endif
#ifdef IFF_DHCPRUNNING	/* Interface is under control of DHCP.  */
  { "DHCPRUNNING", IFF_DHCPRUNNING },
#endif
#ifdef IFF_PRIVATE	/* Do not advertise.  */
  { "PRIVATE", IFF_PRIVATE },
#endif

  /* Available on HP-UX 10.20 systems.  */

#ifdef IFF_NOTRAILERS	/* Avoid use of trailers.  */
  { "NOTRAILERS", IFF_NOTRAILERS },
#endif
#ifdef IFF_LOCALSUBNETS	/* Subnets of this net are local.  */
  { "LOCALSUBNETS", IFF_LOCALSUBNETS },
#endif
#ifdef IFF_CKO		/* Interface supports header checksum.  */
  { "CKO", IFF_CKO },
#endif
#ifdef IFF_NOACC	/* No data access on outbound.  */
  { "NOACC", IFF_NOACC },
#endif
#ifdef IFF_OACTIVE	/* Transmission in progress.  */
  { "OACTIVE", IFF_OACTIVE },
#endif
#ifdef IFF_NOSR8025	/* No source route 802.5.  */
  { "NOSR8025", IFF_NOSR8025 },
#endif
#ifdef IFF_CKO_ETC	/* Interface supports trailer checksum.  */
  { "CKO_ETC", IFF_CKO_ETC },
#endif
#ifdef IFF_AR_SR8025	/* All routes broadcast for ARP 8025.  */
  { "AR_SR8025", IFF_AR_SR8025 },
#endif
#ifdef IFF_ALT_SR8025	/* Alternating no rif, rif for ARP on.  */
  { "ALT_SR8025", IFF_ALT_SR8025 },
#endif

  /* Defined on OSF 4.0g systems.  */

#ifdef IFF_PFCOPYALL	/* PFILT gets packets to this host.  */
  { "PFCOPYALL", IFF_PFCOPYALL },
#endif
#ifdef IFF_UIOMOVE	/* DART.  */
  { "UIOMOVE", IFF_UIOMOVE },
#endif
#ifdef IFF_PKTOK	/* DART.  */
  { "PKTOK", IFF_PKTOK },
#endif
#ifdef IFF_SOCKBUF	/* DART.  */
  { "SOCKBUF", IFF_SOCKBUF },
#endif
#ifdef IFF_VAR_MTU	/* Interface supports variable MTUs.  */
  { "VAR_MTU", IFF_VAR_MTU },
#endif
#ifdef IFF_NOCHECKSUM	/* No checksums needed (reliable media).  */
  { "NOCHECKSUM", IFF_NOCHECKSUM },
#endif
#ifdef IFF_MULTINET	/* Multiple networks on interface.  */
  { "MULTINET", IFF_MULTINET },
#endif
#ifdef IFF_VMIFNET	/* Used to identify a virtual MAC address.  */
  { "VMIFNET", IFF_VMIFNET },
#endif
#if defined(IFF_D1) && defined(IFF_SNAP)
#  if IFF_D1 == IFF_SNAP
  /* IFF_SNAP == IFF_D1 on OSF 4.0g systems.  This entry is used as a
     fallback for if_flagtoname conversion, if no relevant EXPECT_
     macro is specified to figure out which one is meant.  */
  { "D1/SNAP", IFF_D2 },
#  endif
#endif
#ifdef IFF_D2		/* Flag is specific to device.  */
  { "D2", IFF_D2 },
#endif
#ifdef IFF_SNAP		/* Ethernet driver outputs SNAP header.  */
  { "SNAP", IFF_SNAP },
#endif
#ifdef IFF_D2		/* Flag is specific to device.  */
  { "D2", IFF_D2 },
#endif
};

/* Return the name corresponding to the interface flag FLAG.
   If FLAG is unknown, return NULL.
   AVOID contains a ':' surrounded and seperated list of flag names
   that should be avoided if alternative names with the same flag value
   exists.  The first unavoided match is returned, or the first avoided
   match if no better is available.  */
const char *
if_flagtoname (int flag, const char *avoid)
{
  struct if_flag *fp = if_flags;
  const char *first_match = NULL;
  char *start;

  while (fp->name)
    {
      if (flag == fp->mask)
	break;
      fp++;
    }
  if (! fp->name)
    return NULL;

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

/* Return the flag mask corresponding to flag name NAME.  If no flag
   with this name is found, return 0.  */
int
if_nametoflag (const char *name)
{
  struct if_flag *fp = if_flags;

  while (fp->name && strcasecmp (name, fp->name))
    fp++;

  return fp->mask;
}

/* Print the flags in FLAGS, using AVOID as in if_flagtoname, and
   SEPERATOR between individual flags.  Returns the number of
   characters printed.  */
int
print_if_flags (int flags, const char *avoid, char seperator)
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
		  putchar (seperator);
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
	  putchar (seperator);
	  length++;
	}
      length += printf ("%#x", flags);
    }
  return length;
}
