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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#ifdef HAVE_OSOCKADDR_H
# include <osockaddr.h>
#endif
#include <protocols/talkd.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include "talk_ctl.h"

int
get_addrs (char *my_machine_name, char *his_machine_name)
{
  struct hostent *hp;
  struct servent *sp;

  msg.pid = htonl (getpid ());
  /* look up the address of the local host */
  hp = gethostbyname (my_machine_name);
  if (hp == NULL)
    {
      fprintf (stderr, "talk: %s: ", my_machine_name);
      herror ((char *) NULL);
      exit (-1);
    }
  bcopy (hp->h_addr, (char *) &my_machine_addr, hp->h_length);
  /*
   * If the callee is on-machine, just copy the
   * network address, otherwise do a lookup...
   */
  if (strcmp (his_machine_name, my_machine_name))
    {
      hp = gethostbyname (his_machine_name);
      if (hp == NULL)
	{
	  fprintf (stderr, "talk: %s: ", his_machine_name);
	  herror ((char *) NULL);
	  exit (-1);
	}
      bcopy (hp->h_addr, (char *) &his_machine_addr, hp->h_length);
    }
  else
    his_machine_addr = my_machine_addr;
  /* find the server's port */
  sp = getservbyname ("ntalk", "udp");
  if (sp == 0)
    {
      fprintf (stderr, "talk: %s/%s: service is not registered.\n",
	       "ntalk", "udp");
      exit (-1);
    }
  daemon_port = sp->s_port;

  return 0;
}
