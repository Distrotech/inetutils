bin_PROGRAMS = telnetd

telnetd_SOURCES = authenc.c global.c slc.c state.c sys_term.c telnetd.c termstat.c utility.c

noinst_HEADERS = defs.h ext.h telnetd.h

man_MANS = telnetd.8

@PATHDEFS_MAKE@

INCLUDES = $(PATHDEF_TTY) $(PATHDEF_LOGIN)

LIBTERMCAP = @LIBTERMCAP@
LIBCRYPT = @LIBCRYPT@
LIBUTIL = @LIBUTIL@

LDADD = -L../libinetutils -L../libtelnet -linetutils -ltelnet \
	$(LIBTERMCAP) $(LIBUTIL) $(LIBCRYPT)
