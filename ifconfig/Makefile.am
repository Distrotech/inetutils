AUTOMAKE_OPTIONS = ../ansi2knr

bin_PROGRAMS = @BUILD_IFCONFIG@

EXTRA_PROGRAMS = ifconfig

ifconfig_SOURCES = ifconfig.c flags.c changeif.c options.c printif.c system.c

noinst_HEADERS = ifconfig.h flags.h options.h

@PATHDEFS_MAKE@

LDADD = -L../libinetutils -linetutils

