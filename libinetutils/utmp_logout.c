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
#if defined(UTMPX) && defined(HAVE_UTMPX_H)
# define __USE_GNU
# include <utmpx.h>
#else
# include <utmp.h>
#endif
#include <string.h>

/* utmp_logout - update utmp and wtmp after logout */

void
utmp_logout (char *line)
{
#ifdef UTMPX
  struct utmpx utx;
  struct utmpx *ut;

  strncpy (utx.ut_line, line, sizeof (utx.ut_line));

  ut = getutxline (&utx);
  if (ut)
    {
      struct timeval tv;

      ut->ut_type = DEAD_PROCESS;
      ut->ut_exit.e_termination = 0;
      ut->ut_exit.e_exit = 0;
      gettimeofday (&tv, 0);
      ut->ut_tv.tv_sec = tv_sec;
      ut->ut_tv.tv_usec = tv_usec;
      pututxline (ut);
      updwtmpx (PATH_WTMPX, ut);
    }
  endutxent ();
#else
  struct utmp utx;
  struct utmp *ut;

  strncpy (utx.ut_line, line, sizeof (utx.ut_line));

  ut = getutline (&utx);
  if (ut)
    {
# ifdef HAVE_STRUCT_UTMP_UT_TV
      struct timeval tv;
# endif

# ifdef HAVE_STRUCT_UTMP_UT_TYPE
      ut->ut_type = DEAD_PROCESS;
# endif
# ifdef HAVE_STRUCT_UTMP_UT_EXIT
      ut->ut_exit.e_termination = 0;
      ut->ut_exit.e_exit = 0;
# endif
# ifdef HAVE_STRUCT_UTMP_UT_TV
      gettimeofday (&tv, 0);
      ut->ut_tv.tv_sec = tv.tv_sec;
      ut->ut_tv.tv_usec = tv.tv_usec;
# else
      time (&(ut->ut_time));
# endif
      pututline (ut);
# ifdef HAVE_UPDWTMP
      ut->ut_name[0] = 0;
      ut->ut_host[0] = 0;
      updwtmp (WTMP_FILE, ut);
# else
      logwtmp (ut->ut_line, "", "");
# endif
    }
  endutent ();
#endif
}
