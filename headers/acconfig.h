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
/* Lines from here until @BOTTOM@ are processed by autoheader.  */

/* Define this symbol if authentication is enabled.  */
#undef AUTHENTICATION

/* Define this symbol if connection encryption is enabled.  */
#undef ENCRYPTION

/* Define this symbol if connection DES encryption is enabled.  */
#undef DES_ENCRYPTION

/* Define this symbol if `struct osockaddr' is undefd in <sys/socket.h>.  */
#undef HAVE_OSOCKADDR

/* Define this symbol if time fields in struct stat are of type `struct
   timespec', and called `st_mtimespec' &c.  */
#undef HAVE_ST_TIMESPEC

/* Define this symbol if in addition to the normal time fields in struct stat
   (st_mtime &c), there are additional fields `st_mtime_usec' &c.  */
#undef HAVE_ST_TIME_USEC

/* Define this if using Kerberos version 4.  */
#undef KRB4

/* Define this to be `setpgrp' if on a BSD system that doesn't have setpgid. */
#undef setpgid

/* Define this if __P is defined in <stdlib.h>.  */
#undef HAVE___P

@BOTTOM@

/* If the system includes don't seem to define __P, do it here instead.  */
#ifndef HAVE___P
#if defined (__GNUC__) || (defined (__STDC__) && __STDC__) || defined (__cplusplus)
#define	__P(args)	args	/* Use prototypes.  */
#else
#define	__P(args)	()	/* No prototypes.  */
#endif
#endif /* !HAVE___P */
