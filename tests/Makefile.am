#
# Copyright (C) 2007, 2008, 2009, 2010, 2011 Free Software Foundation,
# Inc.
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

AM_CPPFLAGS = $(iu_INCLUDES)

LDADD = $(iu_LIBRARIES)

check_PROGRAMS = localhost
if ENABLE_inetd
check_PROGRAMS += addrpeek
endif

dist_check_SCRIPTS =
if ENABLE_ping
dist_check_SCRIPTS += ping-localhost.sh
endif
if ENABLE_traceroute
dist_check_SCRIPTS += traceroute-localhost.sh
endif
if ENABLE_inetd
if ENABLE_tftpd
if ENABLE_tftp
dist_check_SCRIPTS += tftp.sh
endif
endif
endif
if ENABLE_logger
if ENABLE_syslogd
dist_check_SCRIPTS += syslogd.sh
endif
endif

if ENABLE_inetd
if ENABLE_ftp
if ENABLE_ftpd
dist_check_SCRIPTS += ftp-localhost.sh
endif
endif
endif

if ENABLE_hostname
dist_check_SCRIPTS += hostname.sh
endif

TESTS = $(check_PROGRAMS) $(dist_check_SCRIPTS)

TESTS_ENVIRONMENT = EXEEXT=$(EXEEXT)
