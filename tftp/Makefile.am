AUTOMAKE_OPTIONS = ../ansi2knr

INCLUDES = -I$(top_srcdir)/headers -I$(top_srcdir)/libinetutils -I$(top_srcdir)/lib

bin_PROGRAMS = @tftp_BUILD@

EXTRA_PROGRAMS = tftp

tftp_SOURCES = main.c tftp.c

noinst_HEADERS = extern.h

man_MANS = tftp.1

LDADD = -L../lib -lgnu -L$(top_builddir)/libinetutils -linetutils

EXTRA_DIST = $(man_MANS)
