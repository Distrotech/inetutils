/* options.c -- process the command line options
  Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009
  Free Software Foundation, Inc.

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

/* Written by Marcus Brinkmann.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

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

static struct ifconfig ifconfig_initializer = {
  NULL,				/* name */
  0,				/* valid */
};

struct format formats[] = {
  /* This is the default output format if no system_default_format is
     specified.  */
  {"default", "${format}{gnu}"},
  /* This is the standard GNU output.  */
  {"gnu", "${first?}{}{${\\n}}${format}{gnu-one-entry}"},
  {"gnu-one-entry",
   "${format}{check-existence}"
   "${name} (${index}):${\\n}"
   "${addr?}{  inet address ${tab}{16}${addr}${\\n}}"
   "${netmask?}{  netmask ${tab}{16}${netmask}${\\n}}"
   "${brdaddr?}{  broadcast ${tab}{16}${brdaddr}${\\n}}"
   "${dstaddr?}{  peer address ${tab}{16}${dstaddr}${\\n}}"
   "${flags?}{  flags ${tab}{16}${flags}${\\n}}"
   "${mtu?}{  mtu ${tab}{16}${mtu}${\\n}}"
   "${metric?}{  metric ${tab}{16}${metric}${\\n}}"
   "${exists?}{hwtype?}{${hwtype?}{  link encap ${tab}{16}${hwtype}${\\n}}}"
   "${exists?}{hwaddr?}{${hwaddr?}{  hardware addr ${tab}{16}${hwaddr}${\\n}}}"
   "${exists?}{txqlen?}{${txqlen?}{  tx queue len ${tab}{16}${txqlen}${\\n}}}"},
  /* Resembles the output of ifconfig 1.39 (1999-03-19) in net-tools 1.52.  */
  {"net-tools",
   "${format}{check-existence}"
   "${name}${exists?}{hwtype?}{${hwtype?}{${tab}{10}Link encap:${hwtype}}"
   "${hwaddr?}{  HWaddr ${hwaddr}}}${\\n}"
   "${addr?}{${tab}{10}inet addr:${addr}"
   "${brdaddr?}{  Bcast:${brdaddr}}"
   "${netmask?}{  Mask:${netmask}}"
   "${newline}}"
   "${tab}{10}${flags}"
   "${mtu?}{  MTU:${mtu}}"
   "${metric?}{  Metric:${metric}}"
   "${exists?}{ifstat?}{"
   "${ifstat?}{"
   "${newline}          RX packets:${rxpackets}"
   " errors:${rxerrors} dropped:${rxdropped} overruns:${rxfifoerr}"
   " frame:${rxframeerr}"
   "${newline}          TX packets:${txpackets}"
   " errors:${txerrors} dropped:${txdropped} overruns:${txfifoerr}"
   " carrier:${txcarrier}"
   "${newline}"
   "          collisions:${collisions}"
   "${exists?}{txqlen?}{${txqlen?}{ txqueuelen:${txqlen}}}"
   "${newline}"
   "          RX bytes:${rxbytes}  TX bytes:${txbytes}"
   "}}{"
   "${newline}"
   "${exists?}{txqlen?}{${txqlen?}{ ${tab}{10}txqueuelen:${txqlen}}${\\n}}}"
   "${newline}"
   "${exists?}{map?}{${map?}{${irq?}{"
   "          Interrupt:${irq}"
   "${baseaddr?}{ Base address:0x${baseaddr}{%x}}"
   "${memstart?}{ Memory:${memstart}{%lx}-${memend}{%lx}}"
   "${dma?}{ DMA chan:${dma}{%x}}"
   "${newline}"
   "}}}"
   "${newline}"
  },
  /* Resembles the output of ifconfig shipped with unix systems like
     Solaris 2.7 or HPUX 10.20.  */
  {"unix",
   "${format}{check-existence}"
   "${name}: flags=${flags}{number}<${flags}{string}{,}>"
   "${mtu?}{ mtu ${mtu}}${\\n}"
   "${addr?}{${\\t}inet ${addr}"
   " netmask ${netmask}{0}{%02x}${netmask}{1}{%02x}"
   "${netmask}{2}{%02x}${netmask}{3}{%02x}"
   "${brdaddr?}{ broadcast ${brdaddr}}${\\n}}"
   "${exists?}{hwtype?}{${hwtype?}{${\\t}${hwtype}"
   "}${exists?}{hwaddr?}{${hwaddr?}{ ${hwaddr}}}${\\n}}"},
  /* Resembles the output of ifconfig shipped with OSF 4.0g.  */
  {"osf",
   "${format}{check-existence}"
   "${name}: flags=${flags}{number}{%x}<${flags}{string}{,}>${\\n}"
   "${addr?}{${\\t}inet ${addr}"
   " netmask ${netmask}{0}{%02x}${netmask}{1}{%02x}"
   "${netmask}{2}{%02x}${netmask}{3}{%02x}"
   "${brdaddr?}{ broadcast ${brdaddr}}" "${mtu?}{ ipmtu ${mtu}}${\\n}}"},
  /* If interface does not exist, print error message and exit. */
  {"check-existence",
   "${index?}{}"
   "{${error}{${progname}: error: interface `${name}' does not exist${\\n}}"
   "${exit}{1}}"},
  {0, 0}
};

/* Default format.  */
const char *default_format;

enum {
  METRIC_OPTION = 256,
  FORMAT_OPTION
};

static struct argp_option argp_options[] = {
  { "verbose", 'v', NULL, 0,
    "output information when configuring interface" },
  { "interface", 'i', "NAME", 0,
    "configure network interface NAME" },
  { "address", 'a', "ADDR", 0,
    "set interface address to ADDR" },
  { "netmask", 'm', "MASK", 0,
    "set netmask to MASK" },
  { "dstaddr", 'd', "ADDR", 0,
    "set destination (peer) address to ADDR" },
  { "peer", 'p', NULL, OPTION_ALIAS },
  { "broadcast", 'B', "ADDR", 0,
    "set broadcast address to ADDR" },
  { "brdaddr", 'b', NULL, OPTION_ALIAS, }, /* FIXME: Do we really need it? */
  { "mtu", 'M', "N", 0,
    "et mtu of interface to N" },
  { "metric", METRIC_OPTION, "N", 0,
    "set metric of interface to N" },
  { "format", FORMAT_OPTION, "FORMAT", 0,
    "select output format (or set back to default)" },
  { NULL }
};

const char doc[] = "Configure network interfaces.";
const char *program_authors[] =
  {
    "Marcus Brinkmann",
    NULL
  };
    
struct ifconfig *
parse_opt_new_ifs (char *name)
{
  struct ifconfig *ifp;

  ifs = realloc (ifs, ++nifs * sizeof (struct ifconfig));
  if (!ifs)
    error (EXIT_FAILURE, errno, "can't get memory for interface configuration");
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
    error (EXIT_FAILURE, 0,                                     \
           "no interface specified for %s `%s'", #fname, addr); \
  if (ifp->valid & IF_VALID_##fvalid)				\
    error (EXIT_FAILURE, 0,                                     \
           "only one %s allowed for interface `%s'",		\
	   #fname, ifp->name);				        \
  ifp->field = addr;						\
  ifp->valid |= IF_VALID_##fvalid;				\
}

PARSE_OPT_SET_ADDR (address, address, ADDR)
PARSE_OPT_SET_ADDR (netmask, netmask, NETMASK)
PARSE_OPT_SET_ADDR (dstaddr, destination / peer address, DSTADDR)
PARSE_OPT_SET_ADDR (brdaddr, broadcast address, BRDADDR)
#define PARSE_OPT_SET_INT(field, fname, fvalid) \
void								\
parse_opt_set_##field (struct ifconfig *ifp, char *arg)		\
{								\
  char *end;							\
  if (!ifp)							\
    error (EXIT_FAILURE, 0,                                     \
           "no interface specified for %s `%s'\n",              \
           #fname, arg);                                        \
  if (ifp->valid & IF_VALID_##fvalid)				\
    error (EXIT_FAILURE, 0,                                     \
           "only one %s allowed for interface `%s'",		\
           #fname, ifp->name);	 			        \
  ifp->field =  strtol (arg, &end, 0);				\
  if (*arg == '\0' || *end != '\0')				\
    error (EXIT_FAILURE, 0,                                     \
           "mtu value `%s' for interface `%s' is not a number",	\
           arg, ifp->name);			                \
  ifp->valid |= IF_VALID_##fvalid;				\
}
PARSE_OPT_SET_INT (mtu, mtu value, MTU)
PARSE_OPT_SET_INT (metric, metric value, METRIC)
void parse_opt_set_af (struct ifconfig *ifp, char *af)
{
  if (!ifp)
    error (EXIT_FAILURE, 0,
           "no interface specified for address family `%s'", af);

  if (!strcasecmp (af, "inet"))
    ifp->af = AF_INET;
  else
    error (EXIT_FAILURE, 0,
           "unknown address family `%s' for interface `%s': is not a number", 
           af, ifp->name);
  ifp->valid |= IF_VALID_AF;
}

void
parse_opt_set_default_format (const char *new_format)
{
  struct format *frm = formats;
  const char *format = new_format;

  if (!format)
    format = system_default_format ? system_default_format : "default";

  while (frm->name)
    {
      if (!strcmp (format ? format : "default", frm->name))
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
  if (ifp && !ifp->valid)
    {
      ifp->valid = IF_VALID_FORMAT;
      ifp->format = default_format;
    }
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct ifconfig *ifp = *(struct ifconfig **)state->input;
  
  switch (key)
    {
    case ARGP_KEY_INIT:
      state->child_inputs[0] = state->input;
      break;
      
    case 'i':		/* Interface name.  */
      parse_opt_finalize (ifp);
      ifp = parse_opt_new_ifs (arg);
      *(struct ifconfig **) state->input = ifp;
      break;

    case 'a':		/* Interface address.  */
      parse_opt_set_address (ifp, arg);
      break;

    case 'm':		/* Interface netmask.  */
      parse_opt_set_netmask (ifp, arg);
      break;

    case 'd':		/* Interface dstaddr.  */
    case 'p':
      parse_opt_set_dstaddr (ifp, arg);
      break;

    case 'b':		/* Interface broadcast address.  */
    case 'B':
      parse_opt_set_brdaddr (ifp, arg);
      break;

    case 'M':		/* Interface MTU.  */
      parse_opt_set_mtu (ifp, arg);
      break;

    case METRIC_OPTION:		/* Interface metric.  */
      parse_opt_set_metric (ifp, arg);
      break;

    case FORMAT_OPTION:		/* Output format.  */
      parse_opt_set_default_format (arg);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }

  return 0;
}

static struct argp_child argp_children[2];
static struct argp argp =
  {
    argp_options,
    parse_opt,
    NULL,
    doc,
  };


void
parse_cmdline (int argc, char *argv[])
{
  int index;
  struct ifconfig *ifp = ifs;
  set_program_name (argv[0]);

  parse_opt_set_default_format (NULL);
  iu_argp_init ("ifconfig", program_authors);
  argp_children[0] = system_argp_child;
  argp.children = argp_children;
  argp.args_doc = system_help;
  argp_parse (&argp, argc, argv, ARGP_IN_ORDER, &index, &ifp);

  parse_opt_finalize (ifp);

  if (index < argc)
    {
      if (!system_parse_opt_rest (&ifp, argc - index, &argv[index]))
	error (EXIT_FAILURE, 0, "invalid arguments");
      parse_opt_finalize (ifp);
    }
  if (!ifs)
    {
      /* No interfaces specified.  Get a list of all interfaces.  */
      struct if_nameindex *ifnx, *ifnxp;

      ifnx = ifnxp = if_nameindex ();
      while (ifnxp->if_index != 0 || ifnxp->if_name != NULL)
	{
	  struct ifconfig *ifp;

	  ifs = realloc (ifs, ++nifs * sizeof (struct ifconfig));
	  if (!ifs)
	    error (EXIT_FAILURE, errno,
	           "can't get memory for interface configuration");
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
