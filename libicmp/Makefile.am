AUTOMAKE_OPTIONS = ../ansi2knr
OMIT_DEPENDENCIES=crypt.h err.h getopt.h osockaddr.h paths.h poll.h syslog-int.h \
 tftpsubs.h

noinst_LIBRARIES = libicmp.a

libicmp_a_SOURCES = libping.c \
 icmp_echo.c \
 icmp_timestamp.c \
 icmp_address.c \
 icmp_cksum.c

noinst_HEADERS = icmp.h ping.h 
