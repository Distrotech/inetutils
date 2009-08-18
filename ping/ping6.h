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

#define PING_MAX_DATALEN (65535 - sizeof (struct icmp6_hdr))

#define USE_IPV6 1

static PING *ping_init (int type, int ident);
static int ping_set_dest (PING * ping, char *host);
static int ping_recv (PING * p);
static int ping_xmit (PING * p);

static int ping_run (PING * ping, int (*finish) ());
static int ping_finish (void);
