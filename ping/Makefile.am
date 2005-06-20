AUTOMAKE_OPTIONS = ../ansi2knr

bin_PROGRAMS = @ping_BUILD@ @ping6_BUILD@

EXTRA_PROGRAMS = ping ping6

man_MANS = ping.8
EXTRA_DIST = $(man_MANS)

ping_LDADD = -L../lib -lgnu -L../libinetutils -linetutils -L../libicmp -licmp
ping6_LDADD = -L../lib -lgnu -L../libinetutils -linetutils
INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/libicmp 

ping_SOURCES = ping.c ping_common.c ping_echo.c ping_address.c \
  ping_router.c ping_timestamp.c ping_common.h  ping_impl.h

ping6_SOURCES = ping6.c ping_common.c ping_common.h ping6.h

SUIDMODE = -o root -m 4775

install-ping-hook:
	-@for program in $(bin_PROGRAMS); do \
	$(INSTALL_PROGRAM) $$program $(SUIDMODE) $(AM_INSTALL_PROGRAM_FLAGS) $(DESTDIR)$(bindir)/`echo $$program|sed '$(transform)'` ; \
	if test $$? -ne 0; then \
		echo 'INSTALL-ERROR:'; \
		echo 'INSTALL-ERROR: THIS PROGRAM('$$program') MUST BE INSTALLED SETUID ROOT'; \
		echo 'INSTALL-ERROR: $(INSTALL_PROGRAM) '$$program' $(SUIDMODE) $(DESTDIR)$(bindir)/'$$program; \
		echo 'INSTALL-ERROR:'; \
	fi \
	done

install-exec-hook: @ping_INSTALL_HOOK@

# EOF
