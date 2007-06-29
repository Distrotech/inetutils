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

inetdaemon_PROGRAMS = @rshd_BUILD@

EXTRA_PROGRAMS = rshd

rshd_SOURCES = rshd.c

man_MANS = rshd.8

@PATHDEFS_MAKE@

AM_CPPFLAGS = $(PATHDEF_BSHELL) $(PATHDEF_NOLOGIN) $(PATHDEF_DEFPATH)
INCLUDES = -I$(top_srcdir)/headers -I$(top_srcdir)/libinetutils -I$(top_srcdir)/lib -I../lib 

LDADD = -L../libinetutils -linetutils  -L../lib -lgnu \
        $(LIBUTIL) @LIBCRYPT@ @LIBAUTH@

EXTRA_DIST = $(man_MANS)
