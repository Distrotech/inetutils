/*
  Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008,
  2009, 2010, 2011, 2012, 2013 Free Software Foundation, Inc.

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
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include "extern.h"

#ifndef LINE_MAX
# define LINE_MAX 2048
#endif

int
display_file (const char *name, int code)
{
  char *cp, line[LINE_MAX];
  FILE *fp = fopen (name, "r");
  if (fp != NULL)
    {
      while (fgets (line, sizeof (line), fp) != NULL)
	{
	  cp = strchr (line, '\n');
	  if (cp != NULL)
	    *cp = '\0';
	  lreply (code, "%s", line);
	}
      fflush (stdout);
      fclose (fp);
      return 0;
    }
  return errno;
}

/*
 * Check if a user is in the file `filename',
 * typically PATH_FTPUSERS or PATH_FTPCHROOT.
 * Return 1 if yes, 0 otherwise.
 */
int
checkuser (const char *filename, const char *name)
{
  FILE *fp;
  int found = 0;
  char *p, line[BUFSIZ];

  fp = fopen (filename, "r");
  if (fp != NULL)
    {
      while (fgets (line, sizeof (line), fp) != NULL)
	{
	  /* Properly terminate input.  */
	  p = strchr (line, '\n');
	  if (p != NULL)
	    *p = '\0';

	  /* Disregard initial blank characters.  */
	  p = line;
	  while (isblank (*p))
	    p++;

	  /* Skip comments, and empty lines.  */
	  if (*p == '#' || *p == 0)
	    continue;

	  /* User name ends at the first blank character.  */
	  if (strncmp (p, name, strlen (name)) == 0
	      && (p[strlen (name)] == 0
		  || isblank (p[strlen (name)])))
	    {
	      found = 1;
	      break;
	    }
	}
      fclose (fp);
    }
  return (found);
}
