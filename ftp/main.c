/*
 * Copyright (c) 1985, 1989, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008
   Free Software Foundation, Inc.

   This file is part of GNU Inetutils.

   GNU Inetutils is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GNU Inetutils is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GNU Inetutils; see the file COPYING.  If not, write
   to the Free Software Foundation, Inc., 51 Franklin Street,
   Fifth Floor, Boston, MA 02110-1301 USA. */

/*
 * FTP User Program -- Command Interface.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/*#include <sys/ioctl.h>*/
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/ftp.h>

#include <argp.h>
#include <ctype.h>
#include <error.h>
#include <netdb.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Define macro to nothing so declarations in ftp_var.h become definitions. */
#define FTP_EXTERN
#include "ftp_var.h"

#include "libinetutils.h"

#if HAVE_LIBREADLINE
#  include <readline/readline.h>
#else
#  include "readline.h"
#endif


#define DEFAULT_PROMPT "ftp> "
static char *prompt = 0;

ARGP_PROGRAM_DATA_SIMPLE ("ftp", "2007");

const char args_doc[] = "[HOST [PORT]]";
const char doc[] = "Remote file transfer.";

static struct argp_option argp_options[] = {
#define GRP 0
  {"debug", 'd', NULL, 0, "Set the SO_DEBUG option", GRP+1},
  {"no-glob", 'g', NULL, 0, "Turn off file name globbing", GRP+1},
  {"no-prompt", 'i', NULL, 0, "Don't prompt during multiple file transfers",
   GRP+1},
  {"no-login", 'n', NULL, 0, "Don't automatically login to the remote system",
   GRP+1},
  {"trace", 't', NULL, 0, "Enable packet tracing", GRP+1},
  {"prompt", 'p', "PROMPT", OPTION_ARG_OPTIONAL, "Print a command line PROMPT "
   "(optionally), even if not on a tty", GRP+1},
  {"verbose", 'v', NULL, 0, "Verbose output", GRP+1},
#undef GRP
  {NULL}
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'd':		/* Enable debug mode.  */
      options |= SO_DEBUG;
      debug++;
      break;

    case 'g':		/* No glob.  */
      doglob = 0;
      break;

    case 'i':		/* No prompt.  */
      interactive = 0;
      break;

    case 'n':		/* No automatic login.  */
      autologin = 0;
      break;

    case 't':		/* Enable packet tracing.  */
      trace++;
      break;

    case 'v':		/* Verbose.  */
      verbose++;
      break;

    case 'p':		/* Print command line prompt.  */
      prompt = arg ? arg : DEFAULT_PROMPT;
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }

  return 0;
}

static struct argp argp = {argp_options, parse_opt, args_doc, doc};


int
main (int argc, char *argv[])
{
  int top;
  int index;
  struct passwd *pw = NULL;
  char *cp;

  set_program_name (argv[0]);
  
  sp = getservbyname ("ftp", "tcp");
  if (sp == 0)
    error (EXIT_FAILURE, 0, "ftp/tcp: unknown service");
  doglob = 1;
  interactive = 1;
  autologin = 1;

  /* Parse command line */
  argp_parse (&argp, argc, argv, 0, &index, NULL);

  argc -= index;
  argv += index;

  fromatty = isatty (fileno (stdin));
  if (fromatty)
    {
      verbose++;
      if (!prompt)
	prompt = DEFAULT_PROMPT;
    }

  cpend = 0;			/* no pending replies */
  proxy = 0;			/* proxy not active */
  passivemode = 0;		/* passive mode not active */
  crflag = 1;			/* strip c.r. on ascii gets */
  sendport = -1;		/* not using ports */
  /*
   * Set up the home directory in case we're globbing.
   */
  cp = getlogin ();
  if (cp != NULL)
    pw = getpwnam (cp);
  if (pw == NULL)
    pw = getpwuid (getuid ());
  if (pw != NULL)
    {
      char *buf = malloc (strlen (pw->pw_dir) + 1);
      if (buf)
	{
	  strcpy (buf, pw->pw_dir);
	  home = buf;
	}
    }
  if (argc > 0)
    {
      char *xargv[5];

      if (setjmp (toplevel))
	exit (EXIT_SUCCESS);
      signal (SIGINT, intr);
      signal (SIGPIPE, lostpeer);
      xargv[0] = program_invocation_name;
      xargv[1] = argv[0];
      xargv[2] = argv[1];
      xargv[3] = argv[2];
      xargv[4] = NULL;
      setpeer (argc + 1, xargv);
    }
  top = setjmp (toplevel) == 0;
  if (top)
    {
      signal (SIGINT, intr);
      signal (SIGPIPE, lostpeer);
    }
  for (;;)
    {
      cmdscanner (top);
      top = 1;
    }
}

RETSIGTYPE
intr (int sig ARG_UNUSED)
{
  longjmp (toplevel, 1);
}

RETSIGTYPE
lostpeer (int sig ARG_UNUSED)
{
  if (connected)
    {
      if (cout != NULL)
	{
	  shutdown (fileno (cout), 1 + 1);
	  fclose (cout);
	  cout = NULL;
	}
      if (data >= 0)
	{
	  shutdown (data, 1 + 1);
	  close (data);
	  data = -1;
	}
      connected = 0;
    }
  pswitch (1);
  if (connected)
    {
      if (cout != NULL)
	{
	  shutdown (fileno (cout), 1 + 1);
	  fclose (cout);
	  cout = NULL;
	}
      connected = 0;
    }
  proxflag = 0;
  pswitch (0);
}

/*
char *
tail(filename)
	char *filename;
{
	char *s;

	while (*filename) {
		s = strrchr(filename, '/');
		if (s == NULL)
			break;
		if (s[1])
			return (s + 1);
		*s = '\0';
	}
	return (filename);
}
*/

/*
 * Command parser.
 */
void
cmdscanner (int top)
{
  struct cmd *c;
  int l;

  if (!top)
    putchar ('\n');
  for (;;)
    {
      if (line)
	{
	  free (line);
	  line = NULL;
	}
      line = readline (prompt);
      if (!line)
	quit (0, 0);
      l = strlen (line);
      if (l >= MAXLINE)
	{
	  printf ("Line too long.\n");
	  break;
	}

#if HAVE_LIBHISTORY
      if (line && *line)
	add_history (line);
#endif

      if (l == 0)
	break;

      makeargv ();
      if (margc == 0)
	continue;

      c = getcmd (margv[0]);
      if (c == (struct cmd *) -1)
	{
	  printf ("?Ambiguous command\n");
	  continue;
	}
      if (c == 0)
	{
	  printf ("?Invalid command\n");
	  continue;
	}
      if (c->c_conn && !connected)
	{
	  printf ("Not connected.\n");
	  continue;
	}
      (*c->c_handler) (margc, margv);
      if (bell && c->c_bell)
	putchar ('\007');
      if (c->c_handler != help)
	break;
    }
  signal (SIGINT, intr);
  signal (SIGPIPE, lostpeer);
}

/*
 * Slice a string up into argc/argv.
 */

int slrflag;

void
makeargv ()
{
  char **argp;

  margc = 0;
  argp = margv;
  stringbase = line;		/* scan from first of buffer */
  argbase = argbuf;		/* store from first of buffer */
  slrflag = 0;
  while ((*argp++ = slurpstring ()))
    margc++;
}

/*
 * Parse string into argbuf;
 * implemented with FSM to
 * handle quoting and strings
 */
char *
slurpstring ()
{
  int got_one = 0;
  char *sb = stringbase;
  char *ap = argbase;
  char *tmp = argbase;		/* will return this if token found */

  if (*sb == '!' || *sb == '$')
    {				/* recognize ! as a token for shell */
      switch (slrflag)		/* and $ as token for macro invoke */
	{
	case 0:
	  slrflag++;
	  stringbase++;
	  return ((*sb == '!') ? "!" : "$");

	case 1:
	  slrflag++;
	  altarg = stringbase;
	  break;

	default:
	  break;
	}
    }

S0:
  switch (*sb)
    {
    case '\0':
      goto OUT;

    case ' ':
    case '\t':
      sb++;
      goto S0;

    default:
      switch (slrflag)
	{
	case 0:
	  slrflag++;
	  break;

	case 1:
	  slrflag++;
	  altarg = sb;
	  break;

	default:
	  break;
	}
      goto S1;
    }

S1:
  switch (*sb)
    {
    case ' ':
    case '\t':
    case '\0':
      goto OUT;			/* end of token */

    case '\\':
      sb++;
      goto S2;			/* slurp next character */

    case '"':
      sb++;
      goto S3;			/* slurp quoted string */

    default:
      *ap++ = *sb++;		/* add character to token */
      got_one = 1;
      goto S1;
    }

S2:
  switch (*sb)
    {
    case '\0':
      goto OUT;

    default:
      *ap++ = *sb++;
      got_one = 1;
      goto S1;
    }

S3:
  switch (*sb)
    {
    case '\0':
      goto OUT;

    case '"':
      sb++;
      goto S1;

    default:
      *ap++ = *sb++;
      got_one = 1;
      goto S3;
    }

OUT:
  if (got_one)
    *ap++ = '\0';
  argbase = ap;			/* update storage pointer */
  stringbase = sb;		/* update scan pointer */
  if (got_one)
    return (tmp);
  switch (slrflag)
    {
    case 0:
      slrflag++;
      break;

    case 1:
      slrflag++;
      altarg = (char *) 0;
      break;

    default:
      break;
    }
  return ((char *) 0);
}
