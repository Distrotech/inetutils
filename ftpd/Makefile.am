inetdaemondir = @inetdaemondir@

inetdaemon_PROGRAMS = @BUILD_FTPD@

EXTRA_PROGRAMS = ftpd

ftpd_SOURCES = ftpcmd.y ftpd.c logwtmp.c popen.c

noinst_HEADERS = extern.h

man_MANS = ftpd.8

@PATHDEFS_MAKE@

INCLUDES = $(PATHDEF_FTPWELCOME) $(PATHDEF_FTPUSERS) \
	$(PATHDEF_FTPLOGINMESG) $(PATHDEF_DEVNULL) \
	$(PATHDEF_NOLOGIN) $(PATHDEF_BSHELL)

LDADD = -L../libls -lls -L../libinetutils -L../libtelnet -linetutils -ltelnet

EXTRA_DIST = $(man_MANS)
