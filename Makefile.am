AUTOMAKE_OPTIONS = gnits 1.2d

EXTRA_DIST = paths

SUBDIRS = headers libinetutils libtelnet glob \
	inetd telnetd ftpd rshd rlogind uucpd rexecd syslogd tftpd talkd \
	telnet ftp rsh rcp rlogin tftp syslog whois ping talk

DISTCLEANFILES = pathdefs.make paths.defs include/*.h include/*/*.h \
	include/stamp-h


release:
	$(MAKE) dist distdir=$(PACKAGE)`date +"%y%m%d"`
