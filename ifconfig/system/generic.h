/* generic.h

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

#ifndef IFCONFIG_SYSTEM_GENERIC_H
# define IFCONFIG_SYSTEM_GENERIC_H


/* Option support.  */

# undef SYSTEM_SHORT_OPTIONS
# undef SYSTEM_LONG_OPTIONS

struct system_ifconfig
{
  int valid;
};


/* Output format support.  */

/* Set this to a comma seperated, comma terminated list of struct
   format_handler entries.  They are inserted at the beginning of the
   default list, so they override generic implementations if they have
   the same name.  For example:
   #define SYSTEN_FORMAT_HANDLER { "foobar", system_fh_nothing }, \
   { "newline", system_fh_newline },
   Define some architecture symbol like "foobar", so it can be tested
   for in generic format strings with ${exists?}{foobar?}.  */
# undef SYSTEM_FORMAT_HANDLER

/* Prototype system_fh_* functions here. 
   void system_fh_newline (format_data_t, int, char **);
*/

#endif
