/*
  Copyright (C) 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002,
  2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Free Software
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
 * Copyright (c) 1988, 1989, 1992, 1993, 1994
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
 * remote shell server exchange protocol:
 *	[port]\0
 *	remuser\0
 *	locuser\0
 *	command\0
 *	data
 */

#include <config.h>

#include <alloca.h>

#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#ifdef HAVE_NETINET_IN_SYSTM_H
# include <netinet/in_systm.h>	/* n_time */
#endif
#ifdef HAVE_NETINET_IP_H
# include <netinet/ip.h>	/* IPOPT_* */
#endif

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#ifdef HAVE_SYS_FILIO_H
# include <sys/filio.h>
#endif
#include <pwd.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <grp.h>
#include <sys/select.h>
#include <error.h>
#include <progname.h>
#include <argp.h>
#include <libinetutils.h>

int keepalive = 1;		/* flag for SO_KEEPALIVE scoket option */
int check_all;
int log_success;		/* If TRUE, log all successful accesses */
int sent_null;

void doit (int, struct sockaddr_in *);
void rshd_error (const char *, ...);
char *getstr (const char *);
int local_domain (const char *);
const char *topdomain (const char *);

#if defined KERBEROS || defined SHISHI
# ifdef KERBEROS
#  include <kerberosIV/des.h>
#  include <kerberosIV/krb.h>
Key_schedule schedule;
char authbuf[sizeof (AUTH_DAT)];
char tickbuf[sizeof (KTEXT_ST)];
# elif defined(SHISHI)
#  include <shishi.h>
#  include <shishi_def.h>
Shishi *h;
Shishi_ap *ap;
Shishi_key *enckey;
shishi_ivector iv1, iv2, iv3, iv4;
shishi_ivector *ivtab[4];
int protocol;
# endif
# define VERSION_SIZE	9
# define SECURE_MESSAGE  "This rsh session is using DES encryption for all transmissions.\r\n"
int doencrypt, use_kerberos, vacuous;
#else
#endif

static struct argp_option options[] = {
  { "verify-hostname", 'a', NULL, 0,
    "ask hostname for verification" },
#ifdef HAVE___CHECK_RHOSTS_FILE
  { "no-rhosts", 'l', NULL, 0,
    "ignore .rhosts file" },
#endif
  { "no-keepalive", 'n', NULL, 0,
    "do not set SO_KEEPALIVE" },
  { "log-sessions", 'L', NULL, 0,
    "log successfull logins" },
#ifdef	KERBEROS
  /* FIXME: The option semantics does not match that of others r* utilities */
  { "kerberos", 'k', NULL, 0,
    "use kerberos IV authentication" },
  /* FIXME: Option name is misleading */
  { "vacuous", 'v', NULL, 0,
    "require Kerberos authentication" },
#endif
  { NULL }
};

#ifdef HAVE___CHECK_RHOSTS_FILE
extern int __check_rhosts_file;	/* hook in rcmd(3) */
#endif

#if defined __GLIBC__ && defined WITH_IRUSEROK
extern int iruserok (uint32_t raddr, int superuser,
                     const char *ruser, const char *luser);
#endif

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'a':
      check_all = 1;
      break;

#ifdef HAVE___CHECK_RHOSTS_FILE
    case 'l':
      __check_rhosts_file = 0;	/* don't check .rhosts file */
      break;
#endif

    case 'n':
      keepalive = 0;	/* don't enable SO_KEEPALIVE */
      break;

#if defined KERBEROS || defined SHISHI
    case 'k':
      use_kerberos = 1;
      break;

    case 'v':
      vacuous = 1;
      break;

# ifdef ENCRYPTION
    case 'x':
      doencrypt = 1;
      break;
# endif
#endif

    case 'L':
      log_success = 1;
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }

  return 0;
}


const char doc[] = "Remote shell server";
static struct argp argp = { options, parse_opt, NULL, doc};


/* Remote shell server. We're invoked by the rcmd(3) function. */
int
main (int argc, char *argv[])
{
  int index;
  struct linger linger;
  int on = 1;
  socklen_t fromlen;
  struct sockaddr_in from;
  int sockfd;

  set_program_name (argv[0]);
  iu_argp_init ("rshd", default_program_authors);
  argp_parse (&argp, argc, argv, 0, &index, NULL);

  openlog ("rshd", LOG_PID | LOG_ODELAY, LOG_DAEMON);

  argc -= index;
  if (argc > 0)
    {
      syslog (LOG_ERR, "%d extra arguments", argc);
      exit (EXIT_FAILURE);
    }

#if defined KERBEROS || defined SHISHI
  if (use_kerberos && vacuous)
    {
      syslog (LOG_ERR, "only one of -k and -v allowed");
      exit (EXIT_FAILURE);
    }
# ifdef ENCRYPTION
  if (doencrypt && !use_kerberos)
    {
      syslog (LOG_ERR, "-k is required for -x");
      exit (EXIT_FAILURE);
    }
# endif
#endif

  /*
   * We assume we're invoked by inetd, so the socket that the
   * connection is on, is open on descriptors 0, 1 and 2.
   * STD{IN,OUT,ERR}_FILENO.
   * We may in the future make it standalone for certain platform.
   */
  sockfd = STDIN_FILENO;

  /*
   * First get the Internet address of the client process.
   * This is requored for all the authentication we perform.
   */

  fromlen = sizeof from;
  if (getpeername (sockfd, (struct sockaddr *) &from, &fromlen) < 0)
    {
      syslog (LOG_ERR, "getpeername: %m");
      _exit (1);
    }

  /* Set the socket options: SO_KEEPALIVE and SO_LINGER */
  if (keepalive && setsockopt (sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *) &on,
			       sizeof on) < 0)
    syslog (LOG_WARNING, "setsockopt (SO_KEEPALIVE): %m");
  linger.l_onoff = 1;
  linger.l_linger = 60;		/* XXX */
  if (setsockopt (sockfd, SOL_SOCKET, SO_LINGER, (char *) &linger,
		  sizeof linger) < 0)
    syslog (LOG_WARNING, "setsockopt (SO_LINGER): %m");
  doit (sockfd, &from);
  return 0;
}

char username[20] = "USER=";
char logname[23] = "LOGNAME=";
char homedir[64] = "HOME=";
char shell[64] = "SHELL=";
char path[100] = "PATH=";
char *envinit[] = { homedir, shell, path, logname, username, 0 };
extern char **environ;

void
doit (int sockfd, struct sockaddr_in *fromp)
{
#ifdef HAVE___RCMD_ERRSTR
  extern char *__rcmd_errstr;	/* syslog hook from libc/net/rcmd.c. */
#endif
  struct hostent *hp;
  struct passwd *pwd;
  unsigned short port;
  fd_set ready, readfrom;
  int cc, nfd, pv[2], pid, s = sockfd;
  int one = 1;
  const char *hostname, *errorstr, *errorhost = NULL;
  char *cp, sig, buf[BUFSIZ];
  char *cmdbuf, *locuser, *remuser;

#ifdef	KERBEROS
  AUTH_DAT *kdata = (AUTH_DAT *) NULL;
  KTEXT ticket = (KTEXT) NULL;
  char instance[INST_SZ], version[VERSION_SIZE];
  struct sockaddr_in fromaddr;
  int rc;
  long authopts;
  int pv1[2], pv2[2];
  fd_set wready, writeto;

  fromaddr = *fromp;
#elif defined SHISHI
  int n;
  int pv1[2], pv2[2];
  fd_set wready, writeto;
  int keytype, keylen;
  int cksumtype, cksumlen;
  char *cksum = NULL;
#endif

  signal (SIGINT, SIG_DFL);
  signal (SIGQUIT, SIG_DFL);
  signal (SIGTERM, SIG_DFL);
#ifdef DEBUG
  {
    int t = open (PATH_TTY, O_RDWR);
    if (t >= 0)
      {
	ioctl (t, TIOCNOTTY, (char *) 0);
	close (t);
      }
  }
#endif
  /* Verify that the client's address is an Internet adress. */
  if (fromp->sin_family != AF_INET)
    {
      syslog (LOG_ERR, "malformed \"from\" address (af %d)\n",
	      fromp->sin_family);
      exit (EXIT_FAILURE);
    }
#ifdef IP_OPTIONS
  {
    unsigned char optbuf[BUFSIZ / 3], *cp;
    char lbuf[BUFSIZ], *lp;
    socklen_t optsize = sizeof (optbuf);
    int ipproto;
    struct protoent *ip;

    if ((ip = getprotobyname ("ip")) != NULL)
      ipproto = ip->p_proto;
    else
      ipproto = IPPROTO_IP;
    if (!getsockopt (sockfd, ipproto, IP_OPTIONS, (char *) optbuf,
		     &optsize) && optsize != 0)
      {
	lp = lbuf;
	/* The client has set IP options.  This isn't allowed.
	 * Use syslog() to record the fact.  Only the option
	 * types are printed, not their contents.
	 */
	for (cp = optbuf; optsize > 0; )
	  {
	    sprintf (lp, " %2.2x", *cp);
	    lp += 3;

	    if (*cp == IPOPT_SSRR || *cp == IPOPT_LSRR)
	      {
		/* Already the TCP handshake suffices for
		 * a man-in-the-middle attack vector.
		 */
		syslog (LOG_NOTICE,
			"Discarding connection from %s with set source routing",
			inet_ntoa (fromp->sin_addr));
		exit (EXIT_FAILURE);
	      }
	    if (*cp == IPOPT_EOL)
	      break;
	    if (*cp == IPOPT_NOP)
	      cp++, optsize--;
	    else
	      {
		/* Options using a length octet, see RFC 791.  */
		int inc = cp[1];

		optsize -= inc;
		cp += inc;
	      }
	  }

	/* At this point presumably harmless options are present.
	 * Make a report about them, erase them, and continue.  */
	syslog (LOG_NOTICE,
		"Connection received from %s using IP options (erased):%s",
		inet_ntoa (fromp->sin_addr), lbuf);

	/* Turn off the options.  If this doesn't work, we quit.  */
	if (setsockopt (sockfd, ipproto, IP_OPTIONS,
			(char *) NULL, optsize) != 0)
	  {
	    syslog (LOG_ERR, "setsockopt IP_OPTIONS NULL: %m");
	    exit (EXIT_FAILURE);
	  }
      }
  }
#endif

  /* Need host byte ordered port# to compare */
  fromp->sin_port = ntohs ((unsigned short) fromp->sin_port);
  /* Verify that the client's address was bound to a reserved port */
#if defined KERBEROS || defined SHISHI
  if (!use_kerberos)
#endif
    if (fromp->sin_port >= IPPORT_RESERVED
	|| fromp->sin_port < IPPORT_RESERVED / 2)
      {
	syslog (LOG_NOTICE | LOG_AUTH,
		"Connection from %s on illegal port %u",
		inet_ntoa (fromp->sin_addr), fromp->sin_port);
	exit (EXIT_FAILURE);
      }

  /* Read the ASCII string specifying the secondary port# from
   * the socket.  We set a timer of 60 seconds to do this read,
   * else we assume something is wrong.  If the client doesn't want
   * the secondary port, they just send the terminating null byte.
   */
  alarm (60);
  port = 0;
  for (;;)
    {
      char c;
      if ((cc = read (sockfd, &c, 1)) != 1)
	{
	  if (cc < 0)
	    syslog (LOG_NOTICE, "read: %m");
	  shutdown (sockfd, 2);
	  exit (EXIT_FAILURE);
	}
      /* null byte terminates the string */
      if (c == 0)
	break;
      port = port * 10 + c - '0';
    }

  alarm (0);
  if (port != 0)
    {
      /* If the secondary port# is non-zero, then we have to
       * connect to that port (which the client has already
       * created and is listening on).  The secondary port#
       * that the client tells us to connect to has also to be
       * a reserved port#.  Also, our end of this secondary
       * connection has also to have a reserved TCP port bound
       * to it, plus.
       */
      int lport = IPPORT_RESERVED - 1;
      s = rresvport (&lport);
      if (s < 0)
	{
	  syslog (LOG_ERR, "can't get stderr port: %m");
	  exit (EXIT_FAILURE);
	}
#if defined KERBEROS || defined SHISHI
      if (!use_kerberos)
#endif
	if (port >= IPPORT_RESERVED || port < IPPORT_RESERVED / 2)
	  {
	    syslog (LOG_ERR, "2nd port not reserved\n");
	    exit (EXIT_FAILURE);
	  }
      /* Use the fromp structure that we already have available.
       * The 32-bit Internet address is obviously that of the
       * client; just change the port# to the one specified
       * as secondary port by the client.
       */
      fromp->sin_port = htons (port);
      if (connect (s, (struct sockaddr *) fromp, sizeof (*fromp)) < 0)
	{
	  syslog (LOG_INFO, "connect second port %d: %m", port);
	  exit (EXIT_FAILURE);
	}
    }

#if defined KERBEROS || defined SHISHI
  if (vacuous)
    {
      rshd_error ("rshd: remote host requires Kerberos authentication\n");
      exit (EXIT_FAILURE);
    }
#endif

  /* from inetd, socket is already on 0, 1, 2 */
  if (sockfd != STDIN_FILENO)
    {
      dup2 (sockfd, STDIN_FILENO);
      dup2 (sockfd, STDOUT_FILENO);
      dup2 (sockfd, STDERR_FILENO);
    }

  /* Get the "name" of the client from its Internet address.  This is
   * used for the authentication below.
   */
  errorstr = NULL;
  hp = gethostbyaddr ((char *) &fromp->sin_addr, sizeof (struct in_addr),
		      fromp->sin_family);
  if (hp)
    {
      /*
       * If name returned by gethostbyaddr is in our domain,
       * attempt to verify that we haven't been fooled by someone
       * in a remote net; look up the name and check that this
       * address corresponds to the name.
       */
      hostname = strdup (hp->h_name);
#if defined KERBEROS || defined SHISHI
      if (!use_kerberos)
#endif
	if (check_all || local_domain (hp->h_name))
	  {
	    char *remotehost = alloca (strlen (hostname) + 1);
	    if (!remotehost)
	      errorstr = "Out of memory\n";
	    else
	      {
		strcpy (remotehost, hostname);
		errorhost = remotehost;
		hp = gethostbyname (remotehost);
		if (hp == NULL)
		  {
		    syslog (LOG_INFO,
			    "Couldn't look up address for %s", remotehost);
		    errorstr =
		      "Couldn't look up address for your host (%s)\n";
		    hostname = inet_ntoa (fromp->sin_addr);
		  }
		else
		  for (;; hp->h_addr_list++)
		    {
		      if (hp->h_addr_list[0] == NULL)
			{
			  syslog (LOG_NOTICE,
				  "Host addr %s not listed for host %s",
				  inet_ntoa (fromp->sin_addr), hp->h_name);
			  errorstr = "Host address mismatch for %s\n";
			  hostname = inet_ntoa (fromp->sin_addr);
			  break;
			}
		      if (!memcmp (hp->h_addr_list[0],
				   (caddr_t) & fromp->sin_addr,
				   sizeof fromp->sin_addr))
			{
			  hostname = hp->h_name;
			  break;	/* equal, OK */
			}
		    }
	      }
	  }
    }
  else
    errorhost = hostname = inet_ntoa (fromp->sin_addr);

#ifdef	KERBEROS
  if (use_kerberos)
    {
      kdata = (AUTH_DAT *) authbuf;
      ticket = (KTEXT) tickbuf;
      authopts = 0L;
      strcpy (instance, "*");
      version[VERSION_SIZE - 1] = '\0';
# ifdef ENCRYPTION
      if (doencrypt)
	{
	  struct sockaddr_in local_addr;
	  rc = sizeof local_addr;
	  if (getsockname (STDIN_FILENO,
			   (struct sockaddr *) &local_addr, &rc) < 0)
	    {
	      syslog (LOG_ERR, "getsockname: %m");
	      rshd_error ("rlogind: getsockname: %s", strerror (errno));
	      exit (EXIT_FAILURE);
	    }
	  authopts = KOPT_DO_MUTUAL;
	  rc = krb_recvauth (authopts, 0, ticket,
			     "rcmd", instance, &fromaddr,
			     &local_addr, kdata, "", schedule, version);
	  des_set_key (kdata->session, schedule);
	}
      else
# endif
	rc = krb_recvauth (authopts, 0, ticket, "rcmd",
			   instance, &fromaddr,
			   (struct sockaddr_in *) 0,
			   kdata, "", (bit_64 *) 0, version);
      if (rc != KSUCCESS)
	{
	  rshd_error ("Kerberos authentication failure: %s\n",
		      krb_err_txt[rc]);
	  exit (EXIT_FAILURE);
	}
    }
  else
#elif defined (SHISHI)
  if (use_kerberos)
    {
      int rc;
      char *err_msg;

      rc = get_auth (STDIN_FILENO, &h, &ap, &enckey, &err_msg, &protocol,
		     &cksumtype, &cksum, &cksumlen);
      if (rc != SHISHI_OK)
	{
	  rshd_error ("Kerberos authentication failure: %s\n",
		      err_msg ? err_msg : shishi_strerror (rc));
	  exit (EXIT_FAILURE);
	}
    }
  else
#endif
    remuser = getstr ("remuser");

  /* Read three strings from the client. */
  locuser = getstr ("locuser");
  cmdbuf = getstr ("command");

#ifdef SHISHI
  {
    int error;
    int rc;
    char *compcksum;
    size_t compcksumlen;
    char cksumdata[100];
    struct sockaddr_in sock;
    size_t socklen;

# ifdef ENCRYPTION
    if (strlen (cmdbuf) >= 3)
      if (!strncmp (cmdbuf, "-x ", 3))
	{
	  doencrypt = 1;
	  int i;

	  ivtab[0] = &iv1;
	  ivtab[1] = &iv2;
	  ivtab[2] = &iv3;
	  ivtab[3] = &iv4;

	  keytype = shishi_key_type (enckey);
	  keylen = shishi_cipher_blocksize (keytype);

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
		  memset (ivtab[i]->iv, 2 * i - 3 * (i >= 2),
			  ivtab[i]->ivlen);
		  ivtab[i]->ctx =
		    shishi_crypto (h, enckey, ivtab[i]->keyusage,
				   shishi_key_type (enckey), ivtab[i]->iv,
				   ivtab[i]->ivlen);
		  break;

		case SHISHI_ARCFOUR_HMAC:
		case SHISHI_ARCFOUR_HMAC_EXP:
		  ivtab[i]->keyusage =
		    SHISHI_KEYUSAGE_KCMD_DES + 4 * (i < 2) + 2 + 2 * (i % 2);
		  ivtab[i]->ctx =
		    shishi_crypto (h, enckey, ivtab[i]->keyusage,
				   shishi_key_type (enckey), NULL, 0);
		  break;

		default:
		  ivtab[i]->keyusage =
		    SHISHI_KEYUSAGE_KCMD_DES + 4 * (i < 2) + 2 + 2 * (i % 2);
		  ivtab[i]->iv = malloc (ivtab[i]->ivlen);
		  memset (ivtab[i]->iv, 0, ivtab[i]->ivlen);
		  if (protocol == 2)
		    ivtab[i]->ctx =
		      shishi_crypto (h, enckey, ivtab[i]->keyusage,
				     shishi_key_type (enckey), ivtab[i]->iv,
				     ivtab[i]->ivlen);
		}
	    }

	}
# endif

    remuser = getstr ("remuser");
    rc = read (STDIN_FILENO, &error, sizeof (int));
    if ((rc != sizeof (int)) && rc)
      exit (EXIT_FAILURE);

    /* verify checksum */

    /* Doesn't give socket port ?
       if (getsockname (STDIN_FILENO, (struct sockaddr *)&sock, &socklen) < 0)
       {
       syslog (LOG_ERR, "Can't get sock name");
       exit (EXIT_FAILURE);
       }
     */
    snprintf (cksumdata, 100, "544:%s%s", /*ntohs(sock.sin_port), */ cmdbuf,
	      locuser);
    rc =
      shishi_checksum (h, enckey, 0, cksumtype, cksumdata, strlen (cksumdata),
		       &compcksum, &compcksumlen);
    free (cksum);
    if (rc != SHISHI_OK
	|| compcksumlen != cksumlen
	|| memcmp (compcksum, cksum, cksumlen) != 0)
      {
	/* err_msg crash ? */
	/* *err_msg = "checksum verify failed"; */
	syslog (LOG_ERR, "checksum verify failed: %s", shishi_error (h));
	free (compcksum);
	exit (EXIT_FAILURE);
      }

    rc = shishi_authorized_p (h, shishi_ap_tkt (ap), locuser);
    if (!rc)
      {
	syslog (LOG_ERR, "User is not authorized to log in as: %s", locuser);
	shishi_ap_done (ap);
	exit (EXIT_FAILURE);
      }

    free (compcksum);

    shishi_ap_done (ap);

  }
#endif

  /* Look up locuser in the passerd file.  The locuser has\* to be a
   * valid account on this system.
   */
  setpwent ();
  pwd = getpwnam (locuser);
  if (pwd == NULL)
    {
      syslog (LOG_INFO | LOG_AUTH, "%s@%s as %s: unknown login. cmd='%.80s'",
	      remuser, hostname, locuser, cmdbuf);
      if (errorstr == NULL)
	errorstr = "Login incorrect.\n";
      goto fail;
    }

#ifdef	KERBEROS
  if (use_kerberos)
    {
      if (pwd->pw_passwd != 0 && *pwd->pw_passwd != '\0')
	{
	  if (kuserok (kdata, locuser) != 0)
	    {
	      syslog (LOG_INFO | LOG_AUTH, "Kerberos rsh denied to %s.%s@%s",
		      kdata->pname, kdata->pinst, kdata->prealm);
	      rshd_error ("Permission denied.\n");
	      exit (EXIT_FAILURE);
	    }
	}
    }
  else
#elif defined(SHISHI)
  if (use_kerberos)
    {				/*
				   if (pwd->pw_passwd != 0 && *pwd->pw_passwd != '\0')
				   {
				   if (kuserok (kdata, locuser) != 0)
				   {
				   syslog (LOG_INFO|LOG_AUTH, "Kerberos rsh denied to %s.%s@%s",
				   kdata->pname, kdata->pinst, kdata->prealm);
				   rshd_error ("Permission denied.\n");
				   exit (EXIT_FAILURE);
				   }
				   } */
    }
  else
#endif
#ifdef WITH_IRUSEROK
    if (errorstr || (pwd->pw_passwd != 0 && *pwd->pw_passwd != '\0'
                     && (iruserok (fromp->sin_addr.s_addr, pwd->pw_uid == 0,
                                   remuser, locuser)) < 0))
#elif defined WITH_RUSEROK
    if (errorstr || (pwd->pw_passwd != 0 && *pwd->pw_passwd != '\0'
                     && (ruserok (inet_ntoa (fromp->sin_addr),
				  pwd->pw_uid == 0, remuser, locuser)) < 0))
#else /* !WITH_IRUSEROK && !WITH_RUSEROK */
#error Unable to use mandatory iruserok/ruserok.  This should not happen.
#endif /* !WITH_IRUSEROK && !WITH_RUSEROK */
    {
#ifdef HAVE___RCMD_ERRSTR
      if (__rcmd_errstr)
	syslog (LOG_INFO | LOG_AUTH,
		"%s@%s as %s: permission denied (%s). cmd='%.80s'",
		remuser, hostname, locuser, __rcmd_errstr, cmdbuf);
      else
#endif
	syslog (LOG_INFO | LOG_AUTH,
		"%s@%s as %s: permission denied. cmd='%.80s'",
		remuser, hostname, locuser, cmdbuf);
    fail:
      if (errorstr == NULL)
	errorstr = "Permission denied.\n";
      rshd_error (errorstr, errorhost);
      exit (EXIT_FAILURE);
    }

  /* If the locuser isn't root, then check if logins are disabled. */
  if (pwd->pw_uid && !access (PATH_NOLOGIN, F_OK))
    {
      rshd_error ("Logins currently disabled.\n");
      exit (EXIT_FAILURE);
    }

  /* Now write the null byte back to the client telling it
   * that everything is OK.
   * Note that this means that any error message that we generate
   * from now on (such as the perror() if the execl() fails), won't
   * be seen by the rcomd() fucntion, but will be seen by the
   * application that called rcmd() when it reads from the socket.
   */
  if (write (STDERR_FILENO, "\0", 1) < 0)
    {
      rshd_error ("Lost connection.\n");
      exit (EXIT_FAILURE);
    }
  sent_null = 1;

  if (port)
    {
      /* We nee a secondary channel,  Here's where we create
       * the control process that'll handle this secondary
       * channel.
       * First create a pipe to use for communication between
       * the parent and child, then fork.
       */
      if (pipe (pv) < 0)
	{
	  rshd_error ("Can't make pipe.\n");
	  exit (EXIT_FAILURE);
	}
#ifdef ENCRYPTION
# if defined KERBEROS || defined SHISHI
      if (doencrypt)
	{
	  if (pipe (pv1) < 0)
	    {
	      rshd_error ("Can't make 2nd pipe.\n");
	      exit (EXIT_FAILURE);
	    }
	  if (pipe (pv2) < 0)
	    {
	      rshd_error ("Can't make 3rd pipe.\n");
	      exit (EXIT_FAILURE);
	    }
	}
# endif
#endif
      pid = fork ();
      if (pid == -1)
	{
	  rshd_error ("Can't fork; try again.\n");
	  exit (EXIT_FAILURE);
	}
      if (pid)
	{
	  /* Parent process == control process.
	   * We: (1) read from the pipe and write to s;
	   *     (2) read from s and send corresponding
	   *         signal.
	   */
#ifdef ENCRYPTION
# if defined KERBEROS
	  if (doencrypt)
	    {
	      static char msg[] = SECURE_MESSAGE;
	      close (pv1[1]);
	      close (pv2[1]);
	      des_write (s, msg, sizeof (msg) - 1);
	    }
	  else
# elif defined(SHISHI)
	  if (doencrypt)
	    {
	      close (pv1[1]);
	      close (pv2[1]);
	    }
	  else
# endif
#endif
	    {
	      /* child handles the original socket */
	      close (STDIN_FILENO);
	      /* (0, 1, and 2 were from inetd */
	      close (STDOUT_FILENO);
	    }
	  close (STDERR_FILENO);
	  close (pv[1]);	/* close write end of pipe */

	  FD_ZERO (&readfrom);
	  FD_SET (s, &readfrom);
	  FD_SET (pv[0], &readfrom);
	  /* set max fd + 1 for select */
	  if (pv[0] > s)
	    nfd = pv[0];
	  else
	    nfd = s;
#ifdef ENCRYPTION
# if defined KERBEROS || defined SHISHI
	  if (doencrypt)
	    {
	      FD_ZERO (&writeto);
	      FD_SET (pv2[0], &writeto);
	      FD_SET (pv1[0], &readfrom);

	      nfd = MAX (nfd, pv2[0]);
	      nfd = MAX (nfd, pv1[0]);
	    }
	  else
# endif
#endif
	    ioctl (pv[0], FIONBIO, (char *) &one);
	  /* should set s nbio! */
	  nfd++;
	  do
	    {
	      ready = readfrom;
#ifdef ENCRYPTION
# if defined KERBEROS || defined SHISHI
	      if (doencrypt)
		{
#  ifdef SHISHI
		  wready = readfrom;
#  else
		  wready = writeto;
#  endif
		  if (select (nfd, &ready, &wready, (fd_set *) 0,
			      (struct timeval *) 0) < 0)
		    break;
		}
	      else
# endif
#endif
	      if (select (nfd, &ready, (fd_set *) 0,
			    (fd_set *) 0, (struct timeval *) 0) < 0)
		/* wait until something to read */
		break;
	      if (FD_ISSET (s, &ready))
		{
		  int ret;
#ifdef ENCRYPTION
# ifdef KERBEROS
		  if (doencrypt)
		    ret = des_read (s, &sig, 1);
		  else
# elif defined(SHISHI)
		  if (doencrypt)
		    readenc (h, s, &sig, &ret, &iv2, enckey, protocol);
		  else
# endif
#endif
		    ret = read (s, &sig, 1);
		  if (ret <= 0)
		    FD_CLR (s, &readfrom);
		  else
		    killpg (pid, sig);
		}
	      if (FD_ISSET (pv[0], &ready))
		{
		  errno = 0;
		  cc = read (pv[0], buf, sizeof buf);
		  if (cc <= 0)
		    {
		      shutdown (s, 1 + 1);
		      FD_CLR (pv[0], &readfrom);
		    }
		  else
		    {
#ifdef ENCRYPTION
# ifdef KERBEROS
		      if (doencrypt)
			des_write (s, buf, cc);
		      else
# elif defined(SHISHI)
		      if (doencrypt)
			writeenc (h, s, buf, cc, &n, &iv4, enckey, protocol);
		      else
# endif
#endif
			write (s, buf, cc);
		    }
		}
#ifdef ENCRYPTION
# if defined KERBEROS || defined SHISHI
	      if (doencrypt && FD_ISSET (pv1[0], &ready))
		{
		  errno = 0;
		  cc = read (pv1[0], buf, sizeof (buf));
		  if (cc <= 0)
		    {
		      shutdown (pv1[0], 1 + 1);
		      FD_CLR (pv1[0], &readfrom);
		    }
		  else
#  ifdef SHISHI
		    writeenc (h, STDOUT_FILENO, buf, cc, &n, &iv3, enckey,
			      protocol);
#  else
		    des_write (STDOUT_FILENO, buf, cc);
#  endif
		}

	      if (doencrypt && FD_ISSET (pv2[0], &wready))
		{
		  errno = 0;
#  ifdef SHISHI
		  readenc (h, STDIN_FILENO, buf, &cc, &iv1, enckey, protocol);
#  else
		  cc = des_read (STDIN_FILENO, buf, sizeof buf);
#  endif
		  if (cc <= 0)
		    {
		      shutdown (pv2[0], 1 + 1);
		      FD_CLR (pv2[0], &writeto);
		    }
		  else
		    write (pv2[0], buf, cc);
		}
# endif
#endif
	    }
	  while (FD_ISSET (s, &readfrom) ||
#ifdef ENCRYPTION
# if defined KERBEROS || defined SHISHI
		 (doencrypt && FD_ISSET (pv1[0], &readfrom)) ||
# endif
#endif
		 FD_ISSET (pv[0], &readfrom));
	  /* The pipe will generat an EOR whe the shell
	   * terminates.  The socket will terninate whe the
	   * client process terminates.
	   */
	  exit (EXIT_SUCCESS);
	}

      /* Child process. Become a process group leader, so that
       * the control process above can send signals to all the
       * processes we may be the parent of.  The process group ID
       * (the getpid() value below) equals the childpid value from
       * the fork above.
       */
      setpgid (0, getpid ());
      close (s);		/* control process handles this fd */
      close (pv[0]);		/* close read end of pipe */
#ifdef ENCRYPTION
# if defined KERBEROS || defined SHISHI
      if (doencrypt)
	{
	  close (pv1[0]);
	  close (pv2[0]);
	  dup2 (pv1[1], 1);
	  dup2 (pv2[1], 0);
	  close (pv1[1]);
	  close (pv2[1]);
	}
# endif
#endif

#if defined SHISHI
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

      dup2 (pv[1], STDERR_FILENO);	/* stderr of shell has to go
					   pipe to control process */
      close (pv[1]);
    }
  if (*pwd->pw_shell == '\0')
    pwd->pw_shell = PATH_BSHELL;
#if BSD > 43
  if (setlogin (pwd->pw_name) < 0)
    syslog (LOG_ERR, "setlogin() failed: %m");
#endif

  /* Set the fid, then uid to become the user specified by "locuser" */
  setegid ((gid_t) pwd->pw_gid);
  setgid ((gid_t) pwd->pw_gid);
#ifdef HAVE_INITGROUPS
  initgroups (pwd->pw_name, pwd->pw_gid);	/* BSD groups */
#endif

  setuid ((uid_t) pwd->pw_uid);

  /* We'll execute the client's command in the home directory
   * of locuser. Note, that the chdir must be executed after
   * setuid(), otherwise it may fail on NFS mounted directories
   * (root mapped to nobody).
   */
  if (chdir (pwd->pw_dir) < 0)
    {
      chdir ("/");
      syslog (LOG_INFO | LOG_AUTH,
	      "%s@%s as %s: no home directory. cmd='%.80s'", remuser,
	      hostname, locuser, cmdbuf);
      rshd_error ("No remote directory.\n");
    }

  /* Set up an initial environment for the shell that we exec() */
  environ = envinit;
  strncat (homedir, pwd->pw_dir, sizeof (homedir) - 6);
  strcat (path, PATH_DEFPATH);
  strncat (shell, pwd->pw_shell, sizeof (shell) - 7);
  strncat (username, pwd->pw_name, sizeof (username) - 6);
  cp = strrchr (pwd->pw_shell, '/');
  if (cp)
    cp++;			/* step past first slash */
  else
    cp = pwd->pw_shell;		/* no slash in shell string */
  endpwent ();
  if (log_success || pwd->pw_uid == 0)
    {
#ifdef	KERBEROS
      if (use_kerberos)
	syslog (LOG_INFO | LOG_AUTH,
		"Kerberos shell from %s.%s@%s on %s as %s, cmd='%.80s'",
		kdata->pname, kdata->pinst, kdata->prealm,
		hostname, locuser, cmdbuf);
      else
#endif
	syslog (LOG_INFO | LOG_AUTH, "%s@%s as %s: cmd='%.80s'",
		remuser, hostname, locuser, cmdbuf);
    }
#ifdef SHISHI
  if (doencrypt)
    execl (pwd->pw_shell, cp, "-c", cmdbuf + 3, NULL);
  else
#endif
    execl (pwd->pw_shell, cp, "-c", cmdbuf, NULL);
  error (EXIT_FAILURE, errno, "cannot execute %s", pwd->pw_shell);
}

/*
 * Report error to client.  Note: can't be used until second socket has
 * connected to client, or older clients will hang waiting for that
 * connection first.
 */

void
rshd_error (const char *fmt, ...)
{
  va_list ap;
  int len;
  char *bp, buf[BUFSIZ];
  va_start (ap, fmt);

  bp = buf;
  if (sent_null == 0)
    {
      *bp++ = 1;
      len = 1;
    }
  else
    len = 0;
  vsnprintf (bp, sizeof (buf) - 1, fmt, ap);
  write (STDERR_FILENO, buf, len + strlen (bp));
}

char *
getstr (const char *err)
{
  size_t buf_len = 100;
  char *buf = malloc (buf_len), *end = buf;

  if (!buf)
    {
      rshd_error ("Out of space reading %s\n", err);
      exit (EXIT_FAILURE);
    }

  do
    {
      /* Oh this is efficient, oh yes.  [But what can be done?] */
      int rd = read (STDIN_FILENO, end, 1);
      if (rd <= 0)
	{
	  if (rd == 0)
	    rshd_error ("EOF reading %s\n", err);
	  else
	    perror (err);
	  exit (EXIT_FAILURE);
	}

      end += rd;
      if ((buf + buf_len - end) < (buf_len >> 3))
	{
	  /* Not very much room left in our buffer, grow it. */
	  size_t end_offs = end - buf;
	  buf_len += buf_len;
	  buf = realloc (buf, buf_len);
	  if (!buf)
	    {
	      rshd_error ("Out of space reading %s\n", err);
	      exit (EXIT_FAILURE);
	    }
	  end = buf + end_offs;
	}
    }
  while (*(end - 1));

  return buf;
}

/*
 * Check whether host h is in our local domain,
 * defined as sharing the last two components of the domain part,
 * or the entire domain part if the local domain has only one component.
 * If either name is unqualified (contains no '.'),
 * assume that the host is local, as it will be
 * interpreted as such.
 */
int
local_domain (const char *h)
{
  char *hostname = localhost ();

  if (!hostname)
    return 0;
  else
    {
      int is_local = 0;
      const char *p1 = topdomain (hostname);
      const char *p2 = topdomain (h);

      if (p1 == NULL || p2 == NULL || !strcasecmp (p1, p2))
	is_local = 1;

      free (hostname);

      return is_local;
    }
}

const char *
topdomain (const char *h)
{
  const char *p, *maybe = NULL;
  int dots = 0;

  for (p = h + strlen (h); p >= h; p--)
    {
      if (*p == '.')
	{
	  if (++dots == 2)
	    return p;
	  maybe = p;
	}
    }
  return maybe;
}
