/* linux.c -- Linux specific code for ifconfig

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

#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <linux/if_ether.h>

#include "../ifconfig.h"


/* ARPHRD stuff.  */

static void
print_hwaddr_ether (format_data_t form, unsigned char *data)
{
  *column += printf ("%02X:%02X:%02X:%02X:%02X:%02X",
		    data[0], data[1], data[2], data[3], data[4], data[5]);
}

static void
print_hwaddr_arcnet (format_data_t form, unsigned char *data)
{
  *column += printf ("%02X", data[0]);
}

static void
print_hwaddr_dlci (format_data_t form, unsigned char *data)
{
  *column += printf ("%i", *((short *) data));
}

static void
print_hwaddr_ax25 (format_data_t form, unsigned char *data)
{
  int i = 0;
  while (i < 6 && (data[i] >> 1) != ' ')
    {
      put_char (form, (data[i] >> 1));
      i++;
    }
  i = (data[6] & 0x1E) >> 1;
  if (i)
    *column += printf ("-%i", i);
#undef mangle
}

static void
print_hwaddr_irda (format_data_t form, unsigned char *data)
{
  *column += printf ("%02X:%02X:%02X:%02X",
		    data[3], data[2], data[1], data[0]);
}

static void
print_hwaddr_rose (format_data_t form, unsigned char *data)
{
  *column += printf ("%02X%02X%02X%02X%02X",
		    data[0], data[1], data[2], data[3], data[4]);
}

struct arphrd_symbol
{
  const char *name;
  const char *title;
  int value;
  void (*print_hwaddr) (format_data_t form, unsigned char *data);
} arphrd_symbols[] =
{
  /* ARP protocol HARDWARE identifiers. */

#ifdef ARPHRD_NETROM	/* From KA9Q: NET/ROM pseudo.  */
  { "NETROM", "AMPR NET/ROM", ARPHRD_NETROM, print_hwaddr_ax25 },
#endif
#ifdef ARPHRD_ETHER	/* Ethernet 10/100Mbps.  */
  { "ETHER", "Ethernet", ARPHRD_ETHER, print_hwaddr_ether },
#endif
#ifdef ARPHRD_EETHER	/* Experimental Ethernet.  */
  { "EETHER", "Experimental Etherner", ARPHRD_EETHER, NULL },
#endif
#ifdef ARPHRD_AX25	/* AX.25 Level 2.  */
  { "AX25", "AMPR AX.25", ARPHRD_AX25, print_hwaddr_ax25 },
#endif
#ifdef ARPHRD_PRONET	/* PROnet token ring.  */
  { "PRONET", "PROnet token ring", ARPHRD_PRONET, NULL },
#endif
#ifdef ARPHRD_CHAOS	/* Chaosnet.  */
  { "CHAOS", "Chaosnet", ARPHRD_CHAOS, NULL },
#endif
#ifdef ARPHRD_IEEE802	/* IEEE 802.2 Ethernet/TR/TB.  */
  { "IEEE802", "16/4 Mbps Token Ring", ARPHRD_IEEE802, print_hwaddr_ether },
#endif
#ifdef ARPHRD_ARCNET	/* ARCnet.  */
  { "ARCNET", "ARCnet", ARPHRD_ARCNET, print_hwaddr_arcnet },
#endif
#ifdef ARPHRD_APPLETLK	/* APPLEtalk.  */
  { "APPLETLK", "Appletalk", ARPHRD_APPLETLK, NULL },
#endif
#ifdef ARPHRD_DLCI	/* Frame Relay DLCI.  */
  { "DLCI", "Frame Relay DLCI", ARPHRD_DLCI, print_hwaddr_dlci },
#endif
#ifdef ARPHRD_ATM	/* ATM.  */
  { "ATM", "ATM", ARPHRD_ATM, NULL },
#endif
#ifdef ARPHRD_METRICOM	/* Metricom STRIP (new IANA id).  */
  { "METRICOM", "Metricom STRIP", ARPHRD_METRICOM, NULL },
#endif

  /* Dummy types for non ARP hardware.  */

#ifdef ARPHRD_SLIP
  { "SLIP", "Serial Line IP", ARPHRD_SLIP, NULL },
#endif
#ifdef ARPHRD_CSLIP
  { "CSLIP", "VJ Serial Line IP", ARPHRD_CSLIP, NULL },
#endif
#ifdef ARPHRD_SLIP6
  { "SLIP6", "6-bit Serial Line IP", ARPHRD_SLIP6, NULL },
#endif
#ifdef ARPHRD_CSLIP6
  { "CSLIP6", "VJ 6-bit Serial Line IP", ARPHRD_CSLIP6, NULL },
#endif
#ifdef ARPHRD_RSRVD	/* Notional KISS type.  */
  { "SLIP", "Notional KISS type", ARPHRD_SLIP, NULL },
#endif
#ifdef ARPHRD_ADAPT
  { "ADAPT", "Adaptive Serial Line IP", ARPHRD_ADAPT, NULL },
#endif
#ifdef ARPHRD_ROSE
  { "ROSE", "AMPR ROSE", ARPHRD_ROSE, print_hwaddr_rose },
#endif
#ifdef ARPHRD_X25	/* CCITT X.25.  */
  { "X25", "CCITT X.25", ARPHRD_X25, NULL },
#endif
#ifdef ARPHRD_HWX25	/* Boards with X.25 in firmware.  */
  { "HWX25", "CCITT X.25 in firmware", ARPHRD_HWX25, NULL },
#endif
#ifdef ARPHRD_PPP
  { "PPP", "Point-to-Point Protocol", ARPHRD_PPP, NULL },
#endif
#ifdef ARPHRD_HDLC	/* (Cisco) HDLC.  */
  { "HDLC", "(Cisco)-HDLC", ARPHRD_HDLC, NULL},
#endif
#ifdef ARPHRD_LAPB	/* LAPB.  */
  { "LAPB", "LAPB", ARPHRD_LAPB, NULL },
#endif
#ifdef ARPHRD_DDCMP	/* Digital's DDCMP.  */
  { "DDCMP", "DDCMP", ARPHRD_DDCMP, NULL },
#endif

#ifdef ARPHRD_TUNNEL	/* IPIP tunnel.  */
  { "TUNNEL", "IPIP Tunnel", ARPHRD_TUNNEL, NULL },
#endif
#ifdef ARPHRD_TUNNEL6	/* IPIP6 tunnel.  */
  { "TUNNEL", "IPIP6 Tunnel", ARPHRD_TUNNEL6, NULL },
#endif
#ifdef ARPHRD_FRAD	/* Frame Relay Access Device.  */
  { "FRAD", "Frame Relay Access Device", ARPHRD_FRAD, NULL },
#endif
#ifdef ARPHRD_SKIP	/* SKIP vif.  */
  { "SKIP", "SKIP vif", ARPHRD_SKIP, NULL },
#endif
#ifdef ARPHRD_LOOPBACK	/* Loopback device.  */
  { "LOOPBACK", "Local Loopback", ARPHRD_LOOPBACK, NULL },
#endif
#ifdef ARPHRD_LOCALTALK	/* Localtalk device.  */
  { "LOCALTALK", "Localtalk", ARPHRD_LOCALTALK, NULL },
#endif
#ifdef ARPHRD_FDDI	/* Fiber Distributed Data Interface. */
  { "FDDI", "Fiber Distributed Data Interface", ARPHRD_FDDI, NULL },
#endif
#ifdef ARPHRD_BIF	/* AP1000 BIF.  */
  { "BIF", "AP1000 BIF", ARPHRD_BIF, NULL },
#endif
#ifdef ARPHRD_SIT	/* sit0 device - IPv6-in-IPv4.  */
  { "SIT", "IPv6-in-IPv4", ARPHRD_SIT, NULL },
#endif
#ifdef ARPHRD_IPDDP	/* IP-in-DDP tunnel.  */
  { "IPDDP", "IP-in-DDP", ARPHRD_IPDDP, NULL },
#endif
#ifdef ARPHRD_IPGRE	/* GRE over IP.  */
  { "IPGRE", "GRE over IP", ARPHRD_IPGRE, NULL },
#endif
#ifdef ARPHRD_PIMREG	/* PIMSM register interface.  */
  { "PIMREG", "PIMSM register", ARPHRD_PIMREG, NULL },
#endif
#ifdef ARPHRD_HIPPI	/* High Performance Parallel I'face. */
  { "HIPPI", "HIPPI", ARPHRD_HIPPI, print_hwaddr_ether },
#endif
#ifdef ARPHRD_ASH	/* (Nexus Electronics) Ash.  */
  { "ASH", "Ash", ARPHRD_ASH, NULL },
#endif
#ifdef ARPHRD_ECONET	/* Acorn Econet.  */
  { "ECONET", "Econet", ARPHRD_ECONET, NULL },
#endif
#ifdef ARPHRD_IRDA	/* Linux-IrDA.  */
  { "IRDA", "IrLap", ARPHRD_IRDA, print_hwaddr_irda },
#endif
#ifdef ARPHRD_FCPP	/* Point to point fibrechanel.  */
  { "FCPP", "FCPP", ARPHRD_FCPP, NULL },
#endif
#ifdef ARPHRD_FCAL	/* Fibrechanel arbitrated loop.  */
  { "FCAL", "FCAL", ARPHRD_FCAL, NULL },
#endif
#ifdef ARPHRD_FCPL	/* Fibrechanel public loop.  */
  { "FCPL", "FCPL", ARPHRD_FCPL, NULL },
#endif
#ifdef ARPHRD_FCPFABRIC	/* Fibrechanel fabric.  */
  { "FCFABRIC", "FCFABRIC", ARPHRD_FCPFABRIC, NULL },
#endif
#ifdef ARPHRD_IEEE802_TR /* Magic type ident for TR.  */
  { "IEEE802_TR", "16/4 Mbps Token Ring (New)", ARPHRD_IEEE802_TR,
    print_hwaddr_ether },
#endif
#ifdef ARPHRD_VOID
  { "VOID", "Void (nothing is known)", ARPHRD_VOID, NULL },
#endif
};

struct arphrd_symbol *
arphrd_findvalue (int value)
{
  struct arphrd_symbol *arp = arphrd_symbols;
  while (arp->name != NULL)
    {
      if (arp->value == value)
	break;
      arp++;
    }
  if (arp->name)
    return arp;
  else
    return NULL;
}


/* Output format stuff.  */

const char *system_default_format = "net-tools";

void system_fh_hwaddr_query (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFHWADDR
  struct arphrd_symbol *arp;

  if (ioctl (form->sfd, SIOCGIFHWADDR, form->ifr) < 0)
    select_arg (form, argc, argv, 1);

  arp = arphrd_findvalue (form->ifr->ifr_hwaddr.sa_family);
  select_arg (form, argc, argv, (arp && arp->print_hwaddr) ? 0 : 1);
#else
  select_arg (form, argc, argv, 1);
#endif
}

void
system_fh_hwaddr (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFHWADDR
  if (ioctl (form->sfd, SIOCGIFHWADDR, form->ifr) < 0)
    {
      fprintf (stderr, "%s: SIOCGIFHWADDR failed for interface `%s': %s\n",
	       __progname, form->ifr->ifr_name, strerror (errno));
      exit (EXIT_FAILURE);
    }
  else
    {
      struct arphrd_symbol *arp;

      arp = arphrd_findvalue (form->ifr->ifr_hwaddr.sa_family);
      if (arp && arp->print_hwaddr)
	arp->print_hwaddr (form, form->ifr->ifr_hwaddr.sa_data);
      else
	put_string (form, "(hwaddr unknown)");
    }
#else
  *column += printf ("(not available)");
  had_output = 1;
#endif
}

void
system_fh_hwtype_query (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFHWADDR
  if (ioctl (form->sfd, SIOCGIFHWADDR, form->ifr) >= 0)
    select_arg (form, argc, argv, 0);
  else
#endif
    select_arg (form, argc, argv, 1);
}

void
system_fh_hwtype (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFHWADDR
  if (ioctl (form->sfd, SIOCGIFHWADDR, form->ifr) < 0)
    {
      fprintf (stderr, "%s: SIOCGIFHWADDR failed for interface `%s': %s\n",
	       __progname, form->ifr->ifr_name, strerror (errno));
      exit (EXIT_FAILURE);
    }
  else
    {
      struct arphrd_symbol *arp;

      arp = arphrd_findvalue (form->ifr->ifr_hwaddr.sa_family);
      if (arp)
	put_string (form, arp->title);
      else
	put_string (form, "(hwtype unknown)");
    }
#else
  *column += printf ("(not available)");
  had_output = 1;
#endif
}

void
system_fh_txqlen_query (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFTXQLEN
  if (ioctl (form->sfd, SIOCGIFTXQLEN, form->ifr) >= 0)
    select_arg (form, argc, argv, (form->ifr->ifr_qlen >= 0) ? 0 : 1);
  else
#endif
    select_arg (form, argc, argv, 1);
}

void
system_fh_txqlen (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFTXQLEN
  if (ioctl (form->sfd, SIOCGIFTXQLEN, form->ifr) < 0)
    {
      fprintf (stderr, "%s: SIOCGIFTXQLEN failed for interface `%s': %s\n",
	       __progname, form->ifr->ifr_name, strerror (errno));
      exit (EXIT_FAILURE);
    }
  else
    put_int (form, argc, argv, form->ifr->ifr_qlen);
#else
  *column += printf ("(not available)");
  had_output = 1;
#endif
}


/* Argument parsing stuff.  */

const char *system_help = "\
  NAME [ADDR] [broadcast BRDADDR]\n\
  [pointopoint|dstaddr DSTADDR] [netmask MASK]\n\
  [metric N] [mtu N] [txqueuelen N]";

const char *system_help_options =
"      --txqlen N        Set transmit queue length to N";

void
system_parse_opt_set_txqlen(struct ifconfig *ifp, char *arg)
{
  char *end;

  if (!ifp)
    {
      fprintf (stderr, "%s: no interface specified for txqlen"
	       " `%s'\n", __progname, arg);
      usage (EXIT_FAILURE);
    }

  if (! (ifp->valid & IF_VALID_SYSTEM))
    {
      ifp->system = malloc (sizeof (struct system_ifconfig));
      if (! ifp->system)
	{
	  fprintf (stderr, "%s: can't get memory for system interface "
		   "configuration: %s\n", __progname, strerror (errno));
	  exit (EXIT_FAILURE);
	}
      ifp->system->valid = 0;
      ifp->valid |= IF_VALID_SYSTEM;
    }
  if (ifp->system->valid & IF_VALID_TXQLEN)
    {
      fprintf (stderr, "%s: only one txqlen allowed for interface `%s'\n",
	       __progname, ifp->name);
      usage (EXIT_FAILURE);
    }
  ifp->system->txqlen = strtol (arg, &end, 0);
  if (*arg == '\0' || *end != '\0')
    {
      fprintf (stderr, "%s: txqlen value `%s' for interface `%s' "
	       "is not a number.\n",
	       __progname, arg, ifp->name);
      exit (EXIT_FAILURE);
    }
  ifp->system->valid |= IF_VALID_TXQLEN;
}

int
system_parse_opt(struct ifconfig **ifpp, char option, char *arg)
{
  struct ifconfig *ifp = *ifpp;

  switch (option)
    {
    case 'T':  /* txqlen */
      system_parse_opt_set_txqlen (ifp, optarg);
      break;

    default:
      return 0;
    }
  return 1;
}

int
system_parse_opt_rest (struct ifconfig **ifp, int argc, char *argv[])
{
  int i = 0;
  enum
  {
    EXPECT_NOTHING,
    EXPECT_BROADCAST,
    EXPECT_DSTADDR,
    EXPECT_NETMASK,
    EXPECT_MTU,
    EXPECT_METRIC,
    EXPECT_TXQLEN
  } expect = EXPECT_NOTHING;

  *ifp = parse_opt_new_ifs (argv[0]);

  while (++i < argc)
    {
      switch (expect)
	{
	case EXPECT_BROADCAST:
	  parse_opt_set_brdaddr (*ifp, argv[i]);
	  break;

	case EXPECT_DSTADDR:
	  parse_opt_set_dstaddr (*ifp, argv[i]);
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

	case EXPECT_TXQLEN:
	  system_parse_opt_set_txqlen (*ifp, argv[i]);
	  break;

	case EXPECT_NOTHING:
	  break;
	}

      if (expect != EXPECT_NOTHING)
	expect = EXPECT_NOTHING;
      else if (! strcmp (argv[i], "broadcast"))
	expect = EXPECT_BROADCAST;
      else if (! strcmp (argv[i], "dstaddr") || ! strcmp (argv[i], "pointopoint"))
	expect = EXPECT_DSTADDR;
      else if (! strcmp (argv[i], "netmask"))
	expect = EXPECT_NETMASK;
      else if (! strcmp (argv[i], "metric"))
	expect = EXPECT_METRIC;
      else if (! strcmp (argv[i], "mtu"))
	expect = EXPECT_MTU;
      else if (! strcmp (argv[i], "txqueuelen"))
	expect = EXPECT_TXQLEN;
      else
	parse_opt_set_address (*ifp, argv[i]);
    }

  switch (expect)
    {
    case EXPECT_BROADCAST:
      fprintf (stderr, "%s: option `broadcast' requires an argument\n",
	       __progname);
      break;

    case EXPECT_DSTADDR:
      fprintf (stderr, "%s: option `pointopoint' (`dstaddr') requires an argument\n",
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

    case EXPECT_TXQLEN:
      fprintf (stderr, "%s: option `txqueuelen' requires an argument\n",
	       __progname);
      break;

    case EXPECT_NOTHING:
      break;
    }
  if (expect != EXPECT_NOTHING)
    usage (EXIT_FAILURE);

  return 1;
}


int
system_configure (int sfd, struct ifreq *ifr, struct system_ifconfig *ifs)
{
  if (ifs->valid & IF_VALID_TXQLEN)
    {
#ifndef SIOCSIFTXQLEN
      printf ("%s: Don't know how to set the txqlen on this system.\n",
	      __progname);
      return -1;
#else
      int err = 0;

      ifr->ifr_qlen = ifs->txqlen;
      err = ioctl (sfd, SIOCSIFTXQLEN, ifr);
      if (err < 0)
	{
	  fprintf (stderr, "%s: SIOCSIFTXQLEN failed: %s\n",
		   __progname, strerror (errno));
	  return -1;
	}
      if (verbose)
	printf ("Set txqlen value of `%s' to `%i'.\n",
		ifr->ifr_name, ifr->ifr_qlen);
#endif
    }
  return 0;
}
