/* solaris.h

   Copyright (C) 2001 Free Software Foundation, Inc.

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

#ifndef IFCONFIG_SYSTEM_SOLARIS_H
#define IFCONFIG_SYSTEM_SOLARIS_H

#include "../printif.h"
#include "../options.h"
#include <sys/sockio.h>


/* XXX: Gross. Have autoconf check and put in system.h or so.
   The correctness is documented in Solaris 2.7, if_tcp(7p).  */

#define ifr_mtu ifr_metric


/* Option support.  */

struct system_ifconfig
{
  int valid;
};


/* Output format support.  */

#define SYSTEM_FORMAT_HANDLER \
  {"solaris", fh_nothing},

#endif
