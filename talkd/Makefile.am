bin_PROGRAMS = talkd

talkd_SOURCES = announce.c print.c process.c table.c talkd.c

man_MANS = talkd.8

@PATHDEFS_MAKE@

INCLUDES = $(PATHDEF_DEV)

LDADD = -L../libinetutils -linetutils

EXTRA_DIST = $(man_MANS)
