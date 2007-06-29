# Copyright (C) 2005, 2007 Free Software Foundation, Inc.
# This file is part of GNU Inetutils.
#
# GNU Inetutils is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Inetutils is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Inetutils; see the file COPYING.  If not, write
# to the Free Software Foundation, Inc., 51 Franklin Street,
# Fifth Floor, Boston, MA 02110-1301 USA.

inetdaemondir = @inetdaemondir@

inetdaemon_PROGRAMS = @ftpd_BUILD@

EXTRA_PROGRAMS = ftpd

ftpd_SOURCES = ftpcmd.y ftpd.c logwtmp.c popen.c pam.c auth.c \
               conf.c server_mode.c

noinst_HEADERS = extern.h

man_MANS = ftpd.8

@PATHDEFS_MAKE@

INCLUDES = -I$(top_srcdir)/lib -I../lib -I$(top_srcdir)/libinetutils 
AM_CPPFLAGS = $(PATHDEF_FTPWELCOME) $(PATHDEF_FTPUSERS) \
           $(PATHDEF_FTPLOGINMESG) $(PATHDEF_FTPCHROOT) $(PATHDEF_FTPDPID) \
           $(PATHDEF_DEVNULL)  $(PATHDEF_NOLOGIN) $(PATHDEF_BSHELL)

LIBCRYPT = @LIBCRYPT@
LIBWRAP = @LIBWRAP@
LIBPAM = @LIBPAM@
LIBLS = @LIBLS@

LDADD = -L../libinetutils -linetutils -L../lib -lgnu \
          $(LIBLS) $(LIBCRYPT) $(LIBWRAP) $(LIBPAM)

EXTRA_DIST = $(man_MANS) ftpcmd.c
