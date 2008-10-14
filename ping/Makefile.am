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

bin_PROGRAMS = @ping_BUILD@ @ping6_BUILD@

EXTRA_PROGRAMS = ping ping6

man_MANS = ping.8
EXTRA_DIST = $(man_MANS)

ping_LDADD = -L../libinetutils -linetutils -L../libicmp -licmp -L../lib -lgnu
ping6_LDADD = -L../lib -lgnu -L../libinetutils -linetutils
INCLUDES = -I$(top_srcdir)/lib -I../lib -I$(top_srcdir)/libicmp \
  -I$(top_srcdir)/libinetutils

ping_SOURCES = ping.c ping_common.c ping_echo.c ping_address.c \
  ping_router.c ping_timestamp.c ping_common.h  ping_impl.h ping.h libping.c
ping6_SOURCES = ping6.c ping_common.c ping_common.h ping6.h

SUIDMODE = -o root -m 4775

install-ping-hook:
	-@for program in $(bin_PROGRAMS); do \
	$(INSTALL_PROGRAM) $$program $(SUIDMODE) $(AM_INSTALL_PROGRAM_FLAGS) $(DESTDIR)$(bindir)/`echo $$program|sed '$(transform)'` ; \
	if test $$? -ne 0; then \
		echo 'INSTALL-ERROR:'; \
		echo 'INSTALL-ERROR: THIS PROGRAM('$$program') MUST BE INSTALLED SETUID ROOT'; \
		echo 'INSTALL-ERROR: $(INSTALL_PROGRAM) '$$program' $(SUIDMODE) $(DESTDIR)$(bindir)/'$$program; \
		echo 'INSTALL-ERROR:'; \
	fi \
	done

install-exec-hook: @ping_INSTALL_HOOK@

# EOF
