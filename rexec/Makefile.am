#
# Copyright (C) 2009 Free Software Foundation, Inc.
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

bin_PROGRAMS = @rexec_BUILD@

EXTRA_PROGRAMS = rexec

rexec_SOURCES = rexec.c

noinst_HEADERS = 

@PATHDEFS_MAKE@

INCLUDES = -I$(top_srcdir)/lib -I../lib -I$(top_srcdir)/libinetutils
AM_CPPFLAGS = $(PATHDEF_DEFPATH) $(PATHDEF_BSHELL)

LIBCRYPT = @LIBCRYPT@

LDADD = -L../libinetutils -linetutils ../lib/libgnu.a $(LIBCRYPT)
