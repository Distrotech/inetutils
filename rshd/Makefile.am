AUTOMAKE_OPTIONS = ../ansi2knr

inetdaemondir = @inetdaemondir@

inetdaemon_PROGRAMS = @rshd_BUILD@

EXTRA_PROGRAMS = rshd

rshd_SOURCES = rshd.c

man_MANS = rshd.8

@PATHDEFS_MAKE@

INCLUDES = $(PATHDEF_BSHELL) $(PATHDEF_NOLOGIN) $(PATHDEF_DEFPATH)

LDADD = -L../lib -lgnu -L../libinetutils -linetutils $(LIBUTIL) @LIBCRYPT@ @LIBAUTH@

EXTRA_DIST = $(man_MANS)
