AUTOMAKE_OPTIONS = ../ansi2knr
OMIT_DEPENDENCIES=crypt.h err.h getopt.h osockaddr.h paths.h poll.h syslog-int.h \
 tftpsubs.h

bin_PROGRAMS = ping

LDADD = -L../libinetutils -linetutils -lm -L../libicmp -licmp
INCLUDES = -I$(top_srcdir)/libicmp

ping_SOURCES = ping.c \
 ping_echo.c \
 ping_address.c \
 ping_router.c \
 ping_timestamp.c \
 ping_impl.h

