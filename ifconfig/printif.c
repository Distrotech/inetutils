/* printif.c -- print an interface configuration

   Copyright (C) 2001, 2002 Free Software Foundation, Inc.

   Written by Marcus Brinkmann.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#if STDC_HEADERS
# include <stdlib.h>
#else
# ifndef HAVE_STRCHR
#  define strchr index
#  define strrchr rindex
# endif
#endif

#include <ctype.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "ifconfig.h"

FILE *ostream;	/* Either stdout or stderror.  */
int column_stdout;	/* The column position of the cursor on stdout.  */
int column_stderr;	/* The column position of the cursor on stderr.  */
int *column = &column_stdout;	/* The column marker of ostream.  */
int had_output;		/* True if we had any output.  */

struct format_handle format_handles[] =
{
#ifdef SYSTEM_FORMAT_HANDLER
  SYSTEM_FORMAT_HANDLER
#endif
  {"", fh_nothing},
  {"newline", fh_newline},
  {"\\n", fh_newline},
  {"\\t", fh_tabulator},
  {"first?", fh_first},
  {"tab", fh_tab},
  {"join", fh_join},
  {"exists?", fh_exists_query},
  {"format", fh_format},
  {"error", fh_error},
  {"progname", fh_progname},
  {"exit", fh_exit},

  {"name", fh_name},
  {"index?", fh_index_query},
  {"index", fh_index},
  {"addr?", fh_addr_query},
  {"addr", fh_addr},
  {"netmask?", fh_netmask_query},
  {"netmask", fh_netmask},
  {"brdaddr?", fh_brdaddr_query},
  {"brdaddr", fh_brdaddr},
  {"dstaddr?", fh_dstaddr_query},
  {"dstaddr", fh_dstaddr},
  {"flags?", fh_flags_query},
  {"flags", fh_flags},
  {"mtu?", fh_mtu_query},
  {"mtu", fh_mtu},
  {"metric?", fh_metric_query},
  {"metric", fh_metric},
  {NULL, NULL}
};

/* Various helper functions to get the job done.  */

void
put_char (format_data_t form, char c)
{
  switch (c)
    {
    case '\n':
      *column = 0;
      break;
    case '\t':
      *column = ((*column / TAB_STOP) + 1) * TAB_STOP;
      break;
    default:
      (*column)++;
    }
  putc (c, ostream);
  had_output = 1;
}

/* This is a simple print function, which tries to keep track of the
   column.  Of course, terminal behaviour can defeat this.  We should
   provide a handler to switch on/off column counting.  */
void
put_string (format_data_t form, const char *s)
{
  while (*s != '\0')
    put_char (form, *(s++));
}

void
put_int (format_data_t form, int argc, char *argv[], int nr)
{
  char *fmt;
  if (argc > 0)
    {
      char *p = argv[0];

      if (*p != '%')
	fmt = "%i";
      else
	{
	  p++;

	  while (isdigit (*p))
	    p++;

	  if (*p == '#')
	    p++;

	  switch (*p)
	    {
	    default:
	    case 'i':
	    case 'd':
	    case 'D':
	      *p = 'i';
	      break;
	    case 'x':
	    case 'h':
	      *p = 'x';
	      break;
	    case 'X':
	    case 'H':
	      *p = 'X';
	      break;
	    case 'o':
	    case 'O':
	      *p = 'o';
	      break;
	    }
	  p++;
	  *p = '\0';
	  fmt = argv[0];
	}
    }
  else
    fmt = "%i";

  *column += printf (fmt, nr);
  had_output = 1;
}

void
select_arg (format_data_t form, int argc, char *argv[], int nr)
{
  if (nr < argc)
    {
      form->format = argv[nr];
      print_interfaceX (form, 0);
    }
}

void
put_addr (format_data_t form, int argc, char *argv[], struct sockaddr *sa)
{
  struct sockaddr_in *sin = (struct sockaddr_in *) sa;
  char *addr = inet_ntoa (sin->sin_addr);
  long byte[4];
  char *p = strchr (addr, '.');

  *p = '\0';
  byte[0] = strtol (addr, NULL, 0);
  addr = p + 1;
  p = strchr (addr, '.');
  *p = '\0';
  byte[1] = strtol (addr, NULL, 0);
  addr = p + 1;
  p = strchr (addr, '.');
  *p = '\0';
  byte[2] = strtol (addr, NULL, 0);
  byte[3] = strtol (p + 1, NULL, 0);

  addr = inet_ntoa (sin->sin_addr);

  if (argc > 0)
    {
      long i = strtol (argv[0], NULL, 0);
      if (i >= 0 && i <= 3)
	put_int (form, argc - 1, &argv[1], byte[i]);
    }
  else
    put_string (form, addr);
}

void
put_flags (format_data_t form, int argc, char *argv[], short flags)
{
  /* XXX */
  short int f = 1;
  const char *name;
  int first = 1;

  while (flags && f)
    {
      if (f & flags)
        {
          name = if_flagtoname (f, "" /* XXX: avoid */);
          if (name)
            {
              if (!first)
		{
		  if (argc > 0)
		    put_string (form, argv[0]);
		  else
		    put_char (form, ' ');
		}
              put_string (form, name);
              flags &= ~f;
              first = 0;
            }
        }
      f = f << 1;
    }
  if (flags)
    {
      if (!first)
	{
	  if (argc > 0)
	    put_string (form, argv[0]);
	  else
	    put_char (form, ' ');
	}
      put_int (form, argc - 1, &argv[1], flags);
    }
}

/* Format handler can mangle form->format, so update it after calling
   here.  */
void
format_handler (const char *name, format_data_t form, int argc, char *argv[])
{
  struct format_handle *fh;
  fh = format_handles;

  while (fh->name != NULL)
    {
      if (! strcmp (fh->name, name))
	break;
      fh++;
    }

  if (fh->handler)
    (fh->handler) (form, argc, argv);
  else
    {
      *column += printf ("(");
      put_string (form, name);
      *column += printf (" unknown)");
      had_output = 1;
    }
}

void
fh_nothing (format_data_t form, int argc, char *argv[])
{
}

void
fh_newline (format_data_t form, int argc, char *argv[])
{
  put_char (form, '\n');
}

void
fh_tabulator (format_data_t form, int argc, char *argv[])
{
  put_char (form, '\t');
}

void
fh_first (format_data_t form, int argc, char *argv[])
{
  select_arg (form, argc, argv, form->first ? 0 : 1);
}

/* A tab implementation, which fills with spaces up to requested column or next
   tabstop.  */
void
fh_tab (format_data_t form, int argc, char *argv[])
{
  long goal = 0;

  errno = 0;
  if (argc >= 1)
    goal = strtol (argv[0], NULL, 0);
  if (goal <= 0)
    goal = ((*column / TAB_STOP) + 1) * TAB_STOP;

  while (*column < goal)
    put_char (form, ' ');
}

void
fh_join (format_data_t form, int argc, char *argv[])
{
  int had_output_saved = had_output;
  int count = 0;

  if (argc < 2)
    return;

  /* Suppress delimiter before first argument.  */
  had_output = 0;

  while (++count < argc)
    {
      if (had_output)
	{
	  put_string (form, argv[0]);
	  had_output = 0;
	  had_output_saved = 1;
	}
      form->format = argv[count];
      print_interfaceX (form, 0);
    }
  had_output = had_output_saved;
}

void
fh_exists_query (format_data_t form, int argc, char *argv[])
{
  struct format_handle *fh;
  fh = format_handles;

  if (argc > 0)
    {
      while (fh->name != NULL)
	{
	  if (! strcmp (fh->name, argv[0]))
	    break;
	  fh++;
	}
      select_arg (form, argc, argv, (fh->name != NULL) ? 1 : 2);
    }
}

void
fh_format (format_data_t form, int argc, char *argv[])
{
  int i = 0;

  while (i < argc)
    {
      struct format *frm = formats;

      while (frm->name && strcmp (argv[i], frm->name))
	frm++;

      if (frm->name)
	{
	  /* XXX: Avoid infinite recursion by appending name to a list
	   during the next call (but removing it afterwards, and
	   checking in this function if the name is in the list
	   already.  */
	  form->format = frm->templ;
	  print_interfaceX (form, 0);
	}
      i++;
    }
}

void
fh_error (format_data_t form, int argc, char *argv[])
{
  int i = 0;
  FILE *s = ostream;
  int *c = column;

  ostream = stderr;
  column = &column_stderr;
  while (i < argc)
    select_arg (form, argc, argv, i++);
  ostream = s;
  column = c;
}

void
fh_progname (format_data_t form, int argc, char *argv[])
{
  put_string (form, __progname);
}

void
fh_exit (format_data_t form, int argc, char *argv[])
{
  int err = 0;

  if (argc > 0)
    err = strtoul (argv[0], NULL, 0);

  exit (err);
}

void
fh_name (format_data_t form, int argc, char *argv[])
{
  put_string (form, form->name);
}

void
fh_index_query (format_data_t form, int argc, char *argv[])
{
  select_arg (form, argc, argv,
	      (if_nametoindex (form->name) == 0) ? 1 : 0);
}

void
fh_index (format_data_t form, int argc, char *argv[])
{
  int indx = if_nametoindex (form->name);

  if (indx == 0)
    {
      fprintf (stderr, "%s: No index number found for interface `%s': %s\n",
	       __progname, form->name, strerror (errno));
      exit (EXIT_FAILURE);
    }
  *column += printf ("%i", indx);
  had_output = 1;
}

void
fh_addr_query (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFADDR
  if (ioctl (form->sfd, SIOCGIFADDR, form->ifr) >= 0)
    select_arg (form, argc, argv, 0);
  else
#endif
    select_arg (form, argc, argv, 1);
}

void
fh_addr (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFADDR
  if (ioctl (form->sfd, SIOCGIFADDR, form->ifr) < 0)
    {
      fprintf (stderr, "%s: SIOCGIFADDR failed for interface `%s': %s\n",
	       __progname, form->ifr->ifr_name, strerror (errno));
      exit (EXIT_FAILURE);
    }
  else
    put_addr (form, argc, argv, &form->ifr->ifr_addr);
#else
  *column += printf ("(not available)");
  had_output = 1;
#endif
}

void
fh_netmask_query (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFNETMASK
  if (ioctl (form->sfd, SIOCGIFNETMASK, form->ifr) >= 0)
    select_arg (form, argc, argv, 0);
  else
#endif
    select_arg (form, argc, argv, 1);
}

void
fh_netmask (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFNETMASK
  if (ioctl (form->sfd, SIOCGIFNETMASK, form->ifr) < 0)
    {
      fprintf (stderr, "%s: SIOCGIFNETMASK failed for interface `%s': %s\n",
	       __progname, form->ifr->ifr_name, strerror (errno));
      exit (EXIT_FAILURE);
    }
  else
    put_addr (form, argc, argv, &form->ifr->ifr_netmask);
#else
  *column += printf ("(not available)");
  had_output = 1;
#endif
}

void
fh_brdaddr_query (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFBRDADDR
#ifdef SIOCGIFFLAGS
  int f;

  if (0 == (f = if_nametoflag("BROADCAST"))
      || (ioctl (form->sfd, SIOCGIFFLAGS, form->ifr) < 0)
      || ((f & form->ifr->ifr_flags) == 0))
    {
      select_arg (form, argc, argv, 1);
      return;
    }
#endif
  if (ioctl (form->sfd, SIOCGIFBRDADDR, form->ifr) >= 0)
    select_arg (form, argc, argv, 0);
  else
#endif
    select_arg (form, argc, argv, 1);
}

void
fh_brdaddr (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFBRDADDR
  if (ioctl (form->sfd, SIOCGIFBRDADDR, form->ifr) < 0)
    {
      fprintf (stderr, "%s: SIOCGIFBRDADDR failed for interface `%s': %s\n",
	       __progname, form->ifr->ifr_name, strerror (errno));
      exit (EXIT_FAILURE);
    }
  else
    put_addr (form, argc, argv, &form->ifr->ifr_broadaddr);
#else
  *column += printf ("(not available)");
  had_output = 1;
#endif
}

void
fh_dstaddr_query (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFDSTADDR
#ifdef SIOCGIFFLAGS
  int f;

  if (0 == (f = if_nametoflag("POINTOPOINT"))
      || (ioctl (form->sfd, SIOCGIFFLAGS, form->ifr) < 0)
      || ((f & form->ifr->ifr_flags) == 0))
    {
      select_arg (form, argc, argv, 1);
      return;
    }
#endif
  if (ioctl (form->sfd, SIOCGIFDSTADDR, form->ifr) >= 0)
    select_arg (form, argc, argv, 0);
  else
#endif
    select_arg (form, argc, argv, 1);
}

void
fh_dstaddr (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFDSTADDR
  if (ioctl (form->sfd, SIOCGIFDSTADDR, form->ifr) < 0)
    {
      fprintf (stderr, "%s: SIOCGIFDSTADDR failed for interface `%s': %s\n",
	       __progname, form->ifr->ifr_name, strerror (errno));
      exit (EXIT_FAILURE);
    }
  else
    put_addr (form, argc, argv, &form->ifr->ifr_dstaddr);
#else
  *column += printf ("(not available)");
  had_output = 1;
#endif
}

void
fh_mtu_query (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFMTU
  if (ioctl (form->sfd, SIOCGIFMTU, form->ifr) >= 0)
    select_arg (form, argc, argv, 0);
  else
#endif
    select_arg (form, argc, argv, 1);
}

void
fh_mtu (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFMTU
  if (ioctl (form->sfd, SIOCGIFMTU, form->ifr) < 0)
    {
      fprintf (stderr, "%s: SIOCGIFMTU failed for interface `%s': %s\n",
	       __progname, form->ifr->ifr_name, strerror (errno));
      exit (EXIT_FAILURE);
    }
  else
    put_int (form, argc, argv, form->ifr->ifr_mtu);
#else
  *column += printf ("(not available)");
  had_output = 1;
#endif
}

void
fh_metric_query (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFMETRIC
  if (ioctl (form->sfd, SIOCGIFMETRIC, form->ifr) >= 0)
    select_arg (form, argc, argv, (form->ifr->ifr_metric > 0) ? 0 : 1);
  else
#endif
    select_arg (form, argc, argv, 1);
}

void
fh_metric (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFMETRIC
  if (ioctl (form->sfd, SIOCGIFMETRIC, form->ifr) < 0)
    {
      fprintf (stderr, "%s: SIOCGIFMETRIC failed for interface `%s': %s\n",
	       __progname, form->ifr->ifr_name, strerror (errno));
      exit (EXIT_FAILURE);
    }
  else
    put_int (form, argc, argv, form->ifr->ifr_metric);
#else
  *column += printf ("(not available)");
  had_output = 1;
#endif
}

void
fh_flags_query (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFFLAGS
  if (ioctl (form->sfd, SIOCGIFFLAGS, form->ifr) >= 0)
    select_arg (form, argc, argv, 0);
  else
#endif
    select_arg (form, argc, argv, 1);
}

void
fh_flags (format_data_t form, int argc, char *argv[])
{
#ifdef SIOCGIFFLAGS
  if (ioctl (form->sfd, SIOCGIFFLAGS, form->ifr) < 0)
    {
      fprintf (stderr, "%s: SIOCGIFFLAGS failed for interface `%s': %s\n",
	       __progname, form->ifr->ifr_name, strerror (errno));
      exit (EXIT_FAILURE);
    }
  else
    {
      if (argc >= 1)
	{
	  if (! strcmp (argv[0], "number"))
	    put_int (form, argc - 1, &argv[1], form->ifr->ifr_flags);
	  else if (! strcmp (argv[0], "string"))
	    put_flags (form, argc - 1, &argv[1], form->ifr->ifr_flags);
	}
      else
	put_flags (form, argc, argv, form->ifr->ifr_flags);
    }
#else
  *column += printf ("(not available)");
  had_output = 1;
#endif
}

void
print_interfaceX (format_data_t form, int quiet)
{
  const char *p = form->format;
  const char *q;

  form->depth++;

  while (! (*p == '\0' || (form->depth > 1 && *p == '}')))
    {
      /* Echo until end of string or '$'.  */
      while (! (*p == '$' || *p == '\0' || (form->depth > 1 && *p == '}')))
	{
	  quiet || (put_char (form, *p), 0);
	  p++;
	}

      if (*p != '$')
	break;

      /* Look at next character.  If it is a '$' or '}', print that
         and skip the '$'.  If it is something else than '{', print
         both.  Otherwise enter substitution mode.  */
      switch (*(++p))
	{
	default:
	  quiet || (put_char (form, '$'), 0);
	  /* Fallthrough. */

	case '$':
	case '}':
	  quiet || (put_char (form, *p), 0);
	  p++;
	  continue;
	  /* Not reached.  */

	case '{':
	  p++;
	  break;
	}

      /* P points to character following '{' now.  */
      q = strchr (p, '}');
      if (!q)
	{
	  /* Without a following '}', no substitution at all can occure,
	     so just dump the string that is missing.  */
	  p -= 2;
	  put_string (form, p);
	  p = strchr (p, '\0');
	  continue;
	}
      else
	{
	  char *id;
	  id = alloca (q - p + 1);
	  memcpy (id, p, q - p);
	  id[q - p] = '\0';
	  p = q + 1;

	  /* We have now in ID the content of the first field, and
	     in P the following string.  Now take the arguments. */
	  if (quiet)
	    {
	      /* Just consume all arguments.  */
	      form->format = p;

	      while (*(form->format) == '{')
		{
		  form->format++;
		  print_interfaceX (form, 1);
		  if (*(form->format) == '}')
		    form->format++;
		}

	      p = form->format;
	    }
	  else
	    {
	      int argc = 0;
	      char **argv;
	      argv = alloca (strlen (q) / 2);

	      while (*p == '{')
		{
		  p++;
		  form->format = p;
		  print_interfaceX (form, 1);
		  q = form->format;
		  argv[argc] = malloc (q - p + 1);
		  memcpy (argv[argc], p, q - p);
		  argv[argc][q - p] = '\0';
		  if (*q == '}')
		    q++;
		  p = q;
		  argc++;
		}

	      format_handler (id, form, argc, argv);

	      /* Clean up.  */
	      form->format = p;
	      while (--argc >= 0)
		free (argv[argc]);
	    }
	}
    }

  form->format = p;
  form->depth--;
}

void
print_interface (int sfd, const char *name, struct ifreq *ifr,
		 const char *format)
{
  struct format_data form;
  static int first_passed_already;

  if (! ostream)
    ostream = stdout;

  if (! first_passed_already)
    first_passed_already = form.first = 1;
  else
    form.first = 0;

  form.name = name;
  form.ifr = ifr;
  form.format = format;
  form.sfd = sfd;
  form.depth = 0;

  print_interfaceX (&form, 0);
}
