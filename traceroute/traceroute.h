/* Traceroute
   
   Copyright (C) 2007 Free Software Foundation, Inc.
   
   This file is part of GNU Inetutils.
   
   Written by Elian Gidoni.
   
   GNU Inetutils is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 3 of the
   License, or (at your option) any later version.
   
   GNU Inetutils is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301 USA.
*/

#ifndef TRACEROUTE_H
#define TRACEROUTE_H 1

#define TRACE_UDP_PORT 33434
#define TRACE_TTL 1

enum trace_type
{
  TRACE_UDP,			/* UDP datagrams.  */
  TRACE_ICMP,			/* ICMP echo requests.  */
  TRACE_1393			/* RFC 1393 requests. */
};

typedef struct trace
{
  int icmpfd, udpfd;
  enum trace_type type;
  struct sockaddr_in to, from;
  unsigned char ttl;
  struct timeval tsent;
} trace_t;

void trace_init (trace_t * t, const struct sockaddr_in to,
		 const enum trace_type type);
void trace_inc_ttl (trace_t * t);
void trace_inc_port (trace_t * t);
void trace_port (trace_t * t, const unsigned short port);
int trace_read (trace_t * t);
int trace_write (trace_t * t);
int trace_udp_sock (trace_t * t);
int trace_icmp_sock (trace_t * t);

#endif /* TRACEROUTE_H */
