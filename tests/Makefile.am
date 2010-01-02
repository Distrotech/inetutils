#
# Copyright (C) 2007, 2008, 2009, 2010 Free Software Foundation, Inc.
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

AM_CPPFLAGS = -I$(top_srcdir)/libinetutils -I$(top_srcdir)/lib

LDADD = -L../libinetutils -linetutils  ../lib/libgnu.a

check_PROGRAMS = localhost
dist_check_SCRIPTS = ping-localhost.sh traceroute-localhost.sh
TESTS = $(check_PROGRAMS) $(dist_check_SCRIPTS)

TESTS_ENVIRONMENT = EXEEXT=$(EXEEXT)
