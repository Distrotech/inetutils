#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdio.h>
#include <errno.h>
#include <string.h>
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
      while (fgets (line, sizeof(line), fp) != NULL)
	{
	  cp = strchr (line, '\n');
	  if (cp != NULL)
	    *cp = '\0';
	  lreply (code, "%s", line);
	}
      (void) fflush (stdout);
      (void) fclose (fp);
      return 0;
    }
  return errno;
}

/*
 * Determine whether something is to happen (allow access, chroot)
 * for a user. Each line is a shell-style glob followed by
 * `yes' or `no'.
 *
 * For backward compatability, `allow' and `deny' are synonymns
 * for `yes' and `no', respectively.
 *
 * Each glob is matched against the username in turn, and the first
 * match found is used. If no match is found, the result is the
 * argument `def'. If a match is found but without and explicit
 * `yes'/`no', the result is the opposite of def.
 *
 * If the file doesn't exist at all, the result is the argument
 * `nofile'
 *
 * Any line starting with `#' is considered a comment and ignored.
 *
 * Returns 0 if the user is denied, or 1 if they are allowed.
 *
 * NOTE: needs struct passwd *pw setup before use.
 */       
/*
 * Check if a user is in the file PATH_FTPUSERS
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
      while (fgets (line, sizeof(line), fp) != NULL)
	{
	  if (line[0] == '#')
	    continue;
	  p = strchr (line, '\n');
	  if (p != NULL)
	    {
	      *p = '\0';
	      if (strcmp (line, name) == 0)
		{
		  found = 1;
		  break;
		}
	    }
	}
      (void) fclose (fp);
    }
  return (found);
}
