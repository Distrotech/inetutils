AUTOMAKE_OPTIONS = ../ansi2knr

bin_PROGRAMS = @BUILD_RLOGIN@

EXTRA_PROGRAMS = rlogin

rlogin_SOURCES = rlogin.c

man_MANS = rlogin.1

LDADD = -L../libinetutils -linetutils

EXTRA_DIST = $(man_MANS)
