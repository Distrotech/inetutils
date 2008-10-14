/* Copyright (C) 2004, 2006, 2007 Free Software Foundation, Inc.

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

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/icmp6.h>
#include <icmp.h>

#include <stdbool.h>

#define DEFAULT_PING_COUNT 4

/* Not sure about this step*/
#define _PING_BUFLEN(p, USE_IPV6) ((USE_IPV6)? ((p)->ping_datalen + sizeof (struct icmp6_hdr)) : \
				   ((p)->ping_datalen + sizeof (icmphdr_t)))

typedef int (*ping_efp6) (int code, void *closure, struct sockaddr_in6 * dest,
			  struct sockaddr_in6 * from, struct icmp6_hdr * icmp,
			  int datalen);

typedef int (*ping_efp) (int code,
			 void *closure,
			 struct sockaddr_in * dest,
			 struct sockaddr_in * from,
			 struct ip * ip, icmphdr_t * icmp, int datalen);

union event {
  ping_efp6 handler6;
  ping_efp handler;
};

union ping_address {
  struct sockaddr_in ping_sockaddr;
  struct sockaddr_in6 ping_sockaddr6;
};

typedef struct ping_data PING;

struct ping_data
{
  int ping_fd;                 /* Raw socket descriptor */
  int ping_type;               /* Type of packets to send */
  size_t ping_count;           /* Number of packets to send */
  size_t ping_interval;                /* Number of seconds to wait between sending pkts */
  union ping_address ping_dest;        /* whom to ping */
  char *ping_hostname;         /* Printable hostname */
  size_t ping_datalen;         /* Length of data */
  int ping_ident;              /* Our identifier */
  union event ping_event;      /* User-defined handler */
  void *ping_closure;          /* User-defined data */
  
  /* Runtime info */
  int ping_cktab_size;
  char *ping_cktab;
  
  u_char *ping_buffer;         /* I/O buffer */
  union ping_address ping_from;
  long ping_num_xmit;          /* Number of packets transmitted */
  long ping_num_recv;          /* Number of packets received */
  long ping_num_rept;          /* Number of duplicates received */
};

void tvsub (struct timeval *out, struct timeval *in);
double nabs (double a);
double nsqrt (double a, double prec);

size_t ping_cvt_number (const char *optarg, size_t maxval, int allow_zero);

void init_data_buffer (unsigned char *pat, int len);

void decode_pattern (const char *text, int *pattern_len,
		     unsigned char *pattern_data);
int _ping_setbuf (PING * p, bool USE_IPV6);
int ping_set_data (PING *p, void *data, size_t off, size_t len, bool USE_IPV6);
