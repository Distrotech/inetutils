noinst_LIBRARIES = libinetutils.a

libinetutils_a_SOURCES = cleansess.c des_rw.c kcmd.c krcmd.c localhost.c \
	logwtmpko.c setsig.c tftpsubs.c ttymsg.c xmalloc.c xstrdup.c

libinetutils_a_LIBADD = @LIBOBJS@

noinst_HEADERS = snprintf.h

@PATHDEFS_MAKE@

INCLUDES = $(PATHDEF_TTY_PFX) $(PATHDEF_DEVNULL) $(PATHDEF_HEQUIV)
