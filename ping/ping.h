/* Copyright (C) 1998, 2007 Free Software Foundation, Inc.

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

#define USE_IPV6 0

#define _C_BIT(p,bit)    (p)->ping_cktab[(bit)>>3]	/* byte in ck array */
#define _C_MASK(bit)     (1 << ((bit) & 0x07))

#define _PING_SET(p,bit) (_C_BIT (p,bit) |= _C_MASK (bit))
#define _PING_CLR(p,bit) (_C_BIT (p,bit) &= (~_C_MASK (bit)))
#define _PING_TST(p,bit) (_C_BIT (p,bit) & _C_MASK (bit))

PING *ping_init (int type, int ident);
void ping_reset (PING * p);
void ping_set_type (PING * p, int type);
void ping_set_packetsize (PING * ping, size_t size);
int ping_set_dest (PING * ping, char *host);
int ping_set_pattern (PING * p, int len, u_char * pat);
void ping_set_event_handler (PING * ping, ping_efp fp, void *closure);
int ping_recv (PING * p);
int ping_xmit (PING * p);
