/* Copyright (C) 1998 Free Software Foundation, Inc.

   This file is part of GNU Inetutils.

   GNU Inetutils is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GNU Inetutils is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GNU Inetutils; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA. */

typedef struct ping_data PING;
typedef int (*ping_efp) __P((int code,
			     void *closure,
			     struct sockaddr_in *dest, 
			     struct sockaddr_in *from,
			     u_char *data,
			     int datalen));

struct ping_data
{
  int    ping_fd;        /* Raw socket descriptor */
  int    ping_count;     /* Number of packets to send */
  int    ping_interval;  /* Number of seconds to wait between sending pkts */
  struct sockaddr_in ping_dest; /* whom to ping */
  char   *ping_hostname;     /* Printable hostname */
  int    ping_datalen;   /* Length of data */
  u_char *ping_outpack;  /* Packet to send */
  int    ping_ident;     /* Our identifier */
                          
  ping_efp ping_event;   /* User-defined handler */
  void *ping_closure;                /* User-defined data */

  /* Runtime info */
  int ping_cktab_size;
  char   *ping_cktab;
  
  u_char *ping_inpack;   /* Received packet */
  struct sockaddr_in ping_from;
  int    ping_num_xmit;  /* Number of packets transmitted */
  int    ping_num_recv;  /* Number of packets received */
  int    ping_num_rept;  /* Number of duplicates received */
};

#define PEV_RESPONSE 0
#define PEV_DUPLICATE 1
#define PEV_NOECHO  2

#define PING_INTERVAL 1
#define	PING_CKTABSIZE 128

#define	PING_HEADER_LEN	sizeof(struct timeval)
#define	PING_DATALEN	(64 - PING_HEADER_LEN)	/* default data length */

#define _PING_OUTBUFLEN(p) ((p)->ping_datalen + PING_HEADER_LEN)
#define _PING_INBUFLEN(p) ((p)->ping_datalen + MAXIPLEN + MAXICMPLEN)
#define _PING_TIMING(p) ((p)->ping_datalen >= PING_HEADER_LEN)

#define _C_BIT(p,bit)    (p)->ping_cktab[(bit)>>3]  /* byte in ck array */
#define _C_MASK(bit)     (1 << ((bit) & 0x07))

#define _PING_SET(p,bit) (_C_BIT(p,bit) |= _C_MASK(bit))
#define _PING_CLR(p,bit) (_C_BIT(p,bit) &= (~_C_MASK(bit)))
#define _PING_TST(p,bit) (_C_BIT(p,bit) & _C_MASK(bit))

PING *ping_init(int ident);
void ping_set_count(PING *ping, int count);
void ping_set_sockopt(PING *ping, int opt, void *val, int valsize);
void ping_set_interval(PING *ping, int interval);
void ping_set_packetsize(PING *ping, int size);
int ping_set_dest(PING *ping, char *host);
int ping_set_pattern(PING *p, int len, u_char *pat);
void ping_set_event_handler(PING *ping, ping_efp fp, void *closure);
