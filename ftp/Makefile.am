AUTOMAKE_OPTIONS = ../ansi2knr

bin_PROGRAMS = @BUILD_FTP@

EXTRA_PROGRAMS = ftp

ftp_SOURCES = cmds.c cmdtab.c domacro.c ftp.c main.c ruserpass.c

noinst_HEADERS = extern.h ftp_var.h

man_MANS = ftp.1

@PATHDEFS_MAKE@

INCLUDES = $(PATHDEF_TMP) $(PATHDEF_BSHELL) -I$(top_builddir)/include

LDADD = -L../libinetutils -linetutils $(LIBGLOB) $(LIBTERMCAP)

EXTRA_DIST = $(man_MANS)
