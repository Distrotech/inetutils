AUTOMAKE_OPTIONS = ../ansi2knr

bin_PROGRAMS = @ping_BUILD@

EXTRA_PROGRAMS = ping

man_MANS = ping.8
EXTRA_DIST = $(man_MANS)

LDADD = -L../lib -lgnu -L../libinetutils -linetutils -L../libicmp -licmp
INCLUDES = -I$(top_srcdir)/libicmp

ping_SOURCES = ping.c \
 ping_echo.c \
 ping_address.c \
 ping_router.c \
 ping_timestamp.c \
 ping_impl.h

SUIDMODE = -o root -m 4775

install-ping-hook:
	-@$(INSTALL_PROGRAM) $(bin_PROGRAMS) $(SUIDMODE) $(AM_INSTALL_PROGRAM_FLAGS) $(DESTDIR)$(bindir)/`echo $(bin_PROGRAMS)|sed '$(transform)'` ; \
	if test $$? -ne 0; then \
		echo 'INSTALL-ERROR:'; \
		echo 'INSTALL-ERROR: THIS PROGRAM($(bin_PROGRAMS)) MUST BE INSTALLED SETUID ROOT'; \
		echo 'INSTALL-ERROR: $(INSTALL_PROGRAM) $(bin_PROGRAMS) $(SUIDMODE) $(DESTDIR)$(bindir)/$(bin_PROGRAMS)'; \
		echo 'INSTALL-ERROR:'; \
	fi

install-exec-hook: @ping_INSTALL_HOOK@

# EOF
