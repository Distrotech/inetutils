/* options.c -- process the command line options

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

#include <getopt.h>
#include <stdio.h>
#include <errno.h>

#if STDC_HEADERS
# include <stdlib.h>
#else
# ifndef HAVE_STRCHR
#  define strchr index
#  define strrchr rindex
# endif
#endif

#if HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#include <sys/socket.h>
#include <net/if.h>
#include "ifconfig.h"

/* Be verbose about actions.  */
int verbose;

/* Array of all interfaces on the command line.  */
struct ifconfig *ifs;

/* Size of IFS.  */
int nifs;

static struct ifconfig ifconfig_initializer =
{
  NULL, /* name */
  0, /* valid */
};

struct format formats[] =
{
  /* This is the default output format if no system_default_format is
     specified.  */
  {
    "default",
    "${format}{gnu}",
  },
  /* This is the standard GNU output.  */
  {
    "gnu",
    "${first?}{}{${\\n}}${format}{gnu-one-entry}"
  },
  {
    "gnu-one-entry",
    "${format}{check-existance}" \
    "${name} (${index}):${\\n}" \
    "${addr?}{  inet address ${tab}{16}${addr}${\\n}}" \
    "${netmask?}{  netmask ${tab}{16}${netmask}${\\n}}" \
    "${brdaddr?}{  broadcast ${tab}{16}${brdaddr}${\\n}}" \
    "${dstaddr?}{  peer address ${tab}{16}${dstaddr}${\\n}}" \
    "${flags?}{  flags ${tab}{16}${flags}${\\n}}" \
    "${mtu?}{  mtu ${tab}{16}${mtu}${\\n}}" \
    "${metric?}{  metric ${tab}{16}${metric}${\\n}}" \
    "${exists?}{hwtype?}{${hwtype?}{  link encap ${tab}{16}${hwtype}${\\n}}}" \
    "${exists?}{hwaddr?}{${hwaddr?}{  hardware addr ${tab}{16}${hwaddr}${\\n}}}" \
    "${exists?}{txqlen?}{${txqlen?}{  tx queue len ${tab}{16}${txqlen}${\\n}}}"
  },
  /* Resembles the output of ifconfig 1.39 (1999-03-19) in net-tools 1.52.  */
  {
    "net-tools",
    "${format}{check-existance}" \
    "${name}${exists?}{hwtype?}{${hwtype?}{${tab}{10}Link encap:${hwtype}}" \
    "${hwaddr?}{  HWaddr ${hwaddr}}}${\\n}" \
    "${addr?}{${tab}{10}inet addr:${addr}" \
    "${brdaddr?}{  Bcast:${brdaddr}}" \
    "${netmask?}{  Mask:${netmask}}" \
    "${newline}}" \
    "${tab}{10}${flags}" \
    "${mtu?}{  MTU:${mtu}}" \
    "${metric?}{  Metric:${metric}}" \
    "${newline}" \
    "${exists?}{txqlen?}{${txqlen?}{${tab}{10}txqueuelen:${txqlen}}${\\n}}" \
    "${newline}"
  },
  /* Resembles the output of ifconfig shipped with unix systems like
     Solaris 2.7 or HPUX 10.20.  */
  {
    "unix",
    "${format}{check-existance}" \
    "${name}: flags=${flags}{number}<${flags}{string}{,}>" \
    "${mtu?}{ mtu ${mtu}}${\\n}" \
    "${addr?}{${\\t}inet ${addr}" \
    " netmask ${netmask}{0}{%02x}${netmask}{1}{%02x}" \
    "${netmask}{2}{%02x}${netmask}{3}{%02x}" \
    "${brdaddr?}{ broadcast ${brdaddr}}${\\n}}" \
    "${exists?}{hwtype?}{${hwtype?}{${\\t}${hwtype}" \
    "}${exists?}{hwaddr?}{${hwaddr?}{ ${hwaddr}}}${\\n}}"
  },
  /* Resembles the output of ifconfig shipped with OSF 4.0g.  */
  {
    "osf",
    "${format}{check-existance}" \
    "${name}: flags=${flags}{number}{%x}<${flags}{string}{,}>${\\n}" \
    "${addr?}{${\\t}inet ${addr}" \
    " netmask ${netmask}{0}{%02x}${netmask}{1}{%02x}" \
    "${netmask}{2}{%02x}${netmask}{3}{%02x}" \
    "${brdaddr?}{ broadcast ${brdaddr}}" \
    "${mtu?}{ ipmtu ${mtu}}${\\n}}"
  },
  /* If interface does not exist, print error message and exit. */
  {
    "check-existance",
    "${index?}{}" \
    "{${error}{${progname}: error: interface `${name}' does not exist${\\n}}" \
    "${exit}{1}}"
  },
  { 0, 0 }
};

/* Default format.  */
const char *default_format;

/* The "+" is necessary to avoid parsing of system specific pseudo options
   like `-promisc'.  */
static const char *short_options = "+i:a:m:d:p:b:M:vV" ;

static struct option long_options[] = {
#ifdef SYSTEM_LONG_OPTIONS
SYSTEM_LONG_OPTIONS
#endif
  {"verbose",		no_argument,		0,	'v'},
  {"version",		no_argument,		0,	'V'},
  {"help",		no_argument, 		0,	'&'},
  {"interface",		required_argument,	0,	'i'},
  {"address",		required_argument,	0,	'a'},
  {"netmask",		required_argument,	0,	'm'},
  {"dstaddr",		required_argument,	0,	'd'},
  {"peer",		required_argument,	0,	'p'},
  {"brdaddr",		required_argument,	0,	'b'},
  {"broadcast",		required_argument,	0,	'B'},
  {"mtu",		required_argument,	0,	'M'},
  {"metric",		required_argument,	0,	'3'},
  {"format",		optional_argument,	0,	'4'},
  {0, 0, 0, 0}
};

void
usage (int err)
{
  if (err != EXIT_SUCCESS)
    {
      fprintf (stderr, "Usage: %s [OPTION]...%s\n", __progname,
	       system_help ? " [SYSTEM OPTION]..." : "");
      fprintf (stderr, "Try `%s --help' for more information.\n", __progname);
    }
  else
    {
      fprintf (stdout, "Usage: %s [OPTION]...%s\n", __progname,
	       system_help ? " [SYSTEM OPTION]..." : "");
      puts ("Configure network interfaces.\n\n\
Options are:\n\
  -i, --interface NAME  Configure network interface NAME\n\
  -a, --address ADDR    Set interface address to ADDR\n\
  -m, --netmask MASK    Set netmask to MASK\n\
  -d, --dstaddr DSTADDR,\n\
  -p, --peer DSTADDR    Set destination (peer) address to DSTADDR\n\
  -b, --broadcast BRDADDR,\n\
      --brdaddr BRDADDR Set broadcast address to BRDADDR\n\
  -M, --mtu N           Set mtu of interface to N\n\
      --metric N        Set metric of interface to N\n\
      --format=FORMAT   Select output format (or set back to default)");

      if (system_help_options)
	puts (system_help_options);
      puts("\
  -v, --verbose         Output information when configuring interface.\n\
      --help            Display this help and exit\n\
  -V, --version         Output version information and exit");

      if (system_help)
	{
	  puts ("\nSystem options are:");
	  puts (system_help);
	  puts ("\
The system options provide an alternative, backward compatible command line\n\
interface. It is discouraged and should not be used.");
	}
      fprintf (stdout, "\nSubmit bug reports to %s.\n", PACKAGE_BUGREPORT);
    }
  exit (err);
}

struct ifconfig *
parse_opt_new_ifs (char *name)
{
  struct ifconfig *ifp;

  ifs = realloc (ifs, ++nifs * sizeof (struct ifconfig));
  if (!ifs)
    {
      fprintf (stderr, "%s: can't get memory for interface "
	       "configuration: %s\n", __progname, strerror (errno));
      exit (EXIT_FAILURE);
    }
  ifp = &ifs[nifs - 1];
  *ifp = ifconfig_initializer;
  ifp->name = name;
  return ifp;
}

#define PARSE_OPT_SET_ADDR(field, fname, fvalid) \
void								\
parse_opt_set_##field (struct ifconfig *ifp, char *addr)	\
{								\
  if (!ifp)							\
    {								\
      fprintf (stderr, "%s: no interface specified for " #fname	\
	       " `%s'\n", __progname, addr);			\
      usage (EXIT_FAILURE);					\
    }								\
  if (ifp->valid & IF_VALID_##fvalid)				\
    {								\
      fprintf (stderr, "%s: only one " #fname			\
	       " allowed for interface `%s'\n",			\
	       __progname, ifp->name);				\
      usage (EXIT_FAILURE);					\
    }								\
  ifp->##field = addr;						\
  ifp->valid |= IF_VALID_##fvalid##;				\
}

PARSE_OPT_SET_ADDR(address, address, ADDR)
PARSE_OPT_SET_ADDR(netmask, netmask, NETMASK)
PARSE_OPT_SET_ADDR(dstaddr, destination/peer address, DSTADDR)
PARSE_OPT_SET_ADDR(brdaddr, broadcast address, BRDADDR)

#define PARSE_OPT_SET_INT(field, fname, fvalid) \
void								\
parse_opt_set_##field (struct ifconfig *ifp, char *arg)		\
{								\
  char *end;							\
  if (!ifp)							\
    {								\
      fprintf (stderr, "%s: no interface specified for " #fname	\
	       " `%s'\n", __progname, arg);			\
      usage (EXIT_FAILURE);					\
    }								\
  if (ifp->valid & IF_VALID_##fvalid)				\
    {								\
      fprintf (stderr, "%s: only one " #fname			\
	       " allowed for interface `%s'\n",			\
	       __progname, ifp->name);				\
      usage (EXIT_FAILURE);					\
    }								\
  ifp->##field =  strtol (arg, &end, 0);			\
  if (*arg == '\0' || *end != '\0')				\
    {								\
      fprintf (stderr, "%s: mtu value `%s' for interface `%s' "	\
              "is not a number.\n",				\
	      __progname, optarg, ifp->name);			\
      exit (EXIT_FAILURE);					\
    }								\
  ifp->valid |= IF_VALID_##fvalid##;				\
}

PARSE_OPT_SET_INT(mtu, mtu value, MTU)
PARSE_OPT_SET_INT(metric, metric value, METRIC)

void
parse_opt_set_af (struct ifconfig *ifp, char *af)
{
  if (!ifp)
    {
      fprintf (stderr, "%s: no interface specified for address"
	       " family `%s'\n", __progname, af);
      usage (EXIT_FAILURE);
    }

  if (! strcasecmp (af, "inet"))
    ifp->af = AF_INET;
  else
    {
      fprintf (stderr, "%s: unknown address family `%s' for interface `%s'"
	       " is not a number.\n", __progname, optarg, ifp->name);
      exit (EXIT_FAILURE);
    }
  ifp->valid |= IF_VALID_AF;
}

void
parse_opt_set_default_format (const char *new_format)
{
  struct format *frm = formats;
  const char *format = new_format;

  if (! format)
    format = system_default_format ?
      system_default_format : "default";

  while (frm->name)
    {
      if (! strcmp (format ? format : "default", frm->name))
	break;
      frm++;
    }

  /* If we didn't find the format, set the user specified one.  */
  if (frm->name)
    default_format = frm->templ;
  else if (format)
    default_format = format;
  else
    default_format = "default";
}

/* Must be reentrant!  */
void
parse_opt_finalize (struct ifconfig *ifp)
{
  if (ifp && ! ifp->valid)
    {
      ifp->valid = IF_VALID_FORMAT;
      ifp->format = default_format;
    }
}

void
parse_opt (int argc, char *argv[])
{
  int option;
  struct ifconfig *ifp = ifs;

#ifndef HAVE___PROGNAME
  __progname = argv[0];
#endif

  parse_opt_set_default_format (NULL);

  while ((option = getopt_long (argc, argv, short_options,
				long_options, 0)) != EOF)
    {
      /* XXX: Allow new ifs be created by system_parse_opt. Provide
         helper function for that (esp necessary for system specific
         parsing of remaining args.  */
      if (system_parse_opt (&ifp, option, optarg))
	continue;

      switch(option)
	{
	case 'i': /* Interface name.  */
	  parse_opt_finalize (ifp);
	  ifp = parse_opt_new_ifs (optarg);
	  break;

	case 'a': /* Interface address.  */
	  parse_opt_set_address (ifp, optarg);
	  break;

	case 'm': /* Interface netmask.  */
	  parse_opt_set_netmask (ifp, optarg);
	  break;

	case 'd': /* Interface dstaddr.  */
	case 'p':
	  parse_opt_set_dstaddr (ifp, optarg);
	  break;

	case 'b': /* Interface broadcast address.  */
	case 'B':
	  parse_opt_set_brdaddr (ifp, optarg);
	  break;

	case 'M': /* Interface MTU.  */
	  parse_opt_set_mtu (ifp, optarg);
	  break;

	case '3': /* Interface metric.  */
	  parse_opt_set_metric (ifp, optarg);
	  break;

	case '4': /* Output format.  */
	  parse_opt_set_default_format (optarg);
	  break;

	case 'v': /* Verbose.  */
	  verbose = 1;
	  break;

	case '&': /* Help.  */
	  usage (EXIT_SUCCESS);
          /* Not reached.  */

	case 'V': /* Version.  */
          printf ("ifconfig (%s) %s\n", PACKAGE_NAME, PACKAGE_VERSION);
          exit (EXIT_SUCCESS);

        case '?':
        default:
	  usage (EXIT_FAILURE);
          /* Not reached.  */
        }
    }
  parse_opt_finalize (ifp);

  if (optind < argc)
    {
      if (! system_parse_opt_rest (&ifp, argc - optind, &argv[optind]))
	usage (EXIT_FAILURE);
      parse_opt_finalize (ifp);
    }
  if (!ifs)
    {
      /* No interfaces specified.  Get a list of all interfaces.  */
      struct if_nameindex *ifnx, *ifnxp;

      ifnx = ifnxp = if_nameindex();
      while (ifnxp->if_index != 0 || ifnxp->if_name != NULL)
        {
	  struct ifconfig *ifp;

	  ifs = realloc (ifs, ++nifs * sizeof (struct ifconfig));
	  if (!ifs)
	    {
	      fprintf (stderr, "%s: can't get memory for interface "
		       "configuration: %s\n", __progname, strerror (errno));
	      exit (EXIT_FAILURE);
	    }
	  ifp = &ifs[nifs - 1];
	  *ifp = ifconfig_initializer;
	  ifp->name = ifnxp->if_name;
	  ifp->valid = IF_VALID_FORMAT;
	  ifp->format = default_format;
          ifnxp++;
        }
      /* XXX: We never do if_freenameindex (ifnx), because we are
         keeping the names for later instead using strdup
         (if->if_name) here.  */
    }
}
