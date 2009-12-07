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

dist_man1_MANS = hostname.1 ifconfig.1 inetd.1 ftp.1 ftpd.1 logger.1	\
	ping.1 rcp.1 rexec.1 rexecd.1 rlogin.1 rlogind.1 rsh.1 rshd.1	\
	syslogd.1 talk.1 talkd.1 telnet.1 telnetd.1 traceroute.1	\
	uucpd.1 whois.1

# FIXME: ping6 ifconfig

man_aux = $(dist_man1_MANS:.1=.x)
EXTRA_DIST = $(man_aux)

MAINTAINERCLEANFILES = $(dist_man1_MANS)

ftp.1: ftp.x $(top_srcdir)/ftp/main.c $(top_srcdir)/configure.ac

ftpd.1: ftpd.x $(top_srcdir)/ftpd/ftpd.c $(top_srcdir)/configure.ac

hostname.1: hostname.x $(top_srcdir)/hostname/hostname.c $(top_srcdir)/configure.ac

ifconfig.1: ifconfig.x $(top_srcdir)/ifconfig/options.c $(top_srcdir)/configure.ac

inetd.1: inetd.x $(top_srcdir)/inetd/inetd.c $(top_srcdir)/configure.ac

logger.1: logger.x $(top_srcdir)/logger/logger.c $(top_srcdir)/configure.ac

ping.1: ping.x $(top_srcdir)/ping/ping.c $(top_srcdir)/configure.ac

rcp.1: rcp.x $(top_srcdir)/rcp/rcp.c $(top_srcdir)/configure.ac

rexec.1: rexec.x $(top_srcdir)/rexec/rexec.c $(top_srcdir)/configure.ac

rexecd.1: rexecd.x $(top_srcdir)/rexecd/rexecd.c $(top_srcdir)/configure.ac

rlogin.1: rlogin.x $(top_srcdir)/rlogin/rlogin.c $(top_srcdir)/configure.ac

rlogind.1: rlogind.x $(top_srcdir)/rlogind/rlogind.c $(top_srcdir)/configure.ac

rsh.1: rsh.x $(top_srcdir)/rsh/rsh.c $(top_srcdir)/configure.ac

rshd.1: rshd.x $(top_srcdir)/rshd/rshd.c $(top_srcdir)/configure.ac

syslogd.1: syslogd.x $(top_srcdir)/syslogd/syslogd.c $(top_srcdir)/configure.ac

talk.1: talk.x $(top_srcdir)/talk/talk.c $(top_srcdir)/configure.ac

talkd.1: talkd.x $(top_srcdir)/talkd/talkd.c $(top_srcdir)/configure.ac

telnet.1: telnet.x $(top_srcdir)/telnet/main.c $(top_srcdir)/configure.ac

telnetd.1: telnetd.x $(top_srcdir)/telnetd/telnetd.c $(top_srcdir)/configure.ac

tftp.1: tftp.x $(top_srcdir)/tftp/main.c $(top_srcdir)/configure.ac

tftpd.1: tftpd.x $(top_srcdir)/tftpd/tftpd.c $(top_srcdir)/configure.ac

traceroute.1: traceroute.x $(top_srcdir)/traceroute/traceroute.c $(top_srcdir)/configure.ac

uucpd.1: uucpd.x $(top_srcdir)/uucpd/uucpd.c $(top_srcdir)/configure.ac

whois.1: whois.x $(top_srcdir)/whois/whois.c $(top_srcdir)/configure.ac

.x.1:
	rm -f $@ $@-t						\
	&& {							\
		rm -rf tmp;					\
		mkdir tmp;					\
		(cd tmp && $(LN_S) ../../$*/$* $*);		\
		    $(HELP2MAN) 				\
		     --source='$(PACKAGE_STRING)'		\
		     --include=$(srcdir)/$*.x			\
		     --output=tmp/$@ tmp/$*;			\
	}							\
	&& cp tmp/$@ $@-t					\
	&& rm -rf tmp						\
	&& mv $@-t $@
