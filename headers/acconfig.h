/* acconfig.h
   This file is in the public domain.

   Descriptive text for the C preprocessor macros that
   the distributed Autoconf macros can define.
   No software package will use all of them; autoheader copies the ones
   your configure.in uses into your configuration header file templates.

   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  Although this order
   can split up related entries, it makes it easier to check whether
   a given entry is in the file.

   Leave the following blank line there!!  Autoheader needs it.  */


/* Define is you have PATH_RSH. */
#undef TEEHEE 

/* Define if you have the Andrew File System.  */
#undef AFS

/* Define if there is a member named d_ino in the struct describing
   directory headers.  */
#undef D_INO_IN_DIRENT

/* Define to 1 if NLS is requested.  */
#undef ENABLE_NLS

/* Define to rpl_fnmatch if the replacement function should be used.  */
#undef fnmatch

/* Define as rpl_getgroups if getgroups doesn't work right.  */
#undef getgroups

/* Define to gnu_strftime if the replacement function should be used.  */
#undef strftime

/* Define if your system defines TIOCGWINSZ in sys/ioctl.h.  */
#undef GWINSZ_IN_SYS_IOCTL

/* Define as 1 if you have catgets and don't want to use GNU gettext.  */
#undef HAVE_CATGETS

/* Define as 1 if you have gettext and don't want to use GNU gettext.  */
#undef HAVE_GETTEXT

/* Define if your locale.h file contains LC_MESSAGES.  */
#undef HAVE_LC_MESSAGES

/* Define to 1 if you have the obstack functions from GNU libc.  */
#undef HAVE_OBSTACK

/* Define if your struct stat has st_dm_mode.  */
#undef HAVE_ST_DM_MODE

/* Define to 1 if you have the stpcpy function.  */
#undef HAVE_STPCPY

/* Define to `unsigned long' if <sys/types.h> doesn't define.  */
#undef ino_t

/* Define if `struct utimbuf' is declared -- usually in <utime.h>.  */
#undef HAVE_STRUCT_UTIMBUF

/* Define to gnu_mktime if the replacement function should be used.  */
#undef mktime

/* Define one of the following to indicate how a program can
   get a list of mounted filesystems.  */

/* Define if there is no specific function for reading the list of
   mounted filesystems.  fread will be used to read /etc/mnttab.  [SVR2]  */
#undef MOUNTED_FREAD

/* Define if (like SVR2) there is no specific function for reading the
   list of mounted filesystems, and your system has these header files:
   <sys/fstyp.h> and <sys/statfs.h>.  [SVR3]  */
#undef MOUNTED_FREAD_FSTYP

/* Define if there is a function named getfsstat for reading the list
   of mounted filesystems.  [DEC Alpha running OSF/1]  */
#undef MOUNTED_GETFSSTAT

/* Define if there is a function named getmnt for reading the list of
   mounted filesystems.  [Ultrix]  */
#undef MOUNTED_GETMNT

/* Define if there is a function named getmntent for reading the list
   of mounted filesystems, and that function takes a single argument.
   [4.3BSD, SunOS, HP-UX, Dynix, Irix]  */
#undef MOUNTED_GETMNTENT1

/* Define if there is a function named getmntent for reading the list of
   mounted filesystems, and that function takes two arguments.  [SVR4]  */
#undef MOUNTED_GETMNTENT2

/* Define if there is a function named getmntinfo for reading the list
   of mounted filesystems.  [4.4BSD]  */
#undef MOUNTED_GETMNTINFO

/* Define if there is a function named listmntent that can be used to
   list all mounted filesystems. [UNICOS] */
#undef MOUNTED_LISTMNTENT

/* Define if there is a function named mntctl that can be used to read
   the list of mounted filesystems, and there is a system header file
   that declares `struct vmount.'  [AIX]  */
#undef MOUNTED_VMOUNT

/* Define to the name of the distribution.  */
#undef PACKAGE

/* The concatenation of the strings "GNU ", and PACKAGE.  */
#undef GNU_PACKAGE

/* Define to 1 if ANSI function prototypes are usable.  */
#undef PROTOTYPES


/* Define one of the following to indicate how a program can obtain
   filesystems usage information.  */

/*  Define if  statfs takes 3 args.  [DEC Alpha running OSF/1]  */
#undef STAT_STATFS3_OSF1

/* Define if there is no specific function for reading filesystems usage
   information and you have the <sys/filsys.h> header file.  [SVR2]  */
#undef STAT_READ_FILSYS

/* Define if statfs takes 2 args and struct statfs has a field named f_bsize.
   [4.3BSD, SunOS 4, HP-UX, AIX PS/2]  */
#undef STAT_STATFS2_BSIZE

/* Define if statfs takes 2 args and struct statfs has a field named f_fsize.
   [4.4BSD, NetBSD]  */
#undef STAT_STATFS2_FSIZE

/* Define if statfs takes 2 args and the second argument has
   type struct fs_data.  [Ultrix]  */
#undef STAT_STATFS2_FS_DATA

/* Define if statfs takes 4 args.  [SVR3, Dynix, Irix, Dolphin]  */
#undef STAT_STATFS4

/* Define if there is a function named statvfs.  [SVR4]  */
#undef STAT_STATVFS

/* Define if the block counts reported by statfs may be truncated to 2GB
   and the correct values may be stored in the f_spare array.
   [SunOS 4.1.2, 4.1.3, and 4.1.3_U1 are reported to have this problem.
   SunOS 4.1.1 seems not to be affected.]  */
#undef STATFS_TRUNCATES_BLOCK_COUNTS

/* Define to the version of the distribution.  */
#undef VERSION

/* Define to 1 if GNU regex should be used instead of GNU rx.  */
#undef WITH_REGEX

/* Defining this causes gnu libc to act more like BSD.  */
/* FIXME: JB - does this need to be here? #define _BSD_SOURCE */

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

/* Define this this symbol if struct sockaddr_in has a sin_len field.  */
#undef HAVE_SOCKADDR_IN_SIN_LEN

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
/* Define this if struct utmpx has a ut_tv field.  */
#undef HAVE_UTMPX_UT_TV
/* Define this if struct utmp has a ut_exit field.  */
#undef HAVE_UTMP_UT_EXIT

/* Define this if using Kerberos version 4.  */
#undef KRB4

/* Define this if using Kerberos.  */
#undef KERBEROS

/* Define this to be `setpgrp' if on a BSD system that doesn't have setpgid. */
#undef setpgid

/* Define this if __P is defined in <stdlib.h>.  */
#undef HAVE___P

/* Define this if a definition of sys_errlist is available.  */
#undef HAVE_SYS_ERRLIST

/* Define this if sys_errlist is declared in <stdio.h> or <errno.h>.  */
#undef HAVE_SYS_ERRLIST_DECL

/* Define this if errno is declared by <errno.h>.  */
#undef HAVE_ERRNO_DECL

/* Define this if ENVIRON is declared by <unistd.h> or <stdlib.h>.  */
#undef HAVE_ENVIRON_DECL

/* Define this if hstrerror is declared in <netdb.h>.  */
#undef HAVE_HSTRERROR_DECL

/* Define this if H_ERRLIST is declared in <netdb.h>  */
#undef HAVE_H_ERRLIST_DECL

/* Define this if a definition of h_errno is available after including <netdb.h>.  */
#undef HAVE_H_ERRNO

/* Define this if H_ERRNO is declared in <netdb.h>  */
#undef HAVE_H_ERRNO_DECL

/* Define this if struct hostent has a h_addr_list field.  */
#undef HAVE_HOSTENT_H_ADDR_LIST

/* Define this if the system supplies the __PROGNAME variable.  */
#undef HAVE___PROGNAME

/* Define this if the system defines snprintf.  */
#undef HAVE_SNPRINTF

/* Define this if the system defines vsnprintf. */
#undef HAVE_VSNPRINTF

/* Define this if sig_t is declared by including <sys/types.h> & <signal.h> */
#undef HAVE_SIG_T

/* Define this to be `int' if sig_atomic_t isn't declared by including
   <sys/types.h> & <signal.h> */
#undef sig_atomic_t
/* Define this to be `unsigned long' if sigset_t isn't declared by including
   <sys/types.h> & <signal.h> */
#undef sigset_t

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

/* Define this if fclose is declared by including <stdio.h>.  */
#undef HAVE_FCLOSE_DECL

/* Define this if pclose is declared by including <stdio.h>.  */
#undef HAVE_PCLOSE_DECL

/* Define this if getcwd is declared by including <unistd.h>.  */
#undef HAVE_GETCWD_DECL

/* Define this if getlogin is declared by including <unistd.h>.  */
#undef HAVE_GETLOGIN_DECL

/* Define this if getpass is declared by including <unistd.h>.  */
#undef HAVE_GETPASS_DECL

/* Define this if getusershell is declared by including <unistd.h>.  */
#undef HAVE_GETUSERSHELL_DECL

/* Define this if ttyname is declared by including <unistd.h>.  */
#undef HAVE_TTYNAME_DECL

/* Define this if strchr is declared by including <stdlib.h> & <string.h>.  */
#undef HAVE_STRCHR_DECL

/* Define this if strerror is declared by including <stdlib.h> & <string.h>.  */
#undef HAVE_STRERROR_DECL

/* Define this if getgrnam is declared by including <grp.h>.  */
#undef HAVE_GETGRNAM_DECL

/* Define this if htons is declared by including <sys/types.h>,
   <sys/param.h>, & <netinet/in.h>.  */
#undef HAVE_HTONS_DECL

/* Define this if <paths.h> exists.  */
#undef HAVE_PATHS_H

/* If these aren't defined by <unistd.h>, define them here. */
#undef SEEK_SET
#undef SEEK_CUR
#undef SEEK_END
#undef STDIN_FILENO
#undef STDOUT_FILENO
#undef STDERR_FILENO

/* If the fd_set macros (FD_ZERO &c) are defined by including <sys/time.h> (a
   truly bizarre place), define this.  */
#undef HAVE_FD_SET_MACROS_IN_SYS_TIME_H

/* If <syslog.h> declares special internal stuff when SYSLOG_NAMES is
   defined, define this.  */
#undef HAVE_SYSLOG_INTERNAL

/* If EWOULDBLOCK isn't defined by <errno.h>, define it here.  */
#undef EWOULDBLOCK

/* SOLARIS uses STREAM ptys and other funky things */
#undef SOLARIS
#undef HAVE_STREAMSPTY

/* certain system must have an entry in utmpx before login work */
#undef UTMPX

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

#endif /* HAVE___P */

#ifndef HAVE_SIG_T
typedef RETSIGTYPE (*sig_t) ();
#endif

/* Define memory frobbing ops.  memmove is always defined, even if we have to
   do so ourselves, as is memset.  bcopy, memcpy, & bzero are defined in
   terms of those if necessary (since that can always be done with a macro). */

#ifndef HAVE_BCOPY
#define bcopy(f,t,z) memmove(t,f,z)
#endif
#ifndef HAVE_BZERO
#define bzero(x,z) memset(x,0,z)
#endif
#ifndef HAVE_MEMCPY
#define memcpy memmove
#endif

#ifndef HAVE_KILLPG
#define killpg(pid, sig) kill(-(pid), (sig))
#endif

#ifndef HAVE_SETEUID
#ifdef HAVE_SETREUID
#define seteuid(uid) setreuid(-1, (uid))
#else /* !HAVE_SETREUID */
#ifdef HAVE_SETRESUID
#define seteuid(uid) setresuid(-1, (uid), -1)
#endif /* HAVE_SETRESUID */
#endif /* HAVE_SETREUID */
#endif /* ! HAVE_SETEUID */

#ifndef HAVE_SETEGID
#ifdef HAVE_SETREGID
#define setegid(gid) setregid(-1, (gid))
#else /* !HAVE_SETREGID */
#ifdef HAVE_SETRESGID
#define setegid(gid) setresgid(-1, (gid), -1)
#endif /* HAVE_SETRESGID */
#endif /* HAVE_SETREGID */
#endif /* ! HAVE_SETEGID */

#if !defined(HAVE_MEMMOVE) || !defined(HAVE_MEMSET)
/* Make sure size_t is defined */
#include <sys/types.h>
#endif
#ifndef HAVE_MEMMOVE
/* Declare our own silly version.  */
extern void *memmove __P ((void *to, const void *from, size_t sz));
#endif
#ifndef HAVE_MEMSET
/* Declare our own silly version.  */
extern void memset __P ((void *mem, int val, size_t sz));
#endif

/* Define string index ops. */
#ifndef HAVE_STRCHR_DECL
extern char *strchr __P ((char *str, int ch));
extern char *strrchr __P ((char *str, int ch));
#endif
#ifndef HAVE_STRCHR
#define strchr index
#define strrchr rindex
#endif

#ifndef HAVE_VSNPRINTF
#include <sys/types.h>
#if defined(HAVE_STDARG_H) && defined(__STDC__) && __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif
extern int vsnprintf __P ((char *, size_t, const char *, va_list));
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/types.h>
#include <sys/param.h>
#endif
/* Get or fake the disk device blocksize.
   Usually defined by sys/param.h (if at all).  */
#if !defined(DEV_BSIZE) && defined(BSIZE)
#define DEV_BSIZE BSIZE
#endif
#if !defined(DEV_BSIZE) && defined(BBSIZE) /* SGI */
#define DEV_BSIZE BBSIZE
#endif
#ifndef DEV_BSIZE
#define DEV_BSIZE 4096
#endif

/* Extract or fake data from a `struct stat'.
   ST_BLKSIZE: Optimal I/O blocksize for the file, in bytes. */
#ifndef HAVE_ST_BLKSIZE
# define ST_BLKSIZE(statbuf) DEV_BSIZE
#else /* HAVE_ST_BLKSIZE */
/* Some systems, like Sequents, return st_blksize of 0 on pipes. */
# define ST_BLKSIZE(statbuf) ((statbuf).st_blksize > 0 \
                              ? (statbuf).st_blksize : DEV_BSIZE)
#endif /* HAVE_ST_BLKSIZE */

#ifndef HAVE_GETCWD_DECL
extern char *getcwd __P((char *, size_t));
#endif

#ifndef HAVE_GETLOGIN_DECL
extern char *getlogin __P((void));
#endif

#ifndef HAVE_GETPASS_DECL
extern char *getpass __P((const char *));
#endif

#ifndef HAVE_HSTRERROR_DECL
extern const char *hstrerror __P ((int));
#endif

#ifndef HAVE_TTYNAME_DECL
extern char *ttyname __P ((int));
#endif

#ifndef HAVE_STRERROR_DECL
extern const char *strerror __P ((int));
#endif

#ifndef HAVE_GETGRNAM_DECL
extern struct group *getgrnam __P ((const char *));
#endif

/* Defaults for PATH_ variables.  */
#include <confpaths.h>
