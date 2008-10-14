/* Copyright (C) 1998, 2004, 2007 Free Software Foundation, Inc.

   This file is part of GNU Inetutils.

   GNU Inetutils is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GNU Inetutils is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GNU Inetutils; see the file COPYING.  If not, write
   to the Free Software Foundation, Inc., 51 Franklin Street,
   Fifth Floor, Boston, MA 02110-1301 USA. */

#include "ping_common.h"

struct ping_stat
{
  double tmin;			/* minimum round trip time */
  double tmax;			/* maximum round trip time */
  double tsum;			/* sum of all times, for doing average */
  double tsumsq;		/* sum of all times squared, for std. dev. */
};

#define PEV_RESPONSE 0
#define PEV_DUPLICATE 1
#define PEV_NOECHO  2

#define PING_INTERVAL 1
#define PING_CKTABSIZE 128

#define MAXWAIT		10	/* max seconds to wait for response */

#define OPT_FLOOD	0x001
#define OPT_INTERVAL	0x002
#define OPT_NUMERIC	0x004
#define OPT_QUIET	0x008
#define OPT_RROUTE	0x010
#define OPT_VERBOSE	0x020

#define PING_TIMING(s) (s >= PING_HEADER_LEN)
#define PING_HEADER_LEN	sizeof (struct timeval)
#define PING_DATALEN	(64 - PING_HEADER_LEN)	/* default data length */
#define PING_MAX_DATALEN (65535 - sizeof (struct icmp6_hdr))

#define USE_IPV6 1
#define _C_BIT(p,bit)    (p)->ping_cktab[(bit)>>3]	/* byte in ck array */
#define _C_MASK(bit)     (1 << ((bit) & 0x07))

#define _PING_SET(p,bit) (_C_BIT (p,bit) |= _C_MASK (bit))
#define _PING_CLR(p,bit) (_C_BIT (p,bit) &= (~_C_MASK (bit)))
#define _PING_TST(p,bit) (_C_BIT (p,bit) & _C_MASK (bit))

static PING *ping_init (int type, int ident);
static int ping_set_dest (PING * ping, char *host);
static int ping_recv (PING * p);
static int ping_xmit (PING * p);

static int ping_run (PING * ping, int (*finish) ());
static int ping_finish (void);
