/*   opie interfacing routines for the gnu inetutils
     Copyright (C) 1998 Free Software Foundation, Inc.

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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "opie_cfg.h"
#include <stdio.h>
#include "opie.h"

/* chanllenge is a null-terminated string with the challenge from the server.

   response is where the response to send to the server is returned.

   target is a string describing which host you're connecting to, and possibly
   what hosts you're hoping through and what protocol you're using. */



int gnu_opie_client (char *challenge, char *response)
{
  char secret[OPIE_SECRET_MAX+1];
  char *opieclient;
  char *opiesecure;
  int secure;
  int result;

  /* FIXME: read ~/.passphrase */

  opieclient = getenv ("OPIECLIENT");
  if (opieclient && !strcmp (opieclient, "t")) {
    printf ("\n%s\n%s\n%s\n", GNU_OPIE_MAGIC, target, challenge);
    result = opiereadpass (response, OPIE_RESPONSE_MAX, 0);
  } else {
    opiesecure = getenv ("OPIESECURE");
    if (opiesecure) {
      if (!strcmp (opiesecure, "t"))
	secure = 1;
      else if (!strcmp (opiesecure, "nil"))
	secure = 0;
      else
	secure = !opieinsecure();
    } else {
      secure = !opieinsecure();
    }
    if (secure) {
      fputs("Secret pass phrase: ", stderr);
      if (!opiereadpass(secret, OPIE_SECRET_MAX, 0)) {
	fputs("Error reading secret pass phrase!\n", stderr);
      }
      result = opiegenerator(buffer, secret, response);
    } else {
      fputs(challenge, stderr);
      result = opiereadpass(response, , OPIE_RESPONSE_MAX, 0);
    }
  }
  memset (secret, 0, strlen (secret));
  return result;
}
