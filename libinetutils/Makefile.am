noinst_LIBRARIES = libinetutils.a

libinetutils_a_SOURCES = cleansess.c des_rw.c kcmd.c krcmd.c localhost.c \
	logwtmpko.c setsig.c tftpsubs.c ttymsg.c xmalloc.c xstrdup.c \
	xgetcwd.c utmp_init.c utmp_logout.c memcmp.c memcpy.c memmove.c \
	memset.c

libinetutils_a_LIBADD = @LIBOBJS@

noinst_HEADERS = snprintf.h pathmax.h

@PATHDEFS_MAKE@

INCLUDES = $(PATHDEF_TTY_PFX) $(PATHDEF_DEVNULL) $(PATHDEF_HEQUIV)
