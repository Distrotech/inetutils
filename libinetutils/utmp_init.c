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
/* light changes where done to accomodate libinetutils: Alain Magloire */

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

/* utmp_init - update utmp and wtmp before login */

utmp_init(line, user, id)
char   *line;
char   *user;
char   *id;
{
#ifdef HAVE_UTMPX_H
    struct utmpx utx;
#else
    struct utmp utx;
#endif

    memset((char *) &utx, 0, sizeof(utx));
    strncpy(utx.ut_id, id, sizeof(utx.ut_id));
    strncpy(utx.ut_user, user, sizeof(utx.ut_user));
    strncpy(utx.ut_line, line, sizeof(utx.ut_line));
    utx.ut_pid = getpid();
    utx.ut_type = LOGIN_PROCESS;
#if defined(HAVE_UTMPX_UT_TV)
    gettimeofday(&(utx.ut_tv), 0);
#else
    time(&(utx.ut_time));
#endif
#ifdef HAVE_UTMPX_H
    pututxline(&utx);
# ifdef HAVE_UPDWTMPX
    updwtmpx(PATH_WTMPX, &utx);
# endif
    endutxent();
#else
    pututline(&utx);
# ifdef HAVE_UPDWTMP
    updwtmp(PATH_WTMP, &utx);
# else
    logwtmp(line, user, id);
# endif
    endutent();
#endif
}

/* utmp_ptsid - generate utmp id for pseudo terminal */

char   *utmp_ptsid(line, tag)
char   *line;
char   *tag;
{
    static char buf[5];

    strncpy(buf, tag, 2);
    strncpy(buf + 2, line + strlen(line) - 2, 2);
    return (buf);
}
