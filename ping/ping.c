/* Copyright (C) 1998 Free Software Foundation, Inc.

This file is part of GNU Inetutils.

GNU Inetutils is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Emacs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
almong with GNU Inetutils; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA. */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include "getopt.h"

static void
show_usage (void)
{
  printf("\
Usage: ping [OPTION]... [ADDRESS]...\n\
\n\
  -h, --help      display this help and exit\n\
  -v, --version   output version information and exit\n\
\n\
report bugs to <bug-inetutils@gnu.org>.\n\
");
}

int
main (int argc, char **argv)
{
  {
    int opt;
    static char short_options[] = "vh";
    static struct option long_options[] = 
    {
      {"version", 0, NULL, 'v'},
      {"help", 0, NULL, 'h'},
      {NULL, 0, NULL, 0}
    };

    for (opt = getopt_long (argc, argv, short_options, long_options, (int *)0);
         opt != EOF;
         opt = getopt_long (argc, argv, short_options, long_options, (int *)0))
      {
        switch (opt)
          {
          case 'v':
            printf ("ping - %s %s\n", GNU_PACKAGE, VERSION);
            printf ("Copyright (C) 1998 Free Software Foundation, Inc.\n");
            printf ("%s comes with ABSOLUTELY NO WARRANTY.\n", GNU_PACKAGE);
            printf ("You may redistribute copies of %s\n", PACKAGE);
            printf ("under the terms of the GNU General Public License.\n");
            printf ("For more information about these matters, ");
            printf ("see the files named COPYING.\n");
            exit (0);
            break;
          case 'h':
            show_usage ();
            exit (0);
            break;
          }
      }
  } 
}
