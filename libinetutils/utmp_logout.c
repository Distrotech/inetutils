/************************************************************************
* Copyright 1995 by Wietse Venema.  All rights reserved. Individual files
* may be covered by other copyrights (as noted in the file itself.)
*
* This material was originally written and compiled by Wietse Venema at
* Eindhoven University of Technology, The Netherlands, in 1990, 1991,
* 1992, 1993, 1994 and 1995.
*
* Redistribution and use in source and binary forms are permitted
* provided that this entire copyright notice is duplicated in all such
* copies.  
*
* This software is provided "as is" and without any expressed or implied
* warranties, including, without limitation, the implied warranties of
* merchantibility and fitness for any particular purpose.
************************************************************************/
/* Author: Wietse Venema <wietse@wzv.win.tue.nl> */
/* light changes for inetutils : Alain Magloire */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/types.h>
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#else
# include <time.h>
#endif
#ifdef HAVE_UTMPX_H
# include <utmpx.h>
#else
# include <utmp.h>
#endif
#ifdef HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

/* utmp_logout - update utmp and wtmp after logout */

utmp_logout(line)
char   *line;
{
#ifdef HAVE_UTMPX_H
	struct utmpx utx;
	struct utmpx *ut;

	strncpy(utx.ut_line, line, sizeof(utx.ut_line));

	if (ut = getutxline(&utx)) {
		ut->ut_type = DEAD_PROCESS;
		ut->ut_exit.e_termination = 0;
		ut->ut_exit.e_exit = 0;
		gettimeofday(&(ut->ut_tv), 0);
		pututxline(ut);
		updwtmpx(WTMPX_FILE, ut);
	}
	endutxent();
#else
	struct utmp utx;
	struct utmp *ut;

	strncpy(utx.ut_line, line, sizeof(utx.ut_line));

	if (ut = getutline(&utx)) {
		ut->ut_type = DEAD_PROCESS;
		ut->ut_exit.e_termination = 0;
		ut->ut_exit.e_exit = 0;
		time(&(utx.ut_time));
		pututline(ut);
		updwtmp(WTMP_FILE, ut);
	}
	endutent();
#endif
}
