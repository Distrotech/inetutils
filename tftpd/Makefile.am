inetdaemondir = @inetdaemondir@

inetdaemon_PROGRAMS = @BUILD_TFTPD@

EXTRA_PROGRAMS = tftpd

tftpd_SOURCES = tftpd.c

man_MANS = tftpd.8

LDADD = -L../libinetutils -linetutils

EXTRA_DIST = $(man_MANS)
