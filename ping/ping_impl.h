/* Copyright (C) 1998,2001, 2002, 2005, 2006, 2007, 2008
   Free Software Foundation, Inc.

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

#define PING_MAX_DATALEN (65535 - MAXIPLEN - MAXICMPLEN)

extern unsigned options;
extern PING *ping;
extern u_char *data_buffer;
extern size_t data_length;

extern int ping_run (PING * ping, int (*finish) ());
extern int ping_finish (void);
extern void print_icmp_header (struct sockaddr_in *from,
			       struct ip *ip, icmphdr_t * icmp, int len);
