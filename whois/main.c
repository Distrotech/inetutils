/*   whois/main.c
     Copyright (C) 1998, 2000, 2002 Free Software Foundation, Inc.

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

#include <config.h>
#include <fnmatch.h>
#include <getopt.h>
#include <sys/stat.h>
#include <stdio.h>

#if HAVE_FCNTL_H
#include <fcntl.h>
#endif

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


static int version = 0, help = 0;

extern char *whois_servers;

char *server = 0;

char *config_file = 0;

static struct option longopts[] = {
  {"version",		no_argument,		0,	'v'},
  {"help",		no_argument, 		0,	'H'},
  {"server",		required_argument,	0,	'h'},
  {"host",		required_argument,	0,	'h'},
  {"config-file",	required_argument,	0,	'c'},
  {0, 0, 0, 0}
};

#define MYISNEWLINE(c) (((c) == '\n') || ((c) == '\r') || ((c) == '\f'))
#define MYISSPACE(c) (((c) == ' ') || ((c) == '\t'))
#define MYISLWS(c) (MYISNEWLINE(c) || MYISSPACE(c))

int
config_buffer_netwhois (char *who, char *buf, size_t count)
{
  char *start, *end;
  while (count) {
    while (count && (MYISLWS(*buf) || (*buf == '#'))) {
      if (*buf == '#')
	while (count && (!MYISNEWLINE(*buf)))
	  buf++, count--;
      else
	buf++, count--;
    }
    start = buf;
    while ((!MYISLWS(*buf)) && (*buf != '#') && count)
      buf++, count--;
    if (count < 2)
      return 0;
    if ((*buf == '#') || MYISNEWLINE (*buf))
      continue;
    *buf = 0;
    buf++, count--;
#if defined(FNM_CASEFOLD)
    if (fnmatch (start, who, FNM_CASEFOLD)) {
#elif defined(FNM_IGNORECASE)
    if (fnmatch (start, who, FNM_IGNORECASE)) {
#else
    if (fnmatch (start, who, 0)) {
#endif
      while (count && !MYISNEWLINE (*buf))
	buf++, count--;
    } else {
      while (count && MYISSPACE (*buf))
	buf++, count--;
      if (count < 2)
	return 0;
      if ((*buf == '#') || (MYISNEWLINE (*buf)))
	continue;
      start = buf;
      while (count && (!(MYISLWS (*buf)) && (*buf != '#')))
	buf++, count--;
      end = buf;
      while (count && MYISSPACE (*buf))
	buf++, count--;
      if ((!count) || (MYISNEWLINE (*buf) || (*buf == '#'))) {
	*end = 0;
	netwhois (who, start);
	return 1;
      }
    }
  }

  return 0;
}


/* returns 1 if successful */
int
config_file_netwhois (char *who, char *filename)
{
  size_t count;
  struct stat stat_buf;
  char *buf;
  int fd;

  if (stat (filename, &stat_buf))
    return 0;

  buf = malloc (stat_buf.st_size);

  fd = open (filename, O_RDONLY);
  if (fd == -1)
    return 0;

  count = read (fd, buf, stat_buf.st_size);

  close (fd);

  if (count != stat_buf.st_size)
    return 0;

  return config_buffer_netwhois (who, buf, count);
}


int
main(int argc, char *argv[])
{
  char c, *at;
  char *home, *homecfg;

  do {
    c = getopt_long(argc, argv, "hvs:c:", longopts, 0);
    switch(c) {
    case 'v':
      version = 1;
      break;
    case 'H':
      help = 1;
      break;
    case 'c':
      config_file = optarg;
      break;
    case 'h':
      server = optarg;
      break;
    }
  } while (c != -1);

  if (help) {
    fprintf(stderr, "whois: look up in the whois database"
		    "-v, --version           Print version and exit.\n"
		    "-H, --help              Print help and exit.\n"
		    "-c, --config-file       Specify alternate config file.\n"
		    "-h, --host, --server    Specify whois server.\n");
  } else if (version) {
    fprintf(stderr, "%s %s\n", PACKAGE_NAME, PACKAGE_VERSION);
  } else if ((optind + 1) != argc) {
    fprintf (stderr, "There must be exactly one non-option argument.\n");
    exit(1);
  } else {
    at = strchr(argv[optind], '@');
    if (at) {
      *at = 0;
      at++;
      netwhois (argv[optind], at);
    } else if (server) {
      netwhois (argv[optind], server);
    } else if (config_file && config_file_netwhois (argv[optind],
						    config_file)) {
    } else {
      home = getenv ("HOME");
      if (!home)
	home = ".";
      homecfg = malloc (strlen (home) + 32);
      strcpy (homecfg, home);
      strcpy (homecfg + strlen (home), "/.whois-servers");
      if (config_file_netwhois (argv[optind], homecfg)) {
      } else if (config_file_netwhois (argv[optind],
				       SYSCONFDIR "/whois-servers")) {
      } else if (config_file_netwhois (argv[optind],
				       DATADIR "/inet/whois-servers")) {
      } else if (config_buffer_netwhois (argv[optind],
					 whois_servers,
					 strlen (whois_servers))) {
      } else {
	netwhois (argv[optind], "whois.internic.net");
      }
    }
  }
  exit(0);
}
