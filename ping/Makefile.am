AUTOMAKE_OPTIONS = ../ansi2knr
OMIT_DEPENDENCIES=crypt.h err.h getopt.h osockaddr.h paths.h poll.h syslog-int.h \
 tftpsubs.h

bin_PROGRAMS = @BUILD_PING@
EXTRA_PROGRAMS=ping

man_MANS = ping.8

LDADD = -L../libinetutils -linetutils -lm -L../libicmp -licmp
INCLUDES = -I$(top_srcdir)/libicmp

ping_SOURCES = ping.c \
 ping_echo.c \
 ping_address.c \
 ping_router.c \
 ping_timestamp.c \
 ping_impl.h

SUIDMODE = -o root -m 4775

install-exec-hook:
	-@$(INSTALL_PROGRAM) $(bin_PROGRAMS) $(SUIDMODE) $(AM_INSTALL_PROGRAM_FLAGS) $(DESTDIR)$(bindir)/`echo $(bin_PROGRAMS)|sed '$(transform)'` ; \
	if test $$? -ne 0; then \
		echo 'INSTALL-ERROR:'; \
		echo 'INSTALL-ERROR: THIS PROGRAM($(bin_PROGRAMS)) MUST BE INSTALLED SETUID ROOT'; \
		echo 'INSTALL-ERROR: $(INSTALL_PROGRAM) $(bin_PROGRAMS) $(SUIDMODE) $(DESTDIR)$(bindir)/$(bin_PROGRAMS)'; \
		echo 'INSTALL-ERROR:'; \
	fi
