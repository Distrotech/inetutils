/*
  Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008,
  2009, 2010 Free Software Foundation, Inc.

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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <syslog.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#ifdef HAVE_TCPD_H
# include <tcpd.h>
#endif

static void reapchild (int);

#define DEFPORT 21

#ifdef WITH_WRAP

int allow_severity = LOG_INFO;
int deny_severity = LOG_NOTICE;

static int
check_host (struct sockaddr *sa)
{
  struct sockaddr_in *sin;
  struct hostent *hp;
  char *addr;

  if (sa->sa_family != AF_INET)
    return 1;

  sin = (struct sockaddr_in *) sa;
  hp = gethostbyaddr ((char *) &sin->sin_addr,
		      sizeof (struct in_addr), AF_INET);
  addr = inet_ntoa (sin->sin_addr);
  if (hp)
    {
      if (!hosts_ctl ("ftpd", hp->h_name, addr, STRING_UNKNOWN))
	{
	  syslog (LOG_NOTICE, "tcpwrappers rejected: %s [%s]",
		  hp->h_name, addr);
	  return 0;
	}
    }
  else
    {
      if (!hosts_ctl ("ftpd", STRING_UNKNOWN, addr, STRING_UNKNOWN))
	{
	  syslog (LOG_NOTICE, "tcpwrappers rejected: [%s]", addr);
	  return 0;
	}
    }
  return (1);
}
#endif

static RETSIGTYPE
reapchild (int signo ARG_UNUSED)
{
  int save_errno = errno;

  while (waitpid (-1, NULL, WNOHANG) > 0)
    ;
  errno = save_errno;
}

int
server_mode (const char *pidfile, struct sockaddr_in *phis_addr)
{
  int ctl_sock, fd;
  struct servent *sv;
  int port;
  static struct sockaddr_in server_addr;	/* Our address.  */

  /* Become a daemon.  */
  if (daemon (1, 1) < 0)
    {
      syslog (LOG_ERR, "failed to become a daemon");
      return -1;
    }
  signal (SIGCHLD, reapchild);

  /* Get port for ftp/tcp.  */
  sv = getservbyname ("ftp", "tcp");
  port = (sv == NULL) ? DEFPORT : ntohs (sv->s_port);

  /* Open socket, bind and start listen.  */
  ctl_sock = socket (AF_INET, SOCK_STREAM, 0);
  if (ctl_sock < 0)
    {
      syslog (LOG_ERR, "control socket: %m");
      return -1;
    }

  /* Enable local address reuse.  */
  {
    int on = 1;
    if (setsockopt (ctl_sock, SOL_SOCKET, SO_REUSEADDR,
		    (char *) &on, sizeof (on)) < 0)
      syslog (LOG_ERR, "control setsockopt: %m");
  }

  memset (&server_addr, 0, sizeof server_addr);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons (port);

  if (bind (ctl_sock, (struct sockaddr *) &server_addr, sizeof server_addr))
    {
      syslog (LOG_ERR, "control bind: %m");
      return -1;
    }
  if (listen (ctl_sock, 32) < 0)
    {
      syslog (LOG_ERR, "control listen: %m");
      return -1;
    }

  /* Stash pid in pidfile.  */
  {
    FILE *pid_fp = fopen (pidfile, "w");
    if (pid_fp == NULL)
      syslog (LOG_ERR, "can't open %s: %m", PATH_FTPDPID);
    else
      {
	fprintf (pid_fp, "%d\n", getpid ());
	fchmod (fileno (pid_fp), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	fclose (pid_fp);
      }
  }

  /* Loop forever accepting connection requests and forking off
     children to handle them.  */
  while (1)
    {
      socklen_t addrlen = sizeof (*phis_addr);
      fd = accept (ctl_sock, (struct sockaddr *) phis_addr, &addrlen);
      if (fork () == 0)		/* child */
	{
	  dup2 (fd, 0);
	  dup2 (fd, 1);
	  close (ctl_sock);
	  break;
	}
      close (fd);
    }

#ifdef WITH_WRAP
  /* In the child.  */
  if (!check_host ((struct sockaddr *) phis_addr))
    return -1;
#endif
  return fd;
}
