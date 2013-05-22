/*
  Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005,
  2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013 Free Software
  Foundation, Inc.

  This file is part of GNU Inetutils.

  GNU Inetutils is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or (at
  your option) any later version.

  GNU Inetutils is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see `http://www.gnu.org/licenses/'. */

/*
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <config.h>

#ifdef ENCRYPTION
# ifdef KRB4
#  include <sys/param.h>

#  ifdef HAVE_KERBEROSIV_DES_H
#   include <kerberosIV/des.h>
#  endif
#  ifdef HAVE_KERBEROSIV_KRB_H
#   include <kerberosIV/krb.h>
#  endif

#  include <stdlib.h>
#  include <string.h>
#  include <time.h>
#  include <unistd.h>

#  ifndef MIN
#   define MIN(a,b)	(((a)<(b))? (a):(b))
#  endif
#  ifndef roundup
#   define roundup(x, y)   ((((x)+((y)-1))/(y))*(y))
#  endif

static unsigned char des_inbuf[10240], storage[10240], *store_ptr;
static bit_64 *key;
static unsigned char *key_schedule;

/* XXX these should be in a kerberos include file */
int krb_net_read (int, char *, int);

/*
 * NB: These routines will not function properly if NBIO
 *	is set
 */

void
des_clear_key ()
{
  memset (key, 0, sizeof (C_Block));
  memset (key_schedule, 0, sizeof (Key_schedule));
}


int
des_read (fd, buf, len)
     int fd;
     register char *buf;
     int len;
{
  int nreturned = 0;
  long net_len, rd_len;
  int nstored = 0;

  if (nstored >= len)
    {
      memcpy (buf, store_ptr, len);
      store_ptr += len;
      nstored -= len;
      return (len);
    }
  else if (nstored)
    {
      memcpy (buf, store_ptr, nstored);
      nreturned += nstored;
      buf += nstored;
      len -= nstored;
      nstored = 0;
    }

  if (krb_net_read (fd, (char *) &net_len, sizeof (net_len)) !=
      sizeof (net_len))
    {
      /* XXX can't read enough, pipe
         must have closed */
      return (0);
    }
  net_len = ntohl (net_len);
  if (net_len <= 0 || net_len > sizeof (des_inbuf))
    {
      /* preposterous length; assume out-of-sync; only
         recourse is to close connection, so return 0 */
      return (0);
    }
  /* the writer tells us how much real data we are getting, but
     we need to read the pad bytes (8-byte boundary) */
  rd_len = roundup (net_len, 8);
  if (krb_net_read (fd, (char *) des_inbuf, rd_len) != rd_len)
    {
      /* pipe must have closed, return 0 */
      return (0);
    }
  des_pcbc_encrypt (des_inbuf,	/* inbuf */
		    storage,	/* outbuf */
		    net_len,	/* length */
		    key_schedule,	/* DES key */
		    key,	/* IV */
		    DECRYPT);	/* direction */

  if (net_len < 8)
    store_ptr = storage + 8 - net_len;
  else
    store_ptr = storage;

  nstored = net_len;
  if (nstored > len)
    {
      memcpy (buf, store_ptr, len);
      nreturned += len;
      store_ptr += len;
      nstored -= len;
    }
  else
    {
      memcpy (buf, store_ptr, nstored);
      nreturned += nstored;
      nstored = 0;
    }

  return (nreturned);
}

static unsigned char des_outbuf[10240];	/* > longest write */

int
des_write (fd, buf, len)
     int fd;
     char *buf;
     int len;
{
  static int seeded = 0;
  static char garbage_buf[8];
  long net_len, garbage;

  if (len < 8)
    {
      if (!seeded)
	{
	  seeded = 1;
	  srandom ((int) time ((long *) 0));
	}
      garbage = random ();
      /* insert random garbage */
      memcpy (garbage_buf, &garbage, MIN (sizeof (long), 8));
      /* this "right-justifies" the data in the buffer */
      memcpy (garbage_buf + 8 - len, buf, len);
    }
  /* pcbc_encrypt outputs in 8-byte (64 bit) increments */

  des_pcbc_encrypt ((len < 8) ? garbage_buf : buf, des_outbuf, (len < 8) ? 8 : len, key_schedule,	/* DES key */
		    key,	/* IV */
		    ENCRYPT);

  /* tell the other end the real amount, but send an 8-byte padded
     packet */
  net_len = htonl (len);
  write (fd, &net_len, sizeof (net_len));
  write (fd, des_outbuf, roundup (len, 8));
  return (len);
}
# endif	/* KRB4 */
#endif /* CRYPT */
