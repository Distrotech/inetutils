AUTOMAKE_OPTIONS = ../ansi2knr
OMIT_DEPENDENCIES=crypt.h err.h getopt.h osockaddr.h paths.h poll.h syslog-int.h \
 tftpsubs.h

SUBDIRS=system

bin_PROGRAMS = @ifconfig_BUILD@

EXTRA_PROGRAMS = ifconfig

ifconfig_SOURCES = ifconfig.c flags.c changeif.c options.c printif.c system.c if_index.c

noinst_HEADERS = ifconfig.h if_index.h flags.h options.h printif.h system.h

@PATHDEFS_MAKE@

LDADD = -L../libinetutils -linetutils
