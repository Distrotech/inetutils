/* GNU mailutils - a suite of utilities for electronic mail
   Copyright (C) 1999, 2000, 2001 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Library Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  <termios.h>

/* Alain: Parts originally from GNU Lib C.  */

static void
echo_off(int fd, struct termios *stored_settings)
{
  struct termios new_settings;
  tcgetattr (fd, stored_settings);
  new_settings = *stored_settings;
  new_settings.c_lflag &= (~ECHO);
  tcsetattr (fd, TCSANOW, &new_settings);
}

static void
echo_on(int fd, struct termios *stored_settings)
{
  tcsetattr (fd, TCSANOW, stored_settings);
}

char *
getpass (const char * prompt)
{
  FILE *in, *out;
  struct termios stored_settings;
  static char *buf;
  static size_t buf_size;
  char *pbuf;

  /* First pass initialize the buffer.  */
  if (buf_size == 0)
    {
      buf_size = 256;
      buf = calloc (1, buf_size);
      if (buf == NULL)
	return NULL;
    }
  else
    memset (buf, '\0', buf_size);

  /* Turn echoing off if it is on now.  */
  echo_off (fileno (stdin), &stored_settings);

  /* Write the prompt.  */
  fputs (prompt, stdout);
  fflush (stdout);

  /* Read the password.  */
  pbuf = fgets (buf, buf_size, stdin);
  if (pbuf)
    {
      size_t nread = strlen (pbuf);
      if (nread && pbuf[nread - 1] == '\n')
        {
          /* Remove the newline.  */
          pbuf[nread - 1] = '\0';
	  /* Write the newline that was not echoed.  */
	  putc ('\n', stdout);
        }
    }

  /* Restore the original setting.  */
  echo_on (fileno (stdin), &stored_settings);

  return pbuf;
}

#ifdef _GETPASS_STANDALONE_TEST

int
main ()
{
  char *p;
  p = getpass ("my prompt: ");
  if (p)
    printf ("Passwd: %s\n", p);
  return 0;
}
#endif
