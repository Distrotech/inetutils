AUTOMAKE_OPTIONS = ../ansi2knr

bin_PROGRAMS = @whois_BUILD@

EXTRA_PROGRAMS = whois

whois_SOURCES = main.c net.c

whois_DEPENDENCIES = config.o

CLEANFILES = config.c

INCLUDES = -DSYSCONFDIR=\"$(sysconfdir)\" -DDATADIR=\"$(datadir)\"

inetdatadir = $(datadir)/inet

inetdata_DATA = whois-servers

EXTRA_DIST = whois-servers

LDADD = -L../libinetutils -L../libtelnet -linetutils -ltelnet

whois_LDADD = config.o $(LDADD)

config.c: ${srcdir}/whois-servers
	echo 'char *whois_servers =' > config.c
	sed < $^ >> config.c -e 's/^/"/' -e 's/$$/\\n"/'
	echo ';' >>config.c
