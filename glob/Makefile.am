AUTOMAKE_OPTIONS = ../ansi2knr

INCLUDES=-I$(top_builddir)/include

noinst_LIBRARIES = @BUILD_LIBGLOB@

EXTRA_LIBRARIES = libglob.a

EXTRA_DIST = configure.bat SMakefile

libglob_a_SOURCES = glob.c glob.h fnmatch.c fnmatch.h

