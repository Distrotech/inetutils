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

bin_PROGRAMS = @rsh_BUILD@

EXTRA_PROGRAMS = rsh

rsh_SOURCES = rsh.c

man_MANS = rsh.1

@PATHDEFS_MAKE@

AM_CPPFLAGS = $(PATHDEF_RLOGIN)
INCLUDES = -I$(top_srcdir)/headers -I$(top_srcdir)/lib -I../lib 

LDADD = -L../libinetutils -linetutils -L../lib -lgnu \
         $(LIBUTIL) @LIBCRYPT@ @LIBAUTH@

EXTRA_DIST = $(man_MANS)

SUIDMODE = -o root -m 4775

install-rsh-hook:
	-@$(INSTALL_PROGRAM) $(bin_PROGRAMS) $(SUIDMODE) $(AM_INSTALL_PROGRAM_FLAGS) $(DESTDIR)$(bindir)/`echo $(bin_PROGRAMS)|sed '$(transform)'` ; \
	if test $$? -ne 0; then \
		echo 'INSTALL-ERROR:'; \
		echo 'INSTALL-ERROR: THIS PROGRAM($(bin_PROGRAMS)) MUST BE INSTALLED SETUID ROOT'; \
		echo 'INSTALL-ERROR: $(INSTALL_PROGRAM) $(bin_PROGRAMS) $(SUIDMODE) $(DESTDIR)$(bindir)/$(bin_PROGRAMS)'; \
		echo 'INSTALL-ERROR:'; \
	fi

install-exec-hook: @rsh_INSTALL_HOOK@

