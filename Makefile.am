# Copyright (C) 2005, 2006, 2007 Free Software Foundation, Inc. 
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

AUTOMAKE_OPTIONS = 1.9

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = README-alpha paths ChangeLog.0

SUBDIRS = lib headers libinetutils libtelnet \
	hostname inetd telnetd libls ftpd rshd rlogind uucpd rexecd syslogd \
	tftpd talkd telnet ftp rsh rcp rlogin tftp logger gwhois talk \
	libicmp ping doc ifconfig traceroute tests

DISTCLEANFILES = pathdefs.make paths.defs

snapshot:
	$(MAKE) dist distdir=$(PACKAGE)-`date +"%Y%m%d"`
