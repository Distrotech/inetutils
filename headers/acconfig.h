/* inetutils-specific things to put in config.h.in

  Copyright (C) 1996 Free Software Foundation, Inc.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2, or (at your option)
  any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Defining this causes gnu libc to act more like BSD.  */
#define _BSD_SOURCE

@TOP@

/* Define this symbol if authentication is enabled.  */
#undef AUTHENTICATION

/* Define this symbol if connection encryption is enabled.  */
#undef ENCRYPTION

/* Define this symbol if connection DES encryption is enabled.  */
#undef DES_ENCRYPTION

/* Define this symbol if `struct osockaddr' is defined by <osockaddr.h>.  */
#undef HAVE_OSOCKADDR_H

/* Define this symbol if `struct lastlog' is defined in <utmp.h>.  */
#undef HAVE_STRUCT_LASTLOG

/* Define this symbol if time fields in struct stat are of type `struct
   timespec', and called `st_mtimespec' &c.  */
#undef HAVE_STAT_ST_MTIMESPEC

/* Define this symbol if in addition to the normal time fields in struct stat
   (st_mtime &c), there are additional fields `st_mtime_usec' &c.  */
#undef HAVE_STAT_ST_MTIME_USEC

/* Define this if struct utmp has a ut_type field.  */
#undef HAVE_UTMP_UT_TYPE
/* Define this if struct utmp has a ut_host field.  */
#undef HAVE_UTMP_UT_HOST
/* Define this if struct utmp has a ut_tv field.  */
#undef HAVE_UTMP_UT_TV

/* Define this if using Kerberos version 4.  */
#undef KRB4

/* Define this to be `setpgrp' if on a BSD system that doesn't have setpgid. */
#undef setpgid

/* Define this if __P is defined in <stdlib.h>.  */
#undef HAVE___P

/* Define this if SYS_ERRLIST is declared in <stdio.h> or <errno.h>.  */
#undef HAVE_SYS_ERRLIST_DECL

/* Define this if ERRNO is declared by <errno.h>.  */
#undef HAVE_ERRNO_DECL

/* Define this if a definition of hstrerror is available.  */
#undef HAVE_HSTRERROR

/* Define this if a definition of hstrerror is declared in <netdb.h>.  */
#undef HAVE_HSTRERROR_DECL

/* Define this if H_ERRLIST is declared in <netdb.h>  */
#undef HAVE_H_ERRLIST_DECL

/* Define this if the system supplies the __PROGNAME variable.  */
#undef HAVE___PROGNAME

/* Define this if the system defines snprintf.  */
#undef HAVE_SNPRINTF

/* Define this if sig_t is declared by including <sys/types.h> & <signal.h> */
#undef HAVE_SIG_T

/* Define this if weak references of any sort are supported.  */
#undef HAVE_WEAK_REFS
/* Define this if gcc-style weak references work: ... __attribute__ ((weak)) */
#undef HAVE_ATTR_WEAK_REFS
/* Define this if #pragma weak references work: #pragma weak foo */
#undef HAVE_PRAGMA_WEAK_REFS
/* Define this if gnu-as weak references work: asm (".weak foo") */
#undef HAVE_ASM_WEAK_REFS

/* Define this if crypt is declared by including <unistd.h>.  */
#undef HAVE_CRYPT_DECL

/* Define this if <paths.h> exists.  */
#undef HAVE_PATHS_H

/* If these aren't defined by <unistd.h>, define them here. */
#undef SEEK_SET
#undef SEEK_CUR
#undef SEEK_END

/* If the fd_set macros (FD_ZERO &c) are defined by including <sys/time.h> (a
   truly bizarre place), define this.  */
#undef HAVE_FD_SET_MACROS_IN_SYS_TIME_H

/* If EWOULDBLOCK isn't defined by <errno.h>, define it here.  */
#undef EWOULDBLOCK

@BOTTOM@

#ifdef HAVE___P
/* The system defines __P; we tested for it in <sys/cdefs.h>, so include that
   if we can.  */
#ifdef HAVE_SYS_CDEFS_H
#include <sys/cdefs.h>
#endif

#else /* !HAVE___P */
/* If the system includes don't seem to define __P, do it here instead.  */

#if defined (__GNUC__) || (defined (__STDC__) && __STDC__) || defined (__cplusplus)
#define	__P(args)	args	/* Use prototypes.  */
#else
#define	__P(args)	()	/* No prototypes.  */
#endif

#ifndef HAVE_SIG_T
typedef RETSIGTYPE (*sig_t) ();
#endif

#endif /* HAVE___P */

/* Defaults for PATH_ variables.  */
#include <confpaths.h>
