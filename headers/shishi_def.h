/* Copyright (C) 1991, 1992, 1993, 2007 Free Software Foundation, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 3 of the
License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

#ifdef SHISHI
# include <sys/socket.h>
# include <netinet/in.h>

# define SERVICE "host"
# define BUFLEN 1040

struct shishi_iv
{
  char *iv;
  int ivlen;
  int keyusage;
  Shishi_crypto *ctx;
  int first;
};
typedef struct shishi_iv shishi_ivector;

struct auth_data
{
  struct sockaddr_in from;
  char *hostname;
  char *lusername;
  char *rusername;
  char *term;
  char *env[2];
  int kerberos_version;
  int verbose;
  Shishi *h;
  Shishi_ap *ap;
  int protocol;
  Shishi_key *enckey;
  shishi_ivector iv1, iv2;
  shishi_ivector *ivtab[2];
};

extern int shishi_auth (Shishi ** handle, int verbose, char **cname,
			const char *sname, int sock,
			char *cmd, int port, Shishi_key ** enckey,
			char *realm);

extern int get_auth (int infd, Shishi ** handle, Shishi_ap ** ap,
		     Shishi_key ** enckey, const char **err_msg,
		     int *protoversion, int *cksumtype, char **cksum,
		     int *cksumlen);

extern int readenc (Shishi * h, int sock, char *buf, int *len,
		    shishi_ivector * iv, Shishi_key * enckey, int proto);

extern int writeenc (Shishi * h, int sock, char *buf, int wlen, int *len,
		     shishi_ivector * iv, Shishi_key * enckey, int proto);

#endif
