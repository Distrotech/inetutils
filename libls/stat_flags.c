/*	$OpenBSD: stat_flags.c,v 1.4 1999/09/08 07:21:29 millert Exp $	*/
/*	$NetBSD: stat_flags.c,v 1.5 1995/09/07 06:43:01 jtc Exp $	*/

/*-
 * Copyright (c) 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <stddef.h>
#include <string.h>

#define SAPPEND(s) {							\
	if (prefix != NULL)						\
		strcat(string, prefix);				\
	strcat(string, s);					\
	prefix = ",";							\
}

/*
 * flags_to_string --
 *	Convert stat flags to a comma-separated string.  If no flags
 *	are set, return the default string.
 */
char *
flags_to_string (flags, def)
     u_int flags;
     char *def;
{
  static char string[128];
  char *prefix;

  string[0] = '\0';
  prefix = NULL;
  return (prefix == NULL && def != NULL ? def : string);
}

#define TEST(a, b, f) {							\
	if (!memcmp(a, b, sizeof(b))) {					\
		if (clear) {						\
			if (clrp)					\
				*clrp |= (f);				\
		} else if (setp)					\
			*setp |= (f);					\
		break;							\
	}								\
}

/*
 * string_to_flags --
 *	Take string of arguments and return stat flags.  Return 0 on
 *	success, 1 on failure.  On failure, stringp is set to point
 *	to the offending token.
 */
int
string_to_flags (stringp, setp, clrp)
     char **stringp;
     u_int *setp, *clrp;
{
  if (setp)
    *setp = 0;
  if (clrp)
    *clrp = 0;
  return (0);
}
