/* This file is part of GNU inetutils
   Copyright (C) 2006, 2007, 2008 Free Software Foundation, Inc.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General
   Public License along with this library; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301 USA */

#include "config.h"

void utmp_init (char *line, char *user, char *id);
char *utmp_ptsid (char *line, char *tag);
void utmp_logout (char *line);
char *localhost (void);
void logwtmp (const char *, const char *, const char *);
void cleanup_session (char *tty, int pty_fd);
void logwtmp_keep_open (char *line, char *name, char *host);

/* Convenience macro for argp.  */
#define ARGP_PROGRAM_DATA_SIMPLE(name, year)				\
  const char *argp_program_bug_address = "<" PACKAGE_BUGREPORT ">";	\
  const char *argp_program_version =					\
    name " (" PACKAGE_NAME ") " PACKAGE_VERSION "\n"			\
    "\n"								\
    "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n" \
    "This is free software: you are free to change and redistribute it.\n" \
    "There is NO WARRANTY, to the extent permitted by law.\n\n"

#define ARGP_PROGRAM_DATA(name, year, authors)				\
	ARGP_PROGRAM_DATA_SIMPLE(name, year)                            \
        "Written by " authors "."
