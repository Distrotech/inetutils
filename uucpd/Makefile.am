inetdaemondir = @inetdaemondir@

inetdaemon_PROGRAMS = @BUILD_UUCPD@

EXTRA_PROGRAMS = uucpd

uucpd_SOURCES = uucpd.c

@PATHDEFS_MAKE@

INCLUDES = $(PATHDEF_UUCICO) $(PATHDEF_TTY)

LDADD = -L../libinetutils -linetutils
