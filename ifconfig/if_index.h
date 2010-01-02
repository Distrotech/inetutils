/*
  Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009,
  2010 Free Software Foundation, Inc.

  This file is part of GNU Inetutils.

  GNU Inetutils is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or (at
  your option) any later version.

  GNU Inetutils is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see `http://www.gnu.org/licenses/'. */

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
