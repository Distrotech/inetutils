EXTRA_DIST = crypt.h err.h getopt.h osockaddr.h paths.h syslog-int.h \
	tftpsubs.h version.h.in confpaths.h.in

dist-hook:
	mkdir $(distdir)/arpa
	cp -p $(srcdir)/arpa/ftp.h $(srcdir)/arpa/telnet.h \
		$(srcdir)/arpa/tftp.h $(distdir)/arpa
	mkdir $(distdir)/protocols
	cp -p $(srcdir)/protocols/talkd.h $(distdir)/protocols/talkd.h
