bin_PROGRAMS = @BUILD_RCP@

EXTRA_PROGRAMS = rcp

rcp_SOURCES = rcp.c util.c

noinst_HEADERS = extern.h

man_MANS = rcp.1

@PATHDEFS_MAKE@

INCLUDES = $(PATHDEF_RSH) $(PATHDEF_CP) $(PATHDEF_BSHELL)

LDADD = -L../libinetutils -linetutils

EXTRA_DIST = $(man_MANS)
