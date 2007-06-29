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

bin_PROGRAMS = @telnet_BUILD@

EXTRA_PROGRAMS = telnet

telnet_SOURCES = authenc.c commands.c main.c network.c ring.c sys_bsd.c \
	telnet.c terminal.c tn3270.c utilities.c

noinst_HEADERS = defines.h externs.h fdset.h general.h ring.h types.h

man_MANS = telnet.1

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/lib -I../lib -I$(top_srcdir)/libinetutils
AM_CPPFLAGS=-DTERMCAP -DLINEMODE -DKLUDGELINEMODE -DENV_HACK

LIBTERMCAP = @LIBTERMCAP@
LIBCRYPT = @LIBCRYPT@

LDADD = -L../libinetutils -L../libtelnet -linetutils	\
	-ltelnet -L../lib -lgnu $(LIBTERMCAP) $(LIBCRYPT) $(LIBAUTH)

EXTRA_DIST = $(man_MANS)
