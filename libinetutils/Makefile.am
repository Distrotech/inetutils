AUTOMAKE_OPTIONS = ../ansi2knr

noinst_LIBRARIES = libinetutils.a

noinst_HEADERS = argcv.h

EXTRA_DIST = logwtmp.c

libinetutils_a_SOURCES = argcv.c cleansess.c daemon.c des_rw.c kcmd.c	\
	krcmd.c localhost.c logwtmpko.c setsig.c tftpsubs.c ttymsg.c	\
	utmp_init.c utmp_logout.c xalloc_die.c

libinetutils_a_LIBADD = daemon.o $(top_builddir)/lib/libgnu.a

@PATHDEFS_MAKE@

INCLUDES = $(PATHDEF_WTMPX) $(PATHDEF_TTY_PFX) $(PATHDEF_DEVNULL) $(PATHDEF_HEQUIV) -I$(top_srcdir)/headers -I$(top_srcdir)/lib
