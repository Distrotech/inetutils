bin_PROGRAMS = rlogind

rlogind_SOURCES = rlogind.c

man_MANS = rlogind.8

@PATHDEFS_MAKE@

INCLUDES = $(PATHDEF_DEV) $(PATHDEF_LOGIN)

LDADD = -L../libinetutils -linetutils

EXTRA_DIST = $(man_MANS)
