bin_PROGRAMS = @BUILD_INETD@

EXTRA_PROGRAMS = inetd

inetd_SOURCES = inetd.c

man_MANS = inetd.8

@PATHDEFS_MAKE@

INCLUDES = $(PATHDEF_INETDCONF)

LDADD = -L../libinetutils -linetutils

EXTRA_DIST = $(man_MANS)
