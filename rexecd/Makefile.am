AUTOMAKE_OPTIONS = ../ansi2knr

inetdaemondir = @inetdaemondir@

inetdaemon_PROGRAMS = @BUILD_REXECD@

EXTRA_PROGRAMS = rexecd

rexecd_SOURCES = rexecd.c

man_MANS = rexecd.8

@PATHDEFS_MAKE@

INCLUDES = $(PATHDEF_DEFPATH) $(PATHDEF_BSHELL)

LIBCRYPT = @LIBCRYPT@

LDADD = -L../libinetutils -linetutils $(LIBCRYPT)

EXTRA_DIST = $(man_MANS)
