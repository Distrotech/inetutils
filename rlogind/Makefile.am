inetdaemondir = @inetdaemondir@

inetdaemon_PROGRAMS = @BUILD_RLOGIND@

EXTRA_PROGRAMS = rlogind

rlogind_SOURCES = rlogind.c

man_MANS = rlogind.8

@PATHDEFS_MAKE@

INCLUDES = $(PATHDEF_DEV) $(PATHDEF_LOGIN)

LIBUTIL = @LIBUTIL@

LDADD = -L../libinetutils -linetutils $(LIBUTIL)

EXTRA_DIST = $(man_MANS)
