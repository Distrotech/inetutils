# Copyright (C) 2005, 2007, 2008 Free Software Foundation, Inc.
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

bin_PROGRAMS = @traceroute_BUILD@

EXTRA_PROGRAMS = traceroute

traceroute_SOURCES = traceroute.c

noinst_HEADERS = traceroute.h

INCLUDES = -I$(top_srcdir)/lib -I../lib -I$(top_srcdir)/libicmp -I$(top_srcdir)/libinetutils

LDADD = -L../libicmp -licmp -L../lib -lgnu

SUIDMODE = -o root -m 4775

install-traceroute-hook:
	-@for program in $(bin_PROGRAMS); do \
	$(INSTALL_PROGRAM) $$program $(SUIDMODE) $(AM_INSTALL_PROGRAM_FLAGS) $(DESTDIR)$(bindir)/`echo $$program|sed '$(transform)'` ; \
	if test $$? -ne 0; then \
		echo 'INSTALL-ERROR:'; \
		echo 'INSTALL-ERROR: THIS PROGRAM('$$program') MUST BE INSTALLED SETUID ROOT'; \
		echo 'INSTALL-ERROR: $(INSTALL_PROGRAM) '$$program' $(SUIDMODE) $(DESTDIR)$(bindir)/'$$program; \
		echo 'INSTALL-ERROR:'; \
	fi \
	done

install-exec-hook: @traceroute_INSTALL_HOOK@
