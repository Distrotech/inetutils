bin_PROGRAMS = @BUILD_WHOIS@

EXTRA_PROGRAMS = whois

whois_SOURCES = main.c net.c

INCLUDES = -DSYSCONFDIR=\"$(sysconfdir)\" -DDATADIR=\"$(datadir)\"

inetdatadir = $(datadir)/inet

inetdata_DATA = whois-servers

EXTRA_DIST = whois-servers
