bin_PROGRAMS = inetd

inetd_SOURCES = inetd.c

man_MANS = inetd.8

@PATHDEFS_MAKE@

INCLUDES = $(PATHDEF_INETDCONF)

LDADD = -L../libinetutils -linetutils
