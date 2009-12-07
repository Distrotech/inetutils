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

dist_man1_MANS = telnet.1 telnetd.1 rshd.1

man_aux = $(dist_man1_MANS:.1=.x)
EXTRA_DIST = $(man_aux)

MAINTAINERCLEANFILES = $(dist_man1_MANS)

telnet.1: telnet.x $(top_srcdir)/telnet/main.c $(top_srcdir)/configure.ac
	$(HELP2MAN) \
		--include=telnet.x \
		--output=$@ $(top_builddir)/telnet/telnet$(EXEEXT)

telnetd.1: telnetd.x $(top_srcdir)/telnetd/telnetd.c $(top_srcdir)/configure.ac
	$(HELP2MAN) \
		--include=telnetd.x \
		--output=$@ $(top_builddir)/telnetd/telnetd$(EXEEXT)

rshd.1: rshd.x $(top_srcdir)/rshd/rshd.c $(top_srcdir)/configure.ac
	$(HELP2MAN) \
		--include=rshd.x \
		--output=$@ $(top_builddir)/rshd/rshd$(EXEEXT)
