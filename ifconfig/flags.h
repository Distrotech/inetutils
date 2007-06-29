/* flags.h

   Copyright (C) 2001, 2007 Free Software Foundation, Inc.

   Written by Marcus Brinkmann.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 3
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301 USA.
 */

#ifndef IFCONFIG_FLAGS_H
# define IFCONFIG_FLAGS_H

/* Using these avoid strings with if_flagtoname, the caller can set a
   preference on returned flag names.  If one of the names in the list
   is found for the flag, the search continues to attempt a better
   match.  */
/* FreeBSD */
# define EXPECT_LINK2 ":LINK2/ALTPHYS:ALTPHYS:"
# define EXPECT_ALTPHYS ":LINK2/ALTPHYS:LINK2:"

/* OSF 4.0g */
# define EXPECT_D2 ":D2/SNAP:SNAP:"
# define EXPECT_SNAP ":D2/SNAP:D2:"

/* Return the name corresponding to the interface flag FLAG.
   If FLAG is unknown, return NULL.
   AVOID contains a ':' surrounded and seperated list of flag names
   that should be avoided if alternative names with the same flag value
   exists.  The first unavoided match is returned, or the first avoided
   match if no better is available.  */
const char *if_flagtoname (int flag, const char *avoid);

/* Return the flag mask corresponding to flag name NAME.  If no flag
   with this name is found, return 0.  */
int if_nametoflag (const char *name);

/* Print the flags in FLAGS, using AVOID as in if_flagtoname, and
   SEPERATOR between individual flags.  Returns the number of
   characters printed.  */
int print_if_flags (int flags, const char *avoid, char seperator);

#endif
