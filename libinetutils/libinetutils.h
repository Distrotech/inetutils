/* This file is part of GNU inetutils
   Copyright (C) 2006, 2007 Free Software Foundation, Inc.

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
char *localhost (void);
void logwtmp (const char *, const char *, const char *);
void cleanup_session (char *tty, int pty_fd);

/* Convenience macro for argp.  */
#define ARGP_PROGRAM_DATA(name, year, authors)				\
  const char *argp_program_bug_address = "<" PACKAGE_BUGREPORT ">";	\
  const char *argp_program_version =					\
    name " (" PACKAGE_NAME ") " PACKAGE_VERSION "\n"			\
    "Copyright (C) " year " Free Software Foundation, Inc.\n"		\
    "This is free software.  You may redistribute copies of it under the terms of\n" \
    "the GNU General Public License <http://www.gnu.org/licenses/gpl.html>.\n" \
    "There is NO WARRANTY, to the extent permitted by law.\n"		\
    "\n"								\
    "Written by " authors ".";

