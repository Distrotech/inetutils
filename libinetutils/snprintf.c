/* A bogus version of snprintf (that ignores the buffer limit)

   Copyright (C) 1996, 1997 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#if defined(HAVE_STDARG_H) && defined(__STDC__)

#include <stdarg.h>

int
snprintf (char *buf, int len, char *fmt, ...)
{
  int rv;
  va_list ap;

  va_start (ap, fmt);
  rv = vsprintf (buf, fmt, ap);
  va_end (ap);

  return rv;
}

#else  /* ! (HAVE_STDARG_H && __STDC__) */

#include <varargs.h>

int
snprintf (va_alist) va_dcl
{
  int rv;
  va_list ap;
  char *buf, *fmt;

  va_start (ap);

  buf = va_arg (ap, char *);
  va_arg (ap, int);			/* skip the length */
  fmt = va_arg (ap, char *);
  rv = vsprintf (buf, fmt, ap);

  va_end (ap);

  return rv;
}

#endif /* HAVE_STDARG_H */

int
vsnprintf (buf, len, fmt, ap)
  char *buf;
  int len;
  const char *fmt;
  va_list ap;
{
  return vsprintf (buf, fmt, ap);
}
