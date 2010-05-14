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

inetdaemondir = @inetdaemondir@

# Initialise variables.
inetdaemon_PROGRAMS =
bin_PROGRAMS =
EXTRA_PROGRAMS =


bin_PROGRAMS += @hostname_BUILD@
hostname_SOURCES = hostname.c
EXTRA_PROGRAMS += hostname

bin_PROGRAMS += @logger_BUILD@
logger_SOURCES = logger.c
EXTRA_PROGRAMS += logger

bin_PROGRAMS += @rcp_BUILD@
rcp_SOURCES = rcp.c
EXTRA_PROGRAMS += rcp

bin_PROGRAMS += @rexec_BUILD@
rexec_SOURCES = rexec.c
EXTRA_PROGRAMS += rexec

bin_PROGRAMS += @rlogin_BUILD@
rlogin_SOURCES = rlogin.c
EXTRA_PROGRAMS += rlogin

bin_PROGRAMS += @rsh_BUILD@
rsh_SOURCES = rsh.c
EXTRA_PROGRAMS += rsh

bin_PROGRAMS += @tftp_BUILD@
tftp_SOURCES = tftp.c
EXTRA_PROGRAMS += tftp

bin_PROGRAMS += @traceroute_BUILD@
traceroute_SOURCES = traceroute.c
EXTRA_PROGRAMS += traceroute

inetdaemon_PROGRAMS += @inetd_BUILD@
inetd_SOURCES = inetd.c
EXTRA_PROGRAMS += inetd

inetdaemon_PROGRAMS += @rexecd_BUILD@
rexecd_SOURCES = rexecd.c
EXTRA_PROGRAMS += rexecd

inetdaemon_PROGRAMS += @rlogind_BUILD@
rlogind_SOURCES = rlogind.c
EXTRA_PROGRAMS += rlogind

inetdaemon_PROGRAMS += @rshd_BUILD@
rshd_SOURCES = rshd.c
EXTRA_PROGRAMS += rshd

inetdaemon_PROGRAMS += @syslogd_BUILD@
syslogd_SOURCES = syslogd.c
EXTRA_PROGRAMS += syslogd

inetdaemon_PROGRAMS += @tftpd_BUILD@
tftpd_SOURCES = tftpd.c
EXTRA_PROGRAMS += tftpd

inetdaemon_PROGRAMS += @uucpd_BUILD@
uucpd_SOURCES = uucpd.c
EXTRA_PROGRAMS += uucpd

INCLUDES = -I$(top_srcdir)/lib \
	-I$(top_srcdir)/libinetutils \
	-I$(top_srcdir)/libicmp

LDADD = -L$(top_builddir)/libicmp -licmp \
	-L$(top_builddir)/libinetutils -linetutils \
	-L$(top_builddir)/lib ../lib/libgnu.a \
	$(LIBAUTH) $(LIBCRYPT) $(LIBUTIL)

AM_CPPFLAGS = $(PATHDEF_BSHELL) $(PATHDEF_CONSOLE) $(PATHDEF_CP) $(PATHDEF_DEFPATH) $(PATHDEF_DEV) $(PATHDEF_INETDCONF) $(PATHDEF_INETDDIR) $(PATHDEF_INETDPID) $(PATHDEF_KLOG) $(PATHDEF_LOG) $(PATHDEF_LOGCONF) $(PATHDEF_LOGIN) $(PATHDEF_LOGPID) $(PATHDEF_NOLOGIN) $(PATHDEF_RLOGIN) $(PATHDEF_RSH) $(PATHDEF_TTY) $(PATHDEF_UTMP) $(PATHDEF_UUCICO)

SUIDMODE = -o root -m 4775

install-exec-hook: @traceroute_INSTALL_HOOK@ @rsh_INSTALL_HOOK@ @rlogin_INSTALL_HOOK@ @rcp_INSTALL_HOOK@

install-traceroute-hook:
	-@$(INSTALL_PROGRAM) traceroute $(SUIDMODE) $(AM_INSTALL_PROGRAM_FLAGS) $(DESTDIR)$(bindir)/`echo traceroute|sed '$(transform)'` ; \
	if test $$? -ne 0; then \
	  echo "WARNING: insufficient access; not installing traceroute"; \
	  echo "NOTE: To install traceroute run 'make install' as root"; \
	fi

install-rsh-hook:
	-@$(INSTALL_PROGRAM) rsh $(SUIDMODE) $(AM_INSTALL_PROGRAM_FLAGS) $(DESTDIR)$(bindir)/`echo rsh|sed '$(transform)'` ; \
	if test $$? -ne 0; then \
	  echo "WARNING: insufficient access; not installing rsh"; \
	  echo "NOTE: To install rsh run 'make install' as root"; \
	fi

install-rlogin-hook:
	-@$(INSTALL_PROGRAM) rlogin $(SUIDMODE) $(AM_INSTALL_PROGRAM_FLAGS) $(DESTDIR)$(bindir)/`echo rlogin|sed '$(transform)'` ; \
	if test $$? -ne 0; then \
	  echo "WARNING: insufficient access; not installing rlogin"; \
	  echo "NOTE: To install rlogin run 'make install' as root"; \
	fi

install-rcp-hook:
	-@$(INSTALL_PROGRAM) rcp $(SUIDMODE) $(AM_INSTALL_PROGRAM_FLAGS) $(DESTDIR)$(bindir)/`echo rcp|sed '$(transform)'` ; \
	if test $$? -ne 0; then \
	  echo "WARNING: insufficient access; not installing rcp"; \
	  echo "NOTE: To install rcp run 'make install' as root"; \
	fi

@PATHDEFS_MAKE@
