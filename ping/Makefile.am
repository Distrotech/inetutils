AUTOMAKE_OPTIONS = ../ansi2knr

bin_PROGRAMS = ping

LDADD = -L../libinetutils -linetutils -lm -L../libicmp -licmp
INCLUDES = -I$(top_srcdir)/libicmp

ping_SOURCES = ping.c \
 ping_echo.c \
 ping_address.c \
 ping_router.c \
 ping_timestamp.c \
 ping_impl.h

