/*
  Copyright (C) 1993, 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001,
  2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010 Free Software
  Foundation, Inc.

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

/*
 * Copyright (c) 1985, 1993
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
 * 3. Neither the name of the University nor the names of its contributors
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

/* This code is derived from software contributed to Berkeley by Rick
   Adams. */

/*
 * 4.2BSD TCP/IP server for uucico
 * uucico's TCP channel causes this server to be run at the remote end.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include <pwd.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_CRYPT_H
# include <crypt.h>
#endif
#ifdef HAVE_TERMIOS_H
# include <termios.h>
#endif
#ifdef HAVE_UTMP_H
# include <utmp.h>
#endif

#include <argp.h>
#include <progname.h>
#include <libinetutils.h>

void dologin ();

struct sockaddr_in hisctladdr;
int hisaddrlen = sizeof hisctladdr;
struct sockaddr_in myctladdr;
int mypid;

char Username[64];
char *nenv[] = {
  Username,
  NULL,
};
extern char **environ;

static struct argp argp =
  {
    NULL,
    NULL,
    NULL,
    "TCP/IP server for uucico"
  };

int
main (int argc, char **argv)
{
  register int s;
  struct servent *sp;
  void dologout ();

  set_program_name (argv[0]);
  iu_argp_init ("uucpd", default_program_authors);
  argp_parse (&argp, argc, argv, 0, NULL, NULL);

  environ = nenv;
  sp = getservbyname ("uucp", "tcp");
  if (sp == NULL)
    {
      perror ("uucpd: getservbyname");
      exit (1);
    }
  if (fork ())
    exit (0);
  if ((s = open (PATH_TTY, O_RDWR)) >= 0)
    {
      ioctl (s, TIOCNOTTY, (char *) 0);
      close (s);
    }

  bzero ((char *) &myctladdr, sizeof (myctladdr));
  myctladdr.sin_family = AF_INET;
  myctladdr.sin_port = sp->s_port;
}

static int
readline (register char *p, register int n)
{
  char c;

  while (n-- > 0)
    {
      if (read (0, &c, 1) <= 0)
	return (-1);
      c &= 0177;
      if (c == '\n' || c == '\r')
	{
	  *p = '\0';
	  return (0);
	}
      *p++ = c;
    }
  return (-1);
}

void
doit (struct sockaddr_in *sinp)
{
  struct passwd *pw, *getpwnam ();
  char user[64], passwd[64];
  char *xpasswd;

  alarm (60);
  printf ("login: ");
  fflush (stdout);
  if (readline (user, sizeof user) < 0)
    {
      fprintf (stderr, "user read\n");
      return;
    }
  /* truncate username to 8 characters */
  user[8] = '\0';
  pw = getpwnam (user);
  if (pw == NULL)
    {
      fprintf (stderr, "user unknown\n");
      return;
    }
  if (strcmp (pw->pw_shell, PATH_UUCICO))
    {
      fprintf (stderr, "Login incorrect.");
      return;
    }
  if (pw->pw_passwd && *pw->pw_passwd != '\0')
    {
      printf ("Password: ");
      fflush (stdout);
      if (readline (passwd, sizeof passwd) < 0)
	{
	  fprintf (stderr, "passwd read\n");
	  return;
	}
      xpasswd = crypt (passwd, pw->pw_passwd);
      if (strcmp (xpasswd, pw->pw_passwd))
	{
	  fprintf (stderr, "Login incorrect.");
	  return;
	}
    }
  alarm (0);
  sprintf (Username, "USER=%s", user);
  dologin (pw, sinp);
  setgid (pw->pw_gid);
  chdir (pw->pw_dir);
  setuid (pw->pw_uid);
  perror ("uucico server: execl");
}

void
dologout ()
{
  int pid;

#ifdef HAVE_WAITPID
  while ((pid = waitpid (-1, 0, WNOHANG)) > 0)
#else
# ifdef HAVE_WAIT3
  while ((pid = wait3 (0, WNOHANG, 0)) > 0)
# else
  while ((pid = wait (0)) > 0)
# endif	/* HAVE_WAIT3 */
#endif /* HAVE_WAITPID */
    {
      char line[100];
      sprintf (line, "uucp%.4d", pid);
      logwtmp (line, "", "");
    }
}

/*
 * Record login in wtmp file.
 */
void
dologin (struct passwd *pw, struct sockaddr_in *sin)
{
  char line[32];
  char remotehost[32];
  int f;
  struct hostent *hp = gethostbyaddr ((char *) &sin->sin_addr,
				      sizeof (struct in_addr), AF_INET);

  if (hp)
    {
      strncpy (remotehost, hp->h_name, sizeof (remotehost));
      endhostent ();
    }
  else
    strncpy (remotehost, inet_ntoa (sin->sin_addr), sizeof (remotehost));

  sprintf (line, "uucp%.4d", getpid ());

  logwtmp (line, pw->pw_name, remotehost);

#if defined (PATH_LASTLOG) && defined (HAVE_STRUCT_LASTLOG)
# define SCPYN(a, b)	strncpy(a, b, sizeof (a))
  f = open (PATH_LASTLOG, O_RDWR);
  if (f >= 0)
    {
      struct lastlog ll;
      time_t t;

      time (&t);
      ll.ll_time = t;
      lseek (f, (long) pw->pw_uid * sizeof (struct lastlog), 0);
      strcpy (line, remotehost);
      SCPYN (ll.ll_line, line);
      SCPYN (ll.ll_host, remotehost);
      write (f, (char *) &ll, sizeof ll);
      close (f);
    }
#endif
}
