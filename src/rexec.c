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

/* Written by Giuseppe Scrivano.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/types.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#ifdef HAVE_SYS_FILIO_H
# include <sys/filio.h>
#endif
#include <sys/socket.h>

#include <netinet/in.h>

#include <errno.h>
#include <netdb.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/select.h>

#include <stdarg.h>
#include <progname.h>
#include <argp.h>
#include <error.h>
#include "libinetutils.h"

#define MAX3(a, b, c) (MAX (MAX (a, b), c))

const char doc[] = "remote execution client";
static char args_doc[] = "COMMAND";

const char *program_authors[] =
  {
    "Giuseppe Scrivano",
    NULL
  };

static struct argp_option options[] = {
  {"user",  'u', "user", 0, "Specify the user"},
  {"host",  'h', "host", 0, "Specify the host"},
  {"password",  'p', "password", 0, "Specify the password"},
  {"port",  'P', "port", 0, "Specify the port to connect to"},
  {"noerr", 'n', NULL, 0, "Disable the stderr stream"},
  {"error",  'e', "error", 0, "Specify a TCP port to use for stderr"},
  { 0 }
};

struct arguments
{
  const char *host;
  const char *user;
  const char *password;
  const char *command;
  int port;
  int use_err;
  int err_port;
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;

  switch (key)
    {
    case 'u':
      arguments->user = arg;
      break;
    case 'p':
      arguments->password = arg;
      break;
    case 'P':
      arguments->port = atoi (arg);
      break;
    case 'e':
      arguments->err_port = atoi (arg);
      break;
    case 'h':
      arguments->host = arg;
      break;
    case 'n':
      arguments->use_err = 0;
      break;
    case ARGP_KEY_ARG:
      arguments->command = arg;
      state->next = state->argc;
    }

  return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

static void do_rexec (struct arguments *arguments);

int
main (int argc, char **argv)
{
  struct arguments arguments;
  int failed = 0;
  set_program_name (argv[0]);

  iu_argp_init ("rexec", program_authors);

  arguments.user = NULL;
  arguments.password = NULL;
  arguments.host = NULL;
  arguments.command = NULL;
  arguments.err_port = 0;
  arguments.use_err = 1;
  arguments.port = 512;

  argp_parse (&argp, argc, argv, ARGP_IN_ORDER, 0, &arguments);

  if (arguments.user == NULL)
    {
      error (0, 0, "user not specified");
      failed++;
    }

  if (arguments.password == NULL)
    {
      error (0, 0, "password not specified");
      failed++;
    }

  if (arguments.host == NULL)
    {
      error (0, 0, "host not specified");
      failed++;
    }

  if (arguments.command == NULL)
    {
      error (0, 0, "command not specified");
      failed++;
    }

  if (failed > 0)
    exit (EXIT_FAILURE);

  do_rexec (&arguments);

  exit (0);
}

static void
safe_write (int socket, const char *str, size_t len)
{
  if (write (socket, str, len) < 0)
    error (EXIT_FAILURE, errno, "error sending data");
}

void
do_rexec (struct arguments *arguments)
{
  int err;
  char buffer[1024];
  int sock;
  char port_str[6];
  struct sockaddr_in addr;
  struct hostent *host;
  int stdin_fd = STDIN_FILENO;
  int err_sock = -1;

  sock = socket (AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    error (EXIT_FAILURE, errno, "cannot open socket");

  host = gethostbyname (arguments->host);
  if (host == NULL)
    error (EXIT_FAILURE, errno, "cannot find host %s", arguments->host);

  memset (&addr, 0, sizeof (addr));
  addr.sin_family = AF_INET;
  memmove ((caddr_t) &addr.sin_addr,
#ifdef HAVE_STRUCT_HOSTENT_H_ADDR_LIST
	       host->h_addr_list[0],
#else
	       host->h_addr,
#endif
	       host->h_length);

  addr.sin_port = htons ((short)arguments->port);

  if (connect (sock, &addr, sizeof (addr)) < 0)
    error (EXIT_FAILURE, errno, "cannot connect to the specified host");

  if (!arguments->use_err)
    {
      port_str[0] = '0';
      port_str[1] = '\0';
      safe_write (sock, port_str, 2);
      arguments->err_port = 0;
    }
  else
    {
      struct sockaddr_in serv_addr;
      socklen_t len;
      int serv_sock = socket (AF_INET, SOCK_STREAM, 0);

      if (serv_sock < 0)
        error (EXIT_FAILURE, errno, "cannot open socket");

      memset (&serv_addr, 0, sizeof (serv_addr));

      serv_addr.sin_port = arguments->err_port;
      if (bind (serv_sock, &serv_addr, sizeof (serv_addr)) < 0)
        error (EXIT_FAILURE, errno, "cannot bind socket");

      len = sizeof (serv_addr);
      if (getsockname (serv_sock, &serv_addr, &len))
        error (EXIT_FAILURE, errno, "error reading socket port");

      if (listen (serv_sock, 1))
        error (EXIT_FAILURE, errno, "error listening on socket");

      arguments->err_port = ntohs (serv_addr.sin_port);
      sprintf (port_str, "%i", arguments->err_port);
      safe_write (sock, port_str, strlen (port_str) + 1);

      err_sock = accept (serv_sock, &serv_addr, &len);
      if (err_sock < 0)
        error (EXIT_FAILURE, errno, "error accepting connection");

      shutdown (err_sock, SHUT_WR);

      close (serv_sock);
    }

  safe_write (sock, arguments->user, strlen (arguments->user) + 1);
  safe_write (sock, arguments->password, strlen (arguments->password) + 1);
  safe_write (sock, arguments->command, strlen (arguments->command) + 1);

  while (1)
    {
      int ret;
      fd_set rsocks;

      /* No other data to read.  */
      if (sock < 0 && err_sock < 0)
        break;

      FD_ZERO (&rsocks);
      if (0 <= sock)
        FD_SET (sock, &rsocks);
      if (0 <= stdin_fd)
	FD_SET (stdin_fd, &rsocks);
      if (0 <= err_sock)
        FD_SET (err_sock, &rsocks);

      ret = select (MAX3 (sock, stdin_fd, err_sock) + 1, &rsocks, NULL, NULL, NULL);
      if (ret == -1)
        error (EXIT_FAILURE, errno, "error select");

      if (0 <= stdin_fd && FD_ISSET (stdin_fd, &rsocks))
        {
	  err = read (stdin_fd, buffer, 1024);

          if (err < 0)
            error (EXIT_FAILURE, errno, "error reading stdin");

          if (!err)
            {
              shutdown (sock, SHUT_WR);
	      close (stdin_fd);
	      stdin_fd = -1;
              continue;
            }

          if (write (STDOUT_FILENO, buffer, err) < 0)
            error (EXIT_FAILURE, errno, "error writing");
        }

      if (0 <= sock && FD_ISSET (sock, &rsocks))
        {
          err = read (sock, buffer, 1024);

          if (err < 0)
            error (EXIT_FAILURE, errno, "error reading out stream");

          if (!err)
            {
              close (sock);
              sock = -1;
              continue;
            }

          if (write (STDOUT_FILENO, buffer, err) < 0)
            error (EXIT_FAILURE, errno, "error writing");
        }

     if (0 <= err_sock && FD_ISSET (err_sock, &rsocks))
        {
          err = read (err_sock, buffer, 1024);

          if (err < 0)
            error (EXIT_FAILURE, errno, "error reading err stream");

          if (!err)
            {
              close (err_sock);
              err_sock = -1;
              continue;
            }

          if (write (STDERR_FILENO, buffer, err) < 0)
            error (EXIT_FAILURE, errno, "error writing to stderr");
        }
    }
}
