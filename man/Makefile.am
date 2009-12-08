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

# FIXME: ping6 ifconfig

all = hostname.1 ifconfig.1 inetd.1 ftp.1 ftpd.1 logger.1 ping.1 rcp.1	\
      rexec.1 rexecd.1 rlogin.1 rlogind.1 rsh.1 rshd.1 syslogd.1	\
      talk.1 talkd.1 telnet.1 telnetd.1 tftp.1 tftpd.1 traceroute.1	\
      uucpd.1 whois.1

dist_man1_MANS =

if ENABLE_hostname
dist_man1_MANS += hostname.1
endif

if ENABLE_ifconfig
dist_man1_MANS += ifconfig.1
endif

if ENABLE_inetd
dist_man1_MANS += inetd.1
endif

if ENABLE_ftp
dist_man1_MANS += ftp.1
endif

if ENABLE_ftpd
dist_man1_MANS += ftpd.1
endif

if ENABLE_logger
dist_man1_MANS += logger.1
endif

if ENABLE_ping
dist_man1_MANS += ping.1
endif

if ENABLE_rcp
dist_man1_MANS += rcp.1
endif

if ENABLE_rexec
dist_man1_MANS += rexec.1
endif

if ENABLE_rexecd
dist_man1_MANS += rexecd.1
endif

if ENABLE_rlogin
dist_man1_MANS += rlogin.1
endif

if ENABLE_rlogind
dist_man1_MANS += rlogind.1
endif

if ENABLE_rsh
dist_man1_MANS += rsh.1
endif

if ENABLE_rshd
dist_man1_MANS += rshd.1
endif

if ENABLE_syslogd
dist_man1_MANS += syslogd.1
endif

if ENABLE_talk
dist_man1_MANS += talk.1
endif

if ENABLE_talkd
dist_man1_MANS += talkd.1
endif

if ENABLE_telnet
dist_man1_MANS += telnet.1
endif

if ENABLE_telnetd
dist_man1_MANS += telnetd.1
endif

if ENABLE_tftp
dist_man1_MANS += tftp.1
endif

if ENABLE_tftpd
dist_man1_MANS += tftpd.1
endif

if ENABLE_traceroute
dist_man1_MANS += traceroute.1
endif

if ENABLE_uucpd
dist_man1_MANS += uucpd.1
endif

if ENABLE_whois
dist_man1_MANS += whois.1
endif

EXTRA_DIST = $(all) $(all:.1=.h2m)

MAINTAINERCLEANFILES = $(all)

ftp.1: ftp.h2m $(top_srcdir)/ftp/main.c $(top_srcdir)/configure.ac

ftpd.1: ftpd.h2m $(top_srcdir)/ftpd/ftpd.c $(top_srcdir)/configure.ac

hostname.1: hostname.h2m $(top_srcdir)/hostname/hostname.c $(top_srcdir)/configure.ac

ifconfig.1: ifconfig.h2m $(top_srcdir)/ifconfig/options.c $(top_srcdir)/configure.ac

inetd.1: inetd.h2m $(top_srcdir)/inetd/inetd.c $(top_srcdir)/configure.ac

logger.1: logger.h2m $(top_srcdir)/logger/logger.c $(top_srcdir)/configure.ac

ping.1: ping.h2m $(top_srcdir)/ping/ping.c $(top_srcdir)/configure.ac

rcp.1: rcp.h2m $(top_srcdir)/rcp/rcp.c $(top_srcdir)/configure.ac

rexec.1: rexec.h2m $(top_srcdir)/rexec/rexec.c $(top_srcdir)/configure.ac

rexecd.1: rexecd.h2m $(top_srcdir)/rexecd/rexecd.c $(top_srcdir)/configure.ac

rlogin.1: rlogin.h2m $(top_srcdir)/rlogin/rlogin.c $(top_srcdir)/configure.ac

rlogind.1: rlogind.h2m $(top_srcdir)/rlogind/rlogind.c $(top_srcdir)/configure.ac

rsh.1: rsh.h2m $(top_srcdir)/rsh/rsh.c $(top_srcdir)/configure.ac

rshd.1: rshd.h2m $(top_srcdir)/rshd/rshd.c $(top_srcdir)/configure.ac

syslogd.1: syslogd.h2m $(top_srcdir)/syslogd/syslogd.c $(top_srcdir)/configure.ac

talk.1: talk.h2m $(top_srcdir)/talk/talk.c $(top_srcdir)/configure.ac

talkd.1: talkd.h2m $(top_srcdir)/talkd/talkd.c $(top_srcdir)/configure.ac

telnet.1: telnet.h2m $(top_srcdir)/telnet/main.c $(top_srcdir)/configure.ac

telnetd.1: telnetd.h2m $(top_srcdir)/telnetd/telnetd.c $(top_srcdir)/configure.ac

tftp.1: tftp.h2m $(top_srcdir)/tftp/main.c $(top_srcdir)/configure.ac

tftpd.1: tftpd.h2m $(top_srcdir)/tftpd/tftpd.c $(top_srcdir)/configure.ac

traceroute.1: traceroute.h2m $(top_srcdir)/traceroute/traceroute.c $(top_srcdir)/configure.ac

uucpd.1: uucpd.h2m $(top_srcdir)/uucpd/uucpd.c $(top_srcdir)/configure.ac

whois.1: whois.h2m $(top_srcdir)/whois/whois.c $(top_srcdir)/configure.ac

.h2m.1:
	rm -f $@ $@-t						\
	&& {							\
		rm -rf tmp;					\
		mkdir tmp;					\
		(cd tmp && $(LN_S) ../../$*/$* $*);		\
		    $(HELP2MAN) 				\
		     --source='$(PACKAGE_STRING)'		\
		     --include=$(srcdir)/$*.h2m			\
		     --output=tmp/$@ tmp/$*;			\
	}							\
	&& cp tmp/$@ $@-t					\
	&& rm -rf tmp						\
	&& mv $@-t $@
