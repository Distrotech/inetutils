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
 * Copyright (c) 1983, 1990, 1993
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

/*
 * rlogin - remote login
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#ifdef HAVE_SYS_RESOURCE_H
# include <sys/resource.h>
#endif
#include <sys/wait.h>
#include <sys/ioctl.h>
#ifdef HAVE_SYS_STREAM_H
# include <sys/stream.h>
#endif
#ifdef HAVE_SYS_TTY_H
# include <sys/tty.h>
#endif
#ifdef HAVE_SYS_PTYVAR_H
# include <sys/ptyvar.h>
#endif
#ifdef HAVE_SYS_SOCKIO_H
# include <sys/sockio.h>
#endif

#include <netinet/in.h>
#ifdef HAVE_NETINET_IN_SYSTM_H
# include <netinet/in_systm.h>
#endif
#ifdef HAVE_NETINET_IP_H
# include <netinet/ip.h>
#endif

#include <argp.h>
#include <error.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <pwd.h>
#include <setjmp.h>
#include <termios.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <stdarg.h>

#include <progname.h>
#include "libinetutils.h"

#ifdef SHISHI
# define REALM_SZ 1040
#endif

#if defined(KERBEROS) || defined(SHISHI)
int use_kerberos = 1, doencrypt;
# ifdef KERBEROS
#  include <kerberosIV/des.h>
#  include <kerberosIV/krb.h>
char dest_realm_buf[REALM_SZ], *dest_realm = NULL;
CREDENTIALS cred;
Key_schedule schedule;

# elif defined(SHISHI)
#  include <shishi.h>
#  include "shishi_def.h"
char dest_realm_buf[REALM_SZ], *dest_realm = NULL;

Shishi *handle;
Shishi_key *key;
shishi_ivector iv1, iv2;
shishi_ivector *ivtab[2];

int keytype;
int keylen;
int rc;
int wlen;
# endif
#endif /* KERBEROS */

/*
  The TIOCPKT_* macros may not be implemented in the pty driver.
  Defining them here allows the program to be compiled.  */
#ifndef TIOCPKT
# define TIOCPKT                 _IOW('t', 112, int)
# define TIOCPKT_FLUSHWRITE      0x02
# define TIOCPKT_NOSTOP          0x10
# define TIOCPKT_DOSTOP          0x20
#endif /*TIOCPKT*/
/* The server sends us a TIOCPKT_WINDOW notification when it starts up.
   The value for this (0x80) can not overlap the kernel defined TIOCPKT_xxx
   values.  */
#ifndef TIOCPKT_WINDOW
# define TIOCPKT_WINDOW	0x80
#endif
/* Concession to Sun.  */
#ifndef SIGUSR1
# define SIGUSR1	30
#endif
#ifndef _POSIX_VDISABLE
# ifdef VDISABLE
#  define _POSIX_VDISABLE VDISABLE
# else
#  define _POSIX_VDISABLE ((cc_t)'\377')
# endif
#endif
/* Returned by speed() when the specified fd is not associated with a
   terminal.  */
#define SPEED_NOTATTY	(-1)
int eight = 0, rem;

int dflag = 0;
int noescape;
char * host = NULL;
char * user = NULL;
u_char escapechar = '~';

#ifdef OLDSUN

struct winsize
{
  unsigned short ws_row;	/* Rows, in characters.  */
  unsigned short ws_col;	/* Columns , in characters.  */
  unsigned short ws_xpixel;	/* Horizontal size, pixels.  */
  unsigned short ws_ypixel;	/* Vertical size. pixels.  */
};

int get_window_size (int, struct winsize *);
#else
# define get_window_size(fd, wp)	ioctl (fd, TIOCGWINSZ, wp)
#endif
struct winsize winsize;

RETSIGTYPE catch_child (int);
RETSIGTYPE copytochild (int);
void doit (sigset_t *);
void done (int);
void echo (char);
u_int getescape (char *);
RETSIGTYPE lostpeer (int);
void mode (int);
RETSIGTYPE oob (int);
int reader (sigset_t *);
void sendwindow (void);
void setsignal (int);
int speed (int);
unsigned int speed_translate (unsigned int);
RETSIGTYPE sigwinch (int);
void stop (char);
void writer (void);
RETSIGTYPE writeroob (int);

#if defined(KERBEROS) || defined(SHISHI)
void warning (const char *, ...);
#endif

extern sig_t setsig (int, sig_t);

#if defined(KERBEROS) || defined(SHISHI)
# define OPTIONS	"8EKde:k:l:xhV"
#else
# define OPTIONS	"8EKde:l:hV"
#endif

const char args_doc[] = "HOST";
const char doc[] = "Starts a terminal session on a remote host.";

static struct argp_option argp_options[] = {
#define GRP 0
  {"8-bit", '8', NULL, 0, "allows an eight-bit input data path at all times",
   GRP+1},
  {"debug", 'd', NULL, 0, "set the SO_DEBUG option", GRP+1},
  {"escape", 'e', "CHAR", 0, "allows user specification of the escape "
   "character, which is ``~'' by default", GRP+1},
  {"no-escape", 'E', NULL, 0, "stops any character from being recognized as "
   "an escape character", GRP+1},
  {"user", 'l', "USER", 0, "run as USER on the remote system", GRP+1},
#if defined(KERBEROS) || defined(SHISHI)
# ifdef ENCRYPTION
  {"encrypt", 'x', NULL, 0, "turns on DES encryption for all data passed via "
   "the rlogin session", GRP+1},
# endif
  {"kerberos", 'K', NULL, 0, "turns off all Kerberos authentication", GRP+1},
  {"realm", 'k', "REALM", 0, "obtain tickets for the remote host in REALM "
   "realm instead of the remote's realm", GRP+1},
#endif
#undef GRP
  {NULL}
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    /* 8-bit input Specifying this forces us to use RAW mode input from
       the user's terminal.  Also, in this mode we won't perform any
       local flow control.  */
    case '8':
      eight = 1;
      break;

    case 'd':
      dflag = 1;
      break;

    case 'e':
      noescape = 0;
      escapechar = getescape (arg);
      if (escapechar == 0)
	error (EXIT_FAILURE, 0, "illegal option value -- e");
      break;

    case 'E':
      noescape = 1;
      break;

    case 'l':
      user = arg;
      break;

#if defined (KERBEROS) || defined(SHISHI)
# ifdef ENCRYPTION
    case 'x':
      doencrypt = 1;
#  ifdef KERBEROS
      des_set_key (cred.session, schedule);
#  endif
      break;
# endif

    case 'K':
      use_kerberos = 0;
      break;

    case 'k':
      strncpy (dest_realm_buf, optarg, sizeof (dest_realm_buf));
      /* Make sure it's null termintated.  */
      dest_realm_buf[sizeof (dest_realm_buf) - 1] = '\0';
      dest_realm = dest_realm_buf;
      break;
#endif

    case ARGP_KEY_NO_ARGS:
      if (host == NULL)
        argp_error (state, "missing host operand");

    default:
      return ARGP_ERR_UNKNOWN;
    }

  return 0;
}

static struct argp argp = {argp_options, parse_opt, args_doc, doc};

int
main (int argc, char *argv[])
{
  struct passwd *pw;
  struct servent *sp;
  sigset_t smask;
  uid_t uid;
  int index;
  int term_speed;
  char term[1024];

  set_program_name (argv[0]);

  /* Traditionnaly, if a symbolic link was made to the rlogin binary
     rlogin --> hostname
     hostname will be use as the name of the server to login too.  */
  {
    char *p = strrchr (argv[0], '/');
    if (p)
      ++p;
    else
      p = argv[0];

    if (strcmp (p, "rlogin") != 0)
      host = p;
  }

  /* Parse command line */
  iu_argp_init ("rlogin", default_program_authors);
  argp_parse (&argp, argc, argv, 0, &index, NULL);

  if (index < argc)
    host = argv[index++];

  argc -= index;

  /* We must be uid root to access rcmd().  */
  if (geteuid ())
    error (1, 0, "must be setuid root.\n");

  /* Get the name of the user invoking us: the client-user-name.  */
  if (!(pw = getpwuid (uid = getuid ())))
    error (1, 0, "unknown user id.");

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
          error (EXIT_FAILURE, 0, "invalid host operand");
      }
  }

  sp = NULL;
#if defined(KERBEROS) || defined(SHISHI)
  if (use_kerberos)
    {
      sp = getservbyname ((doencrypt ? "eklogin" : "klogin"), "tcp");
      if (sp == NULL)
	{
	  use_kerberos = 0;
	  warning ("can't get entry for %s/tcp service",
		   doencrypt ? "eklogin" : "klogin");
	}
    }
#endif

  /* Get the port number for the rlogin service.  */
  if (sp == NULL)
    sp = getservbyname ("login", "tcp");
  if (sp == NULL)
    error (1, 0, "login/tcp: unknown service.");

  /* Get the name of the terminal from the environment.  Also get the
     terminal's spee.  Both the name and the spee are passed to the server
     as the "cmd" argument of the rcmd() function.  This is something like
     "vt100/9600".  */
  term_speed = speed (0);
  if (term_speed == SPEED_NOTATTY)
    {
      char *p;
      snprintf (term, sizeof term, "%s",
		((p = getenv ("TERM")) ? p : "network"));
    }
  else
    {
      char *p;
      snprintf (term, sizeof term, "%s/%d",
		((p = getenv ("TERM")) ? p : "network"), term_speed);
    }
  get_window_size (0, &winsize);

  setsig (SIGPIPE, lostpeer);

  /* Block SIGURG and SIGUSR1 signals.  This will be handled by the
     parent and the child after the fork.  */
  /* Will use SIGUSR1 for window size hack, so hold it off.  */
  sigemptyset (&smask);
  sigaddset (&smask, SIGURG);
  sigaddset (&smask, SIGUSR1);
  sigprocmask (SIG_SETMASK, &smask, &smask);

  /*
   * We set SIGURG and SIGUSR1 below so that an
   * incoming signal will be held pending rather than being
   * discarded. Note that these routines will be ready to get
   * a signal by the time that they are unblocked below.
   */
  setsig (SIGURG, copytochild);
  setsig (SIGUSR1, writeroob);

#if defined (KERBEROS) || defined(SHISHI)
try_connect:
  if (use_kerberos)
    {
      struct hostent *hp;

      /* Fully qualify hostname (needed for krb_realmofhost).  */
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

	  rem = krcmd_mutual (&handle, &host, sp->s_port, &user, term, 0,
			      dest_realm, &key);
	  if (rem > 0)
	    {
	      keytype = shishi_key_type (key);
	      keylen = shishi_cipher_blocksize (keytype);

	      ivtab[0] = &iv1;
	      ivtab[1] = &iv2;

	      for (i = 0; i < 2; i++)
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
		      memset (ivtab[i]->iv, !i, ivtab[i]->ivlen);
		      ivtab[i]->ctx =
			shishi_crypto (handle, key, ivtab[i]->keyusage,
				       shishi_key_type (key), ivtab[i]->iv,
				       ivtab[i]->ivlen);
		      break;
		    case SHISHI_ARCFOUR_HMAC:
		    case SHISHI_ARCFOUR_HMAC_EXP:
		      ivtab[i]->keyusage =
			SHISHI_KEYUSAGE_KCMD_DES + 2 + 4 * i;
		      ivtab[i]->ctx =
			shishi_crypto (handle, key, ivtab[i]->keyusage,
				       shishi_key_type (key), NULL, 0);
		      break;
		    default:
		      ivtab[i]->keyusage =
			SHISHI_KEYUSAGE_KCMD_DES + 2 + 4 * i;
		      ivtab[i]->iv = malloc (ivtab[i]->ivlen);
		      memset (ivtab[i]->iv, 0, ivtab[i]->ivlen);
		      ivtab[i]->ctx =
			shishi_crypto (handle, key, ivtab[i]->keyusage,
				       shishi_key_type (key), ivtab[i]->iv,
				       ivtab[i]->ivlen);
		    }
		}
	    }
	}

      else
#  else
	rem = krcmd_mutual (&host, sp->s_port, user, term, 0,
			    dest_realm, &cred, schedule);
      else
#  endif
# endif	/* CRYPT */

	rem = krcmd (
# if defined (SHISHI)
		      &handle, &host, sp->s_port, &user, term, 0, dest_realm);
# else
		      &host, sp->s_port, user, term, 0, dest_realm);
# endif
      if (rem < 0)
	{
	  use_kerberos = 0;
	  sp = getservbyname ("login", "tcp");
	  if (sp == NULL)
	    error (1, 0, "unknown service login/tcp.");
	  if (errno == ECONNREFUSED)
	    warning ("remote host doesn't support Kerberos");
	  if (errno == ENOENT)
	    warning ("can't provide Kerberos auth data");
	  goto try_connect;
	}
    }

  else
    {
# ifdef ENCRYPTION
      if (doencrypt)
	error (1, 0, "the -x flag requires Kerberos authentication.");
# endif	/* CRYPT */
      if (!user)
	user = pw->pw_name;

      rem = rcmd (&host, sp->s_port, pw->pw_name, user, term, 0);
    }
#else
  if (!user)
    user = pw->pw_name;

  rem = rcmd (&host, sp->s_port, pw->pw_name, user, term, 0);

#endif /* KERBEROS */

  if (rem < 0)
    exit (1);

  {
    int one = 1;
    if (dflag && setsockopt (rem, SOL_SOCKET, SO_DEBUG, (char *) &one,
			     sizeof one) < 0)
      error (0, errno, "setsockopt DEBUG (ignored)");
  }

#if defined (IP_TOS) && defined (IPPROTO_IP) && defined (IPTOS_LOWDELAY)
  {
    int one = IPTOS_LOWDELAY;
    if (setsockopt (rem, IPPROTO_IP, IP_TOS, (char *) &one, sizeof (int)) < 0)
      error (0, errno, "setsockopt TOS (ignored)");
  }
#endif

  /* Now change to the real user ID.  We have to be set-user-ID root
     to get the privileged port that rcmd () uses,  however we now want to
     run as the real user who invoked us.  */
  seteuid (uid);
  setuid (uid);

  doit (&smask);

  return 0;
}

/* Some systems, like QNX/Neutrino , The constant B0, B50,.. maps straigth to
   the actual speed, 0, 50, ..., where on other system like GNU/Linux
   it maps to a const 0, 1, ... i.e the value are encoded.
   cfgetispeed(), according to posix should return a constant value reprensenting the Baud.
   So to be portable we have to the conversion ourselves.  */
/* Some values are not not define by POSIX.  */
#ifndef B7200
# define B7200   B4800
#endif

#ifndef B14400
# define B14400  B9600
#endif

#ifndef B19200
# define B19200 B14400
#endif

#ifndef B28800
# define B28800  B19200
#endif

#ifndef B38400
# define B38400 B28800
#endif

#ifndef B57600
# define B57600  B38400
#endif

#ifndef B76800
# define B76800  B57600
#endif

#ifndef B115200
# define B115200 B76800
#endif

#ifndef B230400
# define B230400 B115200
#endif
struct termspeeds
{
  unsigned int speed;
  unsigned int sym;
} termspeeds[] =
  {
    {0, B0},
    {50, B50},
    {75, B75},
    {110, B110},
    {134, B134},
    {150, B150},
    {200, B200},
    {300, B300},
    {600, B600},
    {1200, B1200},
    {1800, B1800},
    {2400, B2400},
    {4800, B4800},
    {7200, B7200},
    {9600, B9600},
    {14400, B14400},
    {19200, B19200},
    {28800, B28800},
    {38400, B38400},
    {57600, B57600},
    {115200, B115200},
    {230400, B230400},
    {-1, B230400}
  };

unsigned int
speed_translate (unsigned int sym)
{
  unsigned int i;
  for (i = 0; i < (sizeof (termspeeds) / sizeof (*termspeeds)); i++)
    {
      if (termspeeds[i].sym == sym)
	return termspeeds[i].speed;
    }
  return 0;
}

/* Returns the terminal speed for the file descriptor FD, or
   SPEED_NOTATTY if FD is not associated with a terminal.  */
int
speed (int fd)
{
  struct termios tt;

  if (tcgetattr (fd, &tt) == 0)
    {
      /* speed_t sp; */
      unsigned int sp = cfgetispeed (&tt);
      return speed_translate (sp);
    }
  return SPEED_NOTATTY;
}

pid_t child;
struct termios deftt;
struct termios ixon_state;
struct termios nott;

void
doit (sigset_t * smask)
{
  int i;

  for (i = 0; i < NCCS; i++)
    nott.c_cc[i] = _POSIX_VDISABLE;
  tcgetattr (0, &deftt);
  nott.c_cc[VSTART] = deftt.c_cc[VSTART];
  nott.c_cc[VSTOP] = deftt.c_cc[VSTOP];

  setsig (SIGINT, SIG_IGN);
  setsignal (SIGHUP);
  setsignal (SIGQUIT);

  child = fork ();
  if (child == -1)
    {
      error (0, errno, "fork");
      done (1);
    }
  if (child == 0)
    {
      mode (1);
      if (reader (smask) == 0)
	{
	  /* If the reader () return 0, the socket to the server returned an
	     EOF, meaning the client logged out of the remote system.
	     This is the normal termination.  */
          error (0, 0, "connection closed");
          /* EXIT_SUCCESS is usually zero. So error might not exit.  */
          exit (EXIT_SUCCESS);
	}
      /* If the reader () returns nonzero, the socket to the server
         returned an error.  Somethingg went wrong.  */
      sleep (1);
      error (EXIT_FAILURE, 0, "\007connection closed");
    }

  /*
   * Parent process == writer.
   *
   * We may still own the socket, and may have a pending SIGURG (or might
   * receive one soon) that we really want to send to the reader.  When
   * one of these comes in, the trap copytochild simply copies such
   * signals to the child. We can now unblock SIGURG and SIGUSR1
   * that were set above.
   */
  /* Reenables SIGURG and SIUSR1.  */
  sigprocmask (SIG_SETMASK, smask, (sigset_t *) 0);

  setsig (SIGCHLD, catch_child);

  writer ();

  /* If the write returns, it means the user entered "~." on the terminal.
     In this case we terminate and the server will eventually get an EOF
     on its end of the network connection.  This should cause the server to
     log you out on the remote system.  */
  error (0, 0, "closed connection");

#ifdef SHISHI
  if (use_kerberos)
    {
      shishi_done (handle);
# ifdef ENCRYPTION
      if (doencrypt)
	{
	  shishi_key_done (key);
	  shishi_crypto_close (iv1.ctx);
	  shishi_crypto_close (iv2.ctx);
	  free (iv1.iv);
	  free (iv2.iv);
	}
# endif
    }
#endif

  done (0);
}

/* Enable a signal handler, unless the signal is already being ignored.
   This function is called before the fork (), for SIGHUP and SIGQUIT.  */
/* Trap a signal, unless it is being ignored. */
void
setsignal (int sig)
{
  sig_t handler;
  sigset_t sigs;

  sigemptyset (&sigs);
  sigaddset (&sigs, sig);
  sigprocmask (SIG_BLOCK, &sigs, &sigs);

  handler = setsig (sig, exit);
  if (handler == SIG_IGN)
    setsig (sig, handler);

  sigprocmask (SIG_SETMASK, &sigs, (sigset_t *) 0);
}

/* This function is called by the parent:
   (1) at the end (user terminates the client end);
   (2) SIGCLD signal - the sigcld_parent () function.
   (3) SIGPPE signal - the connection has dropped.

   We send the child a SIGKILL signal, which it can't ignore, then
   wait for it to terminate.  */
void
done (int status)
{
  pid_t w;
  int wstatus;

  mode (0);
  if (child > 0)
    {
      /* make sure catch_child does not snap it up */
      setsig (SIGCHLD, SIG_DFL);
      if (kill (child, SIGKILL) >= 0)
	while ((w = wait (&wstatus)) > 0 && w != child)
	  continue;
    }
  exit (status);
}

int dosigwinch;

/*
 * This is called when the reader process gets the out-of-band (urgent)
 * request to turn on the window-changing protocol.
 */
RETSIGTYPE
writeroob (int signo ARG_UNUSED)
{
  if (dosigwinch == 0)
    {
      sendwindow ();
      setsig (SIGWINCH, sigwinch);
    }
  dosigwinch = 1;
}

RETSIGTYPE
catch_child (int signo ARG_UNUSED)
{
  int status;
  pid_t pid;

  for (;;)
    {
      pid = waitpid (-1, &status, WNOHANG | WUNTRACED);
      if (pid == 0)
	return;
      /* if the child (reader) dies, just quit */
      if (pid < 0 || (pid == child && !WIFSTOPPED (status)))
	done (WEXITSTATUS (status) | WTERMSIG (status));
    }
}

/*
 * writer: write to remote: 0 -> line.
 * ~.				terminate
 * ~^Z				suspend rlogin process.
 * ~<delayed-suspend char>	suspend rlogin process, but leave reader alone.
 */
void
writer ()
{
  register int bol, local, n;
  char c;

  bol = 1;			/* beginning of line */
  local = 0;
  for (;;)
    {
      n = read (STDIN_FILENO, &c, 1);
      if (n <= 0)
	{
	  if (n < 0 && errno == EINTR)
	    continue;
	  break;
	}
      /*
       * If we're at the beginning of the line and recognize a
       * command character, then we echo locally.  Otherwise,
       * characters are echo'd remotely.  If the command character
       * is doubled, this acts as a force and local echo is
       * suppressed.
       */
      if (bol)
	{
	  bol = 0;
	  if (!noescape && c == escapechar)
	    {
	      local = 1;
	      continue;
	    }
	}
      else if (local)
	{
	  local = 0;
	  if (c == '.' || c == deftt.c_cc[VEOF])
	    {
	      echo (c);
	      break;
	    }
	  if (c == deftt.c_cc[VSUSP]
#ifdef VDSUSP
	      || c == deftt.c_cc[VDSUSP]
#endif
	    )
	    {
	      bol = 1;
	      echo (c);
	      stop (c);
	      continue;
	    }
	  if (c != escapechar)
#ifdef ENCRYPTION
# ifdef KERBEROS
	    if (doencrypt)
	      des_write (rem, (char *) &escapechar, 1);
	    else
# elif defined(SHISHI)
	    if (doencrypt)
	      writeenc (handle, rem, (char *) &escapechar, 1, &wlen, &iv2,
			key, 2);
	    else
# endif
#endif
	      write (rem, &escapechar, 1);
	}

#ifdef ENCRYPTION
# ifdef KERBEROS
      if (doencrypt)
	{
	  if (des_write (rem, &c, 1) == 0)
	    {
              error (0, 0, "line gone");
	      break;
	    }
	}
      else
# elif defined(SHISHI)
      if (doencrypt)
	{
	  writeenc (handle, rem, &c, 1, &wlen, &iv2, key, 2);
	  if (wlen == 0)
	    {
              error (0, 0, "line gone");
	      break;
	    }
	}
      else
# endif
#endif
      if (write (rem, &c, 1) == 0)
	{
          error (0, 0, "line gone");
	  break;
	}
      bol = c == deftt.c_cc[VKILL] || c == deftt.c_cc[VEOF] ||
	c == deftt.c_cc[VINTR] || c == deftt.c_cc[VSUSP] ||
	c == '\r' || c == '\n';
    }
}

void
echo (register char c)
{
  register char *p;
  char buf[8];

  p = buf;
  c &= 0177;
  *p++ = escapechar;
  if (c < ' ')
    {
      *p++ = '^';
      *p++ = c + '@';
    }
  else if (c == 0177)
    {
      *p++ = '^';
      *p++ = '?';
    }
  else
    *p++ = c;
  *p++ = '\r';
  *p++ = '\n';
  write (STDOUT_FILENO, buf, p - buf);
}

void
stop (char cmdc)
{
  mode (0);
  setsig (SIGCHLD, SIG_IGN);
  kill (cmdc == deftt.c_cc[VSUSP] ? 0 : getpid (), SIGTSTP);
  setsig (SIGCHLD, catch_child);
  mode (1);
  sigwinch (0);			/* check for size changes */
}

RETSIGTYPE
sigwinch (int signo ARG_UNUSED)
{
  struct winsize ws;

  if (dosigwinch && get_window_size (0, &ws) == 0
      && memcmp (&ws, &winsize, sizeof ws))
    {
      winsize = ws;
      sendwindow ();
    }
}

/*
 * Send the window size to the server via the magic escape
 */
void
sendwindow ()
{
  struct winsize *wp;
  char obuf[4 + sizeof (struct winsize)];

  wp = (struct winsize *) (obuf + 4);
  obuf[0] = 0377;
  obuf[1] = 0377;
  obuf[2] = 's';
  obuf[3] = 's';
  wp->ws_row = htons (winsize.ws_row);
  wp->ws_col = htons (winsize.ws_col);
  wp->ws_xpixel = htons (winsize.ws_xpixel);
  wp->ws_ypixel = htons (winsize.ws_ypixel);

#ifdef ENCRYPTION
# ifdef KERBEROS
  if (doencrypt)
    des_write (rem, obuf, sizeof obuf);
  else
# elif defined(SHISHI)
  if (doencrypt)
    writeenc (handle, rem, obuf, sizeof obuf, &wlen, &iv2, key, 2);
  else
# endif
#endif
    write (rem, obuf, sizeof obuf);
}

/*
 * reader: read from remote: line -> 1
 */
#define READING	1
#define WRITING	2

jmp_buf rcvtop;
pid_t ppid;
int rcvcnt, rcvstate;
char rcvbuf[8 * 1024];

RETSIGTYPE
oob (int signo ARG_UNUSED)
{
  char mark;
  struct termios tt;
  int atmark, n, out, rcvd;
  char waste[BUFSIZ];

  out = O_RDWR;
  rcvd = 0;

#ifndef SHISHI
  while (recv (rem, &mark, 1, MSG_OOB) < 0)
    {
      switch (errno)
	{
	case EWOULDBLOCK:
	  /*
	   * Urgent data not here yet.  It may not be possible
	   * to send it yet if we are blocked for output and
	   * our input buffer is full.
	   */
	  if ((size_t) rcvcnt < sizeof rcvbuf)
	    {
	      n = read (rem, rcvbuf + rcvcnt, sizeof (rcvbuf) - rcvcnt);
	      if (n <= 0)
		return;
	      rcvd += n;
	    }
	  else
	    {
	      n = read (rem, waste, sizeof waste);
	      if (n <= 0)
		return;
	    }
	  continue;
	default:
	  return;
	}
    }
#endif
  if (mark & TIOCPKT_WINDOW)
    {
      /* Let server know about window size changes */
      kill (ppid, SIGUSR1);
    }
  if (!eight && (mark & TIOCPKT_NOSTOP))
    {
      tcgetattr (0, &tt);
      tt.c_iflag &= ~(IXON | IXOFF);
      tt.c_cc[VSTOP] = _POSIX_VDISABLE;
      tt.c_cc[VSTART] = _POSIX_VDISABLE;
      tcsetattr (0, TCSANOW, &tt);
    }
  if (!eight && (mark & TIOCPKT_DOSTOP))
    {
      tcgetattr (0, &tt);
      tt.c_iflag |= (IXON | IXOFF);
      tt.c_cc[VSTOP] = deftt.c_cc[VSTOP];
      tt.c_cc[VSTART] = deftt.c_cc[VSTART];
      tcsetattr (0, TCSANOW, &tt);
    }
  if (mark & TIOCPKT_FLUSHWRITE)
    {
#ifdef TIOCFLUSH
      ioctl (1, TIOCFLUSH, (char *) &out);
#endif
      for (;;)
	{
	  if (ioctl (rem, SIOCATMARK, &atmark) < 0)
	    {
	      error (0, errno, "ioctl SIOCATMARK (ignored)");
	      break;
	    }
	  if (atmark)
	    break;
	  n = read (rem, waste, sizeof waste);
	  if (n <= 0)
	    break;
	}
      /*
       * Don't want any pending data to be output, so clear the recv
       * buffer.  If we were hanging on a write when interrupted,
       * don't want it to restart.  If we were reading, restart
       * anyway.
       */
      rcvcnt = 0;
      longjmp (rcvtop, 1);
    }

  /* oob does not do FLUSHREAD (alas!) */

  /*
   * If we filled the receive buffer while a read was pending, longjmp
   * to the top to restart appropriately.  Don't abort a pending write,
   * however, or we won't know how much was written.
   */
  if (rcvd && rcvstate == READING)
    longjmp (rcvtop, 1);
}

/* reader: read from remote: line -> 1 */
int
reader (sigset_t * smask)
{
  pid_t pid;
  int n, remaining;
  char *bufp;

#if BSD >= 43 || defined(SUNOS4)
  pid = getpid ();		/* modern systems use positives for pid */
#else
  pid = -getpid ();		/* old broken systems use negatives */
#endif

  setsig (SIGTTOU, SIG_IGN);
  setsig (SIGURG, oob);

  ppid = getppid ();
  fcntl (rem, F_SETOWN, pid);
  setjmp (rcvtop);
  sigprocmask (SIG_SETMASK, smask, (sigset_t *) 0);
  bufp = rcvbuf;

  for (;;)
    {
#ifdef SHISHI
      if ((rcvcnt >= 5) && (bufp[0] == '\377') && (bufp[1] == '\377'))
	if ((bufp[2] == 'o') && (bufp[3] == 'o'))
	  {
	    oob (1);
	    bufp += 5;
	  }
#endif
      while ((remaining = rcvcnt - (bufp - rcvbuf)) > 0)
	{
	  rcvstate = WRITING;
	  n = write (STDOUT_FILENO, bufp, remaining);
	  if (n < 0)
	    {
	      if (errno != EINTR)
		return -1;
	      continue;
	    }
	  bufp += n;
	}
      bufp = rcvbuf;
      rcvcnt = 0;
      rcvstate = READING;

#ifdef ENCRYPTION
# ifdef KERBEROS
      if (doencrypt)
	rcvcnt = des_read (rem, rcvbuf, sizeof rcvbuf);
      else
# elif defined(SHISHI)
      if (doencrypt)
	readenc (handle, rem, rcvbuf, &rcvcnt, &iv1, key, 2);
      else
# endif
#endif
	rcvcnt = read (rem, rcvbuf, sizeof rcvbuf);
      if (rcvcnt == 0)
	return 0;
      if (rcvcnt < 0)
	{
	  if (errno == EINTR)
	    continue;
	  error (0, errno, "read");
	  return -1;
	}
    }
}
void
mode (int f)
{
  struct termios tt;

  switch (f)
    {
    case 0:
      /* remember whether IXON is set, set it can be restore at mode(1) */
      tcgetattr (0, &ixon_state);
      tcsetattr (0, TCSADRAIN, &deftt);
      break;
    case 1:
      tt = deftt;
      tt.c_oflag &= ~(OPOST);
      tt.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
      tt.c_iflag &= ~(ICRNL);
      tt.c_cc[VMIN] = 1;
      tt.c_cc[VTIME] = 0;
      if (eight)
	{
	  tt.c_iflag &= ~(IXON | IXOFF | ISTRIP);
	  tt.c_cc[VSTOP] = _POSIX_VDISABLE;
	  tt.c_cc[VSTART] = _POSIX_VDISABLE;
	}
      if ((ixon_state.c_iflag & IXON) && !eight)
	tt.c_iflag |= IXON;
      else
	tt.c_iflag &= ~IXON;
      tcsetattr (0, TCSADRAIN, &tt);
      break;

    default:
      return;
    }
}

RETSIGTYPE
lostpeer (int signo ARG_UNUSED)
{
  setsig (SIGPIPE, SIG_IGN);
  error (0, 0, "\007connection closed.");
  done (1);
}

/* copy SIGURGs to the child process. */
RETSIGTYPE
copytochild (int signo ARG_UNUSED)
{
  kill (child, SIGURG);
}

#if defined(KERBEROS) || defined(SHISHI)
void
warning (const char *fmt, ...)
{
  va_list ap;

  fprintf (stderr, "rlogin: warning, using standard rlogin: ");
  va_start (ap, fmt);
  vfprintf (stderr, fmt, ap);
  va_end (ap);
  fprintf (stderr, ".\n");
}
#endif

/*
 * The following routine provides compatibility (such as it is) between older
 * Suns and others.  Suns have only a `ttysize', so we convert it to a winsize.
 */
#ifdef OLDSUN
int
get_window_size (int fd, struct winsize *wp)
{
  struct ttysize ts;
  int error;

  if ((error = ioctl (0, TIOCGSIZE, &ts)) != 0)
    return error;
  wp->ws_row = ts.ts_lines;
  wp->ws_col = ts.ts_cols;
  wp->ws_xpixel = 0;
  wp->ws_ypixel = 0;
  return 0;
}
#endif

u_int
getescape (register char *p)
{
  long val;
  int len;

  if ((len = strlen (p)) == 1)	/* use any single char, including '\'.  */
    return ((u_int) * p);
  /* otherwise, \nnn */
  if (*p == '\\' && len >= 2 && len <= 4)
    {
      val = strtol (++p, NULL, 8);
      for (;;)
	{
	  if (!*++p)
	    return ((u_int) val);
	  if (*p < '0' || *p > '8')
	    break;
	}
    }
  return 0;
}
