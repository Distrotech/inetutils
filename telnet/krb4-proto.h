/*
  Copyright (C) 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003,
  2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014
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

/* add_ticket.c */
int add_ticket (KTEXT, int, char *, int, char *, char *, char *, int, KTEXT);

/* cr_err_reply.c */
void cr_err_reply (KTEXT, char *, char *, char *, unsigned long, unsigned long, char *);

/* create_auth_reply.c */
KTEXT create_auth_reply (char *, char *, char *, long, int, unsigned long,
			 int, KTEXT);

/* create_ciph.c */
int create_ciph (KTEXT, C_Block, char *, char *, char *, unsigned long, int,
		 KTEXT, unsigned long, C_Block);

/* create_death_packet.c */
KTEXT krb_create_death_packet (char *);

/* create_ticket.c */
int krb_create_ticket (KTEXT, unsigned int, char *, char *, char *, long,
		       char *, int, long, char *, char *, C_Block);

/* debug_decl.c */

/* decomp_ticket.c */
int decomp_ticket (KTEXT, unsigned char *, char *, char *, char *,
		   unsigned long *, C_Block, int *, unsigned long *, char *,
		   char *, C_Block, Key_schedule);

/* dest_tkt.c */
int dest_tkt (void);

/* extract_ticket.c */
int extract_ticket (KTEXT, int, char *, int *, int *, char *, KTEXT);

/* fgetst.c */
int fgetst (FILE *, char *, int);

/* get_ad_tkt.c */
int get_ad_tkt (char *, char *, char *, int);

/* get_admhst.c */
int krb_get_admhst (char *, char *, int);

/* get_cred.c */
int krb_get_cred (char *, char *, char *, CREDENTIALS *);

/* get_in_tkt.c */
int krb_get_pw_in_tkt (char *, char *, char *, char *, char *, int, char *);
int placebo_read_password (des_cblock *, char *, int);
int placebo_read_pw_string (char *, int, char *, int);

/* get_krbhst.c */
int krb_get_krbhst (char *, char *, int);

/* get_krbrlm.c */
int krb_get_lrealm (char *, int);

/* get_phost.c */
char *krb_get_phost (char *);

/* get_pw_tkt.c */
int get_pw_tkt (char *, char *, char *, char *);

/* get_request.c */
int get_request (KTEXT, int, char **, char **);

/* get_svc_in_tkt.c */
int krb_get_svc_in_tkt (char *, char *, char *, char *, char *, int, char *);

/* get_tf_fullname.c */
int krb_get_tf_fullname (char *, char *, char *, char *);

/* get_tf_realm.c */
int krb_get_tf_realm (char *, char *);

/* getopt.c */
int getopt (int, char **, char *);

/* getrealm.c */
char *krb_realmofhost (char *);

/* getst.c */
int getst (int, char *, int);

/* in_tkt.c */
int in_tkt (char *, char *);

/* k_gethostname.c */
int k_gethostname (char *, int);

/* klog.c */
char *klog (int, char *, int, int, int, int, int, int, int, int, int, int);
int kset_logfile (char *);

/* kname_parse.c */
int kname_parse (char *, char *, char *, char *);
int k_isname (char *);
int k_isinst (char *);
int k_isrealm (char *);

/* kntoln.c */
int krb_kntoln (AUTH_DAT *, char *);

/* krb_err_txt.c */

/* krb_get_in_tkt.c */
int krb_get_in_tkt (char *, char *, char *, char *, char *, int,
		    int (*key_proc (), int (*decrypt_proc) (), char *));

/* kuserok.c */
int kuserok (AUTH_DAT *, char *);

/* log.c */
void log (char *, int, int, int, int, int, int, int, int, int, int);
int set_logfile (char *);
int new_log (long, char *);

/* mk_err.c */
long krb_mk_err (unsigned char *, long, char *);

/* mk_priv.c */
long krb_mk_priv (unsigned char *, unsigned char *, unsigned long, Key_schedule, C_Block,
		  struct sockaddr_in *, struct sockaddr_in *);

/* mk_req.c */
int krb_mk_req (KTEXT, char *, char *, char *, long);
int krb_set_lifetime (int);

/* mk_safe.c */
long krb_mk_safe (unsigned char *, unsigned char *, unsigned long, C_Block *, struct sockaddr_in *,
		  struct sockaddr_in *);

/* month_sname.c */
char *month_sname (int);

/* netread.c */
int krb_net_read (int, char *, int);

/* netwrite.c */
int krb_net_write (int, char *, int);

/* one.c */

/* pkt_cipher.c */
KTEXT pkt_cipher (KTEXT);

/* pkt_clen.c */
int pkt_clen (KTEXT);

/* rd_err.c */
int krb_rd_err (unsigned char *, unsigned long, long *, MSG_DAT *);

/* rd_priv.c */
long krb_rd_priv (unsigned char *, unsigned long, Key_schedule, C_Block,
		  struct sockaddr_in *, struct sockaddr_in *, MSG_DAT *);

/* rd_req.c */
int krb_set_key (char *, int);
int krb_rd_req (KTEXT, char *, char *, long, AUTH_DAT *, char *);

/* rd_safe.c */
long krb_rd_safe (unsigned char *, unsigned long, C_Block *, struct sockaddr_in *,
		  struct sockaddr_in *, MSG_DAT *);

/* read_service_key.c */
int read_service_key (char *, char *, char *, int, char *, char *);

/* recvauth.c */
int krb_recvauth (long, int, KTEXT, char *, char *, struct sockaddr_in *,
		  struct sockaddr_in *, AUTH_DAT *, char *, Key_schedule,
		  char *);

/* save_credentials.c */
int save_credentials (char *, char *, char *, C_Block, int, int, KTEXT, long);

/* send_to_kdc.c */
int send_to_kdc (KTEXT, KTEXT, char *);

/* sendauth.c */
int krb_sendauth (long, int, KTEXT, char *, char *, char *, unsigned long, MSG_DAT *,
		  CREDENTIALS *, Key_schedule, struct sockaddr_in *,
		  struct sockaddr_in *, char *);
int krb_sendsvc (int, char *);

/* setenv.c */
int setenv (char *, char *, int);
void unsetenv (char *);
char *getenv (char *);
char *_findenv (char *, int *);

/* stime.c */
char *stime (long *);

/* tf_shm.c */
int krb_shm_create (char *);
int krb_is_diskless (void);
int krb_shm_dest (char *);

/* tf_util.c */
int tf_init (char *, int);
int tf_get_pname (char *);
int tf_get_pinst (char *);
int tf_get_cred (CREDENTIALS *);
int tf_close (void);
int tf_save_cred (char *, char *, char *, C_Block, int, int, KTEXT, long);

/* tkt_string.c */
char *tkt_string (void);
void krb_set_tkt_string (char *);

/* util.c */
int ad_print (AUTH_DAT *);
int placebo_cblock_print (des_cblock);
