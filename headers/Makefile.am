EXTRA_DIST = crypt.h err.h getopt.h obstack.h osockaddr.h paths.h poll.h \
        syslog-int.h tftpsubs.h confpaths.h.in stamp-h.in

header_dirs = arpa protocols

dist-hook:
	here=`cd $(top_builddir)/$(subdir) && pwd`; \
	srcdir=`cd $(srcdir) && pwd`; \
	distdir=`cd $(distdir) && pwd`; \
	for dir in $(header_dirs); \
	do \
            cd $$srcdir;\
            mkdir $$distdir/$$dir;\
	    cd $$dir;\
	    for file in DISTFILES `cat DISTFILES`; do \
	        d=$$srcdir/$$dir; \
	        if test -d $$d/$$file; then \
                    cp -pr $$d/$$file $$distdir/$$dir/$$file; \
		else \
	            test -f $$distdir/$$dir/$$file \
                        || cp -p $$d/$$file $$distdir/$$dir/$$file || exit; \
                fi; \
	    done;\
        done;\
	cd $$here
