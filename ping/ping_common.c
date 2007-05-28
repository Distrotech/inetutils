/* Copyright (C) 1998, 2001, 2002, 2004, 2005, 2007
   Free Software Foundation, Inc.

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
   along with GNU Inetutils; see the file COPYING.  If not, write
   to the Free Software Foundation, Inc., 51 Franklin Street,
   Fifth Floor, Boston, MA 02110-1301 USA. */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <xalloc.h>

#include "ping_common.h"

extern unsigned char *data_buffer;
extern size_t data_length;

size_t
ping_cvt_number (const char *optarg, size_t maxval, int allow_zero)
{
  char *p;
  unsigned long int n;

  n = strtoul (optarg, &p, 0);
  if (*p)
    error (EXIT_FAILURE, 0, "invalid value (`%s' near `%s')", optarg, p);

  if (n == 0 && !allow_zero)
    error (EXIT_FAILURE, 0, "option value too small: %s", optarg);

  if (maxval && n > maxval)
    error (EXIT_FAILURE, 0, "option value too big: %s", optarg);

  return n;
}

void
init_data_buffer (u_char * pat, int len)
{
  int i = 0;
  u_char *p;

  if (data_length == 0)
    return;

  data_buffer = (u_char *) xmalloc (data_length);

  if (pat)
    {
      for (p = data_buffer; p < data_buffer + data_length; p++)
	{
	  *p = pat[i];
	  if (i++ >= len)
	    i = 0;
	}
    }
  else
    {
      for (i = 0; i < data_length; i++)
	data_buffer[i] = i;
    }
}

void
decode_pattern (const char *text, int *pattern_len,
		unsigned char *pattern_data)
{
  int i, c, off;

  for (i = 0; *text && i < *pattern_len; i++)
    {
      if (sscanf (text, "%2x%n", &c, &off) != 1)
        error (EXIT_FAILURE, 0, "error in pattern near %s", text);

      text += off;
    }
  *pattern_len = i;
}


/*
 * tvsub --
 *	Subtract 2 timeval structs:  out = out - in.  Out is assumed to
 * be >= in.
 */
void
tvsub (struct timeval *out, struct timeval *in)
{
  if ((out->tv_usec -= in->tv_usec) < 0)
    {
      --out->tv_sec;
      out->tv_usec += 1000000;
    }
  out->tv_sec -= in->tv_sec;
}

double
nabs (double a)
{
  return (a < 0) ? -a : a;
}

double
nsqrt (double a, double prec)
{
  double x0, x1;

  if (a < 0)
    return 0;
  if (a < prec)
    return 0;
  x1 = a / 2;
  do
    {
      x0 = x1;
      x1 = (x0 + a / x0) / 2;
    }
  while (nabs (x1 - x0) > prec);

  return x1;
}
