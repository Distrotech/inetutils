bin_PROGRAMS = @BUILD_WHOIS@

EXTRA_PROGRAMS = whois

whois_SOURCES = main.c net.c config.c

INCLUDES = -DSYSCONFDIR=\"$(sysconfdir)\" -DDATADIR=\"$(datadir)\"

inetdatadir = $(datadir)/inet

inetdata_DATA = whois-servers

EXTRA_DIST = whois-servers gen-config-c

LDADD = -L../libinetutils -L../libtelnet -linetutils -ltelnet

config.c: gen-config-c whois-servers
	cd $(srcdir) && ./gen-config-c
