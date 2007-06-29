/* if_index.h

   Copyright (C) 2001, 2007 Free Software Foundation, Inc.

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
   MA 02110-1301 USA. */

#ifndef IF_INDEX_H
# define IF_INDEX_H

# ifndef HAVE_STRUCT_IF_NAMEINDEX
struct if_nameindex
{
  char *if_name;
  int if_index;
};
extern unsigned int if_nametoindex (const char *ifname);
extern char *if_indextoname (unsigned int ifindex, char *ifname);
extern struct if_nameindex *if_nameindex (void);
extern void if_freenameindex (struct if_nameindex *ptr);
# endif

#endif
