/*
 * Copyright (c) 1983, 1993
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <argp.h>
#include <errno.h>
#include <error.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <progname.h>

#define SYSLOG_NAMES
#include <syslog.h>
#ifndef HAVE_SYSLOG_INTERNAL
# include <syslog-int.h>
#endif

#include "libinetutils.h"

int decode (char *, CODE *);
int pencode (char *);

static char *tag = NULL;
static int logflags = 0;
static int pri = LOG_NOTICE;

ARGP_PROGRAM_DATA_SIMPLE ("logger", "2008");

const char args_doc[] = "[MESSAGE]";
const char doc[] = "Make entries in the system log.";

static struct argp_option argp_options[] = {
#define GRP 0
  {NULL, 'i', NULL, 0, "Log the process id with every line", GRP+1},
#ifdef LOG_PERROR
  {NULL, 's', NULL, 0, "Copy the message to stderr", GRP+1},
#endif
  {"file", 'f', "FILE", 0, "Log the content of FILE", GRP+1},
  {"priority", 'p', "PRI", 0, "Log with priority PRI", GRP+1},
  {"tag", 't', "TAG", 0, "Prepend every line with TAG", GRP+1},
#undef GRP
  {NULL}
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'i':
      logflags |= LOG_PID;
      break;

    case 's':
      logflags |= LOG_PERROR;
      break;

    case 'f':
      if (freopen (arg, "r", stdin) == NULL)
        error (EXIT_FAILURE, errno, "%s", arg);
      break;

    case 'p':
      pri = pencode (arg);
      break;

    case 't':
      tag = arg;
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }

  return 0;
}

static struct argp argp = {argp_options, parse_opt, args_doc, doc};

/* syslog reads from an input and arranges to write the result on the
   system log.  */
int
main (int argc, char *argv[])
{
  char buf[1024];
  int index;

  set_program_name (argv[0]);
  
  /* Parse command line */
  argp_parse (&argp, argc, argv, 0, &index, NULL);

  argc -= index;
  argv += index;

  /* Setup for logging.  */
  openlog (tag ? tag : getlogin (), logflags, 0);
  fclose (stdout);

  /* Log input line if appropriate.  */
  if (argc > 0)
    {
      char *p, *endp;
      int len;

      for (p = buf, endp = buf + sizeof (buf) - 2; *argv;)
	{
	  len = strlen (*argv);
	  if (p + len > endp && p > buf)
	    {
	      syslog (pri, "%s", buf);
	      p = buf;
	    }
	  if (len > sizeof (buf) - 1)
	    syslog (pri, "%s", *argv++);
	  else
	    {
	      if (p != buf)
		*p++ = ' ';
	      memcpy (p, *argv++, len);
	      *(p += len) = '\0';
	    }
	}
      if (p != buf)
	syslog (pri, "%s", buf);
    }
  else
    while (fgets (buf, sizeof (buf), stdin) != NULL)
      syslog (pri, "%s", buf);
  exit (0);
}

/* Decode a symbolic name to a numeric value.  */
int
pencode (char *s)
{
  char *save;
  int fac, lev;

  for (save = s; *s && *s != '.'; ++s);
  if (*s)
    {
      *s = '\0';
      fac = decode (save, facilitynames);
      if (fac < 0)
        error (EXIT_FAILURE, 0, "unknown facility name: %s", save);

      *s++ = '.';
    }
  else
    {
      fac = 0;
      s = save;
    }
  lev = decode (s, prioritynames);
  if (lev < 0)
    error (EXIT_FAILURE, 0, "unknown priority name: %s", save);

  return ((lev & LOG_PRIMASK) | (fac & LOG_FACMASK));
}

int
decode (char *name, CODE * codetab)
{
  CODE *c;

  if (isdigit (*name))
    return (atoi (name));

  for (c = codetab; c->c_name; c++)
    if (!strcasecmp (name, c->c_name))
      return (c->c_val);

  return (-1);
}
