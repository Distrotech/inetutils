#
# Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005,
# 2006, 2007, 2008, 2009, 2010 Free Software Foundation, Inc.
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

bin_PROGRAMS = @telnet_BUILD@

EXTRA_PROGRAMS = telnet

telnet_SOURCES = authenc.c commands.c main.c network.c ring.c sys_bsd.c \
	telnet.c terminal.c tn3270.c utilities.c

noinst_HEADERS = defines.h externs.h general.h ring.h types.h

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/lib -I../lib -I$(top_srcdir)/libinetutils
AM_CPPFLAGS=-DTERMCAP -DLINEMODE -DKLUDGELINEMODE -DENV_HACK

LDADD = -L../libinetutils -L../libtelnet -linetutils	\
	-ltelnet ../lib/libgnu.a $(LIBTERMCAP) $(LIBCRYPT) $(LIBAUTH)
