/*
  Copyright (C) 2009, 2010 Free Software Foundation, Inc.

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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pwd.h>

#include <argp.h>
#include <libinetutils.h>
#include <progname.h>
#include <ctype.h>
#include <error.h>
#include <xalloc.h>
#include <inttostr.h>

#define SYSLOG_NAMES
#include <syslog.h>
#ifndef HAVE_SYSLOG_INTERNAL
# include <syslog-int.h>
#endif

#define MAKE_PRI(fac,pri) (((fac) & LOG_FACMASK) | ((pri) & LOG_PRIMASK))

static char *tag = NULL;
static int logflags = 0;
static int pri = MAKE_PRI (LOG_USER, LOG_NOTICE);
static char *host = PATH_LOG;
static char *source;
static char *pidstr;



int
decode (char *name, CODE *codetab, const char *what)
{
  CODE *cp;

  if (isdigit (*name))
    {
      char *p;
      int c;
      unsigned long n = strtoul (name, &p, 0);

      if (*p || (c = n) != n)
	error (EXIT_FAILURE, 0, "%s: invalid %s number", what, name);
      return c;
    }

  for (cp = codetab; cp->c_name; cp++)
    {
      if (strcasecmp (name, cp->c_name) == 0)
	return cp->c_val;
    }
  error (EXIT_FAILURE, 0, "unknown %s name: %s", what, name);
  return -1; /* to pacify gcc */
}

int
parse_level (char *str)
{
  char *p;
  int fac, pri = 0;

  p = strchr (str, '.');
  if (p)
    *p++ = 0;

  fac = decode (str, facilitynames, "facility");
  if (p)
    pri = decode (p, prioritynames, "priority");
  return MAKE_PRI (fac, pri);
}


union logger_sockaddr
  {
    struct sockaddr sa;
    struct sockaddr_in sinet;
    struct sockaddr_un sunix;
};

int fd;

static void
open_socket ()
{
  union logger_sockaddr sockaddr;
  socklen_t socklen;
  int family;

  if (host[0] == '/')
    {
      size_t len = strlen (host);
      if (len >= sizeof sockaddr.sunix.sun_path)
	error (EXIT_FAILURE, 0, "UNIX socket name too long");
      strcpy (sockaddr.sunix.sun_path, host);
      sockaddr.sunix.sun_family = AF_UNIX;
      family = PF_UNIX;
      socklen = sizeof (sockaddr.sunix);
    }
  else
    {
      struct hostent *hp;
      struct servent *sp;
      unsigned short port;
      char *p;

      p = strchr (host, ':');
      if (p)
	*p++ = 0;

      hp = gethostbyname (host);
      if (hp)
	sockaddr.sinet.sin_addr.s_addr = *(unsigned long*) hp->h_addr_list[0];
      else if (inet_aton (host, (struct in_addr *) &sockaddr.sinet.sin_addr)
	       != 1)
	error (EXIT_FAILURE, 0, "unknown host name");

      sockaddr.sinet.sin_family = AF_INET;
      family = PF_INET;
      if (!p)
	p = "syslog";

      if (isdigit (*p))
	{
	  char *end;
	  unsigned long n = strtoul (p, &end, 10);
	  if (*end || (port = n) != n)
	    error (EXIT_FAILURE, 0, "%s: invalid port number", p);
	  port = htons (port);
	}
      else if ((sp = getservbyname (p, "udp")) != NULL)
	port = sp->s_port;
      else
	error (EXIT_FAILURE, 0, "%s: unknown service name", p);

      sockaddr.sinet.sin_port = port;
      socklen = sizeof (sockaddr.sinet);
    }

  fd = socket (family, SOCK_DGRAM, 0);
  if (fd < 0)
    error (EXIT_FAILURE, errno, "cannot create socket");

  if (family == PF_INET)
    {
      struct sockaddr_in s;
      s.sin_family = AF_INET;

      if (source)
	{
	  if (inet_aton (source, (struct in_addr *) &s.sin_addr) != 1)
	    error (EXIT_FAILURE, 0, "invalid source address");
	}
      else
	s.sin_addr.s_addr = INADDR_ANY;
      s.sin_port = 0;

      if (bind(fd, (struct sockaddr*) &s, sizeof(s)) < 0)
	error (EXIT_FAILURE, errno, "cannot bind to source address");
    }

  if (connect (fd, &sockaddr.sa, socklen))
    error (EXIT_FAILURE, errno, "cannot connect");
}


static void
send_to_syslog (const char *msg)
{
  char *pbuf;
  time_t now = time (NULL);
  size_t len;
  ssize_t rc;

  if (logflags & LOG_PID)
    rc = asprintf (&pbuf, "<%d>%.15s %s[%s]: %s",
		   pri, ctime (&now) + 4, tag, pidstr, msg);
  else
    rc = asprintf (&pbuf, "<%d>%.15s %s: %s",
		   pri, ctime (&now) + 4, tag, msg);
  if (rc == -1)
    error (EXIT_FAILURE, errno, "cannot format message");
  len = strlen (pbuf);

  if (logflags & LOG_PERROR)
    {
      struct iovec iov[2], *ioptr;
      size_t msglen = strlen (msg);

      ioptr = iov;
      ioptr->iov_base = (char*) msg;
      ioptr->iov_len = msglen;

      if (msg[msglen - 1] != '\n')
	{
	  /* provide a newline */
	  ioptr++;
	  ioptr->iov_base = (char *) "\n";
	  ioptr->iov_len = 1;
	}
      writev (fileno (stderr), iov, ioptr - iov + 1);
    }

  rc = send (fd, pbuf, len, 0);
  free (pbuf);
  if (rc == -1)
    error (0, errno, "send failed");
  else if (rc != len)
    error (0, errno, "sent less bytes than expected (%lu vs. %lu)",
	   (unsigned long) rc, (unsigned long) len);
}


const char args_doc[] = "[MESSAGE]";
const char doc[] = "Send messages to syslog";

static struct argp_option argp_options[] = {
  { "host", 'h', "HOST", 0,
    "log to host instead of the default " PATH_LOG },
  { "source", 'S', "IP", 0,
    "set source IP address" },
  { "id", 'i', "PID", OPTION_ARG_OPTIONAL,
    "log the process id with every line" },
#ifdef LOG_PERROR
  { "stderr", 's', NULL, 0, "copy the message to stderr" },
#endif
  { "file", 'f', "FILE", 0, "log the content of FILE" },
  { "priority", 'p', "PRI", 0, "log with priority PRI" },
  { "tag", 't', "TAG", 0, "prepend every line with TAG" },
  {NULL}
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'h':
      host = arg;
      break;

    case 'S':
      source = arg;
      break;

    case 'i':
      logflags |= LOG_PID;
      if (arg)
	pidstr = arg;
      else
	{
	  char buf[INT_BUFSIZE_BOUND (uintmax_t)];
	  arg = umaxtostr (getpid (), buf);
	  pidstr = xstrdup (arg);
	}
      break;

    case 's':
      logflags |= LOG_PERROR;
      break;

    case 'f':
      if (strcmp (arg, "-") && freopen (arg, "r", stdin) == NULL)
        error (EXIT_FAILURE, errno, "%s", arg);
      break;

    case 'p':
      pri = parse_level (arg);
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

const char *program_authors[] = {
  "Sergey Poznyakoff",
  NULL
};

int
main (int argc, char *argv[])
{
  int index;
  char *buf = NULL;

  set_program_name (argv[0]);
  iu_argp_init ("logger", program_authors);
  argp_parse (&argp, argc, argv, 0, &index, NULL);

  argc -= index;
  argv += index;

  if (!tag)
    {
      tag = getenv ("USER");
      if (!tag)
	{
	  struct passwd *pw = getpwuid (getuid ());
	  if (pw)
	    tag = xstrdup (pw->pw_name);
	  else
	    tag = xstrdup ("none");
	}
    }

  open_socket ();

  if (argc > 0)
    {
      int i;
      size_t len = 0;
      char *p;

      for (i = 0; i < argc; i++)
	len += strlen (argv[i]) + 1;

      buf = xmalloc (len);
      for (i = 0, p = buf; i < argc; i++)
	{
	  len = strlen (argv[i]);
	  memcpy (p, argv[i], len);
	  p += len;
	  *p++ = ' ';
	}
      p[-1] = 0;

      send_to_syslog (buf);
    }
  else
    {
      size_t size = 0;

      while (getline (&buf, &size, stdin) > 0)
	send_to_syslog (buf);
    }
  free (buf);
  exit (0);
}
