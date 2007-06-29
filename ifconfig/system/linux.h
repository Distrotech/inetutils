/* linux.h

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

#ifndef IFCONFIG_SYSTEM_LINUX_H
# define IFCONFIG_SYSTEM_LINUX_H

# include "../printif.h"
# include "../options.h"


/* Option support.  */

struct system_ifconfig
{
  int valid;
# define IF_VALID_TXQLEN 0x1
  int txqlen;
};

# define SYSTEM_LONG_OPTIONS \
  {"txqlen",         required_argument,      0,      'T'},


/* Output format support.  */

# define SYSTEM_FORMAT_HANDLER \
  {"linux", fh_nothing}, \
  {"hwaddr?", system_fh_hwaddr_query}, \
  {"hwaddr", system_fh_hwaddr}, \
  {"hwtype?", system_fh_hwtype_query}, \
  {"hwtype", system_fh_hwtype}, \
  {"txqlen?", system_fh_txqlen_query}, \
  {"txqlen", system_fh_txqlen},

/* The RX/TX statistics would have to be parsed from /proc/net/dev,
   which is in an insanely broken format, even for Linux standards.
   Parsing correctly from it in all cases is impossible because device
   names are truncated to six characters (linux-2.4/net/core/dev.c).
   Nevertheless it can be attempted in the future (check for unique
   abbreviations of interfaces with long names).  Aliases are not listed
   there, instead, the aliases statistics are collected in the real
   inetrface. See ipchains(1) for how to get them. */

void system_fh_hwaddr_query (format_data_t form, int argc, char *argv[]);
void system_fh_hwaddr (format_data_t form, int argc, char *argv[]);
void system_fh_hwtype_query (format_data_t form, int argc, char *argv[]);
void system_fh_hwtype (format_data_t form, int argc, char *argv[]);
void system_fh_txqlen_query (format_data_t form, int argc, char *argv[]);
void system_fh_txqlen (format_data_t form, int argc, char *argv[]);

#endif
