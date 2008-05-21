/* Copyright (C) 1998,2001, 2002, 2007 Free Software Foundation, Inc.

   This file is part of GNU Inetutils.

   GNU Inetutils is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GNU Inetutils is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GNU Inetutils; see the file COPYING.  If not, write
   to the Free Software Foundation, Inc., 51 Franklin Street,
   Fifth Floor, Boston, MA 02110-1301 USA. */

#include <argp.h>
#include <intalkd.h>
#include <signal.h>
#include <libinetutils.h>

#ifndef LOG_FACILITY
# define LOG_FACILITY LOG_DAEMON
#endif

void talkd_init (void);
void talkd_run (int fd);

/* Configurable parameters: */
int debug;
unsigned int timeout = 30;
time_t max_idle_time = 120;
time_t max_request_ttl = MAX_LIFE;

char *acl_file;
char *hostname;

ARGP_PROGRAM_DATA ("talkd", "2007", "Sergey Poznyakoff");

const char args_doc[] = "";
const char doc[] = "Talk daemon.";

static struct argp_option argp_options[] = {
#define GRP 0
  {"acl", 'a', "FILE", 0, "Read site-wide ACLs from FILE", GRP+1},
  {"debug", 'd', NULL, 0, "Enable debugging", GRP+1},
  {"idle-timeout", 'i', "SECONDS", 0, "Set idle timeout value to SECONDS",
   GRP+1},
  {"request-ttl", 'r', "SECONDS", 0, "Set request time-to-live value to "
   "SECONDS", GRP+1},
  {"timeout", 't', "SECONDS", 0, "Set timeout value to SECONDS", GRP+1},
#undef GRP
  {NULL}
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'a':
      acl_file = arg;
      break;

    case 'd':
      debug++;
      break;

    case 't':
      timeout = strtoul (arg, NULL, 0);
      break;

    case 'i':
      max_idle_time = strtoul (arg, NULL, 0);
      break;

    case 'r':
      max_request_ttl = strtoul (arg, NULL, 0);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }

  return 0;
}

static struct argp argp = {argp_options, parse_opt, args_doc, doc};

int
main (int argc, char *argv[])
{
  /* Parse command line */
  argp_parse (&argp, argc, argv, 0, NULL, NULL);

  read_acl (acl_file);
  talkd_init ();
  talkd_run (0);
  return 0;
}

void
talkd_init ()
{
  openlog ("talkd", LOG_PID, LOG_FACILITY);
  hostname = localhost ();
  if (!hostname)
    {
      syslog (LOG_ERR, "can't determine my hostname: %m");
      exit (1);
    }
}

time_t last_msg_time;

static void
alarm_handler (int err ARG_UNUSED)
{
  if ((time (NULL) - last_msg_time) >= max_idle_time)
    exit (0);
  alarm (timeout);
}

void
talkd_run (int fd)
{
  signal (SIGALRM, alarm_handler);
  alarm (timeout);
  while (1)
    {
      int rc;
      struct sockaddr_in sa_in;
      CTL_MSG msg;
      CTL_RESPONSE resp;
      int len;

      len = sizeof sa_in;
      rc =
	recvfrom (fd, &msg, sizeof msg, 0, (struct sockaddr *) &sa_in, &len);
      if (rc != sizeof msg)
	{
	  if (rc < 0 && errno != EINTR)
	    syslog (LOG_WARNING, "recvfrom: %m");
	  continue;
	}
      last_msg_time = time (NULL);
      if (process_request (&msg, &sa_in, &resp) == 0)
	{
	  rc = sendto (fd, &resp, sizeof resp, 0,
		       (struct sockaddr *) &msg.ctl_addr,
		       sizeof (msg.ctl_addr));
	  if (rc != sizeof resp)
	    syslog (LOG_WARNING, "sendto: %m");
	}
    }
}

