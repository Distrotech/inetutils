/*
  Copyright (C) 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
  2008, 2009, 2010 Free Software Foundation, Inc.

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
 * Copyright (c) 1995 Wietse Venema.  All rights reserved.
 *
 * Individual files may be covered by other copyrights (as noted in
 * the file itself.)
 *
 * This material was originally written and compiled by Wietse Venema
 * at Eindhoven University of Technology, The Netherlands, in 1990,
 * 1991, 1992, 1993, 1994 and 1995.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this entire copyright notice is duplicated in all
 * such copies.
 *
 * This software is provided "as is" and without any expressed or
 * implied warranties, including, without limitation, the implied
 * warranties of merchantibility and fitness for any particular
 * purpose.
 */

/* Written by Wietse Venema.  With port to GNU Inetutils done by Alain
   Magloire.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/types.h>

#include <sys/time.h>
#include <time.h>
#ifdef HAVE_UTMPX_H
# ifndef __USE_GNU
#  define __USE_GNU
# endif
# include <utmpx.h>
#else
# include <utmp.h>
#endif
#include <string.h>
#include <unistd.h>

/* utmp_init - update utmp and wtmp before login */

void
utmp_init (char *line, char *user, char *id)
{
#ifdef HAVE_UTMPX_H
  struct utmpx utx;
#else
  struct utmp utx;
#endif
#if defined(HAVE_STRUCT_UTMPX_UT_TV)
  struct timeval tv;
#endif

  memset ((char *) &utx, 0, sizeof (utx));
#if defined(HAVE_STRUCT_UTMP_UT_ID)
  strncpy (utx.ut_id, id, sizeof (utx.ut_id));
#endif
#if defined(HAVE_STRUCT_UTMP_UT_USER) || defined(HAVE_STRUCT_UTMPX_UT_USER)
  strncpy (utx.ut_user, user, sizeof (utx.ut_user));
#else
  strncpy (utx.ut_name, user, sizeof (utx.ut_name));
#endif
  strncpy (utx.ut_line, line, sizeof (utx.ut_line));
#if defined(HAVE_STRUCT_UTMP_UT_PID)
  utx.ut_pid = getpid ();
#endif
#if defined(HAVE_STRUCT_UTMP_UT_TYPE)
  utx.ut_type = LOGIN_PROCESS;
#endif
#if defined(HAVE_STRUCT_UTMPX_UT_TV)
  gettimeofday (&tv, 0);
  utx.ut_tv.tv_sec = tv.tv_sec;
  utx.ut_tv.tv_usec = tv.tv_usec;
#else
  time (&(utx.ut_time));
#endif
#ifdef HAVE_UTMPX_H
  pututxline (&utx);
# ifdef HAVE_UPDWTMPX
  updwtmpx (PATH_WTMPX, &utx);
# endif
  endutxent ();
#else
  pututline (&utx);
# ifdef HAVE_UPDWTMP
  updwtmp (PATH_WTMP, &utx);
# else
  logwtmp (line, user, id);
# endif
  endutent ();
#endif
}

/* utmp_ptsid - generate utmp id for pseudo terminal */

char *
utmp_ptsid (char *line, char *tag)
{
  static char buf[5];

  strncpy (buf, tag, 2);
  strncpy (buf + 2, line + strlen (line) - 2, 2);
  return (buf);
}
