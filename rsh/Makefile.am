bin_PROGRAMS = rsh

rsh_SOURCES = rsh.c

man_MANS = rsh.1

@PATHDEFS_MAKE@

INCLUDES = $(PATHDEF_RLOGIN)

LDADD = -L../libinetutils -linetutils

EXTRA_DIST = $(man_MANS)
