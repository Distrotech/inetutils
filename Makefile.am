AUTOMAKE_OPTIONS = gnits 1.2d

SUBDIRS = headers libinetutils libtelnet \
	inetd telnetd ftpd rshd rlogind uucpd rexecd syslogd tftpd talkd \
	telnet ftp rsh rcp rlogin tftp syslog talk

release:
	$(MAKE) dist distdir=$(PACKAGE)`date +"%y%m%d"`
