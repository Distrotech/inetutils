/*
  Copyright (C) 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003,
  2004, 2005, 2006, 2007, 2008, 2009, 2010 Free Software Foundation,
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

/*
 * Copyright (c) 1983, 1990, 1993, 1994
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#ifdef HAVE_SYS_FILIO_H
# include <sys/filio.h>
#endif
#include <sys/file.h>
#include <sys/time.h>
#include <time.h>

#include <netinet/in.h>
#include <netdb.h>

#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/select.h>

#include <error.h>
#include <progname.h>
#include <xalloc.h>
#include <argp.h>
#include <libinetutils.h>

int debug_option = 0;
int null_input_option = 0;
char *user = NULL;

#if defined(KERBEROS) || defined(SHISHI)
int use_kerberos = 1, doencrypt;
# ifdef KERBEROS
#  include <kerberosIV/des.h>
#  include <kerberosIV/krb.h>
char *dest_realm = NULL;
CREDENTIALS cred;
Key_schedule schedule;
extern char *krb_realmofhost ();

# elif defined(SHISHI)
#  include <shishi.h>
#  include "shishi_def.h"
char *dest_realm = NULL;

Shishi *h;
Shishi_key *enckey;
shishi_ivector iv1, iv2, iv3, iv4;
shishi_ivector *ivtab[4];

int keytype;
int keylen;
int rc;
int wlen;
# endif
#endif /* KERBEROS */

/*
 * rsh - remote shell
 */
int rfd2;

char *copyargs (char **);
RETSIGTYPE sendsig (int);
void talk (int, sigset_t *, pid_t, int);
void warning (const char *, ...);


const char args_doc[] = "[USER@]HOST [COMMAND [ARG...]]";
const char doc[] = "remote shell";

static struct argp_option options[] = {
  { "debug", 'd', NULL, 0,
    "turns on socket debugging (see setsockopt(2))" },
  { "user", 'l', "USER", 0,
    "run as USER on the remote system" },
  { "escape", 'e', "CHAR", 0,
    "allows user specification of the escape character (``~'' by default)" },
  { "8-bit", '8', NULL, 0,
    "allows an eight-bit input data path at all times" },
  { "no-input", 'n', NULL, 0,
    "use /dev/null as input" },
#if defined(KERBEROS) || defined(SHISHI)
  { "kerberos", 'K', NULL, 0,
    "turns off all Kerberos authentication" },
  { "realm", 'k', NULL, 0,
    "obtain tickets for the remote host in REALM "
    "instead of the remote host's realm" },
  { "encrypt", 'x', NULL, 0,
    "encrypt all data using DES" },
#endif
  { NULL }
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'L':		/* -8Lew are ignored to allow rlogin aliases */
    case 'e':
    case 'w':
    case '8':
      break;

    case 'd':
      debug_option = 1;
      break;

    case 'l':
      user = arg;
      break;

#if defined(KERBEROS) || defined(SHISHI)
    case 'K':
      use_kerberos = 0;
      break;
#endif

#if defined(KERBEROS) || defined(SHISHI)
    case 'k':
      dest_realm = arg;
      break;

# ifdef ENCRYPTION
    case 'x':
      doencrypt = 1;
#  ifdef KERBEROS
      des_set_key (cred.session, schedule);
#  endif
      break;
# endif
#endif

    case 'n':
      null_input_option = 1;
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }

  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };


int
main (int argc, char **argv)
{
  int index;
  struct passwd *pw;
  struct servent *sp;
  sigset_t sigs, osigs;
  int asrsh, rem;
  pid_t pid = 0;
  uid_t uid;
  char *args, *host;

  set_program_name (argv[0]);

  asrsh = 0;
  host = user = NULL;

  /* If called as something other than "rsh", use it as the host name */
  {
    char *p = strrchr (argv[0], '/');
    if (p)
      ++p;
    else
      p = argv[0];
    if (strcmp (p, "rsh"))
      host = p;
    else
      asrsh = 1;
  }

  /* Parse command line */
  iu_argp_init ("rsh", default_program_authors);
  argp_parse (&argp, argc, argv, ARGP_IN_ORDER, &index, NULL);

  if (index < argc)
    host = argv[index++];

  /* To few args.  */
  if (!host)
    error (1, 0, "host not specified");

  /* If no further arguments, must have been called as rlogin. */
  if (!argv[index])
    {
      if (asrsh)
	*argv = (char *) "rlogin";
      seteuid (getuid ());
      setuid (getuid ());
      execv (PATH_RLOGIN, argv);
      error (1, errno, "cannot execute %s", PATH_RLOGIN);
    }

  argc -= index;
  argv += index;

  /* We must be setuid root.  */
  if (geteuid ())
    error (1, 0, "must be setuid root.\n");

  if (!(pw = getpwuid (uid = getuid ())))
    error (1, 0, "unknown user id");

  /* Accept user1@host format, though "-l user2" overrides user1 */
  {
    char *p = strchr (host, '@');
    if (p)
      {
	*p = '\0';
	if (!user && p > host)
	  user = host;
	host = p + 1;
	if (*host == '\0')
	  error (1, 0, "empty host name");
      }
  }

#if defined(KERBEROS) || defined(SHISHI)
# ifdef ENCRYPTION
  /* -x turns off -n */
  if (doencrypt)
    null_input_option = 0;
# endif
#endif

  args = copyargs (argv);

  sp = NULL;
#ifdef KERBEROS
  if (use_kerberos)
    {
      sp = getservbyname ((doencrypt ? "ekshell" : "kshell"), "tcp");
      if (sp == NULL)
	{
	  use_kerberos = 0;
	  warning ("can't get entry for %s/tcp service",
		   doencrypt ? "ekshell" : "kshell");
	}
    }
#elif defined(SHISHI)
  if (use_kerberos)
    {
      sp = getservbyname ("kshell", "tcp");
      if (sp == NULL)
	{
	  use_kerberos = 0;
	  warning ("can't get entry for %s/tcp service", "kshell");
	}
    }
#endif
  if (sp == NULL)
    sp = getservbyname ("shell", "tcp");
  if (sp == NULL)
    error (1, 0, "shell/tcp: unknown service");


#if defined (KERBEROS) || defined(SHISHI)
try_connect:
  if (use_kerberos)
    {
      struct hostent *hp;

      /* fully qualify hostname (needed for krb_realmofhost) */
      hp = gethostbyname (host);
      if (hp != NULL && !(host = strdup (hp->h_name)))
	error (1, errno, "strdup");

# if defined (KERBEROS)
      rem = KSUCCESS;
      errno = 0;
      if (dest_realm == NULL)
	dest_realm = krb_realmofhost (host);
# elif defined (SHISHI)
      rem = SHISHI_OK;
      errno = 0;
# endif

# ifdef ENCRYPTION
      if (doencrypt)
#  if defined(SHISHI)
	{
	  int i;
	  char *term;

	  term = xmalloc (strlen (args) + 4);
	  strcpy (term, "-x ");
	  strcat (term, args);

	  rem = krcmd_mutual (&h, &host, sp->s_port, &user, term, &rfd2,
			      dest_realm, &enckey);
	  if (rem > 0)
	    {
	      keytype = shishi_key_type (enckey);
	      keylen = shishi_cipher_blocksize (keytype);

	      ivtab[0] = &iv1;
	      ivtab[1] = &iv2;
	      ivtab[2] = &iv3;
	      ivtab[3] = &iv4;

	      for (i = 0; i < 4; i++)
		{
		  ivtab[i]->ivlen = keylen;

		  switch (keytype)
		    {
		    case SHISHI_DES_CBC_CRC:
		    case SHISHI_DES_CBC_MD4:
		    case SHISHI_DES_CBC_MD5:
		    case SHISHI_DES_CBC_NONE:
		    case SHISHI_DES3_CBC_HMAC_SHA1_KD:
		      ivtab[i]->keyusage = SHISHI_KEYUSAGE_KCMD_DES;
		      ivtab[i]->iv = malloc (ivtab[i]->ivlen);
		      memset (ivtab[i]->iv,
			      2 * i + 1 * (i < 2) - 4 * (i >= 2),
			      ivtab[i]->ivlen);
		      ivtab[i]->ctx =
			shishi_crypto (h, enckey, ivtab[i]->keyusage,
				       shishi_key_type (enckey), ivtab[i]->iv,
				       ivtab[i]->ivlen);
		      break;
		    case SHISHI_ARCFOUR_HMAC:
		    case SHISHI_ARCFOUR_HMAC_EXP:
		      ivtab[i]->keyusage =
			SHISHI_KEYUSAGE_KCMD_DES + 2 + 4 * i;
		      ivtab[i]->ctx =
			shishi_crypto (h, enckey, ivtab[i]->keyusage,
				       shishi_key_type (enckey), NULL, 0);
		      break;
		    default:
		      ivtab[i]->keyusage =
			SHISHI_KEYUSAGE_KCMD_DES + 2 + 4 * i;
		      ivtab[i]->iv = malloc (ivtab[i]->ivlen);
		      memset (ivtab[i]->iv, 0, ivtab[i]->ivlen);
		      ivtab[i]->ctx =
			shishi_crypto (h, enckey, ivtab[i]->keyusage,
				       shishi_key_type (enckey), ivtab[i]->iv,
				       ivtab[i]->ivlen);
		    }
		}
	    }
	  free (term);
	}
      else
#  else
	rem = krcmd_mutual (&host, sp->s_port, user, args, &rfd2,
			    dest_realm, &cred, schedule);
      else
#  endif
# endif
	rem = krcmd (
# if defined (SHISHI)
		      &h, &host, sp->s_port, &user, args, &rfd2, dest_realm);
# else
		      &host, sp->s_port, user, args, &rfd2, dest_realm);
# endif
      if (rem < 0)
	{
	  use_kerberos = 0;
	  sp = getservbyname ("shell", "tcp");
	  if (sp == NULL)
	    error (1, 0, "shell/tcp: unknown service");
	  if (errno == ECONNREFUSED)
	    warning ("remote host doesn't support Kerberos");
	  if (errno == ENOENT)
	    warning ("can't provide Kerberos auth data");
	  goto try_connect;
	}
    }
  else
    {
      if (!user)
	user = pw->pw_name;
      if (doencrypt)
	error (1, 0, "the -x flag requires Kerberos authentication");
      rem = rcmd (&host, sp->s_port, pw->pw_name, user, args, &rfd2);
    }
#else
  if (!user)
    user = pw->pw_name;
  rem = rcmd (&host, sp->s_port, pw->pw_name, user, args, &rfd2);
#endif

  if (rem < 0)
    exit (1);

  if (rfd2 < 0)
    error (1, 0, "can't establish stderr");

  if (debug_option)
    {
      int one = 1;
      if (setsockopt (rem, SOL_SOCKET, SO_DEBUG, (char *) &one,
		      sizeof one) < 0)
	error (0, errno, "setsockopt");
      if (setsockopt (rfd2, SOL_SOCKET, SO_DEBUG, (char *) &one,
		      sizeof one) < 0)
	error (0, errno, "setsockopt");
    }

  seteuid (uid);
  setuid (uid);
#ifdef HAVE_SIGACTION
  sigemptyset (&sigs);
  sigaddset (&sigs, SIGINT);
  sigaddset (&sigs, SIGQUIT);
  sigaddset (&sigs, SIGTERM);
  sigprocmask (SIG_BLOCK, &sigs, &osigs);
#else
  sigs = sigmask (SIGINT) | sigmask (SIGQUIT) | sigmask (SIGTERM);
  osigs = sigblock (sigs);
#endif
  if (signal (SIGINT, SIG_IGN) != SIG_IGN)
    signal (SIGINT, sendsig);
  if (signal (SIGQUIT, SIG_IGN) != SIG_IGN)
    signal (SIGQUIT, sendsig);
  if (signal (SIGTERM, SIG_IGN) != SIG_IGN)
    signal (SIGTERM, sendsig);

  if (!null_input_option)
    {
      pid = fork ();
      if (pid < 0)
	error (1, errno, "fork");
    }

#if defined(KERBEROS) || defined(SHISHI)
# ifdef ENCRYPTION
  if (!doencrypt)
# endif
#endif
    {
      int one = 1;
      ioctl (rfd2, FIONBIO, &one);
      ioctl (rem, FIONBIO, &one);
    }

  talk (null_input_option, &osigs, pid, rem);

#ifdef SHISHI
  if (use_kerberos)
    {
      int i;

      shishi_done (h);
# ifdef ENCRYPTION
      if (doencrypt)
	{
	  shishi_key_done (enckey);
	  for (i = 0; i < 4; i++)
	    {
	      shishi_crypto_close (ivtab[i]->ctx);
	      free (ivtab[i]->iv);
	    }
	}
# endif
    }
#endif

  if (!null_input_option)
    kill (pid, SIGKILL);
  return 0;
}

void
talk (int null_input_option, sigset_t * osigs, pid_t pid, int rem)
{
  int cc, wc;
  fd_set readfrom, ready, rembits;
  char *bp, buf[BUFSIZ];

  if (!null_input_option && pid == 0)
    {
      close (rfd2);

    reread:
      errno = 0;
      if ((cc = read (STDIN_FILENO, buf, sizeof buf)) <= 0)
	goto done;
      bp = buf;

    rewrite:
      FD_ZERO (&rembits);
      FD_SET (rem, &rembits);
      if (select (rem + 1, 0, &rembits, 0, 0) < 0)
	{
	  if (errno != EINTR)
	    error (1, errno, "select");
	  goto rewrite;
	}
      if (!FD_ISSET (rem, &rembits))
	goto rewrite;
#ifdef ENCRYPTION
# ifdef KERBEROS
      if (doencrypt)
	wc = des_write (rem, bp, cc);
      else
# elif defined(SHISHI)
      if (doencrypt)
	writeenc (h, rem, bp, cc, &wc, &iv3, enckey, 2);
      else
# endif
#endif
	wc = write (rem, bp, cc);
      if (wc < 0)
	{
	  if (errno == EWOULDBLOCK)
	    goto rewrite;
	  goto done;
	}
      bp += wc;
      cc -= wc;
      if (cc == 0)
	goto reread;
      goto rewrite;
    done:
      shutdown (rem, 1);
      exit (0);
    }

#ifdef HAVE_SIGACTION
  sigprocmask (SIG_SETMASK, osigs, NULL);
#else
  sigsetmask (*osigs);
#endif
  FD_ZERO (&readfrom);
  FD_SET (rfd2, &readfrom);
  FD_SET (rem, &readfrom);
  do
    {
      int maxfd = rem;
      if (rfd2 > maxfd)
	maxfd = rfd2;
      ready = readfrom;
      if (select (maxfd + 1, &ready, 0, 0, 0) < 0)
	{
	  if (errno != EINTR)
	    error (1, errno, "select");
	  continue;
	}
      if (FD_ISSET (rfd2, &ready))
	{
	  errno = 0;
#ifdef KERBEROS
# ifdef CRYPT
	  if (doenencryption)
	    cc = des_read (rfd2, buf, sizeof buf);
	  else
# endif
#elif defined(SHISHI) && defined(ENCRYPTION)
	  if (doencrypt)
	    readenc (h, rfd2, buf, &cc, &iv2, enckey, 2);
	  else
#endif
	    cc = read (rfd2, buf, sizeof buf);
	  if (cc <= 0)
	    {
	      if (errno != EWOULDBLOCK)
		FD_CLR (rfd2, &readfrom);
	    }
	  else
	    write (2, buf, cc);
	}
      if (FD_ISSET (rem, &ready))
	{
	  errno = 0;
#ifdef KERBEROS
# ifdef ENCRYPTION
	  if (doencrypt)
	    cc = des_read (rem, buf, sizeof buf);
	  else
# endif
#elif defined(SHISHI) && defined(ENCRYPTION)
	  if (doencrypt)
	    readenc (h, rem, buf, &cc, &iv1, enckey, 2);
	  else
#endif
	    cc = read (rem, buf, sizeof buf);
	  if (cc <= 0)
	    {
	      if (errno != EWOULDBLOCK)
		FD_CLR (rem, &readfrom);
	    }
	  else
	    write (1, buf, cc);
	}
    }
  while (FD_ISSET (rfd2, &readfrom) || FD_ISSET (rem, &readfrom));
}

void
sendsig (int sig)
{
  char signo;

#if defined(SHISHI) && defined (ENCRYPTION)
  int n;
#endif

  signo = sig;
#ifdef KERBEROS
# ifdef ENCRYPTION
  if (doencrypt)
    des_write (rfd2, &signo, 1);
  else
# endif
#elif defined(SHISHI) && defined (ENCRYPTION)
  if (doencrypt)
    writeenc (h, rfd2, &signo, 1, &n, &iv4, enckey, 2);
  else
#endif

    write (rfd2, &signo, 1);
}

#if defined(KERBEROS) || defined(SHISHI)
void
warning (const char *fmt, ...)
{
  va_list ap;

  fprintf (stderr, "%s: warning, using standard rsh: ", program_name);
  va_start (ap, fmt);
  fmt = va_arg (ap, char *);
  vfprintf (stderr, fmt, ap);
  va_end (ap);
  fprintf (stderr, ".\n");
}
#endif

char *
copyargs (char **argv)
{
  int cc;
  char **ap, *args, *p;

  cc = 0;
  for (ap = argv; *ap; ++ap)
    cc += strlen (*ap) + 1;
  if (!(args = malloc ((u_int) cc)))
    error (1, errno, "copyargs");
  for (p = args, ap = argv; *ap; ++ap)
    {
      strcpy (p, *ap);
      for (p = strcpy (p, *ap); *p; ++p);
      if (ap[1])
	*p++ = ' ';
    }
  return args;
}
