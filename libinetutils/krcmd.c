/*
  Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005,
  2006, 2007, 2008, 2009, 2010 Free Software Foundation, Inc.

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
 * Copyright (c) 1989, 1993
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

#if defined(KERBEROS) || defined(SHISHI)
# include <sys/types.h>
# ifdef ENCRYPTION
#  include <sys/socket.h>
# endif

# include <netinet/in.h>

# ifdef KERBEROS
#  include <kerberosIV/des.h>
#  include <kerberosIV/krb.h>
# elif defined(SHISHI)
#  include <shishi.h>
#  include "shishi_def.h"
# endif

# include <stdio.h>

# define SERVICE_NAME	"rcmd"

# if defined(SHISHI)
int kcmd (Shishi **, int *, char **, u_short, char *, char **,
	  char *, int *, char *, char *, Shishi_key **,
	  struct sockaddr_in *, struct sockaddr_in *, long);
# else
int kcmd (int *, char **, u_short, char *, char *, char *, int *,
	  KTEXT, char *, char *, CREDENTIALS *, Key_schedule,
	  MSG_DAT *, struct sockaddr_in *, struct sockaddr_in *, long);
# endif

/*
 * krcmd: simplified version of Athena's "kcmd"
 *	returns a socket attached to the destination, -1 or krb error on error
 *	if fd2p is non-NULL, another socket is filled in for it
 */

# if defined(SHISHI)
int
krcmd (Shishi ** h, char **ahost, u_short rport, char **remuser, char *cmd,
       int *fd2p, char *realm)
{
  int sock = -1, err = 0;
  long authopts = 0L;

  err = kcmd (h, &sock, ahost, rport, NULL,	/* locuser not used */
	      remuser, cmd, fd2p, SERVICE_NAME, realm, NULL,	/* key schedule not used */
	      NULL,		/* local addr not used */
	      NULL,		/* foreign addr not used */
	      authopts);

  if (err > SHISHI_OK)
    {
      fprintf (stderr, "krcmd: %s\n", "error");
      return (-1);
    }
  if (err < 0)
    return (-1);
  return (sock);
}

# elif defined(KERBEROS)
int
krcmd (char **ahost, u_short rport, char *remuser, char *cmd, int *fd2p,
       char *realm)
{
  int sock = -1, err = 0;
  KTEXT_ST ticket;
  long authopts = 0L;

  err = kcmd (&sock, ahost, rport, NULL,	/* locuser not used */
	      remuser, cmd, fd2p, &ticket, SERVICE_NAME, realm, NULL,	/* credentials not used */
	      (bit_64 *) NULL,	/* key schedule not used */
	      (MSG_DAT *) NULL,	/* MSG_DAT not used */
	      (struct sockaddr_in *) NULL,	/* local addr not used */
	      (struct sockaddr_in *) NULL,	/* foreign addr not used */
	      authopts);

  if (err > KSUCCESS && err < MAX_KRB_ERRORS)
    {
      fprintf (stderr, "krcmd: %s\n", krb_err_txt[err]);
      return (-1);
    }
  if (err < 0)
    return (-1);
  return (sock);
}
# endif

# ifdef ENCRYPTION

#  if defined(SHISHI)
int
krcmd_mutual (Shishi ** h, char **ahost, u_short rport, char **remuser,
	      char *cmd, int *fd2p, char *realm, Shishi_key ** key)
{
  int sock = -1, err = 0;
  struct sockaddr_in laddr, faddr;
  long authopts = SHISHI_APOPTIONS_MUTUAL_REQUIRED;

  err = kcmd (h, &sock, ahost, rport, NULL,	/* locuser not used */
	      remuser, cmd, fd2p, SERVICE_NAME, realm, key,	/* filled in */
	      &laddr,		/* filled in */
	      &faddr,		/* filled in */
	      authopts);

  if (err > SHISHI_OK)
    {
      fprintf (stderr, "krcmd_mutual: %s\n", "error");
      return (-1);
    }

  if (err < 0)
    return (-1);
  return (sock);
}

#  elif defined(KERBEROS)
int
krcmd_mutual (char **ahost, u_short rport, char *remuser, char *cmd,
	      int *fd2p, char *realm, CREDENTIALS * cred, Key_schedule sched)
{
  int sock, err;
  KTEXT_ST ticket;
  MSG_DAT msg_dat;
  struct sockaddr_in laddr, faddr;
  long authopts = KOPT_DO_MUTUAL;

  err = kcmd (&sock, ahost, rport, NULL,	/* locuser not used */
	      remuser, cmd, fd2p, &ticket, SERVICE_NAME, realm, cred,	/* filled in */
	      sched,		/* filled in */
	      &msg_dat,		/* filled in */
	      &laddr,		/* filled in */
	      &faddr,		/* filled in */
	      authopts);

  if (err > KSUCCESS && err < MAX_KRB_ERRORS)
    {
      fprintf (stderr, "krcmd_mutual: %s\n", krb_err_txt[err]);
      return (-1);
    }

  if (err < 0)
    return (-1);
  return (sock);
}
#  endif /* CRYPT */
# endif	/* KERBEROS */
#endif /* KERBEROS */
