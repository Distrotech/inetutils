AUTOMAKE_OPTIONS = gnits 1.2d

SUBDIRS = @DIRS@
ACLOCAL_M4 = $(top_srcdir)/aclocal.m4
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_CLEAN_FILES = 
DIST_COMMON =  README AUTHORS COPYING ChangeLog INSTALL Makefile.am \
Makefile.in NEWS THANKS TODO acinclude.m4 aclocal.m4 config.guess \
config.sub configure configure.in install-sh missing mkinstalldirs

release:
	$(MAKE) dist distdir=$(PACKAGE)`date +"%y%m%d"`
