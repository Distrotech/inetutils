bin_PROGRAMS = @BUILD_WHOIS@

EXTRA_PROGRAMS = whois

whois_SOURCES = main.c net.c

INCLUDES = -DSYSCONFDIR=\"$(sysconfdir)\" -DDATADIR=\"$(datadir)\"

inetdatadir = $(datadir)/inet

inetdata_DATA = whois-servers

EXTRA_DIST = whois-servers

LDADD = -L../libinetutils -L../libtelnet -linetutils -ltelnet

config.c: ${srcdir}/whois-servers
	echo 'char *whois_servers =' > config.c
	sed < $^ >> config.c -e 's/^/"/' -e 's/$$/\\n"/'
	echo ';' >>config.c
