/* Interface to crypt that deals when it's missing

   Copyright (C) 1996 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifndef __CRYPT_H__
#define __CRYPT_H__

#ifdef HAVE_WEAK_REFS
extern char *crypt __P ((char *str, char salt[2])) __attribute__ ((weak));
#else
#ifdef HAVE_PRAGMA_WEAK_REFS
extern char *crypt __P ((char *str, char salt[2]));
#pragma weak crypt
#else
#ifdef HAVE_ASM_WEAK_REFS
extern char *crypt __P ((char *str, char salt[2]));
asm (".weak crypt");
#else
#ifdef HAVE_CRYPT
extern char *crypt __P ((char *str, char salt[2]));
#endif
#endif
#endif
#endif

/* Call crypt, or just return STR if there is none.  */
#if defined (HAVE_WEAK_REFS) || defined (HAVE_PRAGMA_WEAK_REFS) || defined (HAVE_ASM_WEAK_REFS)
#define CRYPT(str, salt) (crypt ? crypt (str, salt) : (str))
#else
#ifdef HAVE_CRYPT
#define CRYPT(str, salt) crypt (str, salt)
#else
#define CRYPT(str, salt) (str)
#endif
#endif

#endif /* __CRYPT_H__ */
