# configure.ac - Configuration for inetutils
# Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004,
# 2005, 2006, 2007, 2008, 2009, 2010 Free Software Foundation, Inc.
#
# This file is part of GNU Inetutils.
#
# GNU Inetutils is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or (at
# your option) any later version.
#
# GNU Inetutils is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see `http://www.gnu.org/licenses/'.

AC_PREREQ(2.59)

AC_INIT([GNU inetutils], [1.8], [bug-inetutils@gnu.org])

AC_CONFIG_SRCDIR([src/inetd.c])
AC_CONFIG_AUX_DIR([build-aux])
AM_CONFIG_HEADER([config.h:config.hin])
AC_CANONICAL_SYSTEM # FIXME: Why not just _HOST?

AM_INIT_AUTOMAKE([1.11])

# config.h turns this on, so make sure it's on for our tests too
#CPPFLAGS=-D_BSD_SOURCE

dnl For now, we always use libexecdir; later we may want to vary where the
dnl daemons go.
inetdaemondir='$(libexecdir)'
AC_SUBST(inetdaemondir)

AC_ARG_WITH([krb4],
            AC_HELP_STRING([--with-krb4[[=PATH]]],
                           [Compile with Kerberos IV]),
            [KERBEROS_VERSION=4
             KERBEROS_DIR=$withval])
AC_ARG_WITH([krb5],
            AC_HELP_STRING([--with-krb5[[=PATH]]],
                           [Compile with Kerberos V]),
            [KERBEROS_VERSION=5
             KERBEROS_DIR=$withval])
AC_ARG_WITH([shishi],
            AC_HELP_STRING([--with-shishi[[=PATH]]],
		           [Compile with Shishi (Kerberos 5)]),
            [KERBEROS_VERSION=Shishi
             KERBEROS_DIR=$withval])

AC_ARG_ENABLE([servers],
              AC_HELP_STRING([--disable-servers],
                             [Don't compile servers]),
	      , [enable_servers=yes])
AC_ARG_ENABLE([clients],
              AC_HELP_STRING([--disable-clients],
                             [Don't compile clients]),
	      , [enable_clients=yes])
AC_ARG_ENABLE([libls],
              AC_HELP_STRING([--disable-libls],
                             [Disable support for libls ]),
              , [enable_libls=yes])
AC_ARG_ENABLE([encryption],
              AC_HELP_STRING([--enable-encryption],
                             [Enable encryption]),
              [test -z "$KERBEROS_VERSION" && AC_MSG_ERROR([--enable-encryption is given but no Kerberos version is required])],
              [test -z "$KERBEROS_VERSION" && enable_encryption=no])
AC_ARG_ENABLE([authentication],
              AC_HELP_STRING([--enable-authentication],
                             [Enable connection authentication]),
              [test -z "$KERBEROS_VERSION" && AC_MSG_ERROR([--enable-authentication is given but no Kerberos version is required])],
	      [test -z "$KERBEROS_VERSION" && enable_authentication=no])

if test -n "$KERBEROS_VERSION"; then
  if test -z "$enable_authentication$enable_encryption"; then
    enable_authentication=yes
    enable_encryption=yes
  fi
fi

AC_ARG_WITH([wrap],
            AC_HELP_STRING([--with-wrap],
                           [Add tcp wrapper support]))
AC_ARG_WITH([pam],
            AC_HELP_STRING([--with-pam],
                           [Add support for PAM]))

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
IU_ENABLE_CLIENT(hostname)
IU_ENABLE_CLIENT(ping)
IU_ENABLE_CLIENT(ping6)
IU_ENABLE_CLIENT(rcp)
IU_ENABLE_CLIENT(rexec)
IU_ENABLE_CLIENT(rlogin)
IU_ENABLE_CLIENT(rsh)
IU_ENABLE_CLIENT(logger)
IU_ENABLE_CLIENT(talk)
IU_ENABLE_CLIENT(telnet)
IU_ENABLE_CLIENT(tftp)
IU_ENABLE_CLIENT(whois)
IU_ENABLE_CLIENT(ifconfig)
IU_ENABLE_CLIENT(traceroute)

AM_INIT_AUTOMAKE

AM_SILENT_RULES([yes]) # make --enable-silent-rules the default.

AC_ISC_POSIX

### Checks for programs.
AC_PROG_CC
gl_EARLY
AC_CHECK_TOOL(AR, ar)
AC_PATH_PROG(RM, rm, rm)
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_YACC
AC_PROG_LN_S
AM_MISSING_PROG(HELP2MAN, help2man, $missing_dir)

gl_INIT

### Checks for libraries.
AC_CHECK_LIB(nsl, inet_ntoa)
AC_CHECK_LIB(resolv, gethostname)
AC_CHECK_LIB(socket, getpeername)
AC_CHECK_LIB(opie, opiegenerator)

# See if a termcap library is available (under one of several names)
IU_LIB_TERMCAP

# We want to use libreadline if it is available.  However, it seems that the
# guile hackers have had problems because there is a Solaris libreadline
# that is different from the GNU libreadline; therefore we test for an obscure
# function in the GNU libreadline.
#AC_CHECK_LIB(termcap, tputs)
#AC_CHECK_LIB(readline, rl_bind_key,,, -ltermcap)
AC_CHECK_LIB(readline, rl_bind_key,
             [LIBREADLINE=-lreadline
              AC_DEFINE([HAVE_LIBREADLINE], 1,
	           [Define to one if you have -lreadline])],
             [LIBREADLINE=], "$LIBTERMCAP")
AC_SUBST(LIBREADLINE)
AC_SUBST(LIBTERMCAP)

AC_CHECK_LIB(history, add_history,
             [LIBHISTORY=-lhistory
              AC_DEFINE([HAVE_LIBHISTORY], 1,
	           [Define to one if you have -lhistory])],
             [LIBHISTORY=])

# If libhistory does not provide add_history check if libreadline has it.
if test -z "$LIBHISTORY" && test -n "$LIBREADLINE"; then
  AC_CHECK_LIB(readline, add_history,
               [LIBHISTORY=-lreadline
                AC_DEFINE([HAVE_LIBHISTORY], 1,
                     [Define to one if you have -lhistory])],
               [LIBHISTORY=])
fi

AC_SUBST(LIBHISTORY)

dnl See if there's a separate libcrypt (many systems put crypt there)
AC_CHECK_LIB(crypt, crypt, LIBCRYPT=-lcrypt)
AC_SUBST(LIBCRYPT)

dnl libutil is a 4.4BSD library that defines several functions handy for
dnl inetutil daemons, doing such things as utmp frobbing, and pty setup.
AC_CHECK_LIB(util, logout, LIBUTIL=-lutil)
AC_SUBST(LIBUTIL)

# Check if they want support for PAM.  Certain daemons like
# ftpd have support for it.

# See if we have libpam.a this is for ftpd.
if test "$with_pam" = yes ; then
  AC_CHECK_LIB(dl, dlopen)
  AC_CHECK_LIB(pam, pam_authenticate, LIBPAM=-lpam)
  if test "$ac_cv_lib_pam_pam_authenticate" = yes ; then
     AC_DEFINE([WITH_PAM], 1, [Define to one if you have -lpam.])
  fi
fi
AC_SUBST(LIBPAM)

# Can we use libls? but we must have fchdir()
if test "$enable_libls" = yes && test "$ac_cv_func_fchdir" = yes ; then
  LIBLS="../libls/libls.a"
  libls_BUILD="libls.a"
  AC_DEFINE([WITH_LIBLS], 1, [Define to one if you have -lls])
else
  LIBLS=''
  libls_BUILD=''
fi
AC_SUBST(LIBLS)
AC_SUBST(libls_BUILD)

# Only talk uses curses, so only check for it if we want that
if test "$enable_talk" != no; then
  IU_LIB_CURSES
fi
if test "$enable_talk" != no && test "$LIBCURSES"; then
  enable_talk=yes
else
  if test "$enable_talk" = yes; then
    AC_MSG_WARN([curses is not available, so not building talk])
  fi
  enable_talk=no
  talk_BUILD=''
fi

dnl FIXME: Make this test case a bit more robust. Remove KRB4
LIBAUTH=
INCAUTH=
if test "$enable_encryption" = yes \
   || test "$enable_authentication" = yes; then
  AH_TEMPLATE(KERBEROS, [Define to one if you have Kerberos])
  AH_TEMPLATE(KRB4, [Define to one if you have Kerberos IV])
  AH_TEMPLATE(KRB5, [Define to one if you have Kerberos V])
  AH_TEMPLATE(SHISHI, [Define to 1 if you have Shishi])
  AH_TEMPLATE(DES_ENCRYPTION, [FIXME])

  case $KERBEROS_VERSION in
  4)
    AC_CHECK_LIB(krb, krb_mk_req,
                 LIBAUTH=-lkrb
                  AC_DEFINE([KRB4]),
                 -L$KERBEROS_DIR)
    if test ".$LIBAUTH" != . ; then
      AC_CHECK_LIB(des, des_key_sched,
                   [LIBAUTH="$LIBAUTH -ldes"],
                   -L$KERBEROS_DIR)
      dnl FIXME: Fix this to one test case.
      test "$enable_authentication" = yes \
      && test "$ac_cv_lib_krb_krb_mk_req" = yes \
      && AC_DEFINE([KRB4])

      test "$enable_authentication" = yes \
      && test "$ac_cv_lib_krb_krb_mk_req" = yes \
      && AC_DEFINE([KERBEROS])

      test "$enable_encryption" = yes \
      && test "$ac_cv_lib_des_des_key_sched" = yes \
      && AC_DEFINE([DES_ENCRYPTION])
    fi
    ;;
  5)
    IU_CHECK_KRB5($KERBEROS_VERSION,$KERBEROS_DIR)
    LIBAUTH="$KRB5_LIBS" # FIXME
    INCAUTH=$KRB5_CFLAGS
    if test ".$LIBAUTH" != . ;then
      AC_DEFINE([KERBEROS])
      AC_DEFINE([KRB5])
    fi
    ;;
  *)
    if test x$KERBEROS_DIR != x; then
      LIBAUTH=-L$KERBEROS_DIR/lib
      INCAUTH=-I$KERBEROS_DIR/include
    fi
    LIBAUTH="$LIBAUTH -lshishi"
    save_LIBS=$LIBS
    LIBS="$LIBS $LIBAUTH"
    save_CPPFLAGS=$CPPFLAGS
    CPPFLAGS="$CPPFLAGS $INCAUTH"
    AC_CHECK_HEADER(shishi.h,
      AC_CHECK_LIB(shishi, shishi_check_version,
        AC_DEFINE([SHISHI]),
        [INCAUTH= LIBAUTH=]))
    LIBS=$save_LIBS
    CPPFLAGS=$save_CPPFLAGS
  esac
fi
AC_SUBST(LIBAUTH)
AC_SUBST(INCAUTH)

dnl Define CPP macros that enable auth/encrypt code
if test ".$LIBAUTH" != .; then
  test "$enable_authentication" = yes && AC_DEFINE([AUTHENTICATION], 1,
                   [Define to one if you want authentication.])
  test "$enable_encryption" = yes     && AC_DEFINE([ENCRYPTION], 1,
                   [Define to one if you want encryption.])
fi

dnl Checks for IPv6 support.

AC_ARG_ENABLE([ipv6],
  AS_HELP_STRING([--disable-ipv6], [disable IPv6 support]),
  [case "${enable_ipv6}" in
  no)
    AC_MSG_NOTICE([Disabling IPv6 at user request])
    ipv6=no
    ;;
  *)
    ipv6=yes
    ;;
  esac],
  [ipv6=auto]
)

working_ipv6=yes
if test ! "X$ipv6" = "Xno" && test "X$working_ipv6" = "Xyes"; then
  IU_CHECK_MACRO(AF_INET6, [#include <sys/socket.h>], , , working_ipv6=no)
fi

if test ! "X$ipv6" = "Xno" && test "X$working_ipv6" = "Xyes"; then
  IU_CHECK_MACRO(IPV6_V6ONLY, [#include <netinet/in.h>], , , working_ipv6=no)
fi

if test ! "X$ipv6" = "Xno" && test "X$working_ipv6" = "Xyes"; then
  AC_CHECK_TYPE(struct sockaddr_storage, , working_ipv6=no, [#include <sys/socket.h>])
fi

if test ! "X$ipv6" = "Xno" && test "X$working_ipv6" = "Xyes"; then
  AC_CHECK_TYPE(struct sockaddr_in6, , working_ipv6=no, [#include <netinet/in.h>])
fi

if test ! "X$ipv6" = "Xno" && test "X$working_ipv6" = "Xyes"; then
  AC_CHECK_TYPE(struct addrinfo, , working_ipv6=no, [#include <netdb.h>])
fi

if test ! "X$ipv6" = "Xno" && test "X$working_ipv6" = "Xyes"; then
  AC_CHECK_FUNC(getnameinfo, ,working_ipv6=no, [#include <netdb.h>])
fi

if test ! "X$ipv6" = "Xno" && test "X$working_ipv6" = "Xyes"; then
  AC_DEFINE([IPV6], 1, [Define to one if you want IPv6.])
else
  if test "X$ipv6" = "Xyes"; then
    AC_MSG_FAILURE([IPv6 support not available])
  else
    if test "X$ipv6" = "Xauto"; then
      AC_MSG_WARN([IPv6 support not available, disabling IPv6])
    fi
  fi
fi

if test "X$ipv6" = "Xno" || test "X$working_ipv6" = "Xno";then
  ping6_BUILD=''
else
  working_icmp6=yes
  AC_CHECK_HEADER(netinet/icmp6.h, , working_icmp6=no,
    [IU_FLUSHLEFT([#ifdef HAVE_SYS_TYPES_H
		   # include <sys/types.h>
		   #endif
		   #include <sys/socket.h>
		   #include <netinet/in.h>])])

  if test "X$working_icmp6" = "Xyes"; then
    AC_CHECK_HEADER(netinet/ip6.h, , working_icmp6=no,
      [IU_FLUSHLEFT([#ifdef HAVE_SYS_TYPES_H
		     # include <sys/types.h>
		     #endif
		     #include <sys/socket.h>
  		     #include <netinet/in.h>])])

  fi

  if test "X$working_icmp6" = "Xyes"; then
    AC_CHECK_TYPE(struct icmp6_filter, , working_icmp6=no,
      [IU_FLUSHLEFT([#ifdef HAVE_SYS_TYPES_H
		     # include <sys/types.h>
		     #endif
		     #include <sys/socket.h>
  		     #include <netinet/in.h>
		     #include <netinet/icmp6.h>])])
  fi

  if test "X$working_icmp6" = "Xyes"; then
    AC_CHECK_TYPE(struct icmp6_hdr, , working_icmp6=no,
      [IU_FLUSHLEFT([#ifdef HAVE_SYS_TYPES_H
		     # include <sys/types.h>
		     #endif
		     #include <sys/socket.h>
  		     #include <netinet/in.h>
		     #include <netinet/icmp6.h>])])
  fi

  if test "X$working_icmp6" = "Xyes"; then
    IU_CHECK_MACRO(IPV6_RECVHOPLIMIT, [#include <netinet/in.h>], ,
    		   [have_recvhoplimt=yes], [have_recvhoplimt=no])
    if test "X$have_recvhoplimt" = "Xno"; then
      IU_CHECK_MACRO(IPV6_HOPLIMIT, [#include <netinet/in.h>], , [
        AC_DEFINE([IPV6_RECVHOPLIMIT], IPV6_HOPLIMIT,
                  [Define to IPV6_HOPLIMIT if not available])
      ], [working_icmp6=no])
    fi
  fi

  if test "X$working_icmp6" = "Xyes"; then
    IU_CHECK_MACRO(ICMP6_DST_UNREACH_BEYONDSCOPE,
                   [#ifdef HAVE_SYS_TYPES_H
		    # include <sys/types.h>
		    #endif
		    #include <sys/socket.h>
  		    #include <netinet/in.h>
		    #include <netinet/icmp6.h>], ,
    		   [have_beyondscope=yes], [have_beyondscope=no])
    if test "X$have_beyondscope" = "Xno"; then
      IU_CHECK_MACRO(ICMP6_DST_UNREACH_NOTNEIGHBOR, [#include <netinet/icmp6.h>], , [
        AC_DEFINE([ICMP6_DST_UNREACH_BEYONDSCOPE], ICMP6_DST_UNREACH_NOTNEIGHBOR,
                  [Define to ICMP6_DST_UNREACH_NOTNEIGHBOR if not available])
      ], [working_icmp6=no])
    fi
  fi

  if test "X$working_icmp6" = "Xno"; then
    AC_MSG_WARN([ICMPv6 support not available, disabling ping6])
    ping6_BUILD=''
  fi
fi


dnl Check if they want support for Wrap.  Certain daemons like
dnl ftpd have support for it.

if test "$with_wrap" = yes; then
dnl See if we have libwrap.a
dnl we can not use AC_CHECK_LIB reliably since for the wrap lib
dnl you have to define some global variables
#AC_CHECK_LIB(wrap, hosts_ctl, LIBWRAP=-lwrap)
  AC_MSG_CHECKING(hosts_ctl in -lwrap);
  save_LIBS=$LIBS
  LIBS="$save_LIBS -lwrap"
  AC_TRY_LINK([int allow_severity = 1; int deny_severity = 1;],
              hosts_ctl(), eval "ac_cv_lib_wrap_hosts_ctl=yes",
              eval "ac_cv_lib_wrap_hosts_ctl=no")
  LIBS=$save_LIBS
  if test "$ac_cv_lib_wrap_hosts_ctl" = yes ; then
    AC_DEFINE([WITH_WRAP], 1, [Define to one if you have -lwrap.])
    LIBWRAP=-lwrap
  fi
  AC_MSG_RESULT($ac_cv_lib_wrap_hosts_ctl);
fi
AC_SUBST(LIBWRAP)

### Checks for header files.
AC_HEADER_STDC
AC_HEADER_TIME
AC_HEADER_DIRENT

AC_CHECK_HEADERS([arpa/nameser.h des.h errno.h fcntl.h features.h glob.h \
                  krb.h memory.h malloc.h netinet/in_systm.h netinet/ip.h \
                  netinet/ip_icmp.h netinet/ip_var.h \
		  readline/readline.h \
		  security/pam_appl.h \
                  shadow.h stdarg.h stdlib.h string.h stropts.h sys/tty.h \
                  sys/utsname.h sys/ptyvar.h sys/msgbuf.h sys/filio.h \
                  sys/ioctl_compat.h sys/cdefs.h sys/stream.h sys/mkdev.h \
		  sys/sockio.h sys/sysmacros.h sys/param.h sys/file.h \
		  sys/proc.h sys/select.h sys/time.h sys/wait.h \
                  sys/resource.h \
		  stropts.h tcpd.h termio.h termios.h utmp.h utmpx.h unistd.h \
                  vis.h], [], [], [
#include <sys/types.h>
#ifdef HAVE_NETINET_IN_SYSTM_H
# include <netinet/in_systm.h>
#endif
#include <netinet/in.h>
#ifdef HAVE_NETINET_IP_H
# include <netinet/ip.h>
#endif
#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif
]

)

### Checks for types.
dnl Checks return type of signal handlers
AC_TYPE_SIGNAL

AC_CHECK_TYPE(socklen_t, ,
              [AC_DEFINE([socklen_t], int,
                         [Define to int if <sys/types.h> does not define])],
              [IU_FLUSHLEFT([#include <sys/types.h>
	                     #include <sys/socket.h>])])

dnl We may need our own definition for struct osockaddr (equivalent to the
dnl old 4.3BSD struct sockaddr, and used in grotty bsd talk protocol)
AC_CHECK_TYPE(struct osockaddr, ,
              [AC_DEFINE([HAVE_OSOCKADDR_H], 1,
                         [Define to one if you have <osockaddr.h>.])],
	      [IU_FLUSHLEFT([#include <sys/types.h>
                             #include <sys/socket.h> ])])

AC_CHECK_TYPES(struct if_nameindex, , ,
  [IU_FLUSHLEFT([#include <sys/types.h>
                 #include <sys/socket.h>
                 #include <net/if.h>])])

dnl sig_t is the type of a signal handler (4.4BSD)
AC_CHECK_TYPES(sig_t, , ,
  [IU_FLUSHLEFT([#include <sys/types.h>
                 #include <signal.h> ])])

dnl sigset_t is a set of signals (4.4BSD)
AC_CHECK_TYPE(sigset_t, , AC_DEFINE([sigset_t], unsigned long, [FIXME]),
  [IU_FLUSHLEFT([#include <sys/types.h>
                 #include <signal.h> ])])

dnl I'm not sure what this is, but netbsd has it; just use int if necessary.
AC_CHECK_TYPE(sig_atomic_t, , AC_DEFINE([sig_atomic_t], int, [FIXME]),
  [IU_FLUSHLEFT([#include <sys/types.h>
                 #include <signal.h> ])])

dnl Check for termio-related types and defines
AC_CHECK_TYPES(cc_t, , ,
 [IU_FLUSHLEFT([#if defined(HAVE_TERMIOS_H)
                # include <termios.h>
                #elif defined(HAVE_TERMIO_H)
                # include <termio.h>
                #else
                # include <sgtty.h>
                #endif ])])

dnl Check for compile support for ceratin type
AC_CHECK_TYPE(long long, , AC_DEFINE([LONG_LONG], long, [Define to long if compiler does not support]))
AC_CHECK_TYPE(long double, , AC_DEFINE([LONG_DOUBLE], double, [Define to double if compiler does not support]))

### Checks for structures.
IU_CHECK_MEMBERS([struct sockaddr_in.sin_len], , ,
    [ #include <sys/types.h>
      #include <netinet/in.h> ])

if test "$ac_cv_header_utmp_h" = yes; then
  AC_CHECK_TYPES(struct lastlog, , , [#include <utmp.h>])
  IU_CHECK_MEMBERS([struct utmp.ut_type, struct utmp.ut_pid,
                    struct utmp.ut_id, struct utmp.ut_user,
		    struct utmp.ut_host, struct utmp.ut_tv,
		    struct utmp.ut_exit], , ,
		   [#include <sys/types.h>
		    #include <utmp.h>])
fi
if test "$ac_cv_header_utmpx_h" = yes; then
  IU_CHECK_MEMBERS([struct utmpx.ut_tv], , , [#include <utmpx.h>])
fi

IU_CHECK_MEMBERS([struct ifreq.ifr_index,
                  struct ifreq.ifr_netmask,
                  struct ifreq.ifr_broadaddr,
		  struct ifreq.ifr_map], , ,
                 [#include <sys/types.h>
                  #include <sys/socket.h>
                  #include <net/if.h>])

IU_CHECK_MEMBERS([struct sockaddr.sa_len], , ,
                 [#include <sys/types.h>
                  #include <sys/socket.h>])
IU_CHECK_MEMBERS([struct hostent.h_addr_list], , , [#include <netdb.h>])

IU_CHECK_MEMBERS([struct stat.st_blksize])

### Checks for compiler characteristics.
AM_C_PROTOTYPES dnl FIXME: Does inetutils even compile on pre-ANSI compilers?
AC_C_CONST

dnl See if `weak refs' are possible; these make it possible (with shared
dnl libraries) to check for the existance of a standard function at run-time
dnl instead of compile time, which is very handy for distributing binary
dnl version programs that automatically adapt -- in inetutils case, to whether
dnl or not crypt is available.
IU_CHECK_WEAK_REFS

### Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_ALLOCA
AC_FUNC_SETVBUF_REVERSED
AC_FUNC_FORK
AC_FUNC_STRCOLL
AC_FUNC_MMAP
AC_FUNC_MALLOC
AC_FUNC_REALLOC

AC_CHECK_FUNCS(bcopy bcmp bzero cfsetspeed cgetent dirfd fchdir flock \
               fpathconf ftruncate \
	       getcwd getmsg getspnam initgroups initsetproctitle killpg \
               mkstemp ptsname \
               setegid seteuid setpgid \
               setsid setregid setreuid setresgid setresuid setutent_r \
               sigaction sigvec strchr setproctitle tcgetattr tzset utimes \
               utime uname \
               updwtmp updwtmpx vhangup wait3 wait4 opendir2)

dnl Functions that we will define if necessary.
AC_REPLACE_FUNCS(getpass getusershell memcmp memcpy memmove memset \
	         setenv strdup poll)

dnl Look for the crypt function itself (in libcrypt if possible)
_SAVE_LIBS="$LIBS"
LIBS="$LIBCRYPT $LIBS"
AC_CHECK_FUNCS(crypt)
LIBS="$_SAVE_LIBS"

AC_CHECK_FUNC(_obstack_free)

dnl Use (posix) `setpgid' if it's around, otherwise assume a BSD setpgrp
AC_CHECK_FUNC(setpgid, , AC_DEFINE([setpgid], setpgrp,
                          [Define to `setpgrp' if you don't have `setpgid'.]))

dnl We use our own version of getopt (including our own header file) if the
dnl system one doesn't have getopt_long.
AC_CHECK_FUNC(getopt_long)

AC_CHECK_HEADER(poll.h)

dnl See if snprintf exists, otherwise just use a bogus version
AC_CHECK_FUNC(snprintf,
  [AC_DEFINE([HAVE_SNPRINTF], 1, [Define to one if you have snprintf])])

dnl See if vsnprintf exists, otherwise use our own.
AC_CHECK_FUNC(vsnprintf,
  [AC_DEFINE([HAVE_VSNPRINTF], 1, [Define to one if you have vsnprintf])])

dnl See if the system has strerror, and replace it if not
AC_CHECK_FUNC(strerror)
if test "$ac_cv_func_strerror" = no; then
  # No strerror, so see if the SYS_ERRLIST variable can be used by ours
  AC_CHECK_FUNC(sys_errlist, AC_DEFINE([HAVE_SYS_ERRLIST], 1,
                              [Define to one if you have sys_errlist.]))
  if test "$ac_cv_func_sys_errlist" = yes; then
    AC_CHECK_DECL(sys_errlist, , ,
      [IU_FLUSHLEFT([#include <stdio.h>
                    #include <errno.h>])])
  fi
fi

dnl See if the system has hstrerror, and replace it if not
AC_CHECK_FUNC(hstrerror)
if test "$ac_cv_func_hstrerror" = yes; then
  AC_CHECK_DECL(hstrerror, , , [#include <netdb.h>])
else
  # No hstrerror, so see if the H_ERRLIST variable can be used by ours
  AC_CHECK_FUNC(h_errlist)
  if test "$ac_cv_func_h_errlist" = yes; then
    AC_CHECK_DECL(h_errlist, , , [#include <netdb.h>])
  fi
fi
if test "$ac_cv_func_hstrerror" = yes \
   || test "$ac_cv_func_h_errlist" = yes; then
  # If there's a system hstrerror, or we can reasonably replace it, say so.
  # We still provide some definition, regardless, but this allows people to use
  # a reasonable alternative if the situation allows, rather than using a
  # degenerate version that only says `Host lookup error N'.
  AC_DEFINE([HAVE_HSTRERROR], 1, [Define to one if you have hstrerror.])
fi

## Checks for function declarations.

AC_DECL_SYS_SIGLIST

AC_CHECK_DECLS(crypt, , , [#include <unistd.h>])

dnl EWOULDBLOCK is more or less the BSD version of posix EAGAIN.
IU_CHECK_MACRO(EWOULDBLOCK, [#include <errno.h>], , ,
  AC_DEFINE([EWOULDBLOCK], EAGAIN, 1, [FIXME]))
AC_CHECK_DECLS(errno, , , [#include <errno.h>])

dnl See whether <syslog.h> will declare special internal stuff if we define
dnl SYSLOG_NAMES before including it.  We check for various syslog internal
dnl macros, the typedef `CODE', and the variable `prioritynames'.
define([iu_syslog_includes],
  [[#include <stdlib.h>
    #define SYSLOG_NAMES
    #include <syslog.h> ]])
IU_CHECK_MACRO(syslog internal macros,
  iu_syslog_includes,
  [LOG_MAKEPRI LOG_PRI LOG_FAC],
  AC_CHECK_TYPE(CODE,
    AC_CHECK_DECL(prioritynames,
      AC_DEFINE([HAVE_SYSLOG_INTERNAL], 1, [FIXME]),,
      iu_syslog_includes), , iu_syslog_includes),)
undefine([iu_syslog_includes])

dnl Some systems don't declare common functions (especially if they return
dnl int), at least in the expected header file.  Check.
AC_CHECK_DECLS([fclose, pclose], , , [#include <stdio.h>])
AC_CHECK_DECLS([getcwd, getlogin, getpass, getusershell, ttyname], , ,
               [#include <unistd.h>])

AC_CHECK_DECLS([getgrnam, initgroups], , , [#include <grp.h>])

AC_CHECK_DECLS(htons, , ,
  [IU_FLUSHLEFT([#ifdef HAVE_SYS_TYPES_H
                 # include <sys/types.h>
                 #endif
                 #ifdef HAVE_SYS_PARAM_H
                 # include <sys/param.h>
                 #endif
                 #include <netinet/in.h> ])])

dnl Our home-grown version of setenv needs to use ENVIRON
AC_CHECK_DECLS(environ, , ,
  [IU_FLUSHLEFT([#include <unistd.h>
                 #include <stdlib.h>])])

dnl See if we have h_errno (the test is here so we can use -lresolv
dnl if necessary).
AC_CACHE_CHECK([for h_errno], inetutils_cv_var_h_errno,
  AC_TRY_COMPILE([#include <netdb.h>],
    [ extern int h_errno; int iu_x = h_errno; ],
    inetutils_cv_var_h_errno=yes, inetutils_cv_var_h_errno=no))
if test "$inetutils_cv_var_h_errno" = yes; then
  dnl FIXME: Isn't this wrong? Shouldn't the declaration check define
  dnl        this if we find h_errno?
  AC_DEFINE([HAVE_H_ERRNO], 1, [Define to one if you have h_errno.])
  AC_CHECK_DECL(h_errno, , , [#include <netdb.h>])
fi

## Checks for CPP macros.
dnl Look for the posix SEEK_ macros (for lseek), and if not found, try the
dnl similar berkeley L_ macros; if neither can be found, use the classic unix
dnl values.
IU_CHECK_MACRO(SEEK_ macros,
  [#include <unistd.h>], SEEK_SET SEEK_CUR SEEK_END,
  :,
  IU_CHECK_MACRO(L_ seek macros,
    [#include <unistd.h>], L_SET L_INCR L_XTND,
    AC_DEFINE([SEEK_SET], L_SET, [FIXME])
    AC_DEFINE([SEEK_CUR], L_INCR, [FIXME])
    AC_DEFINE([SEEK_END], L_XTND, [FIXME]),
    AC_DEFINE([SEEK_SET], 0, [FIXME])
    AC_DEFINE([SEEK_CUR], 1, [FIXME])
    AC_DEFINE([SEEK_END], 2, [FIXME])))

dnl Look for the posix _FILENO macros; if not found, use the classic unix values.
IU_CHECK_MACRO(_FILENO macros,
  [#include <unistd.h>], STDIN_FILENO STDOUT_FILENO STDERR_FILENO,
  :,
  AC_DEFINE([STDIN_FILENO], 0, [FIXME])
  AC_DEFINE([STDOUT_FILENO], 1, [FIXME])
  AC_DEFINE([STDERR_FILENO], 2, [FIXME]))

dnl See where to find fd_set (bit-vectors for select) manipulation macros.
IU_CHECK_MACRO(fd_set macros, [#include <sys/types.h>],
  FD_ZERO FD_CLR FD_SET FD_ISSET)
if test "$inetutils_cv_macro_fd_set_macros" = no; then
  IU_CHECK_MACRO(fd_set macros in <sys/time.h>, [#include <sys/time.h>],
    FD_ZERO FD_CLR FD_SET FD_ISSET)
fi

dnl Configure paths used by inetutils programs.  See the file `paths' for
dnl details.
PATHDEFS_MAKE=pathdefs.make
PATHS_DEFS=paths.defs
IU_CONFIG_PATHS($srcdir/paths, PATHDEFS_MAKE, PATHS_DEFS)
AH_BOTTOM(
[/* Defaults for PATH_ variables.  */
#include <confpaths.h>])

dnl Define ARG_UNUSED macro
dnl
AH_BOTTOM([
#if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 7)
#  define __attribute__(x)
#endif

#ifndef ARG_UNUSED
# define ARG_UNUSED __attribute__ ((__unused__))
#endif
])

dnl Ok this is the end of the rope, even autoconf can't help.
dnl For example on certain system login(1) will not work if we
dnl don't have an entry in utmpx.
dnl Also for some OS they are better handle with STREAMS
case "$host" in
*olaris*)
  AC_DEFINE([SOLARIS], 1, [FIXME])
  AC_DEFINE([UTMPX], 1, [FIXME])
  AC_DEFINE([HAVE_STREAMSPTY], 1, [FIXME])
  ;;
*irix* | *hpux*)
  AC_DEFINE([UTMPX], 1, [FIXME])
  AC_DEFINE([UTMPX], 1, [FIXME])
  ;;
esac

AC_CONFIG_FILES([
Makefile
lib/Makefile
libinetutils/Makefile
libtelnet/Makefile
libicmp/Makefile
libls/Makefile
src/Makefile
telnet/Makefile
telnetd/Makefile
ftp/Makefile
ftpd/Makefile
talk/Makefile
talkd/Makefile
whois/Makefile
ping/Makefile
ifconfig/Makefile
ifconfig/system/Makefile
doc/Makefile
man/Makefile
tests/Makefile
confpaths.h:confpaths.h.in
])
AC_OUTPUT
