/*
  Copyright (C) 2007, 2008, 2009, 2010, 2011 Free Software Foundation,
  Inc.

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

#include <config.h>

#include "libinetutils.h"

#include <stdio.h>
#include <progname.h>

int
main (int argc, char **argv)
{
  const char *p = localhost ();
  set_program_name (argv[0]);
  if (!p)
    return 1;

  printf ("localhost: %s\n", p);

  return 0;
}
