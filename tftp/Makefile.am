AUTOMAKE_OPTIONS = ../ansi2knr

bin_PROGRAMS = @BUILD_TFTP@

EXTRA_PROGRAMS = tftp

tftp_SOURCES = main.c tftp.c

noinst_HEADERS = extern.h

man_MANS = tftp.1

LDADD = -L../libinetutils -linetutils

EXTRA_DIST = $(man_MANS)
