# Configuration for inetutils
#
# Copyright (C) 1995, 1996, 1997, 1998, 2000, 2001 Free Software Foundation, Inc.
#
# Written by Miles Bader <miles@gnu.ai.mit.edu>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

AC_INIT
AC_CONFIG_SRCDIR([inetd/inetd.c])

# Versions of autoconf before 2.12 had bugs in autoheader that screwed up
# the production of headers/config.h.in.
AC_PREREQ(2.52)

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([inetutils], [1.3.2e], nodef)

# Generate <config.h>; note that we use a `header' subdirectory, which
# contains all include files distributed with inetutils, and an `include'
# directory, which holds those actually needed for compilation.
#
# If config.h.in needs to be regenerated (because of changes in configure.in
# or headers/acconfig.h, use the command `autoheader -l headers'.
#
AM_CONFIG_HEADER(config.h)

# config.h turns this on, so make sure it's on for our tests too
#CPPFLAGS=-D_BSD_SOURCE

# Include files that we link into our own include directory from headers.
# Initialize it with the things we always want to use.
INCLUDES="crypt.h arpa/telnet.h arpa/ftp.h arpa/tftp.h protocols/talkd.h
	tftpsubs.h"

# Files that we link from somewhere other than headers to includes .  The
# number of entries in LINK_SRCS should match that in LINK_DSTS.
LINK_SRCS=''
LINK_DSTS=''

# By default we compile both servers and clients, but see what the user wants
AC_ARG_ENABLE(servers, [  --disable-servers       don't compile servers],
	      , enable_servers=yes)
AC_ARG_ENABLE(clients, [  --disable-clients       don't compile clients],
	      , enable_clients=yes)
# Check if they want support for internal ls.
AC_ARG_ENABLE(libls,   [  --disable-libls         disable support for libls ],
              , enable_libls=yes)


IU_ENABLE_SERVER(ftpd)
IU_ENABLE_SERVER(inetd)
IU_ENABLE_SERVER(rexecd)
IU_ENABLE_SERVER(rlogind)
IU_ENABLE_SERVER(rshd)
IU_ENABLE_SERVER(syslogd)
IU_ENABLE_SERVER(talkd)
IU_ENABLE_SERVER(telnetd)
IU_ENABLE_SERVER(tftpd)
IU_ENABLE_SERVER(uucpd)

IU_ENABLE_CLIENT(ftp)
IU_ENABLE_CLIENT(ping)
IU_ENABLE_CLIENT(rcp)
IU_ENABLE_CLIENT(rlogin)
IU_ENABLE_CLIENT(rsh)
IU_ENABLE_CLIENT(logger)
IU_ENABLE_CLIENT(talk)
IU_ENABLE_CLIENT(telnet)
IU_ENABLE_CLIENT(tftp)
IU_ENABLE_CLIENT(whois)
IU_ENABLE_CLIENT(ifconfig)

# Automake will generate lines in each Makefile.in for each variable that is
# specified with AC_SUBST here.  Originally, I tried putting AC_SUBST in
# IU_ENABLE_FOO, but then I discovered that doing so causes automake to not
# recognize the variables.  I later realized that with the way I wrote the
# each Makefile.am, automake doesn't need to recognize the variables, but it's
# not worth the trouble to change it back.  --devnull
AC_SUBST(BUILD_FTPD)
AC_SUBST(BUILD_INETD)
AC_SUBST(BUILD_REXECD)
AC_SUBST(BUILD_RLOGIND)
AC_SUBST(BUILD_RSHD)
AC_SUBST(BUILD_SYSLOGD)
AC_SUBST(BUILD_TALKD)
AC_SUBST(BUILD_TELNETD)
AC_SUBST(BUILD_TFTPD)
AC_SUBST(BUILD_UUCPD)

AC_SUBST(BUILD_FTP)
AC_SUBST(BUILD_PING)
AC_SUBST(BUILD_RCP)
AC_SUBST(BUILD_RLOGIN)
AC_SUBST(BUILD_RSH)
AC_SUBST(BUILD_LOGGER)
AC_SUBST(BUILD_TALK)
AC_SUBST(BUILD_TELNET)
AC_SUBST(BUILD_TFTP)
AC_SUBST(BUILD_WHOIS)
AC_SUBST(BUILD_IFCONFIG)


# Encryption and authentication is turned off by default
#AC_ARG_ENABLE(encryption, [  --enable-encryption     enable encryption],
#	      , enable_encryption=no)
#AC_ARG_ENABLE(authentication,
#	      [  --enable-authentication enable connection authentication],
#	      , enable_authentication=no)

AC_PROG_MAKE_SET

AC_PROG_CC
AC_PROG_CPP
AC_AIX
AC_MINIX
AC_ISC_POSIX

AC_PROG_INSTALL
AC_PROG_YACC
AC_CHECK_TOOL(AR, ar)
AC_CHECK_TOOL(RANLIB, ranlib, :)
AC_PATH_PROG(RM, rm, rm)

# the code is ANSI C
AM_C_PROTOTYPES
AC_HEADER_STDC
AC_C_CONST([])
AC_C_CONST


AC_CHECK_HEADERS(arpa/nameser.h des.h errno.h fcntl.h features.h glob.h \
                 krb.h memory.h malloc.h netinet/in_systm.h netinet/ip.h \
                 netinet/ip_icmp.h netinet/ip_var.h \
                 security/pam_appl.h \
                 shadow.h stdarg.h stdlib.h string.h stropts.h sys/tty.h \
                 sys/utsname.h sys/ptyvar.h sys/msgbuf.h sys/filio.h \
                 sys/ioctl_compat.h sys/cdefs.h sys/stream.h sys/mkdev.h \
		 sys/sockio.h sys/sysmacros.h sys/param.h sys/file.h \
		 sys/proc.h sys/select.h sys/time.h sys/wait.h sys/resource.h \
		 stropts.h tcpd.h termio.h termios.h utmp.h utmpx.h unistd.h \
                 vis.h)
AC_CHECK_HEADER(poll.h,
                [],
                [INCLUDES="$INCLUDES poll.h"]) 
AC_HEADER_TIME

AC_CHECK_LIB(nsl, inet_ntoa)
AC_CHECK_LIB(socket, getpeername)
AC_CHECK_LIB(resolv, gethostname)
AC_CHECK_LIB(bsd, revoke)

AC_CHECK_FUNCS(bcopy bcmp bzero cfsetspeed cgetent dirfd fchdir flock \
               fpathconf ftruncate \
	       getcwd getmsg getspnam initgroups initsetproctitle killpg \
               mkstemp ptsname \
               setegid seteuid setpgid \
               setsid setregid setreuid setresgid setresuid setutent_r \
               sigaction sigvec strchr setproctitle tcgetattr tzset utimes \
               utime uname \
               updwtmp updwtmpx vhangup wait3 wait4)

# Functions that we will define (in libinetutils) if necessary.
AC_REPLACE_FUNCS(getpass getusershell herror memcmp memcpy memmove memset \
	         setenv strdup strcasecmp strsignal waitpid poll)
IU_INCLUDED_REGEX(libinetutils/regex.c)
AC_CHECK_FUNC(_obstack_free, ,
  INCLUDES="$INCLUDES obstack.h"
  LIBOBJS="$LIBOBJS obstack.o")


# Use (posix) `setpgid' if it's around, otherwise assume a BSD setpgrp
AC_CHECK_FUNC(setpgid, , AC_DEFINE(setpgid, setpgrp))

# Some systems don't declare common functions (especially if they return int),
# at least in the expected header file.  Check.
IU_CHECK_DECL(fclose, [#include <stdio.h>])
IU_CHECK_DECL(pclose, [#include <stdio.h>])
IU_CHECK_DECL(getcwd, [#include <unistd.h>])
IU_CHECK_DECL(getlogin, [#include <unistd.h>])
IU_CHECK_DECL(getpass, [#include <unistd.h>])
IU_CHECK_DECL(getusershell, [#include <unistd.h>])
IU_CHECK_DECL(ttyname, [#include <unistd.h>])
IU_CHECK_DECL(strerror, [#include <string.h>])
IU_CHECK_DECL(strchr, [#include <string.h>])
IU_CHECK_DECL(getgrnam, [#include <grp.h>])
IU_CHECK_DECL(initgroups, [#include <grp.h>])

IU_CHECK_DECL(htons,
  [ #ifdef HAVE_SYS_TYPES_H
    # include <sys/types.h>
    #endif
    #ifdef HAVE_SYS_PARAM_H
    # include <sys/param.h>
    #endif
    #include <netinet/in.h> ])

if test "$ac_cv_func_setenv" = no; then
  # Our home-grown version of setenv needs to use ENVIRON
  IU_CHECK_DECL(environ,
    [ #ifdef HAVE_UNISTD_H
      # include <unistd.h>
      #endif
      #ifdef HAVE_STDLIB_H
      # include <stdlib.h>
      #endif ])
fi

# EWOULDBLOCK is more or less the BSD version of posix EAGAIN.
IU_CHECK_MACRO(EWOULDBLOCK, [#include <errno.h>], ,
  :, AC_DEFINE(EWOULDBLOCK, EAGAIN))
IU_CHECK_DECL(errno, [#include <errno.h>])

# __P is the standard name for the ansi-or-not-ansi arg-decl macro.
# We define our own version in <config.h> if the system doesn't supply one.
IU_CHECK_MACRO(__P,
  [ #ifdef HAVE_SYS_CDEFS_H
    #include <sys/cdefs.h>
    #endif ])

# If revoke hasn't been defined yet, just define a dummy version
AC_REPLACE_FUNCS(revoke)


AC_HEADER_DIRENT
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_ALLOCA
AC_FUNC_SETVBUF_REVERSED
AC_FUNC_FORK([])
AC_FUNC_STRCOLL

# See whether <syslog.h> will declare special internal stuff if we define
# SYSLOG_NAMES before including it.  We check for various syslog internal macros,
# the typedef `CODE', and the variable `prioritynames'.
define([iu_syslog_includes],
  [[ #ifdef HAVE_STDLIB_H
     #include <stdlib.h>
     #endif
     #define SYSLOG_NAMES
     #include <syslog.h> ]])
IU_CHECK_MACRO(syslog internal macros,
  iu_syslog_includes,
  [LOG_MAKEPRI LOG_PRI LOG_FAC],
  IU_CHECK_TYPE(CODE,
    iu_syslog_includes,
    IU_CHECK_DECL(prioritynames,
      iu_syslog_includes,
      AC_DEFINE(HAVE_SYSLOG_INTERNAL),
      INCLUDES="$INCLUDES syslog-int.h"),
    INCLUDES="$INCLUDES syslog-int.h"),
  INCLUDES="$INCLUDES syslog-int.h")
undefine([iu_syslog_includes])

# libutil is a 4.4BSD library that defines several functions handy for
# inetutil daemons, doing such things as utmp frobbing, and pty setup.
AC_CHECK_LIB(util, logout, LIBUTIL=-lutil)
AC_SUBST(LIBUTIL)
# Look for the functions typically defined by libutil (which of course may
# not actually be in libutil), and and use our own versions if necesary.
_SAVE_LIBS="$LIBS"
LIBS="$LIBUTIL $LIBS"
AC_REPLACE_FUNCS(login logout logwtmp openpty forkpty ruserok iruserok login_tty)
LIBS="$_SAVE_LIBS"

# See if `weak refs' are possible; these make it possible (with shared
# libraries) to check for the existance of a standard function at run-time
# instead of compile time, which is very handy for distributing binary
# version programs that automatically adapt -- in inetutils case, to whether
# or not crypt is available.
IU_CHECK_WEAK_REFS

# See if there's a separate libcrypt (many systems put crypt there)
AC_CHECK_LIB(crypt, crypt, LIBCRYPT=-lcrypt)
AC_SUBST(LIBCRYPT)

# Check if they want support for Wrap.  Certain daemons like
# ftpd have support for it.
AC_ARG_WITH(wrap, [  --with-wrap    add tcp wrapper support])
if test "$with_wrap" = yes; then
# See if we have libwrap.a
# we can not use AC_CHECK_LIB reliably since for the wrap lib
# you have to define some global variables
#AC_CHECK_LIB(wrap, hosts_ctl, LIBWRAP=-lwrap)
  AC_MSG_CHECKING(hosts_ctl in -lwrap);
  save_LIBS=$LIBS
  LIBS="$save_LIBS -lwrap"
  AC_TRY_LINK([int allow_severity = 1; int deny_severity = 1;],
              hosts_ctl(), eval "ac_cv_lib_wrap_hosts_ctl=yes",
              eval "ac_cv_lib_wrap_hosts_ctl=no")
  LIBS=$save_LIBS
  if test "$ac_cv_lib_wrap_hosts_ctl" = yes ; then
    AC_DEFINE(WITH_WRAP)
    LIBWRAP=-lwrap
  fi
  AC_MSG_RESULT($ac_cv_lib_wrap_hosts_ctl);
fi
AC_SUBST(LIBWRAP)

# Check if they want support for PAM.  Certain daemons like
# ftpd have support for it.
AC_ARG_WITH(pam, [  --with-pam    add support for PAM])
# See if we have libpam.a this is for ftpd.
if test "$with_pam" = yes ; then
  AC_CHECK_LIB(dl, dlopen)
  AC_CHECK_LIB(pam, pam_authenticate, LIBPAM=-lpam)
  if test "$ac_cv_lib_pam_pam_authenticate" = yes ; then
     AC_DEFINE(WITH_PAM)
  fi
fi
AC_SUBST(LIBPAM)

# Look for the crypt function itself (in libcrypt if possible)
_SAVE_LIBS="$LIBS"
LIBS="$LIBCRYPT $LIBS"
AC_CHECK_FUNCS(crypt)
LIBS="$_SAVE_LIBS"
# ... and see if it's declared anywhere
IU_CHECK_DECL(crypt, [#include <unistd.h>])

# Look for the posix SEEK_ macros (for lseek), and if not found, try the
# similar berkeley L_ macros; if neither can be found, use the classic unix
# values.
IU_CHECK_MACRO(SEEK_ macros,
  [#include <unistd.h>], SEEK_SET SEEK_CUR SEEK_END,
  :,
  IU_CHECK_MACRO(L_ seek macros,
    [#include <unistd.h>], L_SET L_INCR L_XTND,
    AC_DEFINE(SEEK_SET, L_SET)
    AC_DEFINE(SEEK_CUR, L_INCR)
    AC_DEFINE(SEEK_END, L_XTND),
    AC_DEFINE(SEEK_SET, 0)
    AC_DEFINE(SEEK_CUR, 1)
    AC_DEFINE(SEEK_END, 2)))

# Look for the posix _FILENO macros; if not found, use the classic unix values.
IU_CHECK_MACRO(_FILENO macros,
  [#include <unistd.h>], STDIN_FILENO STDOUT_FILENO STDERR_FILENO,
  :,
  AC_DEFINE(STDIN_FILENO, 0)
  AC_DEFINE(STDOUT_FILENO, 1)
  AC_DEFINE(STDERR_FILENO, 2))

# See where to find fd_set (bit-vectors for select) manipulation macros.
IU_CHECK_MACRO(fd_set macros, [#include <sys/types.h>],
  FD_ZERO FD_CLR FD_SET FD_ISSET)
if test "$inetutils_cv_macro_fd_set_macros" = no; then
  IU_CHECK_MACRO(fd_set macros in <sys/time.h>, [#include <sys/time.h>],
    FD_ZERO FD_CLR FD_SET FD_ISSET)
fi

# See if structure stat has a st_blksize member.
AC_DIAGNOSE([obsolete],[AC_STRUCT_ST_BLKSIZE:
        your code should no longer depend upon `HAVE_ST_BLKSIZE', but
        `HAVE_STRUCT_STAT_ST_BLKSIZE'.  Remove this warning and
        the `AC_DEFINE' when you adjust the code.])
AC_CHECK_MEMBERS([struct stat.st_blksize],[AC_DEFINE(HAVE_ST_BLKSIZE, 1,
                            [Define if your `struct stat' has
                             `st_blksize'.  Deprecated, use
                             `HAVE_STRUCT_STAT_ST_BLKSIZE' instead.])])


# See if stat includes sub-second time information (the st_?timespec form is
# from bsd, and the st_?time_usec form is used by the hurd; the latter will
# eventually be replaced by the former).
IU_CHECK_STRUCT_FIELD(stat, st_mtimespec,
  [ #include <sys/types.h>
    #include <sys/stat.h> ],
  :,
  IU_CHECK_STRUCT_FIELD(stat, st_mtime_usec,
    [ #include <sys/types.h>
      #include <sys/stat.h>]))

# See if a termcap library is available (under one of several names)
IU_LIB_TERMCAP
# If not, add a bogus version of tgetent to libinetutils, as that's the only
# function actually used (to check terminal type validity), modulo any
# dependencies from curses.
if test -z "$LIBTERMCAP"; then
  # Some packages include termcap just ;
  # for those cases, we really don't need termcap.
  LIBOBJS="$LIBOBJS stub_tgetent.o"
fi

# Can we use libls? but we must have fchdir()
if test "$enable_libls" = yes  -a "$ac_cv_func_fchdir" = yes ; then
  LIBLS="../libls/libls.a"
  BUILD_LIBLS="libls.a"
  AC_DEFINE(WITH_LIBLS)
else
  LIBLS=''
  BUILD_LIBLS=''
fi
AC_SUBST(LIBLS)
AC_SUBST(BUILD_LIBLS)

# Only talk uses curses, so only check for it if we want that
if test "$enable_talk" != no; then
  IU_LIB_CURSES
fi
if test "$enable_talk" != no -a "$LIBCURSES"; then
  enable_talk=yes
else
  if test "$enable_talk" = yes; then
    AC_MSG_WARN([curses is not available, so not building talk])
  fi
  enable_talk=no
  BUILD_TALK=''
fi

# We may need our own definition for struct osockaddr (equivalent to the
# old 4.3BSD struct sockaddr, and used in grotty bsd talk protocol)
IU_CHECK_TYPE(struct osockaddr,
    [ #include <sys/types.h>
      #include <sys/socket.h> ],
    :, AC_DEFINE(HAVE_OSOCKADDR_H) INCLUDES="$INCLUDES osockaddr.h")
IU_CHECK_STRUCT_FIELD(sockaddr_in, sin_len,
    [ #include <sys/types.h>
      #include <netinet/in.h> ])

# Look for a system `glob' function, and use our own if we can't find one (or
# if the system one is missing features we need).
sys_glob=no
if test "$ac_cv_header_glob_h" = yes; then
  AC_CHECK_LIB(glob, glob, LIBGLOB=-lglob)
  _SAVE_LIBS="$LIBS"
  LIBS="$LIBGLOB $LIBS"
  AC_CHECK_FUNC(glob,
    IU_CHECK_MACRO(ok system glob,
      [#include <glob.h>], GLOB_BRACE GLOB_TILDE,
      sys_glob=yes))
  LIBS="$_SAVE_LIBS"
fi
if test "$sys_glob" != yes; then
  BUILD_LIBGLOB=libglob.a
  LINK_SRCS="$LINK_SRCS glob/glob.h"
  LINK_DSTS="$LINK_DSTS include/glob.h"
  LINK_SRCS="$LINK_SRCS glob/fnmatch.h"
  LINK_DSTS="$LINK_DSTS include/fnmatch.h"
  LIBGLOB="-L../glob -lglob"
else
  BUILD_LIBGLOB=''
fi
AC_SUBST(LIBGLOB)
AC_SUBST(BUILD_LIBGLOB)

if test "$enable_encryption" = yes -o "$enable_authentication" = yes; then
  # Look for Kerberos and DES libraries
  AC_CHECK_LIB(krb, krb_mk_req, LIBAUTH=-lkrb)
  AC_CHECK_LIB(des, des_key_sched, LIBAUTH="$LIBAUTH -ldes")
fi
AC_SUBST(LIBAUTH)
# define CPP macros that enable auth/encrypt code
test "$enable_authentication" = yes && AC_DEFINE(AUTHENTICATION)
test "$enable_encryption" = yes     && AC_DEFINE(ENCRYPTION)
test "$enable_authentication" = yes -a "$ac_cv_lib_krb_krb_mk_req" = yes \
  && AC_DEFINE(KRB4)
test "$enable_authentication" = yes -a "$ac_cv_lib_krb_krb_mk_req" = yes \
  && AC_DEFINE(KERBEROS)
test "$enable_encryption" = yes -a "$ac_cv_lib_des_des_key_sched" = yes \
  && AC_DEFINE(DES_ENCRYPTION)

# We use our own version of getopt (including our own header file) if the
# system one doesn't have getopt_long.
AC_CHECK_FUNC(getopt_long, ,
  INCLUDES="$INCLUDES getopt.h"
  LIBOBJS="$LIBOBJS getopt.o getopt1.o")

# Supply a version of poll() if the libray is missing.
# syslogd uses poll() unconditionnaly.
AC_CHECK_FUNC(poll, ,
  AC_DEFINE(HAVE_POLL_H)
  INCLUDES="$INCLUDES poll.h"
  LIBOBJS="$LIBOBJS poll.o")

# Supply versions of the BSD error reporting functions if the system doesn't
AC_CHECK_FUNC(verrx, ,
  INCLUDES="$INCLUDES err.h"
  LIBOBJS="$LIBOBJS err.o")

if test "$ac_cv_header_utmp_h" = yes; then
  IU_CHECK_TYPE(struct lastlog, [#include <utmp.h>])
  IU_CHECK_STRUCT_FIELD(utmp, ut_type, [#ifdef HAVE_SYS_TYPES_H
                                        # include <sys/types.h>
                                        #endif
					#include <utmp.h>])
  IU_CHECK_STRUCT_FIELD(utmp, ut_pid, [#ifdef HAVE_SYS_TYPES_H
                                       # include <sys/types.h>
                                       #endif
				       #include <utmp.h>])
  IU_CHECK_STRUCT_FIELD(utmp, ut_id, [#ifdef HAVE_SYS_TYPES_H
                                      # include <sys/types.h>
                                      #endif
				      #include <utmp.h>])
  IU_CHECK_STRUCT_FIELD(utmp, ut_user, [#ifdef HAVE_SYS_TYPES_H
                                        # include <sys/types.h>
                                        #endif
					#include <utmp.h>])
  IU_CHECK_STRUCT_FIELD(utmp, ut_host, [#ifdef HAVE_SYS_TYPES_H
                                        # include <sys/types.h>
                                        #endif
					#include <utmp.h>])
  IU_CHECK_STRUCT_FIELD(utmp, ut_tv, [#ifdef HAVE_SYS_TYPES_H
                                      # include <sys/types.h>
                                      #endif
                                      #include <utmp.h>])
  IU_CHECK_STRUCT_FIELD(utmp, ut_exit, [#ifdef HAVE_SYS_TYPES_H
                                        # include <sys/types.h>
                                        #endif
					#include <utmp.h>])
fi
if test "$ac_cv_header_utmpx_h" = yes; then
  IU_CHECK_STRUCT_FIELD(utmpx, ut_tv, [#include <utmpx.h>])
fi

IU_CHECK_TYPE(struct if_nameindex,
  [ #include <sys/socket.h>
    #include <net/if.h>])

IU_CHECK_STRUCT_FIELD(ifreq,
                      ifr_index,
              [#include <sys/socket.h>
               #include <net/if.h>])

IU_CHECK_STRUCT_FIELD(ifreq,
                      ifr_netmask,
              [#include <sys/socket.h>
               #include <net/if.h>])

IU_CHECK_STRUCT_FIELD(ifreq,
                      ifr_broadaddr,
              [#include <sys/socket.h>
               #include <net/if.h>])

IU_CHECK_STRUCT_FIELD(sockaddr,
                      sa_len,
              [#include <sys/types.h>
               #include <sys/socket.h>])

# See if the system has strerror, and replace it if not
AC_CHECK_FUNC(strerror, , LIBOBJS="$LIBOBJS strerror.o")
if test "$ac_cv_func_strerror" = no; then
  # No strerror, so see if the SYS_ERRLIST variable can be used by ours
  AC_CHECK_FUNC(sys_errlist, AC_DEFINE(HAVE_SYS_ERRLIST))
  if test "$ac_cv_func_sys_errlist" = yes; then
    IU_CHECK_DECL(sys_errlist, [
      #include <stdio.h>
      #ifdef HAVE_ERRNO_H
      #include <errno.h>
      #endif])
  fi
fi

# See if we have h_errno (the test is here so we can use -lresolv if necessary).
AC_CACHE_CHECK([for h_errno], inetutils_cv_var_h_errno,
  AC_TRY_COMPILE([#include <netdb.h>],
    [ extern int h_errno; int iu_x = h_errno; ],
    inetutils_cv_var_h_errno=yes, inetutils_cv_var_h_errno=no))
if test "$inetutils_cv_var_h_errno" = yes; then
  AC_DEFINE(HAVE_H_ERRNO)
  IU_CHECK_DECL(h_errno, [#include <netdb.h>])
fi

# See if the system has hstrerror, and replace it if not
AC_CHECK_FUNC(hstrerror, , LIBOBJS="$LIBOBJS hstrerror.o")
if test "$ac_cv_func_hstrerror" = yes; then
  IU_CHECK_DECL(hstrerror, [#include <netdb.h>])
else
  # No hstrerror, so see if the H_ERRLIST variable can be used by ours
  AC_CHECK_FUNC(h_errlist)
  if test "$ac_cv_func_h_errlist" = yes; then
    IU_CHECK_DECL(h_errlist, [#include <netdb.h>])
  fi
fi
if test "$ac_cv_func_hstrerror" = yes -o "$ac_cv_func_h_errlist" = yes; then
  # If there's a system hstrerror, or we can reasonably replace it, say so.
  # We still provide some definition, regardless, but this allows people to use
  # a reasonable alternative if the situation allows, rather than using a
  # degenerate version that only says `Host lookup error N'.
  AC_DEFINE(HAVE_HSTRERROR)
fi

IU_CHECK_STRUCT_FIELD(hostent, h_addr_list, [#include <netdb.h>])

# Checks return type of signal handlers
AC_TYPE_SIGNAL
AC_DECL_SYS_SIGLIST
# sig_t is the type of a signal handler (4.4BSD)
IU_CHECK_TYPE(sig_t,
  [ #include <sys/types.h>
    #include <signal.h> ])
# sigset_t is a set of signals (4.4BSD)
IU_CHECK_TYPE(sigset_t,
  [ #include <sys/types.h>
    #include <signal.h> ],
  :, AC_DEFINE(sigset_t, unsigned long))
# I'm not sure what this is, but netbsd has it; just use int if necessary.
IU_CHECK_TYPE(sig_atomic_t,
  [ #include <sys/types.h>
    #include <signal.h> ],
  :, AC_DEFINE(sig_atomic_t, int))


IU_CHECK_TYPE(sa_family_t,
  [
    #include <sys/types.h>
    #ifdef HAVE_SOCKET_H
    #include <socket.h>
    #endif
    #include <sys/socket.h> ],
  :, AC_DEFINE(sa_family_t, unsigned int))

# See if the __PROGNAME variable is defined, otherwise use our own.
AC_CHECK_FUNC(__progname,
  AC_DEFINE(HAVE___PROGNAME),
  LIBOBJS="$LIBOBJS __progname.o")

# See if snprintf exists, otherwise just use a bogus version
AC_CHECK_FUNC(snprintf,
  AC_DEFINE(HAVE_SNPRINTF),
  LIBOBJS="$LIBOBJS snprintf.o")

# See if vsnprintf exists, otherwise use our own.
AC_CHECK_FUNC(vsnprintf,
  AC_DEFINE(HAVE_VSNPRINTF),
  LIBOBJS="$LIBOBJS snprintf.o")

# We want to use libreadline if it is available.  However, it seems that the
# guile hackers have had problems because there is a Solaris libreadline
# that is different from the GNU libreadline; therefore we test for an obscure
# function in the GNU libreadline.
#AC_CHECK_LIB(termcap, tputs)
#AC_CHECK_LIB(readline, rl_bind_key,,, -ltermcap)
AC_CHECK_LIB(readline, rl_bind_key,
             [LIBREADLINE=-lreadline
              AC_DEFINE(HAVE_LIBREADLINE)],
             [LIBREADLINE=], "$LIBTERMCAP")
AC_SUBST(LIBREADLINE)
AC_SUBST(LIBTERMCAP)

AC_CHECK_LIB(opie, opiegenerator)

AC_CHECK_LIB(z, gzdopen)

# Check for termio-related types and defines
IU_CHECK_TYPE(cc_t,
 [ #if defined(HAVE_TERMIOS_H)
   # include <termios.h>
   #elif defined(HAVE_TERMIO_H)
   # include <termio.h>
   #else
   # include <sgtty.h>
   #endif ])

# For now, we always use libexecdir; later we may want to vary where the
# daemons go.
inetdaemondir='$(libexecdir)'
AC_SUBST(inetdaemondir)

# Configure paths used by inetutils programs.  See the file `paths' for
# details.
PATHDEFS_MAKE=pathdefs.make
PATHS_DEFS=paths.defs
IU_CONFIG_PATHS($srcdir/paths, PATHDEFS_MAKE, PATHS_DEFS)

# Supply optional header files by linking a copy into the object include dir
LINK_SRCS="$LINK_SRCS `for I in $INCLUDES; do echo $ac_n ' 'headers/$I$ac_c; done`"
LINK_DSTS="$LINK_DSTS `for I in $INCLUDES; do echo $ac_n ' 'include/$I$ac_c; done`"
ac_sources="$LINK_SRCS"
ac_dests="$LINK_DSTS"
while test -n "$ac_sources"; do
  set $ac_dests; ac_dest=$1; shift; ac_dests=$*
  set $ac_sources; ac_source=$1; shift; ac_sources=$*
  ac_config_links_1="$ac_config_links_1 $ac_dest:$ac_source"
done
AC_CONFIG_LINKS([$ac_config_links_1])

AC_FUNC_MMAP

AC_SUBST(VERSION)

# Ok this is the end of the rope, even autoconf can't help.
# For example on certain system login(1) will not work if we
# don't have an entry in utmpx.
# Also for some OS they are better handle with STREAMS
case "$host" in
*olaris*)
  AC_DEFINE(SOLARIS)
  AC_DEFINE(UTMPX)
  AC_DEFINE(SOLARIS)
  AC_DEFINE(HAVE_STREAMSPTY)
  ;;
*irix*)
  AC_DEFINE(UTMPX)
  ;;
*hpux*)
  AC_DEFINE(UTMPX)
  ;;
esac

AC_CONFIG_FILES([Makefile libinetutils/Makefile libtelnet/Makefile glob/Makefile
libicmp/Makefile ping/Makefile ftp/Makefile ftpd/Makefile inetd/Makefile
rcp/Makefile
rexecd/Makefile rlogin/Makefile rlogind/Makefile rsh/Makefile
rshd/Makefile logger/Makefile syslogd/Makefile talk/Makefile
talkd/Makefile telnet/Makefile telnetd/Makefile tftp/Makefile
tftpd/Makefile uucpd/Makefile gwhois/Makefile libls/Makefile
libicmp/Makefile ping/Makefile
include/confpaths.h:headers/confpaths.h.in
headers/Makefile include/version.h:headers/version.h.in
doc/Makefile doc/rfc/Makefile])
AC_OUTPUT

