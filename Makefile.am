AUTOMAKE_OPTIONS = 1.2d

EXTRA_DIST = README-alpha paths ChangeLog.0

SUBDIRS = headers libinetutils libtelnet glob \
	inetd telnetd libls ftpd rshd rlogind uucpd rexecd syslogd tftpd \
	talkd telnet ftp rsh rcp rlogin tftp logger whois talk

DISTCLEANFILES = pathdefs.make paths.defs include/*.h include/*/*.h \
	include/stamp-h

AUTOHEADER = @AUTOHEADER@ -l headers

release:
	$(MAKE) dist distdir=$(PACKAGE)`date +"%y%m%d"`
