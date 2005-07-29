/* Copyright (C) 1998, 2001, 2002, 2004, 2005 Free Software Foundation, Inc.

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

#include <stdlib.h>
#include <stdio.h>
#include <xalloc.h>

extern unsigned char *data_buffer;
extern size_t data_length;

size_t
ping_cvt_number (const char *optarg, size_t maxval, int allow_zero)
{
  char *p;
  unsigned long int n;

  n = strtoul (optarg, &p, 0);
  if (*p)
    {
      fprintf (stderr, "Invalid value (`%s' near `%s')\n", optarg, p);
      exit (1);
    }
  if (n == 0 && !allow_zero)
    {
      fprintf (stderr, "Option value too small: %s\n", optarg);
      exit (1);
    }
  if (maxval && n > maxval)
    {
      fprintf (stderr, "Option value too big: %s\n", optarg);
      exit (1);
    }
  return n;
}

void
init_data_buffer (u_char *pat, int len)
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
decode_pattern (const char *text, int *pattern_len, unsigned char *pattern_data)
{
  int i, c, off;

  for (i = 0; *text && i < *pattern_len; i++)
    {
      if (sscanf (text, "%2x%n", &c, &off) != 1)
	{
	  fprintf (stderr, "ping: error in pattern near %s\n", text);
	  exit (1);
	}
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
  x1 = a/2;
  do
    {
      x0 = x1;
      x1 = (x0 + a/x0) / 2;
    }
  while (nabs (x1 - x0) > prec);

  return x1;
}

void
show_license (void)
{
  static char license_text[] =
"   This program is free software; you can redistribute it and/or modify\n"
"   it under the terms of the GNU General Public License as published by\n"
"   the Free Software Foundation; either version 2, or (at your option)\n"
"   any later version.\n"
"\n"
"   This program is distributed in the hope that it will be useful,\n"
"   but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"   GNU General Public License for more details.\n"
"\n"
"   You should have received a copy of the GNU General Public License\n"
"   along with this program; if not, write to the Free Software\n"
"   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n";
  printf ("%s", license_text);
}
