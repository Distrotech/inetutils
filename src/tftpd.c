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
 * Trivial file transfer protocol server.
 *
 * This version includes many modifications by Jim Guyton
 * <guyton@rand-unix>.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/param.h>
#include <sys/ioctl.h>
#ifdef HAVE_SYS_FILIO_H
# include <sys/filio.h>
#endif
#include <sys/stat.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/tftp.h>
#include <arpa/inet.h>

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include "tftpsubs.h"

#include <argp.h>
#include <progname.h>
#include <libinetutils.h>

void usage (void);

#define TIMEOUT		5

#ifndef LOG_FTP
# define LOG_FTP LOG_DAEMON	/* Use generic facility.  */
#endif

static int peer;
static int rexmtval = TIMEOUT;
static int maxtimeout = 5 * TIMEOUT;

#define PKTSIZE	SEGSIZE+4
static char buf[PKTSIZE];
static char ackbuf[PKTSIZE];
static struct sockaddr_in from;
static socklen_t fromlen;

void tftp (struct tftphdr *, int);

/*
 * Null-terminated directory prefix list for absolute pathname requests and
 * search list for relative pathname requests.
 *
 * MAXDIRS should be at least as large as the number of arguments that
 * inetd allows (currently 20).
 */
#define MAXDIRS	20
static struct dirlist
{
  char *name;
  int len;
} dirs[MAXDIRS + 1];
static int suppress_naks;
static int logging;

static const char *errtomsg (int);
static void nak (int);
static const char *verifyhost (struct sockaddr_in *);



static struct argp_option options[] = {
  { "logging", 'l', NULL, 0,
    "enable logging" },
  { "nonexistent", 'n', NULL, 0,
    "supress negative acknowledgement of requests for "
    "nonexistent relative filenames" },
  { NULL }
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'l':
      logging = 1;
      break;

    case 'n':
      suppress_naks = 1;
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }

  return 0;
}

static struct argp argp =
  {
    options,
    parse_opt,
    "directory...",
    "Trivial File Transfer Protocol server"
  };


int
main (int argc, char *argv[])
{
  int index;
  register struct tftphdr *tp;
  int on, n;
  struct sockaddr_in sin;

  set_program_name (argv[0]);
  iu_argp_init ("tftpd", default_program_authors);
  argp_parse (&argp, argc, argv, 0, &index, NULL);

  openlog ("tftpd", LOG_PID, LOG_FTP);

  if (index < argc)
    {
      struct dirlist *dirp;

      /* Get list of directory prefixes. Skip relative pathnames. */
      for (dirp = dirs; index < argc && dirp < &dirs[MAXDIRS]; index++)
	{
	  if (argv[index][0] == '/')
	    {
	      dirp->name = argv[index];
	      dirp->len = strlen (dirp->name);
	      dirp++;
	    }
	}
    }

  on = 1;
  if (ioctl (0, FIONBIO, &on) < 0)
    {
      syslog (LOG_ERR, "ioctl(FIONBIO): %m\n");
      exit (1);
    }
  fromlen = sizeof (from);
  n = recvfrom (0, buf, sizeof (buf), 0, (struct sockaddr *) &from, &fromlen);
  if (n < 0)
    {
      syslog (LOG_ERR, "recvfrom: %m\n");
      exit (1);
    }
  /*
   * Now that we have read the message out of the UDP
   * socket, we fork and exit.  Thus, inetd will go back
   * to listening to the tftp port, and the next request
   * to come in will start up a new instance of tftpd.
   *
   * We do this so that inetd can run tftpd in "wait" mode.
   * The problem with tftpd running in "nowait" mode is that
   * inetd may get one or more successful "selects" on the
   * tftp port before we do our receive, so more than one
   * instance of tftpd may be started up.  Worse, if tftpd
   * break before doing the above "recvfrom", inetd would
   * spawn endless instances, clogging the system.
   */
  {
    int pid;
    int i;
    socklen_t j;

    for (i = 1; i < 20; i++)
      {
	pid = fork ();
	if (pid < 0)
	  {
	    sleep (i);
	    /*
	     * flush out to most recently sent request.
	     *
	     * This may drop some request, but those
	     * will be resent by the clients when
	     * they timeout.  The positive effect of
	     * this flush is to (try to) prevent more
	     * than one tftpd being started up to service
	     * a single request from a single client.
	     */
	    j = sizeof from;
	    i = recvfrom (0, buf, sizeof (buf), 0,
			  (struct sockaddr *) &from, &j);
	    if (i > 0)
	      {
		n = i;
		fromlen = j;
	      }
	  }
	else
	  {
	    break;
	  }
      }
    if (pid < 0)
      {
	syslog (LOG_ERR, "fork: %m\n");
	exit (1);
      }
    else if (pid != 0)
      {
	exit (0);
      }
  }
  from.sin_family = AF_INET;
  alarm (0);
  close (0);
  close (1);
  peer = socket (AF_INET, SOCK_DGRAM, 0);
  if (peer < 0)
    {
      syslog (LOG_ERR, "socket: %m\n");
      exit (1);
    }
  memset (&sin, 0, sizeof (sin));
  sin.sin_family = AF_INET;
  if (bind (peer, (struct sockaddr *) &sin, sizeof (sin)) < 0)
    {
      syslog (LOG_ERR, "bind: %m\n");
      exit (1);
    }
  if (connect (peer, (struct sockaddr *) &from, sizeof (from)) < 0)
    {
      syslog (LOG_ERR, "connect: %m\n");
      exit (1);
    }
  tp = (struct tftphdr *) buf;
  tp->th_opcode = ntohs (tp->th_opcode);
  if (tp->th_opcode == RRQ || tp->th_opcode == WRQ)
    tftp (tp, n);
  exit (1);
}

struct formats;
int validate_access (char **, int);
void send_file (struct formats *);
void recvfile (struct formats *);

struct formats
{
  char *f_mode;
  int (*f_validate) (char **, int);
  void (*f_send) (struct formats *);
  void (*f_recv) (struct formats *);
  int f_convert;
} formats[] =
  {
    {"netascii", validate_access, send_file, recvfile, 1},
    {"octet", validate_access, send_file, recvfile, 0},
    {0}
  };

/*
 * Handle initial connection protocol.
 */
void
tftp (struct tftphdr *tp, int size)
{
  register char *cp;
  int first = 1, ecode;
  register struct formats *pf;
  char *filename, *mode;

  filename = cp = tp->th_stuff;
again:
  while (cp < buf + size)
    {
      if (*cp == '\0')
	break;
      cp++;
    }
  if (*cp != '\0')
    {
      nak (EBADOP);
      exit (1);
    }
  if (first)
    {
      mode = ++cp;
      first = 0;
      goto again;
    }
  for (cp = mode; *cp; cp++)
    if (isupper (*cp))
      *cp = tolower (*cp);
  for (pf = formats; pf->f_mode; pf++)
    if (strcmp (pf->f_mode, mode) == 0)
      break;
  if (pf->f_mode == 0)
    {
      nak (EBADOP);
      exit (1);
    }
  ecode = (*pf->f_validate) (&filename, tp->th_opcode);
  if (logging)
    {
      syslog (LOG_INFO, "%s: %s request for %s: %s",
	      verifyhost (&from),
	      tp->th_opcode == WRQ ? "write" : "read",
	      filename, errtomsg (ecode));
    }
  if (ecode)
    {
      /*
       * Avoid storms of naks to a RRQ broadcast for a relative
       * bootfile pathname from a diskless Sun.
       */
      if (suppress_naks && *filename != '/' && ecode == ENOTFOUND)
	exit (0);
      nak (ecode);
      exit (1);
    }
  if (tp->th_opcode == WRQ)
    (*pf->f_recv) (pf);
  else
    (*pf->f_send) (pf);
  exit (0);
}


FILE *file;

/*
 * Validate file access.  Since we
 * have no uid or gid, for now require
 * file to exist and be publicly
 * readable/writable.
 * If we were invoked with arguments
 * from inetd then the file must also be
 * in one of the given directory prefixes.
 * Note also, full path name must be
 * given as we have no login directory.
 */
int
validate_access (char **filep, int mode)
{
  struct stat stbuf;
  int fd;
  struct dirlist *dirp;
  static char *pathname = 0;
  char *filename = *filep;

  /*
   * Prevent tricksters from getting around the directory restrictions
   */
  if (strstr (filename, "/../"))
    return (EACCESS);

  if (*filename == '/')
    {
      /*
       * Allow the request if it's in one of the approved locations.
       * Special case: check the null prefix ("/") by looking
       * for length = 1 and relying on the arg. processing that
       * it's a /.
       */
      for (dirp = dirs; dirp->name != NULL; dirp++)
	{
	  if (dirp->len == 1 ||
	      (!strncmp (filename, dirp->name, dirp->len) &&
	       filename[dirp->len] == '/'))
	    break;
	}
      /* If directory list is empty, allow access to any file */
      if (dirp->name == NULL && dirp != dirs)
	return (EACCESS);
      if (stat (filename, &stbuf) < 0)
	return (errno == ENOENT ? ENOTFOUND : EACCESS);
      if ((stbuf.st_mode & S_IFMT) != S_IFREG)
	return (ENOTFOUND);
      if (mode == RRQ)
	{
	  if ((stbuf.st_mode & S_IROTH) == 0)
	    return (EACCESS);
	}
      else
	{
	  if ((stbuf.st_mode & S_IWOTH) == 0)
	    return (EACCESS);
	}
    }
  else
    {
      int err;

      /*
       * Relative file name: search the approved locations for it.
       * Don't allow write requests or ones that avoid directory
       * restrictions.
       */

      if (mode != RRQ || !strncmp (filename, "../", 3))
	return (EACCESS);

      /*
       * If the file exists in one of the directories and isn't
       * readable, continue looking. However, change the error code
       * to give an indication that the file exists.
       */
      err = ENOTFOUND;
      for (dirp = dirs; dirp->name != NULL; dirp++)
	{
	  free (pathname);
	  pathname = malloc (strlen (dirp->name) + 1 + strlen (filename) + 1);
	  if (!pathname)
	    return ENOMEM;
	  sprintf (pathname, "%s/%s", dirp->name, filename);
	  if (stat (pathname, &stbuf) == 0 &&
	      (stbuf.st_mode & S_IFMT) == S_IFREG)
	    {
	      if ((stbuf.st_mode & S_IROTH) != 0)
		{
		  break;
		}
	      err = EACCESS;
	    }
	}
      if (dirp->name == NULL)
	return (err);
      *filep = filename = pathname;
    }
  fd = open (filename, mode == RRQ ? O_RDONLY : (O_WRONLY | O_TRUNC));
  if (fd < 0)
    return (errno + 100);
  file = fdopen (fd, (mode == RRQ) ? "r" : "w");
  if (file == NULL)
    {
      return errno + 100;
    }
  return (0);
}

int timeout;
jmp_buf timeoutbuf;

void
timer (int sig)
{

  timeout += rexmtval;
  if (timeout >= maxtimeout)
    exit (1);
  siglongjmp (timeoutbuf, 1);
}

/*
 * Send the requested file.
 */
void
send_file (struct formats *pf)
{
  struct tftphdr *dp, *r_init ();
  register struct tftphdr *ap;	/* ack packet */
  register int size, n;
  volatile int block;

  signal (SIGALRM, timer);
  dp = r_init ();
  ap = (struct tftphdr *) ackbuf;
  block = 1;
  do
    {
      size = readit (file, &dp, pf->f_convert);
      if (size < 0)
	{
	  nak (errno + 100);
	  goto abort;
	}
      dp->th_opcode = htons ((u_short) DATA);
      dp->th_block = htons ((u_short) block);
      timeout = 0;
      sigsetjmp (timeoutbuf, SIGALRM);

    send_data:
      if (send (peer, (const char *) dp, size + 4, 0) != size + 4)
	{
	  syslog (LOG_ERR, "tftpd: write: %m\n");
	  goto abort;
	}
      read_ahead (file, pf->f_convert);
      for (;;)
	{
	  alarm (rexmtval);	/* read the ack */
	  n = recv (peer, ackbuf, sizeof (ackbuf), 0);
	  alarm (0);
	  if (n < 0)
	    {
	      syslog (LOG_ERR, "tftpd: read: %m\n");
	      goto abort;
	    }
	  ap->th_opcode = ntohs ((u_short) ap->th_opcode);
	  ap->th_block = ntohs ((u_short) ap->th_block);

	  if (ap->th_opcode == ERROR)
	    goto abort;

	  if (ap->th_opcode == ACK)
	    {
	      if ((u_short) ap->th_block == (u_short) block)
		break;
	      /* Re-synchronize with the other side */
	      synchnet (peer);
	      if ((u_short) ap->th_block == (u_short) (block - 1))
		goto send_data;
	    }

	}
      block++;
    }
  while (size == SEGSIZE);
abort:
  fclose (file);
}

void
justquit (int sig)
{
  exit (0);
}


/*
 * Receive a file.
 */
void
recvfile (struct formats *pf)
{
  struct tftphdr *dp, *w_init ();
  register struct tftphdr *ap;	/* ack buffer */
  register int n, size;
  volatile int block;

  signal (SIGALRM, timer);
  dp = w_init ();
  ap = (struct tftphdr *) ackbuf;
  block = 0;
  do
    {
      timeout = 0;
      ap->th_opcode = htons ((u_short) ACK);
      ap->th_block = htons ((u_short) block);
      block++;
      sigsetjmp (timeoutbuf, SIGALRM);
    send_ack:
      if (send (peer, ackbuf, 4, 0) != 4)
	{
	  syslog (LOG_ERR, "tftpd: write: %m\n");
	  goto abort;
	}
      write_behind (file, pf->f_convert);
      for (;;)
	{
	  alarm (rexmtval);
	  n = recv (peer, (char *) dp, PKTSIZE, 0);
	  alarm (0);
	  if (n < 0)
	    {			/* really? */
	      syslog (LOG_ERR, "tftpd: read: %m\n");
	      goto abort;
	    }
	  dp->th_opcode = ntohs ((u_short) dp->th_opcode);
	  dp->th_block = ntohs ((u_short) dp->th_block);
	  if (dp->th_opcode == ERROR)
	    goto abort;
	  if (dp->th_opcode == DATA)
	    {
	      if (dp->th_block == block)
		{
		  break;	/* normal */
		}
	      /* Re-synchronize with the other side */
	      synchnet (peer);
	      if (dp->th_block == (block - 1))
		goto send_ack;	/* rexmit */
	    }
	}
      /*  size = write(file, dp->th_data, n - 4); */
      size = writeit (file, &dp, n - 4, pf->f_convert);
      if (size != (n - 4))
	{			/* ahem */
	  if (size < 0)
	    nak (errno + 100);
	  else
	    nak (ENOSPACE);
	  goto abort;
	}
    }
  while (size == SEGSIZE);
  write_behind (file, pf->f_convert);
  fclose (file);		/* close data file */

  ap->th_opcode = htons ((u_short) ACK);	/* send the "final" ack */
  ap->th_block = htons ((u_short) (block));
  send (peer, ackbuf, 4, 0);

  signal (SIGALRM, justquit);	/* just quit on timeout */
  alarm (rexmtval);
  n = recv (peer, buf, sizeof (buf), 0);	/* normally times out and quits */
  alarm (0);
  if (n >= 4 &&			/* if read some data */
      dp->th_opcode == DATA &&	/* and got a data block */
      block == dp->th_block)
    {				/* then my last ack was lost */
      send (peer, ackbuf, 4, 0);	/* resend final ack */
    }
abort:
  return;
}

struct errmsg
{
  int e_code;
  const char *e_msg;
} errmsgs[] =
  {
    {EUNDEF, "Undefined error code"},
    {ENOTFOUND, "File not found"},
    {EACCESS, "Access violation"},
    {ENOSPACE, "Disk full or allocation exceeded"},
    {EBADOP, "Illegal TFTP operation"},
    {EBADID, "Unknown transfer ID"},
    {EEXISTS, "File already exists"},
    {ENOUSER, "No such user"},
    {-1, 0}
  };

static const char *
errtomsg (int error)
{
  static char buf[20];
  register struct errmsg *pe;
  if (error == 0)
    return "success";
  for (pe = errmsgs; pe->e_code >= 0; pe++)
    if (pe->e_code == error)
      return pe->e_msg;
  sprintf (buf, "error %d", error);
  return buf;
}

/*
 * Send a nak packet (error message).
 * Error code passed in is one of the
 * standard TFTP codes, or a UNIX errno
 * offset by 100.
 */
static void
nak (int error)
{
  register struct tftphdr *tp;
  int length;
  register struct errmsg *pe;

  tp = (struct tftphdr *) buf;
  tp->th_opcode = htons ((u_short) ERROR);
  tp->th_code = htons ((u_short) error);
  for (pe = errmsgs; pe->e_code >= 0; pe++)
    if (pe->e_code == error)
      break;
  if (pe->e_code < 0)
    {
      pe->e_msg = strerror (error - 100);
      tp->th_code = EUNDEF;	/* set 'undef' errorcode */
    }
  strcpy (tp->th_msg, pe->e_msg);
  length = strlen (pe->e_msg);
  tp->th_msg[length] = '\0';
  length += 5;
  if (send (peer, buf, length, 0) != length)
    syslog (LOG_ERR, "nak: %m\n");
}

static const char *
verifyhost (struct sockaddr_in *fromp)
{
  struct hostent *hp;

  hp = gethostbyaddr ((char *) &fromp->sin_addr, sizeof (fromp->sin_addr),
		      fromp->sin_family);
  if (hp)
    return hp->h_name;
  else
    return inet_ntoa (fromp->sin_addr);
}

static const char usage_str[] =
  "Usage: tftpd [OPTIONS...]\n"
  "\n"
  "Options are:\n"
  "   -l                      Enable logging\n"
  "   -n                      Supress negative acknowledgement of\n"
  "                           requests for nonexistent relative filenames\n"
  "       --help              Display usage instructions\n"
  "       --version           Display program version\n";

void
usage (void)
{
  printf ("%s\n" "Send bug reports to <%s>\n", usage_str, PACKAGE_BUGREPORT);
}
