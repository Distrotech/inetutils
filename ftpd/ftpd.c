/*
 * Copyright (c) 1985, 1988, 1990, 1992, 1993, 1994
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

#ifndef lint
static char copyright[] =
"@(#) Copyright (c) 1985, 1988, 1990, 1992, 1993, 1994\n\
	The Regents of the University of California.  All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)ftpd.c	8.5 (Berkeley) 4/28/95";
#endif /* not lint */

/*
 * FTP server.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#if !defined (__GNUC__) && defined (_AIX)
 #pragma alloca
#endif
#ifndef alloca /* Make alloca work the best possible way.  */
#  ifdef __GNUC__
#    define alloca __builtin_alloca
#  else /* not __GNUC__ */
#    if HAVE_ALLOCA_H
#      include <alloca.h>
#    else /* not __GNUC__ or HAVE_ALLOCA_H */
#      ifndef _AIX /* Already did AIX, up at the top.  */
          char *alloca ();
#      endif /* not _AIX */
#    endif /* not HAVE_ALLOCA_H */
#  endif /* not __GNUC__ */
#endif /* not alloca */

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#ifdef HAVE_SYS_WAIT_H
#  include <sys/wait.h>
#endif

#include <netinet/in.h>
#ifdef HAVE_NETINET_IN_SYSTM_H
#  include <netinet/in_systm.h>
#endif
#ifdef HAVE_NETINET_IP_H
#  include <netinet/ip.h>
#endif

#define	FTP_NAMES
#include <arpa/ftp.h>
#include <arpa/inet.h>
#include <arpa/telnet.h>

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <netdb.h>
#include <pwd.h>
#include <setjmp.h>
#include <signal.h>
#if defined(HAVE_STDARG_H) && defined(__STDC__) && __STDC__
#  include <stdarg.h>
#else
#  include <varargs.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#ifdef TIME_WITH_SYS_TIME
#  include <sys/time.h>
#  include <time.h>
#else
#  ifdef HAVE_SYS_TIME_H
#    include <sys/time.h>
#  else
#    include <time.h>
#  endif
#endif
#include <unistd.h>
#include <crypt.h>
#ifdef HAVE_MMAP
#include <sys/mman.h>
#endif
/* Include glob.h last, because it may define "const" which breaks
   system headers on some platforms. */
#include <glob.h>

#include "extern.h"
#include "version.h"

#ifdef HAVE_SHADOW_H
#include <shadow.h>
#endif

#ifdef HAVE_TCPD_H
#include <tcpd.h>
#endif

#ifdef HAVE_SECURITY_PAM_APPL_H
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#endif

#ifndef LINE_MAX
#  define LINE_MAX 2048
#endif

#ifndef LOG_FTP
#  define LOG_FTP LOG_DAEMON	/* Use generic facility.  */
#endif

#ifndef HAVE_FCLOSE_DECL
/* Some systems don't declare fclose in <stdio.h>, so do it ourselves.  */
extern int fclose __P ((FILE *));
#endif

extern	off_t restart_point;
extern	char cbuf[];

struct  sockaddr_in server_addr;
struct	sockaddr_in ctrl_addr;
struct	sockaddr_in data_source;
struct	sockaddr_in data_dest;
struct	sockaddr_in his_addr;
struct	sockaddr_in pasv_addr;

int	data;
jmp_buf	errcatch, urgcatch;
int	logged_in;
struct	passwd *pw;
int	debug = 0;
int	timeout = 900;    /* timeout after 15 minutes of inactivity */
int	maxtimeout = 7200;/* don't allow idle time to be set beyond 2 hours */
int	logging;
int	guest;
int	type;
int	form;
int	stru;			/* avoid C keyword */
int	mode;
int     anon_only = 0;		/* allow only anonymous login */
int	quiet = 0;		/* don't print version to client */
int	usedefault = 1;		/* for data transfers */
int     daemon_mode = 0;        /* start in daemon mode */
int	pdata = -1;		/* for passive mode */
sig_atomic_t transflag;
off_t	file_size;
off_t	byte_count;
#if !defined(CMASK) || CMASK == 0
#undef CMASK
#define CMASK 027
#endif
int	defumask = CMASK;		/* default umask value */
char	tmpline[7];
char	*hostname = 0;
char	*remotehost = 0;
char    *anonymous_login_name = "ftp";
static int login_attempts;      /* number of failed login attempts */

#ifdef HAVE_FTPD_H
int     allow_severity = LOG_INFO;
int     deny_severity = LOG_NOTICE;
#endif

#ifdef HAVE_LIBPAM
static int PAM_conv (int num_msg, const struct pam_message **msg,
			struct pam_response **resp, void *appdata_ptr);
static struct pam_conv PAM_conversation = { &PAM_conv, NULL };
#endif


#define NUM_SIMUL_OFF_TO_STRS 4

/* Returns a string with the decimal representation of the off_t OFF, taking
   into account that off_t might be longer than a long.  The return value is
   a pointer to a static buffer, but a return value will only be reused every
   NUM_SIMUL_OFF_TO_STRS calls, to allow multiple off_t's to be conveniently
   printed with a single printf statement.  */
static char *
off_to_str (off_t off)
{
  static char bufs[NUM_SIMUL_OFF_TO_STRS][80];
  static char (*next_buf)[80] = bufs;

  if (next_buf > (bufs+NUM_SIMUL_OFF_TO_STRS))
    next_buf = bufs;

  if (sizeof (off) > sizeof (long))
    sprintf (*next_buf, "%qd", off);
  else if (sizeof (off) == sizeof (long))
    sprintf (*next_buf, "%ld", off);
  else
    sprintf (*next_buf, "%d", off);

  return *next_buf++;
}

/*
 * Timeout intervals for retrying connections
 * to hosts that don't accept PORT cmds.  This
 * is a kludge, but given the problems with TCP...
 */
#define	SWAITMAX	90	/* wait at most 90 seconds */
#define	SWAITINT	5	/* interval between retries */

int	swaitmax = SWAITMAX;
int	swaitint = SWAITINT;

#ifdef SETPROCTITLE
char	**Argv = NULL;		/* pointer to argument vector */
char	*LastArgv = NULL;	/* end of argv */
char	proctitle[LINE_MAX];	/* initial part of title */
#endif /* SETPROCTITLE */

#define LOGCMD(cmd, file) \
	if (logging > 1) \
	    syslog(LOG_INFO,"%s %s%s", cmd, \
		*(file) == '/' ? "" : curdir(), file);
#define LOGCMD2(cmd, file1, file2) \
	 if (logging > 1) \
	    syslog(LOG_INFO,"%s %s%s %s%s", cmd, \
		*(file1) == '/' ? "" : curdir(), file1, \
		*(file2) == '/' ? "" : curdir(), file2);
#define LOGBYTES(cmd, file, cnt) \
	if (logging > 1) { \
		if (cnt == (off_t)-1) \
		    syslog(LOG_INFO,"%s %s%s", cmd, \
			*(file) == '/' ? "" : curdir(), file); \
		else \
		    syslog(LOG_INFO, "%s %s%s = %s bytes", \
			cmd, (*(file) == '/') ? "" : curdir(), file, \
			   off_to_str (cnt)); \
	}

static void	 ack __P((char *));
static void	 myoob __P((int));
static int	 checkuser __P((char *));
static FILE	*dataconn __P((char *, off_t, char *));
static void	 dolog __P((struct sockaddr_in *));
static char	*curdir __P((void));
static void	 end_login __P((void));
static FILE	*getdatasock __P((char *));
static char	*gunique __P((char *));
static void	 lostconn __P((int));
static int	 receive_data __P((FILE *, FILE *));
static void	 send_data __P((FILE *, FILE *, off_t));
static struct passwd *
		 sgetpwnam __P((char *));
static char	*sgetsave __P((char *));
static void      reapchild __P((int));
static void      authentication_setup __P((const char *));
static void      sigquit __P((int));
#ifdef HAVE_LIBWRAP
static int	 check_host(struct sockaddr *sa);
#endif

static char *
curdir()
{
        static char *path = 0;
	extern char *xgetcwd();
	if (path)
	        free (path);
	path = xgetcwd ();
	if (! path)
		return ("");
	if (path[1] != '\0') {	/* special case for root dir. */
	        char *new = realloc (path, strlen (path) + 2); /* '/' + '\0' */
		if (! new) {
		    free(path);
		    return "";
		}
		strcat(new, "/");
		path = new;
	}
	/* For guest account, skip / since it's chrooted */
	return (guest ? path+1 : path);
}

int
main(int argc, char *argv[], char **envp)
{
        extern char *localhost ();
	int addrlen, ch, on = 1, tos;
	char *cp, line[LINE_MAX];
	FILE *fd;
	FILE *pid_fp;

#ifdef HAVE_TZSET
	tzset(); /* in case no timezon database in ~ftp */
#endif

#ifdef SETPROCTITLE
	/*
	 *  Save start and extent of argv for setproctitle.
	 */
	Argv = argv;
	while (*envp)
		envp++;
	LastArgv = envp[-1] + strlen(envp[-1]);
#endif /* SETPROCTITLE */

	while ((ch = getopt(argc, argv, "Aa:Ddqlt:T:u:v")) != EOF) {
		switch (ch) {
		case 'A':
			anon_only = 1;
			break;
		case 'a':
			anonymous_login_name = optarg;
			break;
		case 'D':
		        daemon_mode = 1;
		        break;
		case 'd':
			debug = 1;
			break;
		case 'q':
			quiet = 1;
			break;
		case 'l':
			logging++;	/* > 1 == extra logging */
			break;

		case 't':
			timeout = atoi(optarg);
			if (maxtimeout < timeout)
				maxtimeout = timeout;
			break;

		case 'T':
			maxtimeout = atoi(optarg);
			if (timeout > maxtimeout)
				timeout = maxtimeout;
			break;

		case 'u':
		    {
			long val = 0;

			val = strtol(optarg, &optarg, 8);
			if (*optarg != '\0' || val < 0)
				fprintf (stderr,
					 "%s: bad value for -u", argv[0]);
			else
				defumask = val;
			break;
		    }

		case 'v':
			debug = 1;
			break;

		default:
			fprintf (stderr,
				 "%s: unknown flag -%c ignored", argv[0]);
			break;
		}
	}


	/*
	 * LOG_NDELAY sets up the logging connection immediately,
	 * necessary for anonymous ftp's that chroot and can't do it later.
	 */
	openlog("ftpd", LOG_PID | LOG_NDELAY, LOG_FTP);
	(void) freopen(PATH_DEVNULL, "w", stderr);

	if(daemon_mode) {
	    int ctl_sock, fd;
	    struct servent *sv;

	    /* become a daemon */
	    if(daemon(1,1) < 0) {
		syslog(LOG_ERR, "failed to become a daemon");
                        exit(1);
                }
	    (void) signal(SIGCHLD, reapchild);
	    /* get port for ftp/tcp */
	    sv = getservbyname("ftp", "tcp");
	    if (sv == NULL) {
		syslog(LOG_ERR, "getservbyname for ftp failed");
		exit(1);
	    }
	    /* open socket, bind and start listen */
	    ctl_sock = socket(AF_INET, SOCK_STREAM, 0);
	    if (ctl_sock < 0) {
		syslog(LOG_ERR, "control socket: %m");
		exit(1);
	    }
	    if (setsockopt(ctl_sock, SOL_SOCKET, SO_REUSEADDR,
			   (char *)&on, sizeof(on)) < 0)
	    syslog(LOG_ERR, "control setsockopt: %m");
	    memset(&server_addr, 0, sizeof(server_addr));

	    server_addr.sin_family = AF_INET;
	    server_addr.sin_port = sv->s_port;

	    if (bind(ctl_sock, (struct sockaddr *)&server_addr,
		     SA_LEN((struct sockaddr *)&server_addr))) {
			 syslog(LOG_ERR, "control bind: %m");
			 exit(1);
	     }
	    if (listen(ctl_sock, 32) < 0) {
		syslog(LOG_ERR, "control listen: %m");
		exit(1);
	    }
	    /* Stash pid in pidfile */
	    if ((pid_fp = fopen(PATH_FTPDPID, "w")) == NULL)
	        syslog(LOG_ERR, "can't open %s: %m", PATH_FTPDPID);
	    else {
	        fprintf(pid_fp, "%d\n", getpid());
		fchmod(fileno(pid_fp), S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
		fclose(pid_fp);
	    }
	    /*
	     * Loop forever accepting connection requests and forking off
	     * children to handle them.
	     */
	    while (1) {
		addrlen = sizeof(his_addr);
		fd = accept(ctl_sock, (struct sockaddr *)&his_addr,
			    &addrlen);
		if (fork() == 0) {
                                /* child */
                                (void) dup2(fd, 0);
                                (void) dup2(fd, 1);
                                close(ctl_sock);
                                break;
		}
		close(fd);
	    }
#ifdef HAVE_LIBWRAP
	    /* ..in the child. */
	    if (!check_host((struct sockaddr *)&his_addr))
	    exit(1);
#endif
        } else {
	    addrlen = sizeof(his_addr);
	    if (getpeername(0, (struct sockaddr *)&his_addr, &addrlen) < 0) {
		syslog(LOG_ERR, "getpeername (%s): %m",argv[0]);
		exit(1);
	    }
	}

	(void) signal(SIGHUP, sigquit);
        (void) signal(SIGINT, sigquit);
        (void) signal(SIGQUIT, sigquit);
        (void) signal(SIGTERM, sigquit);
	(void) signal(SIGPIPE, lostconn);
	(void) signal(SIGCHLD, SIG_IGN);
	if ((long)signal(SIGURG, myoob) < 0)
		syslog(LOG_ERR, "signal: %m");

	addrlen = sizeof(ctrl_addr);
	if (getsockname(0, (struct sockaddr *)&ctrl_addr, &addrlen) < 0) {
	    syslog(LOG_ERR, "getsockname (%s): %m",argv[0]);
	    exit(1);
	}

#if defined (IP_TOS) && defined (IPTOS_LOWDELAY) && defined (IPPROTO_IP)
	tos = IPTOS_LOWDELAY;
	if (setsockopt(0, IPPROTO_IP, IP_TOS, (char *)&tos, sizeof(int)) < 0)
	syslog(LOG_WARNING, "setsockopt (IP_TOS): %m");
#endif

	/* Try to handle urgent data inline */
#ifdef SO_OOBINLINE
	if (setsockopt(0, SOL_SOCKET, SO_OOBINLINE, (char *)&on, sizeof(on)) < 0)
		syslog(LOG_ERR, "setsockopt: %m");
#endif

#ifdef	F_SETOWN
	if (fcntl(fileno(stdin), F_SETOWN, getpid()) == -1)
		syslog(LOG_ERR, "fcntl F_SETOWN: %m");
#endif
	dolog(&his_addr);
	/*
	 * Set up default state
	 */
	data = -1;
	type = TYPE_A;
	form = FORM_N;
	stru = STRU_F;
	mode = MODE_S;
	tmpline[0] = '\0';

	/* If logins are disabled, print out the message. */
	if ((fd = fopen(PATH_NOLOGIN,"r")) != NULL) {
		while (fgets(line, sizeof(line), fd) != NULL) {
			if ((cp = strchr(line, '\n')) != NULL)
				*cp = '\0';
			lreply(530, "%s", line);
		}
		(void) fflush(stdout);
		(void) fclose(fd);
		reply(530, "System not available.");
		exit(0);
	}
	if ((fd = fopen(PATH_FTPWELCOME, "r")) != NULL) {
		while (fgets(line, sizeof(line), fd) != NULL) {
			if ((cp = strchr(line, '\n')) != NULL)
				*cp = '\0';
			lreply(220, "%s", line);
		}
		(void) fflush(stdout);
		(void) fclose(fd);
		/* reply(220,) must follow */
	}

	hostname = localhost ();
	if (! hostname)
		perror_reply (550, "Local resource failure: malloc");

        if (!quiet) {
        	reply(220, "%s FTP server (%s %s) ready.",
		      hostname, inetutils_package, inetutils_version);
	} else {
		reply(220, "%s FTP server ready.", hostname);
	}

	(void) setjmp(errcatch);
	for (;;)
		(void) yyparse();
	/* NOTREACHED */
}

static void
reapchild(int signo)
{
        int save_errno = errno;

        while (wait3(NULL, WNOHANG, NULL) > 0)
                ;
        errno = save_errno;
}

static void
sigquit(int signo)
{
    syslog(LOG_ERR, "got signal %s", strsignal(signo));
    dologout(-1);
}


static void
lostconn(int signo)
{

	if (debug)
		syslog(LOG_DEBUG, "lost connection");
	dologout(-1);
}

static char ttyline[20];

/*
 * Helper function for sgetpwnam().
 */
static char *
sgetsave(char *s)
{
	char *new = malloc((unsigned) strlen(s) + 1);

	if (new == NULL) {
		perror_reply(421, "Local resource failure: malloc");
		dologout(1);
		/* NOTREACHED */
	}
	(void) strcpy(new, s);
	return (new);
}

/*
 * Save the result of a getpwnam.  Used for USER command, since
 * the data returned must not be clobbered by any other command
 * (e.g., globbing).
 */
static struct passwd *
sgetpwnam(char *name)
{
	static struct passwd save;
	struct passwd *p;

	if ((p = getpwnam(name)) == NULL)
		return (p);
	if (save.pw_name) {
		free(save.pw_name);
		free(save.pw_passwd);
		free(save.pw_gecos);
		free(save.pw_dir);
		free(save.pw_shell);
	}
#if defined(HAVE_GETSPNAM) && defined(HAVE_SHADOW_H)
	{
		struct  spwd *spw;

		setspent();
		if ((spw = getspnam(p->pw_name)) != NULL) {
			time_t now;
			long today;
			now = time((time_t *) 0);
			today = now / (60 * 60 * 24);
			if ((spw->sp_expire > 0 && spw->sp_expire < today)
					|| (spw->sp_max > 0 && spw->sp_lstchg > 0
						&& (spw->sp_lstchg + spw->sp_max < today))) {
				reply(530, "Login expired.");
			}
			p->pw_passwd = spw->sp_pwdp;
		}
		endspent();
	}
#endif
	save = *p;
	save.pw_name = sgetsave(p->pw_name);
	save.pw_passwd = sgetsave(p->pw_passwd);
	save.pw_gecos = sgetsave(p->pw_gecos);
	save.pw_dir = sgetsave(p->pw_dir);
	save.pw_shell = sgetsave(p->pw_shell);


	return (&save);
}


static int askpasswd;		/* had user command, ask for passwd */
static char curname[10];	/* current USER name */

static void
complete_login (char* passwd)
{
	FILE *fd;

	login_attempts = 0;		/* this time successful */
	if (setegid((gid_t)pw->pw_gid) < 0) {
		reply(550, "Can't set gid.");
		return;
	}

#ifdef HAVE_INITGROUPS
	(void) initgroups(pw->pw_name, pw->pw_gid);
#endif

	/* open wtmp before chroot */
	(void)snprintf(ttyline, sizeof(ttyline), "ftp%d", getpid());
	logwtmp_keep_open (ttyline, pw->pw_name, remotehost);
	logged_in = 1;

	if (guest) {
		/*
		 * We MUST do a chdir() after the chroot. Otherwise
		 * the old current directory will be accessible as "."
		 * outside the new root!
		 */
		if (chroot(pw->pw_dir) < 0 || chdir("/") < 0) {
			reply(550, "Can't set guest privileges.");
			goto bad;
		}
	} else if (chdir(pw->pw_dir) < 0) {
		if (chdir("/") < 0) {
			reply(530, "User %s: can't change directory to %s.",
			    pw->pw_name, pw->pw_dir);
			goto bad;
		} else
			lreply(230, "No directory! Logging in with home=/");
	}
	if (seteuid((uid_t)pw->pw_uid) < 0) {
		reply(550, "Can't set uid.");
		goto bad;
	}
	/*
	 * Display a login message, if it exists.
	 * N.B. reply(230,) must follow the message.
	 */
	if ((fd = fopen(PATH_FTPLOGINMESG, "r")) != NULL) {
		char *cp, line[LINE_MAX];

		while (fgets(line, sizeof(line), fd) != NULL) {
			if ((cp = strchr(line, '\n')) != NULL)
				*cp = '\0';
			lreply(230, "%s", line);
		}
		(void) fflush(stdout);
		(void) fclose(fd);
	}
	if (guest) {
		reply(230, "Guest login ok, access restrictions apply.");
#ifdef SETPROCTITLE
		snprintf(proctitle, sizeof(proctitle),
		    "%s: anonymous/%.*s", remotehost,
		    sizeof(proctitle) - sizeof(remotehost) -
		    sizeof(": anonymous/"), passwd);
		setproctitle("%s",proctitle);
#endif /* SETPROCTITLE */
		if (logging)
			syslog(LOG_INFO, "ANONYMOUS FTP LOGIN FROM %s, %s",
			       remotehost, passwd);
	} else {
		reply(230, "User %s logged in.", pw->pw_name);
#ifdef SETPROCTITLE
		snprintf(proctitle, sizeof(proctitle),
		    "%s: %s", remotehost, pw->pw_name);
		setproctitle("%s",proctitle);
#endif /* SETPROCTITLE */
		if (logging)
			syslog(LOG_INFO, "FTP LOGIN FROM %s as %s",
			    remotehost, pw->pw_name);
	}
	(void) umask(defumask);
	return;
bad:
	/* Forget all about it... */
	end_login();
}

/*
 * USER command.
 * Sets global passwd pointer pw if named account exists and is acceptable;
 * sets askpasswd if a PASS command is expected.  If logged in previously,
 * need to reset state.  If name is "ftp" or "anonymous", the name is not in
 * PATH_FTPUSERS, and ftp account exists, set guest and pw, then just return.
 * If account doesn't exist, ask for passwd anyway.  Otherwise, check user
 * requesting login privileges.  Disallow anyone who does not have a standard
 * shell as returned by getusershell().  Disallow anyone mentioned in the file
 * PATH_FTPUSERS to allow people such as root and uucp to be avoided.
 */
void
user(char *name)
{
#ifndef HAVE_LIBPAM
	char *cp, *shell;
#endif
	if (logged_in) {
		if (guest) {
			reply(530, "Can't change user from guest login.");
			return;
		}
		end_login();
	}
	if(anon_only&&(strcmp(name,"ftp")==0 || strcmp(name,"anonymous")==0)) {
		reply(530, "Sorry, only anonymous ftp allowed.");
                return;
	}
	
#ifdef HAVE_LIBPAM
        authentication_setup(name);
	askpasswd = 1;
#else
	guest = 0;
	if (strcmp(name, "ftp") == 0 || strcmp(name, "anonymous") == 0) {
		if (checkuser("ftp") || checkuser("anonymous"))
			reply(530, "User %s access denied.", name);
		else if ((pw = sgetpwnam(anonymous_login_name)) != NULL) {
			guest = 1;
			askpasswd = 1;
			reply(331,
			    "Guest login ok, type your name as password.");
		} else
			reply(530, "User %s unknown.", name);
		if (!askpasswd && logging)
			syslog(LOG_NOTICE,
			    "ANONYMOUS FTP LOGIN REFUSED FROM %s", remotehost);
		return;
	}

	if (pw = sgetpwnam(name)) {
		if ((shell = pw->pw_shell) == NULL || *shell == 0)
			shell = PATH_BSHELL;
		while ((cp = getusershell()) != NULL)
			if (strcmp(cp, shell) == 0)
				break;
		endusershell();

		if (cp == NULL || checkuser(name)) {
			reply(530, "User %s access denied.", name);
			if (logging)
				syslog(LOG_NOTICE,
				    "FTP LOGIN REFUSED FROM %s, %s",
				    remotehost, name);
			pw = (struct passwd *) NULL;
			return;
		}
	}
	strncpy(curname, name, sizeof(curname)-1);

	if (!pw || *pw->pw_passwd) {
		reply(331, "Password required for %s.", name);
		askpasswd = 1;
	} else
	        complete_login (0);

#endif /* !HAVE_LIBPAM */

	/*
	 * Delay before reading passwd after first failed
	 * attempt to slow down passwd-guessing programs.
	 */
	if (login_attempts)
		sleep((unsigned) login_attempts);
}

/*
 * Check if a user is in the file PATH_FTPUSERS
 */
static int
checkuser(char *name)
{
	FILE *fd;
	int found = 0;
	char *p, line[BUFSIZ];

	if ((fd = fopen(PATH_FTPUSERS, "r")) != NULL) {
		while (fgets(line, sizeof(line), fd) != NULL)
			if ((p = strchr(line, '\n')) != NULL) {
				*p = '\0';
				if (line[0] == '#')
					continue;
				if (strcmp(line, name) == 0) {
					found = 1;
					break;
				}
			}
		(void) fclose(fd);
	}
	return (found);
}

/*
 * Terminate login as previous user, if any, resetting state;
 * used when USER command is given or login fails.
 */
static void
end_login()
{

	(void) seteuid((uid_t)0);
	if (logged_in)
		logwtmp_keep_open (ttyline, "", "");
	pw = NULL;
	logged_in = 0;
	guest = 0;
}

#ifdef HAVE_LIBPAM
/*
 * PAM authentication, now using the PAM's async feature.
 */
static pam_handle_t *pamh;
static char *PAM_username;
static char *PAM_password;
static char *PAM_message;
static int PAM_accepted;

static int PAM_conv (int num_msg, const struct pam_message **msg,
                     struct pam_response **resp, void *appdata_ptr)
{
        struct pam_response *repl = NULL;
        int retval, count = 0, replies = 0;
        int size = sizeof(struct pam_response);

#define GET_MEM \
        if (!(repl = realloc(repl, size))) \
                return PAM_CONV_ERR; \
        size += sizeof(struct pam_response)
#define COPY_STRING(s) (s) ? strdup(s) : NULL
        (void) appdata_ptr;
        retval = PAM_SUCCESS;
        for (count = 0; count < num_msg; count++) {
                int savemsg = 0;

                switch (msg[count]->msg_style) {
                case PAM_PROMPT_ECHO_ON:
                        GET_MEM;
                        repl[replies].resp_retcode = PAM_SUCCESS;
                        repl[replies].resp = COPY_STRING(PAM_username);
                        replies++;
                        break;
                case PAM_PROMPT_ECHO_OFF:
                        GET_MEM;
                        if (PAM_password == 0) {
                                savemsg = 1;
                                retval = PAM_CONV_AGAIN;
                        } else {
                                repl[replies].resp_retcode = PAM_SUCCESS;
                                repl[replies].resp = COPY_STRING(PAM_password);
                                replies++;
                        }
                        break;
                case PAM_TEXT_INFO:
                        savemsg = 1;
                        break;
                case PAM_ERROR_MSG:
                default:
                        /* Must be an error of some sort... */
                        reply(530, "%s", msg[count]->msg);
                        retval = PAM_CONV_ERR;
                }

                if (savemsg) {
                        char *sp;

                        if (PAM_message) {
                                /* XXX: make sure we split newlines correctly */
                                lreply(331, "%s", PAM_message);
                                free(PAM_message);
                        }
                        PAM_message = COPY_STRING(msg[count]->msg);

                        /* Remove trailing `: ' */
                        sp = PAM_message + strlen(PAM_message);
                        while (sp > PAM_message && strchr(" \t\n:", *--sp))
                                *sp = '\0';
                }

                /* In case of error, drop responses and return */
                if (retval) {
                        _pam_drop_reply(repl, replies);
                        return retval;
                }
        }
        if (repl)
                *resp = repl;
        return PAM_SUCCESS;
}

static int pam_doit(void)
{
        char *user;
        int error;

        error = pam_authenticate(pamh, 0);
        if (error == PAM_CONV_AGAIN || error == PAM_INCOMPLETE) {
                /* Avoid overly terse passwd messages */
                if (PAM_message && !strcasecmp(PAM_message, "password")) {
                        free(PAM_message);
                        PAM_message = 0;
                }
                if (PAM_message == 0) {
                        reply(331, "Password required for %s.", PAM_username);
                } else {
                        reply(331, "%s", PAM_message);
                        free(PAM_message);
                        PAM_message = 0;
                }
                return 1;
        }
        if (error == PAM_SUCCESS) {
                /* Alright, we got it */
                error = pam_acct_mgmt(pamh, 0);
                if (error == PAM_SUCCESS)
                        error = pam_setcred(pamh, PAM_ESTABLISH_CRED);
                if (error == PAM_SUCCESS)
                        error = pam_get_item(pamh, PAM_USER, (const void **) &user);
                if (error == PAM_SUCCESS) {
                        if (strcmp(user, "ftp") == 0) {
                                guest = 1;
                        }
                        pw = sgetpwnam(user);
                }
                if (error == PAM_SUCCESS && pw)
                        PAM_accepted = 1;
        }
        pam_end(pamh, error);
        pamh = 0;

        return (error == PAM_SUCCESS);
}

static void authentication_setup(const char *username)
{
        int error;

        if (pamh != 0) {
                pam_end(pamh, PAM_ABORT);
                pamh = 0;
        }

        if (PAM_username)
                free(PAM_username);
        PAM_username = strdup(username);
        PAM_password = 0;
        PAM_message  = 0;
        PAM_accepted = 0;

        error = pam_start("ftp", PAM_username, &PAM_conversation, &pamh);
        if (error == PAM_SUCCESS)
                error = pam_set_item(pamh, PAM_RHOST, remotehost);
        if (error != PAM_SUCCESS) {
                reply(550, "Authentication failure");
                pam_end(pamh, error);
                pamh = 0;
        }

        if (pamh && !pam_doit())
                reply(550, "Authentication failure");
}

static int authenticate(char *passwd)
{
        if (PAM_accepted)
                return 1;

        if (pamh == 0)
                return 0;

        PAM_password = passwd;
        pam_doit();
        PAM_password = 0;
        return PAM_accepted;
}
#else /* !HAVE_LIBPAM */
static int authenticate(char *passwd)
{
	if (!guest && (!pw || *pw->pw_passwd)) {
                char *xpasswd;
                if (pw) {
                        char *salt = pw->pw_passwd;
                        xpasswd = CRYPT (passwd, salt);
                }

                if (!pw || !xpasswd || strcmp (xpasswd, pw->pw_passwd) != 0) {
                        pw = NULL;
                        return 0;
                } else 
			return 1;
	
        return 0;
}
#endif /* !HAVE_LIBPAM */


void
pass(char *passwd)
{
  int rval;
#ifdef HAVE_LIBPAM
	 pam_handle_t *pamh;
#endif

	if (logged_in || askpasswd == 0) {
		reply(503, "Login with USER first.");
		return;
	}
	askpasswd = 0;
#ifndef HAVE_LIBPAM
	if (!guest ) {   /* "ftp" is only account allowed no password */
#endif
		/*
                 * Try to authenticate the user
                 */
                if (!authenticate(passwd)) {
                        reply(530, "Login incorrect.");
                        if (logging)
                                syslog(LOG_NOTICE,
                                    "FTP LOGIN FAILED FROM %s, %s",
                                    remotehost, curname);
                        pw = NULL;
                        if (login_attempts++ >= 5) {
                                syslog(LOG_NOTICE,
                                    "repeated login failures from %s",
                                    remotehost);
                                exit(0);
                        }
                        return;
                }
#ifndef HAVE_LIBPAM
	} 
#endif
#if defined(HAVE_SHADOW_H) && !defined(HAVE_LIBPAM)
        switch (isexpired(spw)) {
          case 0: /* success */
                break;
          case 1:
                syslog(LOG_NOTICE, "expired password from %s, %s",
                       remotehost, pw->pw_name);
                reply(530, "Please change your password and try again.");
                return;
          case 2:
                syslog(LOG_NOTICE, "inactive login from %s, %s",
                       remotehost, pw->pw_name);
                reply(530, "Login inactive -- contact administrator.");
                return;
          case 3:
                syslog(LOG_NOTICE, "expired login from %s, %s",
                       remotehost, pw->pw_name);
                reply(530, "Account expired -- contact administrator.");
                return;
        }
#endif

		
	complete_login (passwd);
}

void
retrieve(char *cmd, char *name)
{
	FILE *fin, *dout;
	struct stat st;
	int (*closefunc) __P((FILE *));
	size_t buffer_size=0;

	if (cmd == 0) {
		fin = fopen(name, "r"), closefunc = fclose;
		st.st_size = 0;
	} else {
		char line[BUFSIZ];

		snprintf (line, sizeof line, cmd, name);
		name = line;
		fin = ftpd_popen(line, "r"), closefunc = ftpd_pclose;
		st.st_size = -1;
		buffer_size = BUFSIZ;
	}
	if (fin == NULL) {
		if (errno != 0) {
			perror_reply(550, name);
			if (cmd == 0) {
				LOGCMD("get", name);
			}
		}
		return;
	}
	byte_count = -1;
	if (cmd == 0 && (fstat(fileno(fin), &st) < 0 || !S_ISREG(st.st_mode)
			 || !(buffer_size = ST_BLKSIZE(st)))) {
		reply(550, "%s: not a plain file.", name);
		goto done;
	}
	if (restart_point) {
		if (type == TYPE_A) {
			off_t i, n;
			int c;

			n = restart_point;
			i = 0;
			while (i++ < n) {
				if ((c=getc(fin)) == EOF) {
					perror_reply(550, name);
					goto done;
				}
				if (c == '\n')
					i++;
			}
		} else if (lseek(fileno(fin), restart_point, SEEK_SET) < 0) {
			perror_reply(550, name);
			goto done;
		}
	}
	dout = dataconn(name, st.st_size, "w");
	if (dout == NULL)
		goto done;
	send_data(fin, dout, buffer_size);
	(void) fclose(dout);
	data = -1;
	pdata = -1;
done:
	if (cmd == 0)
		LOGBYTES("get", name, byte_count);
	(*closefunc)(fin);
}

void
store(char *name, char *mode, int unique)
{
	FILE *fout, *din;
	struct stat st;
	int (*closefunc) __P((FILE *));

	if (unique && stat(name, &st) == 0 &&
	    (name = gunique(name)) == NULL) {
		LOGCMD(*mode == 'w' ? "put" : "append", name);
		return;
	}

	if (restart_point)
		mode = "r+";
	fout = fopen(name, mode);
	closefunc = fclose;
	if (fout == NULL) {
		perror_reply(553, name);
		LOGCMD(*mode == 'w' ? "put" : "append", name);
		return;
	}
	byte_count = -1;
	if (restart_point) {
		if (type == TYPE_A) {
			off_t i, n;
			int c;

			n = restart_point;
			i = 0;
			while (i++ < n) {
				if ((c=getc(fout)) == EOF) {
					perror_reply(550, name);
					goto done;
				}
				if (c == '\n')
					i++;
			}
			/*
			 * We must do this seek to "current" position
			 * because we are changing from reading to
			 * writing.
			 */
			if (fseek(fout, 0L, SEEK_CUR) < 0) {
				perror_reply(550, name);
				goto done;
			}
		} else if (lseek(fileno(fout), restart_point, SEEK_SET) < 0) {
			perror_reply(550, name);
			goto done;
		}
	}
	din = dataconn(name, (off_t)-1, "r");
	if (din == NULL)
		goto done;
	if (receive_data(din, fout) == 0) {
		if (unique)
			reply(226, "Transfer complete (unique file name:%s).",
			    name);
		else
			reply(226, "Transfer complete.");
	}
	(void) fclose(din);
	data = -1;
	pdata = -1;
done:
	LOGBYTES(*mode == 'w' ? "put" : "append", name, byte_count);
	(*closefunc)(fout);
}

static FILE *
getdatasock(char *mode)
{
	int on = 1, s, t, tries;

	if (data >= 0)
		return (fdopen(data, mode));
	(void) seteuid((uid_t)0);
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
		goto bad;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
	    (char *) &on, sizeof(on)) < 0)
		goto bad;
	/* anchor socket to avoid multi-homing problems */
	data_source.sin_family = AF_INET;
	data_source.sin_addr = ctrl_addr.sin_addr;
	for (tries = 1; ; tries++) {
		if (bind(s, (struct sockaddr *)&data_source,
		    sizeof(data_source)) >= 0)
			break;
		if (errno != EADDRINUSE || tries > 10)
			goto bad;
		sleep(tries);
	}
	(void) seteuid((uid_t)pw->pw_uid);
#if defined (IP_TOS) && defined (IPTOS_THROUGHPUT) && defined (IPPROTO_IP)
	on = IPTOS_THROUGHPUT;
	if (setsockopt(s, IPPROTO_IP, IP_TOS, (char *)&on, sizeof(int)) < 0)
		syslog(LOG_WARNING, "setsockopt (IP_TOS): %m");
#endif
	return (fdopen(s, mode));
bad:
	/* Return the real value of errno (close may change it) */
	t = errno;
	(void) seteuid((uid_t)pw->pw_uid);
	(void) close(s);
	errno = t;
	return (NULL);
}

static FILE *
dataconn(char *name, off_t size, char *mode)
{
	char sizebuf[32];
	FILE *file;
	int retry = 0, tos;

	file_size = size;
	byte_count = 0;
	if (size != (off_t) -1)
		(void) snprintf(sizebuf, sizeof(sizebuf), " (%s bytes)",
				off_to_str (size));
	else
	    	*sizebuf = '\0';
	if (pdata >= 0) {
		struct sockaddr_in from;
		int s, fromlen = sizeof(from);

		(void) signal(SIGALRM, toolong);
		(void) alarm((unsigned) timeout);
		s = accept(pdata, (struct sockaddr *)&from, &fromlen);
		(void) alarm (0);
		if (s < 0) {
			reply(425, "Can't open data connection.");
			(void) close(pdata);
			pdata = -1;
			return (NULL);
		}
		(void) close(pdata);
		pdata = s;
#if defined (IP_TOS) && defined (IPTOS_LOWDELAY) && defined (IPPROTO_IP)
		tos = IPTOS_LOWDELAY;
		(void) setsockopt(s, IPPROTO_IP, IP_TOS, (char *)&tos,
		    sizeof(int));
#endif
		reply(150, "Opening %s mode data connection for '%s'%s.",
		     type == TYPE_A ? "ASCII" : "BINARY", name, sizebuf);
		return (fdopen(pdata, mode));
	}
	if (data >= 0) {
		reply(125, "Using existing data connection for '%s'%s.",
		    name, sizebuf);
		usedefault = 1;
		return (fdopen(data, mode));
	}
	if (usedefault)
		data_dest = his_addr;
	usedefault = 1;
	file = getdatasock(mode);
	if (file == NULL) {
		reply(425, "Can't create data socket (%s,%d): %s.",
		    inet_ntoa(data_source.sin_addr),
		    ntohs(data_source.sin_port), strerror(errno));
		return (NULL);
	}
	data = fileno(file);
	while (connect(data, (struct sockaddr *)&data_dest,
	    sizeof(data_dest)) < 0) {
		if (errno == EADDRINUSE && retry < swaitmax) {
			sleep((unsigned) swaitint);
			retry += swaitint;
			continue;
		}
		perror_reply(425, "Can't build data connection");
		(void) fclose(file);
		data = -1;
		return (NULL);
	}
	reply(150, "Opening %s mode data connection for '%s'%s.",
	     type == TYPE_A ? "ASCII" : "BINARY", name, sizebuf);
	return (file);
}

/*
 * Tranfer the contents of "instr" to "outstr" peer using the appropriate
 * encapsulation of the data subject * to Mode, Structure, and Type.
 *
 * NB: Form isn't handled.
 */
static void
send_data(FILE *instr, FILE *outstr, off_t blksize)
{
	int c, cnt, filefd, netfd;
	char *buf, *bp;
	off_t curpos;
	size_t len, filesize;

	transflag++;
	if (setjmp(urgcatch)) {
		transflag = 0;
		return;
	}

	netfd = fileno(outstr);
	filefd = fileno(instr);
#ifdef HAVE_MMAP
	if (file_size > 0) {
	    curpos = lseek (filefd, 0, SEEK_CUR);
	    if (curpos >= 0) {
		filesize = file_size - curpos;
		buf = mmap (0, filesize, PROT_READ, MAP_SHARED, filefd, curpos);
	    }
	}
#endif

	switch (type) {

	case TYPE_A:
#ifdef HAVE_MMAP
	    	if (file_size > 0 && curpos >= 0 && buf != MAP_FAILED)
		{
		    len = 0;
		    while (len < filesize) {
			byte_count++;
			if (buf[len] == '\n') {
			    if (ferror(outstr))
				break;
			    (void) putc('\r', outstr);
			}
			(void) putc(buf[len], outstr);
			len++;
		    }
		    fflush(outstr);
		    transflag = 0;
		    munmap (buf, filesize);
		    if (ferror(outstr))
			goto data_err;
		    reply(226, "Transfer complete.");
		    return;
		}
#endif
		while ((c = getc(instr)) != EOF) {
		    byte_count++;
		    if (c == '\n') {
			if (ferror(outstr))
			    goto data_err;
			(void) putc('\r', outstr);
		    }
		    (void) putc(c, outstr);
		}
		fflush(outstr);
		transflag = 0;
		if (ferror(instr))
			goto file_err;
		if (ferror(outstr))
			goto data_err;
		reply(226, "Transfer complete.");
		return;

	case TYPE_I:
	case TYPE_L:
#ifdef HAVE_MMAP
	    	if (file_size > 0 && curpos >= 0 && buf != MAP_FAILED)
		{
		    bp = buf;
		    len = filesize;
		    do {
			cnt = write (netfd, bp, len);
			len -= cnt;
			bp += cnt;
			if (cnt > 0) byte_count += cnt;
		    } while (cnt > 0 && len > 0);
		    transflag = 0;
		    munmap (buf, (size_t)filesize);
		    if (cnt < 0)
			goto data_err;
		    reply (226, "Transfer complete.");
		    return;
		}
#endif
		if ((buf = malloc((u_int)blksize)) == NULL) {
			transflag = 0;
			perror_reply(451, "Local resource failure: malloc");
			return;
		}
		while ((cnt = read(filefd, buf, (u_int)blksize)) > 0 &&
		    write(netfd, buf, cnt) == cnt)
			byte_count += cnt;
		transflag = 0;
		(void)free(buf);
		if (cnt != 0) {
			if (cnt < 0)
				goto file_err;
			goto data_err;
		}
		reply(226, "Transfer complete.");
		return;
	default:
		transflag = 0;
		reply(550, "Unimplemented TYPE %d in send_data", type);
		return;
	}

data_err:
	transflag = 0;
	perror_reply(426, "Data connection");
	return;

file_err:
	transflag = 0;
	perror_reply(551, "Error on input file");
}

/*
 * Transfer data from peer to "outstr" using the appropriate encapulation of
 * the data subject to Mode, Structure, and Type.
 *
 * N.B.: Form isn't handled.
 */
static int
receive_data(FILE *instr, FILE *outstr)
{
	int c;
	int cnt, bare_lfs = 0;
	char buf[BUFSIZ];

	transflag++;
	if (setjmp(urgcatch)) {
		transflag = 0;
		return (-1);
	}
	switch (type) {

	case TYPE_I:
	case TYPE_L:
		while ((cnt = read(fileno(instr), buf, sizeof(buf))) > 0) {
			if (write(fileno(outstr), buf, cnt) != cnt)
				goto file_err;
			byte_count += cnt;
		}
		if (cnt < 0)
			goto data_err;
		transflag = 0;
		return (0);

	case TYPE_E:
		reply(553, "TYPE E not implemented.");
		transflag = 0;
		return (-1);

	case TYPE_A:
		while ((c = getc(instr)) != EOF) {
			byte_count++;
			if (c == '\n')
				bare_lfs++;
			while (c == '\r') {
				if (ferror(outstr))
					goto data_err;
				if ((c = getc(instr)) != '\n') {
					(void) putc ('\r', outstr);
					if (c == '\0' || c == EOF)
						goto contin2;
				}
			}
			(void) putc(c, outstr);
	contin2:	;
		}
		fflush(outstr);
		if (ferror(instr))
			goto data_err;
		if (ferror(outstr))
			goto file_err;
		transflag = 0;
		if (bare_lfs) {
			lreply(226,
		"WARNING! %d bare linefeeds received in ASCII mode",
			    bare_lfs);
		(void)printf("   File may not have transferred correctly.\r\n");
		}
		return (0);
	default:
		reply(550, "Unimplemented TYPE %d in receive_data", type);
		transflag = 0;
		return (-1);
	}

data_err:
	transflag = 0;
	perror_reply(426, "Data Connection");
	return (-1);

file_err:
	transflag = 0;
	perror_reply(452, "Error writing file");
	return (-1);
}

void
statfilecmd(char *filename)
{
	FILE *fin;
	int c;
	char line[LINE_MAX];

	(void)snprintf(line, sizeof(line), "/bin/ls -lgA %s", filename);
	fin = ftpd_popen(line, "r");
	lreply(211, "status of %s:", filename);
	while ((c = getc(fin)) != EOF) {
		if (c == '\n') {
			if (ferror(stdout)){
				perror_reply(421, "control connection");
				(void) ftpd_pclose(fin);
				dologout(1);
				/* NOTREACHED */
			}
			if (ferror(fin)) {
				perror_reply(551, filename);
				(void) ftpd_pclose(fin);
				return;
			}
			(void) putc('\r', stdout);
		}
		(void) putc(c, stdout);
	}
	(void) ftpd_pclose(fin);
	reply(211, "End of Status");
}

void
statcmd()
{
	struct sockaddr_in *sin;
	u_char *a, *p;

	lreply(211, "%s FTP server status:", hostname);
	if (!quiet)
		printf("     ftpd (%s) %s\r\n",
			inetutils_package, inetutils_version);
	printf("     Connected to %s", remotehost);
	if (!isdigit(remotehost[0]))
		printf(" (%s)", inet_ntoa(his_addr.sin_addr));
	printf("\r\n");
	if (logged_in) {
		if (guest)
			printf("     Logged in anonymously\r\n");
		else
			printf("     Logged in as %s\r\n", pw->pw_name);
	} else if (askpasswd)
		printf("     Waiting for password\r\n");
	else
		printf("     Waiting for user name\r\n");
	printf("     TYPE: %s", typenames[type]);
	if (type == TYPE_A || type == TYPE_E)
		printf(", FORM: %s", formnames[form]);
	if (type == TYPE_L)
#ifdef CHAR_BIT
		printf(" %d", CHAR_BIT);
#else
#if NBBY == 8
		printf(" %d", NBBY);
#else
		printf(" %d", bytesize);	/* need definition! */
#endif
#endif
	printf("; STRUcture: %s; transfer MODE: %s\r\n",
	    strunames[stru], modenames[mode]);
	if (data != -1)
		printf("     Data connection open\r\n");
	else if (pdata != -1) {
		printf("     in Passive mode");
		sin = &pasv_addr;
		goto printaddr;
	} else if (usedefault == 0) {
		printf("     PORT");
		sin = &data_dest;
printaddr:
		a = (u_char *) &sin->sin_addr;
		p = (u_char *) &sin->sin_port;
#define UC(b) (((int) b) & 0xff)
		printf(" (%d,%d,%d,%d,%d,%d)\r\n", UC(a[0]),
			UC(a[1]), UC(a[2]), UC(a[3]), UC(p[0]), UC(p[1]));
#undef UC
	} else
		printf("     No data connection\r\n");
	reply(211, "End of status");
}

void
fatal(char *s)
{

	reply(451, "Error in server: %s\n", s);
	reply(221, "Closing connection due to server error.");
	dologout(0);
	/* NOTREACHED */
}

void
#if defined(HAVE_STDARG_H) && defined(__STDC__) && __STDC__
reply(int n, const char *fmt, ...)
#else
reply(n, fmt, va_alist)
	int n;
	char *fmt;
        va_dcl
#endif
{
	va_list ap;
#if defined(HAVE_STDARG_H) && defined(__STDC__) && __STDC__
	va_start(ap, fmt);
#else
	va_start(ap);
#endif
	(void)printf("%d ", n);
	(void)vprintf(fmt, ap);
	(void)printf("\r\n");
	(void)fflush(stdout);
	if (debug) {
		syslog(LOG_DEBUG, "<--- %d ", n);
		vsyslog(LOG_DEBUG, fmt, ap);
	}
}

void
#if defined(HAVE_STDARG_H) && defined(__STDC__) && __STDC__
lreply(int n, const char *fmt, ...)
#else
lreply(n, fmt, va_alist)
	int n;
	char *fmt;
        va_dcl
#endif
{
	va_list ap;
#if defined(HAVE_STDARG_H) && defined(__STDC__) && __STDC__
	va_start(ap, fmt);
#else
	va_start(ap);
#endif
	(void)printf("%d- ", n);
	(void)vprintf(fmt, ap);
	(void)printf("\r\n");
	(void)fflush(stdout);
	if (debug) {
		syslog(LOG_DEBUG, "<--- %d- ", n);
		vsyslog(LOG_DEBUG, fmt, ap);
	}
}

static void
ack(char *s)
{

	reply(250, "%s command successful.", s);
}

void
nack(char *s)
{

	reply(502, "%s command not implemented.", s);
}

/* ARGSUSED */
void
yyerror(char *s)
{
	char *cp;

	if (cp = strchr(cbuf,'\n'))
		*cp = '\0';
	reply(500, "'%s': command not understood.", cbuf);
}

void
delete(char *name)
{
	struct stat st;

	LOGCMD("delete", name);
	if (stat(name, &st) < 0) {
		perror_reply(550, name);
		return;
	}
	if ((st.st_mode&S_IFMT) == S_IFDIR) {
		if (rmdir(name) < 0) {
			perror_reply(550, name);
			return;
		}
		goto done;
	}
	if (unlink(name) < 0) {
		perror_reply(550, name);
		return;
	}
done:
	ack("DELE");
}

void
cwd(char *path)
{

	if (chdir(path) < 0)
		perror_reply(550, path);
	else
		ack("CWD");
}

void
makedir(char *name)
{
	extern char *xgetcwd();
	LOGCMD("mkdir", name);
	if (mkdir(name, 0777) < 0)
		perror_reply(550, name);
	else if (name[0] == '/')
		reply (257, "\"%s\" new directory created.");
	else {
		/* We have to figure out what our current directory is so that we can
		   give an absolute name in the reply.  */
		char *cwd = xgetcwd ();
		if (cwd) {
			if (cwd[1] == '\0')
				cwd[0] = '\0';
			reply (257, "\"%s/%s\" new directory created.", cwd, name);
			free (cwd);
		} else
			reply (257, "(unknown absolute name) new directory created.");
	}
}

void
removedir(char *name)
{

	LOGCMD("rmdir", name);
	if (rmdir(name) < 0)
		perror_reply(550, name);
	else
		ack("RMD");
}

void
pwd()
{
  extern char *xgetcwd();
	char *path = xgetcwd ();
	if (path) {
		reply(257, "\"%s\" is current directory.", path);
		free (path);
	} else
		reply(550, "%s.", strerror (errno));
}

char *
renamefrom(char *name)
{
	struct stat st;

	if (stat(name, &st) < 0) {
		perror_reply(550, name);
		return ((char *)0);
	}
	reply(350, "File exists, ready for destination name");
	return (name);
}

void
renamecmd(char *from, char *to)
{

	LOGCMD2("rename", from, to);
	if (rename(from, to) < 0)
		perror_reply(550, "rename");
	else
		ack("RNTO");
}

static void
dolog(struct sockaddr_in *sin)
{
	const char *name;
	struct hostent *hp = gethostbyaddr((char *)&sin->sin_addr,
		sizeof(struct in_addr), AF_INET);

	if (hp)
		name = hp->h_name;
	else
		name = inet_ntoa(sin->sin_addr);

	if (remotehost)
		free (remotehost);
	remotehost = malloc (strlen (name));
	strcpy (remotehost, name);

#ifdef SETPROCTITLE
	snprintf (proctitle, sizeof(proctitle), "%s: connected", remotehost);
	setproctitle("%s",proctitle);
#endif /* SETPROCTITLE */

	if (logging)
		syslog(LOG_INFO, "connection from %s", remotehost);
}

/*
 * Record logout in wtmp file
 * and exit with supplied status.
 */
void
dologout(int status)
{
  /* racing condition with SIGURG: If SIGURG is receive
     here, it will jump back has root in the main loop
     David Greenman:dg@root.com
  */
  transflag = 0;

	if (logged_in) {
		(void) seteuid((uid_t)0);
		logwtmp_keep_open (ttyline, "", "");
	}
	/* beware of flushing buffers after a SIGPIPE */
	_exit(status);
}

static void
myoob(int signo)
{
	char *cp;

	/* only process if transfer occurring */
	if (!transflag)
		return;
	cp = tmpline;
	if (telnet_fgets(cp, 7, stdin) == NULL) {
		reply(221, "You could at least say goodbye.");
		dologout(0);
	}
	upper(cp);
	if (strcmp(cp, "ABOR\r\n") == 0) {
		tmpline[0] = '\0';
		reply(426, "Transfer aborted. Data connection closed.");
		reply(226, "Abort successful");
		longjmp(urgcatch, 1);
	}
	if (strcmp(cp, "STAT\r\n") == 0) {
		if (file_size != (off_t) -1)
			reply(213, "Status: %s of %s bytes transferred",
			    off_to_str (byte_count), off_to_str (file_size));
		else
			reply(213, "Status: %s bytes transferred",
			      off_to_str (byte_count));
	}
}

/*
 * Note: a response of 425 is not mentioned as a possible response to
 *	the PASV command in RFC959. However, it has been blessed as
 *	a legitimate response by Jon Postel in a telephone conversation
 *	with Rick Adams on 25 Jan 89.
 */
void
passive()
{
	int len;
	char *p, *a;

	pdata = socket(AF_INET, SOCK_STREAM, 0);
	if (pdata < 0) {
		perror_reply(425, "Can't open passive connection");
		return;
	}
	pasv_addr = ctrl_addr;
	pasv_addr.sin_port = 0;
	(void) seteuid((uid_t)0);
	if (bind(pdata, (struct sockaddr *)&pasv_addr, sizeof(pasv_addr)) < 0) {
		(void) seteuid((uid_t)pw->pw_uid);
		goto pasv_error;
	}
	(void) seteuid((uid_t)pw->pw_uid);
	len = sizeof(pasv_addr);
	if (getsockname(pdata, (struct sockaddr *) &pasv_addr, &len) < 0)
		goto pasv_error;
	if (listen(pdata, 1) < 0)
		goto pasv_error;
	a = (char *) &pasv_addr.sin_addr;
	p = (char *) &pasv_addr.sin_port;

#define UC(b) (((int) b) & 0xff)

	reply(227, "Entering Passive Mode (%d,%d,%d,%d,%d,%d)", UC(a[0]),
		UC(a[1]), UC(a[2]), UC(a[3]), UC(p[0]), UC(p[1]));
	return;

pasv_error:
	(void) close(pdata);
	pdata = -1;
	perror_reply(425, "Can't open passive connection");
	return;
}

/*
 * Generate unique name for file with basename "local".
 * The file named "local" is already known to exist.
 * Generates failure reply on error.
 */
static char *
gunique(char *local)
{
	static char *new = 0;
	struct stat st;
	int count;
	char *cp;

	cp = strrchr(local, '/');
	if (cp)
		*cp = '\0';
	if (stat(cp ? local : ".", &st) < 0) {
		perror_reply(553, cp ? local : ".");
		return ((char *) 0);
	}
	if (cp)
		*cp = '/';

	if (new)
		free (new);

	new = malloc (strlen (local) + 5); /* '.' + DIG + DIG + '\0' */
	if (new) {
		strcpy(new, local);
		cp = new + strlen(new);
		*cp++ = '.';
		for (count = 1; count < 100; count++) {
			(void)sprintf(cp, "%d", count);
			if (stat(new, &st) < 0)
				return (new);
		}
	}

	reply(452, "Unique file name cannot be created.");
	return (NULL);
}

/*
 * Format and send reply containing system error number.
 */
void
perror_reply(int code, char *string)
{

	reply(code, "%s: %s.", string, strerror(errno));
}

static char *onefile[] = {
	"",
	0
};

void
send_file_list(char *whichf)
{
	struct stat st;
	DIR *dirp = NULL;
	struct dirent *dir;
	FILE *dout = NULL;
	char **dirlist, *dirname;
	int simple = 0;
	int freeglob = 0;
	glob_t gl;

	if (strpbrk(whichf, "~{[*?") != NULL) {
		int flags = GLOB_NOCHECK;

#ifdef GLOB_BRACE
		flags |= GLOB_BRACE;
#endif
#ifdef GLOB_QUOTE
		flags |= GLOB_QUOTE;
#endif
#ifdef GLOB_TILDE
		flags |= GLOB_TILDE;
#endif

		memset(&gl, 0, sizeof(gl));
		freeglob = 1;
		if (glob(whichf, flags, 0, &gl)) {
			reply(550, "not found");
			goto out;
		} else if (gl.gl_pathc == 0) {
			errno = ENOENT;
			perror_reply(550, whichf);
			goto out;
		}
		dirlist = gl.gl_pathv;
	} else {
		onefile[0] = whichf;
		dirlist = onefile;
		simple = 1;
	}

	if (setjmp(urgcatch)) {
		transflag = 0;
		goto out;
	}
	while (dirname = *dirlist++) {
		if (stat(dirname, &st) < 0) {
			/*
			 * If user typed "ls -l", etc, and the client
			 * used NLST, do what the user meant.
			 */
			if (dirname[0] == '-' && *dirlist == NULL &&
			    transflag == 0) {
				retrieve("/bin/ls %s", dirname);
				goto out;
			}
			perror_reply(550, whichf);
			if (dout != NULL) {
				(void) fclose(dout);
				transflag = 0;
				data = -1;
				pdata = -1;
			}
			goto out;
		}

		if (S_ISREG(st.st_mode)) {
			if (dout == NULL) {
				dout = dataconn("file list", (off_t)-1, "w");
				if (dout == NULL)
					goto out;
				transflag++;
			}
			fprintf(dout, "%s%s\n", dirname,
				type == TYPE_A ? "\r" : "");
			byte_count += strlen(dirname) + 1;
			continue;
		} else if (!S_ISDIR(st.st_mode))
			continue;

		if ((dirp = opendir(dirname)) == NULL)
			continue;

		while ((dir = readdir(dirp)) != NULL) {
			char *nbuf;

			if (dir->d_name[0] == '.' && dir->d_name[1] == '\0')
				continue;
			if (dir->d_name[0] == '.' && dir->d_name[1] == '.' &&
			    dir->d_name[2] == '\0')
				continue;

			nbuf = (char *) alloca (strlen (dirname) + 1 + strlen (dir->d_name) + 1);
			sprintf(nbuf, "%s/%s", dirname, dir->d_name);

			/*
			 * We have to do a stat to insure it's
			 * not a directory or special file.
			 */
			if (simple || (stat(nbuf, &st) == 0 &&
			    S_ISREG(st.st_mode))) {
				if (dout == NULL) {
					dout = dataconn("file list", (off_t)-1,
						"w");
					if (dout == NULL)
						goto out;
					transflag++;
				}
				if (nbuf[0] == '.' && nbuf[1] == '/')
					fprintf(dout, "%s%s\n", &nbuf[2],
						type == TYPE_A ? "\r" : "");
				else
					fprintf(dout, "%s%s\n", nbuf,
						type == TYPE_A ? "\r" : "");
				byte_count += strlen(nbuf) + 1;
			}
		}
		(void) closedir(dirp);
	}

	if (dout == NULL)
		reply(550, "No files found.");
	else if (ferror(dout) != 0)
		perror_reply(550, "Data connection");
	else
		reply(226, "Transfer complete.");

	transflag = 0;
	if (dout != NULL)
		(void) fclose(dout);
	data = -1;
	pdata = -1;
out:
	if (freeglob) {
		freeglob = 0;
		globfree(&gl);
	}
}

#ifdef SETPROCTITLE
/*
 * Clobber argv so ps will show what we're doing.  (Stolen from sendmail.)
 * Warning, since this is usually started from inetd.conf, it often doesn't
 * have much of an environment or arglist to overwrite.
 */
void
#if defined(HAVE_STDARG_H) && defined(__STDC__) && __STDC__
setproctitle(const char *fmt, ...)
#else
setproctitle(fmt, va_alist)
	char *fmt;
        va_dcl
#endif
{
	int i;
	va_list ap;
	char *p, *bp, ch;
	char buf[LINE_MAX];

#if defined(HAVE_STDARG_H) && defined(__STDC__) && __STDC__
	va_start(ap, fmt);
#else
	va_start(ap);
#endif
	(void)vsnprintf(buf, sizeof(buf), fmt, ap);

	/* make ps print our process name */
	p = Argv[0];
	*p++ = '-';

	i = strlen(buf);
	if (i > LastArgv - p - 2) {
		i = LastArgv - p - 2;
		buf[i] = '\0';
	}
	bp = buf;
	while (ch = *bp++)
		if (ch != '\n' && ch != '\r')
			*p++ = ch;
	while (p < LastArgv)
		*p++ = ' ';
}
#endif /* SETPROCTITLE */


#ifdef HAVE_LIBWRAP
static int check_host(struct sockaddr *sa)
{
	struct sockaddr_in *sin;
        struct hostent *hp;
        char *addr;

	if (sa->sa_family != AF_INET)
              return 1;

	sin = (struct sockaddr_in *)sa;
	hp = gethostbyaddr((char *)&sin->sin_addr,
			sizeof(struct in_addr), AF_INET);
        addr = inet_ntoa(sin->sin_addr);
        if (hp) {
                if (!hosts_ctl("ftpd", hp->h_name, addr, STRING_UNKNOWN)) {
                        syslog(LOG_NOTICE, "tcpwrappers rejected: %s [%s]",
				hp->h_name, addr);
                        return (0);
                 }
	} else {
		if (!hosts_ctl("ftpd", STRING_UNKNOWN, addr, STRING_UNKNOWN)) {
                        syslog(LOG_NOTICE, "tcpwrappers rejected: [%s]", addr);
                        return (0);
                }
        }
        return (1);
}
#endif
