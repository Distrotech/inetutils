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
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/signal.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
/*#include <netinet/ip_icmp.h> -- deliberately not including this */
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <icmp.h>
#include "ping.h"

PING *
ping_init(int ident)
{
  int fd;
  struct protoent *proto;
  PING *p;

  /* Initialize raw ICMP socket */
  if (!(proto = getprotobyname("icmp"))) {
    fprintf(stderr, "ping: unknown protocol icmp.\n");
    return NULL;
  }
  if ((fd = socket(AF_INET, SOCK_RAW, proto->p_proto)) < 0) {
    if (errno == EPERM) {
      fprintf(stderr, "ping: ping must run as root\n");
    }
    return NULL;
  }

  /* Allocate PING structure and initialize it to default values */
  if (!(p = malloc(sizeof(*p))))
    {
      close(fd);
      return p;
    }
  
  memset(p, 0, sizeof(*p));

  p->ping_fd = fd;
  p->ping_count = 0;
  p->ping_interval = PING_INTERVAL;
  p->ping_datalen  = PING_DATALEN;
  p->ping_ident = ident;
  p->ping_cktab_size = PING_CKTABSIZE;
  return p;
}

int
ping_set_pattern(PING *p, int len, u_char *pat)
{
  u_char *dp;
  int i;
  
  if (_ping_setbuf(p))
    return -1;
  
  i = 0;
  for (dp = p->ping_outpack + 8 + sizeof(struct timeval);
       dp < p->ping_outpack + p->ping_datalen;
       dp++)
    {
      *dp = pat[i];
      if (i++ >= len)
	i = 0;
    }
  return 0;
}

int
_ping_setbuf(PING *p)
{
  if (!p->ping_outpack)
    {
      int i;
      
      p->ping_outpack = malloc(_PING_OUTBUFLEN(p));
      if (!p->ping_outpack)
	return -1;
      for (i = 8; i < p->ping_datalen; i++)
	p->ping_outpack[i] = i;
    }
  if (!p->ping_inpack)
    {
      p->ping_inpack = malloc(_PING_INBUFLEN(p));
      if (!p->ping_inpack)
	return -1;
    }
  if (!p->ping_cktab)
    {
      p->ping_cktab = malloc(p->ping_cktab_size);
      if (!p->ping_cktab)
	return -1;
      memset(p->ping_cktab, 0, p->ping_cktab_size);
    }
  return 0;
}

int
ping_xmit(PING *p)
{
  register icmphdr_t *icp;
  int i, buflen;

  if (_ping_setbuf(p))
    return -1;
  
  buflen = _PING_OUTBUFLEN(p);
  
  /* Mark sequence number as sent */
  _PING_CLR(p, p->ping_num_xmit % p->ping_cktab_size);
  /* Store timestamp in the packet if the size is sufficient */
  if (_PING_TIMING(p))
    gettimeofday((struct timeval *)&p->ping_outpack[8], NULL);
  /* Encode ICMP header */
  icmp_echo_encode(p->ping_outpack, buflen, p->ping_ident,
		   p->ping_num_xmit++);

  i = sendto(p->ping_fd, (char *)p->ping_outpack, buflen, 0,
	     (struct sockaddr*) &p->ping_dest,
	     sizeof(struct sockaddr_in));
  if (i < 0)
    {
      perror("ping: sendto");
    }
  else if (i != buflen)
    {
      printf("ping: wrote %s %d chars, ret=%d\n", p->ping_hostname, buflen, i);
    }
  return 0;
}

int
ping_recv(PING *p)
{
  int fromlen = sizeof(p->ping_from);
  int n, rc;
  icmphdr_t *icmp;
  struct ip *ip;
  int dupflag;

  if ((n = recvfrom(p->ping_fd,
		    (char *)p->ping_inpack, _PING_INBUFLEN(p), 0,
		    (struct sockaddr *)&p->ping_from, &fromlen)) < 0)
    return -1;

  if ((rc = icmp_echo_decode(p->ping_inpack, n, &ip, &icmp)) < 0)
    {
      /*FIXME: conditional*/
      fprintf(stderr,"packet too short (%d bytes) from %s\n", n,
	      inet_ntoa(p->ping_from.sin_addr));
      return;
    }
  
  if (icmp->icmp_type == ICMP_ECHOREPLY)
    {
      if (icmp->icmp_id != p->ping_ident)
	return;
      if (rc)
	{
	  fprintf(stderr, "checksum mismatch from %s\n",
		  inet_ntoa(p->ping_from.sin_addr));
	}

      p->ping_num_recv++;
      if (_PING_TST(p, icmp->icmp_seq % p->ping_cktab_size))
	{
	  p->ping_num_rept++;
	  p->ping_num_recv--;
	  dupflag = 1;
	}
      else
	{
	  _PING_SET(p, icmp->icmp_seq % p->ping_cktab_size);
	  dupflag = 0;
	}

      if (p->ping_event)
	(*p->ping_event)(dupflag ? PEV_DUPLICATE : PEV_RESPONSE,
			 p->ping_closure,
			 &p->ping_dest,
			 &p->ping_from,
			 p->ping_inpack, n);
    }
  else
    {
      if (p->ping_event)
	(*p->ping_event)(PEV_NOECHO,
			 p->ping_closure,
			 &p->ping_dest,
			 &p->ping_from,
			 p->ping_inpack, n);
    }
  return 0;
}

void
ping_set_event_handler(PING *ping, ping_efp pf, void *closure)
{
  ping->ping_event = pf;
  ping->ping_closure = closure;
}

void
ping_set_count(PING *ping, int count)
{
  ping->ping_count = count;
}

void
ping_set_sockopt(PING *ping, int opt, void *val, int valsize)
{
  setsockopt(ping->ping_fd, SOL_SOCKET, opt, (char*)&val, valsize);
}

void
ping_set_interval(PING *ping, int interval)
{
  ping->ping_interval = interval;
}

void
ping_set_packetsize(PING *ping, int size)
{
  ping->ping_datalen = size;
}

int
ping_set_dest(PING *ping, char *host)
{
  struct sockaddr_in *sin = &ping->ping_dest;
  sin->sin_family = AF_INET;
  if (inet_aton(host, &sin->sin_addr))
    {
      ping->ping_hostname = strdup(host);
    }
  else
    {
      struct hostent *hp = gethostbyname(host);
      if (!hp) 
	return 1;

      sin->sin_family = hp->h_addrtype;
      if (hp->h_length > (int)sizeof(sin->sin_addr)) 
	hp->h_length = sizeof(sin->sin_addr);

      memcpy(&sin->sin_addr, hp->h_addr, hp->h_length);
      ping->ping_hostname = strdup(hp->h_name);
    }
  return 0;
}

