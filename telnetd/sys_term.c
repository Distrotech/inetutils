/*
 * Copyright (c) 1989, 1993
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
static char sccsid[] = "@(#)sys_term.c	8.4 (Berkeley) 5/30/95";
#endif /* not lint */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "telnetd.h"

#if	defined(AUTHENTICATION)
#include <libtelnet/auth.h>
#endif

#if defined(CRAY) || defined(__hpux)
# define PARENT_DOES_UTMP
#endif

#ifdef	HAVE_SYS_STREAM_H
# include <sys/stream.h>
#endif

#if defined(UNICOS5) && defined(CRAY2) && !defined(EXTPROC)
# define EXTPROC 0400
#endif

#ifndef	USE_TERMIO
struct termbuf {
	struct sgttyb sg;
	struct tchars tc;
	struct ltchars ltc;
	int state;
	int lflags;
} termbuf, termbuf2;
# define	cfsetospeed(tp, val)	(tp)->sg.sg_ospeed = (val)
# define	cfsetispeed(tp, val)	(tp)->sg.sg_ispeed = (val)
# define	cfgetospeed(tp)		(tp)->sg.sg_ospeed
# define	cfgetispeed(tp)		(tp)->sg.sg_ispeed
#else	/* USE_TERMIO */
# ifdef	SYSV_TERMIO
#	define termios termio
# endif
# ifndef	TCSANOW
#  ifdef TCSETS
#   define	TCSANOW		TCSETS
#   define	TCSADRAIN	TCSETSW
#   define	tcgetattr(f, t)	ioctl(f, TCGETS, (char *)t)
#  else
#   ifdef TCSETA
#    define	TCSANOW		TCSETA
#    define	TCSADRAIN	TCSETAW
#    define	tcgetattr(f, t)	ioctl(f, TCGETA, (char *)t)
#   else
#    define	TCSANOW		TIOCSETA
#    define	TCSADRAIN	TIOCSETAW
#    define	tcgetattr(f, t)	ioctl(f, TIOCGETA, (char *)t)
#   endif
#  endif
#  define	tcsetattr(f, a, t)	ioctl(f, a, t)
#  define	cfsetospeed(tp, val)	(tp)->c_cflag &= ~CBAUD; \
					(tp)->c_cflag |= (val)
#  define	cfgetospeed(tp)		((tp)->c_cflag & CBAUD)
#  ifdef CIBAUD
#   define	cfsetispeed(tp, val)	(tp)->c_cflag &= ~CIBAUD; \
					(tp)->c_cflag |= ((val)<<IBSHIFT)
#   define	cfgetispeed(tp)		(((tp)->c_cflag & CIBAUD)>>IBSHIFT)
#  else
#   define	cfsetispeed(tp, val)	(tp)->c_cflag &= ~CBAUD; \
					(tp)->c_cflag |= (val)
#   define	cfgetispeed(tp)		((tp)->c_cflag & CBAUD)
#  endif
# endif /* TCSANOW */
struct termios termbuf, termbuf2;	/* pty control structure */
# ifdef  HAVE_STREAMSPTY
int ttyfd = -1;
# endif
#endif	/* USE_TERMIO */

int cleanopen(char *);

/*
 * init_termbuf()
 * copy_termbuf(cp)
 * set_termbuf()
 *
 * These three routines are used to get and set the "termbuf" structure
 * to and from the kernel.  init_termbuf() gets the current settings.
 * copy_termbuf() hands in a new "termbuf" to write to the kernel, and
 * set_termbuf() writes the structure into the kernel.
 */

void
init_termbuf ()
{
#ifndef	USE_TERMIO
  ioctl (pty, TIOCGETP, (char *)&termbuf.sg);
  ioctl (pty, TIOCGETC, (char *)&termbuf.tc);
  ioctl (pty, TIOCGLTC, (char *)&termbuf.ltc);
# ifdef	TIOCGSTATE
  ioctl (pty, TIOCGSTATE, (char *)&termbuf.state);
# endif
#else
# ifdef  HAVE_STREAMSPTY
  tcgetattr(ttyfd, &termbuf);
# else
  tcgetattr(pty, &termbuf);
# endif
#endif
  termbuf2 = termbuf;
}

#if defined(LINEMODE) && defined(TIOCPKT_IOCTL)
void
copy_termbuf (char *cp, int len)
{
  if (len > sizeof (termbuf))
    len = sizeof (termbuf);
  memmove ((char *)&termbuf, cp, len);
  termbuf2 = termbuf;
}
#endif	/* defined(LINEMODE) && defined(TIOCPKT_IOCTL) */

void
set_termbuf ()
{
  /*
   * Only make the necessary changes.
   */
#ifndef	USE_TERMIO
  if (memcmp (&termbuf.sg, &termbuf2.sg, sizeof (termbuf.sg)))
    ioctl(pty, TIOCSETN, (char *)&termbuf.sg);
  if (memcmp (&termbuf.tc, &termbuf2.tc, sizeof (termbuf.tc)))
    ioctl (pty, TIOCSETC, (char *)&termbuf.tc);
  if (memcmp (&termbuf.ltc, &termbuf2.ltc, sizeof (termbuf.ltc)))
    ioctl (pty, TIOCSLTC, (char *)&termbuf.ltc);
  if (termbuf.lflags != termbuf2.lflags)
    ioctl (pty, TIOCLSET, &termbuf.lflags);
#else	/* USE_TERMIO */
  if (memcmp (&termbuf, &termbuf2, sizeof (termbuf)))
    {
# ifdef  HAVE_STREAMSPTY
      tcsetattr (ttyfd, TCSANOW, &termbuf);
# else
      tcsetattr (pty, TCSANOW, &termbuf);
# endif
    }
# if defined(CRAY2) && defined(UNICOS5)
  needtermstat = 1;
# endif
#endif	/* USE_TERMIO */
}

/*
 * spcset(func, valp, valpp)
 *
 * This function takes various special characters (func), and
 * sets *valp to the current value of that character, and
 * *valpp to point to where in the "termbuf" structure that
 * value is kept.
 *
 * It returns the SLC_ level of support for this function.
 */

#ifndef	USE_TERMIO
int
spcset (int func, cc_t *valp, cc_t **valpp)
{
  switch (func)
    {
    case SLC_EOF:
      *valp = termbuf.tc.t_eofc;
      *valpp = (cc_t *)&termbuf.tc.t_eofc;
      return SLC_VARIABLE;
      
    case SLC_EC:
      *valp = termbuf.sg.sg_erase;
      *valpp = (cc_t *)&termbuf.sg.sg_erase;
      return SLC_VARIABLE;
      
    case SLC_EL:
      *valp = termbuf.sg.sg_kill;
      *valpp = (cc_t *)&termbuf.sg.sg_kill;
      return SLC_VARIABLE;
      
    case SLC_IP:
      *valp = termbuf.tc.t_intrc;
      *valpp = (cc_t *)&termbuf.tc.t_intrc;
      return SLC_VARIABLE|SLC_FLUSHIN|SLC_FLUSHOUT;
      
    case SLC_ABORT:
      *valp = termbuf.tc.t_quitc;
      *valpp = (cc_t *)&termbuf.tc.t_quitc;
      return SLC_VARIABLE|SLC_FLUSHIN|SLC_FLUSHOUT;
      
    case SLC_XON:
      *valp = termbuf.tc.t_startc;
      *valpp = (cc_t *)&termbuf.tc.t_startc;
      return SLC_VARIABLE;
      
    case SLC_XOFF:
      *valp = termbuf.tc.t_stopc;
      *valpp = (cc_t *)&termbuf.tc.t_stopc;
      return SLC_VARIABLE;
      
    case SLC_AO:
      *valp = termbuf.ltc.t_flushc;
      *valpp = (cc_t *)&termbuf.ltc.t_flushc;
      return SLC_VARIABLE;
      
    case SLC_SUSP:
      *valp = termbuf.ltc.t_suspc;
      *valpp = (cc_t *)&termbuf.ltc.t_suspc;
      return SLC_VARIABLE;
      
    case SLC_EW:
      *valp = termbuf.ltc.t_werasc;
      *valpp = (cc_t *)&termbuf.ltc.t_werasc;
      return SLC_VARIABLE;
      
    case SLC_RP:
      *valp = termbuf.ltc.t_rprntc;
      *valpp = (cc_t *)&termbuf.ltc.t_rprntc;
      return SLC_VARIABLE;
      
    case SLC_LNEXT:
      *valp = termbuf.ltc.t_lnextc;
      *valpp = (cc_t *)&termbuf.ltc.t_lnextc;
      return SLC_VARIABLE;
      
    case SLC_FORW1:
      *valp = termbuf.tc.t_brkc;
      *valpp = (cc_t *)&termbuf.ltc.t_lnextc;
      return SLC_VARIABLE;
      
    case SLC_BRK:
    case SLC_SYNCH:
    case SLC_AYT:
    case SLC_EOR:
      *valp = (cc_t)0;
      *valpp = (cc_t *)0;
      return SLC_DEFAULT;
      
    default:
      *valp = (cc_t)0;
      *valpp = (cc_t *)0;
      return SLC_NOSUPPORT;
    }
}

#else	/* USE_TERMIO */

#define	setval(a, b)	*valp = termbuf.c_cc[a]; \
			*valpp = &termbuf.c_cc[a]; \
			return b;
#define	defval(a)       *valp = ((cc_t)a); \
                        *valpp = (cc_t *)0; \
			return SLC_DEFAULT;

int
spcset (int func, cc_t *valp, cc_t **valpp)
{
  switch (func)
    {
    case SLC_EOF:
      setval (VEOF, SLC_VARIABLE);
      
    case SLC_EC:
      setval (VERASE, SLC_VARIABLE);
      
    case SLC_EL:
      setval (VKILL, SLC_VARIABLE);
      
    case SLC_IP:
      setval (VINTR, SLC_VARIABLE|SLC_FLUSHIN|SLC_FLUSHOUT);
      
    case SLC_ABORT:
      setval (VQUIT, SLC_VARIABLE|SLC_FLUSHIN|SLC_FLUSHOUT);
      
    case SLC_XON:
#ifdef	VSTART
      setval (VSTART, SLC_VARIABLE);
#else
      defval (0x13);
#endif
      
    case SLC_XOFF:
#ifdef	VSTOP
      setval (VSTOP, SLC_VARIABLE);
#else
      defval (0x11);
#endif
      
    case SLC_EW:
#ifdef	VWERASE
      setval (VWERASE, SLC_VARIABLE);
#else
      defval (0);
#endif
      
    case SLC_RP:
#ifdef	VREPRINT
      setval (VREPRINT, SLC_VARIABLE);
#else
      defval(0);
#endif
      
    case SLC_LNEXT:
#ifdef	VLNEXT
      setval (VLNEXT, SLC_VARIABLE);
#else
      defval (0);
#endif
      
    case SLC_AO:
#if !defined(VDISCARD) && defined(VFLUSHO)
# define VDISCARD VFLUSHO
#endif
#ifdef	VDISCARD
      setval (VDISCARD, SLC_VARIABLE|SLC_FLUSHOUT);
#else
      defval (0);
#endif
      
    case SLC_SUSP:
#ifdef	VSUSP
      setval (VSUSP, SLC_VARIABLE|SLC_FLUSHIN);
#else
      defval (0);
#endif
      
#ifdef	VEOL
    case SLC_FORW1:
      setval (VEOL, SLC_VARIABLE);
#endif
      
#ifdef	VEOL2
    case SLC_FORW2:
      setval (VEOL2, SLC_VARIABLE);
#endif
      
    case SLC_AYT:
#ifdef	VSTATUS
      setval (VSTATUS, SLC_VARIABLE);
#else
      defval (0);
#endif

    case SLC_BRK:
    case SLC_SYNCH:
    case SLC_EOR:
      defval (0);

    default:
      *valp = 0;
      *valpp = 0;
      return SLC_NOSUPPORT;
    }
}
#endif	/* USE_TERMIO */

#ifdef	LINEMODE
/*
 * tty_flowmode()	Find out if flow control is enabled or disabled.
 * tty_linemode()	Find out if linemode (external processing) is enabled.
 * tty_setlinemod(on)	Turn on/off linemode.
 * tty_isecho()		Find out if echoing is turned on.
 * tty_setecho(on)	Enable/disable character echoing.
 * tty_israw()		Find out if terminal is in RAW mode.
 * tty_binaryin(on)	Turn on/off BINARY on input.
 * tty_binaryout(on)	Turn on/off BINARY on output.
 * tty_isediting()	Find out if line editing is enabled.
 * tty_istrapsig()	Find out if signal trapping is enabled.
 * tty_setedit(on)	Turn on/off line editing.
 * tty_setsig(on)	Turn on/off signal trapping.
 * tty_issofttab()	Find out if tab expansion is enabled.
 * tty_setsofttab(on)	Turn on/off soft tab expansion.
 * tty_islitecho()	Find out if typed control chars are echoed literally
 * tty_setlitecho()	Turn on/off literal echo of control chars
 * tty_tspeed(val)	Set transmit speed to val.
 * tty_rspeed(val)	Set receive speed to val.
 */

#ifdef convex
static int linestate;
#endif

int
tty_linemode()
{
#ifndef convex
#ifdef	USE_TERMIO
#ifdef EXTPROC
  return (termbuf.c_lflag & EXTPROC);
#else
  return 0;		/* Can't ever set it either. */
#endif /* EXTPROC */
#else /* !USE_TERMIO */
  return termbuf.state & TS_EXTPROC;
#endif /* USE_TERMIO */
#else /* convex */
  return linestate;
#endif /* !convex */
}

void
tty_setlinemode (int on)
{
#ifdef	TIOCEXT
# ifndef convex
  set_termbuf ();
# else
  linestate = on;
# endif
  ioctl (pty, TIOCEXT, (char *)&on);
# ifndef convex
  init_termbuf ();
# endif
#else	/* !TIOCEXT */
# ifdef	EXTPROC
  if (on)
    termbuf.c_lflag |= EXTPROC;
  else
    termbuf.c_lflag &= ~EXTPROC;
# endif
#endif	/* TIOCEXT */
}
#endif	/* LINEMODE */

int
tty_isecho ()
{
#ifndef USE_TERMIO
  return termbuf.sg.sg_flags & ECHO;
#else
  return termbuf.c_lflag & ECHO;
#endif
}

int
tty_flowmode ()
{
#ifndef USE_TERMIO
  return ((termbuf.tc.t_startc) > 0 && (termbuf.tc.t_stopc) > 0) ? 1 : 0;
#else
  return (termbuf.c_iflag & IXON) ? 1 : 0;
#endif
}

int
tty_restartany ()
{
#ifndef USE_TERMIO
# ifdef	DECCTQ
  return (termbuf.lflags & DECCTQ) ? 0 : 1;
# else
  return -1;
# endif
#else
  return (termbuf.c_iflag & IXANY) ? 1 : 0;
#endif
}

void
tty_setecho(int on)
{
#ifndef	USE_TERMIO
  if (on)
    termbuf.sg.sg_flags |= ECHO|CRMOD;
  else
    termbuf.sg.sg_flags &= ~(ECHO|CRMOD);
#else
  if (on)
    termbuf.c_lflag |= ECHO;
  else
    termbuf.c_lflag &= ~ECHO;
#endif
}

int
tty_israw ()
{
#ifndef USE_TERMIO
  return termbuf.sg.sg_flags & RAW;
#else
  return !(termbuf.c_lflag & ICANON);
#endif
}

#if defined (AUTHENTICATION) && defined(NO_LOGIN_F) && defined(LOGIN_R)
int
tty_setraw(int on)
{
# ifndef USE_TERMIO
  if (on)
    termbuf.sg.sg_flags |= RAW;
  else
    termbuf.sg.sg_flags &= ~RAW;
# else
  if (on)
    termbuf.c_lflag &= ~ICANON;
  else
    termbuf.c_lflag |= ICANON;
# endif
}
#endif

void
tty_binaryin (int on)
{
#ifndef	USE_TERMIO
  if (on)
    termbuf.lflags |= LPASS8;
  else
    termbuf.lflags &= ~LPASS8;
#else
  if (on)
    termbuf.c_iflag &= ~ISTRIP;
  else 
    termbuf.c_iflag |= ISTRIP;
#endif
}

void
tty_binaryout (int on)
{
#ifndef	USE_TERMIO
  if (on)
    termbuf.lflags |= LLITOUT;
  else
    termbuf.lflags &= ~LLITOUT;
#else
  if (on)
    {
      termbuf.c_cflag &= ~(CSIZE|PARENB);
      termbuf.c_cflag |= CS8;
      termbuf.c_oflag &= ~OPOST;
    }
  else
    {
      termbuf.c_cflag &= ~CSIZE;
      termbuf.c_cflag |= CS7|PARENB;
      termbuf.c_oflag |= OPOST;
    }
#endif
}

int
tty_isbinaryin ()
{
#ifndef	USE_TERMIO
  return termbuf.lflags & LPASS8;
#else
  return !(termbuf.c_iflag & ISTRIP);
#endif
}

int
tty_isbinaryout ()
{
#ifndef	USE_TERMIO
  return termbuf.lflags & LLITOUT;
#else
  return !(termbuf.c_oflag&OPOST);
#endif
}

#ifdef	LINEMODE
int
tty_isediting ()
{
#ifndef USE_TERMIO
  return !(termbuf.sg.sg_flags & (CBREAK|RAW));
#else
  return termbuf.c_lflag & ICANON;
#endif
}

int
tty_istrapsig()
{
#ifndef USE_TERMIO
  return !(termbuf.sg.sg_flags&RAW);
#else
  return termbuf.c_lflag & ISIG;
#endif
}

void
tty_setedit (int on)
{
#ifndef USE_TERMIO
  if (on)
    termbuf.sg.sg_flags &= ~CBREAK;
  else
    termbuf.sg.sg_flags |= CBREAK;
#else
  if (on)
    termbuf.c_lflag |= ICANON;
  else
    termbuf.c_lflag &= ~ICANON;
#endif
}

void
tty_setsig (int on)
{
#ifdef	USE_TERMIO
  if (on)
    termbuf.c_lflag |= ISIG;
  else
    termbuf.c_lflag &= ~ISIG;
#endif
}
#endif	/* LINEMODE */

int
tty_issofttab ()
{
#ifndef	USE_TERMIO
  return termbuf.sg.sg_flags & XTABS;
#else
# ifdef	OXTABS
  return termbuf.c_oflag & OXTABS;
# endif
# ifdef	TABDLY
  return (termbuf.c_oflag & TABDLY) == TAB3;
# endif
#endif
}

void
tty_setsofttab (int on)
{
#ifndef	USE_TERMIO
  if (on)
    termbuf.sg.sg_flags |= XTABS;
  else
    termbuf.sg.sg_flags &= ~XTABS;
#else
  if (on)
    {
# ifdef	OXTABS
      termbuf.c_oflag |= OXTABS;
# endif
# ifdef	TABDLY
      termbuf.c_oflag &= ~TABDLY;
      termbuf.c_oflag |= TAB3;
# endif
    }
  else
    {
# ifdef	OXTABS
      termbuf.c_oflag &= ~OXTABS;
# endif
# ifdef	TABDLY
      termbuf.c_oflag &= ~TABDLY;
      termbuf.c_oflag |= TAB0;
# endif
    }
#endif
}

int
tty_islitecho ()
{
#ifndef	USE_TERMIO
  return !(termbuf.lflags & LCTLECH);
#else
# ifdef	ECHOCTL
  return !(termbuf.c_lflag & ECHOCTL);
# endif
# ifdef	TCTLECH
  return !(termbuf.c_lflag & TCTLECH);
# endif
# if!defined(ECHOCTL) && !defined(TCTLECH)
  return 0;	/* assumes ctl chars are echoed '^x' */
# endif
#endif
}

void
tty_setlitecho (int on)
{
#ifndef	USE_TERMIO
  if (on)
    termbuf.lflags &= ~LCTLECH;
  else
    termbuf.lflags |= LCTLECH;
#else
# ifdef	ECHOCTL
  if (on)
    termbuf.c_lflag &= ~ECHOCTL;
  else
    termbuf.c_lflag |= ECHOCTL;
# endif
# ifdef	TCTLECH
  if (on)
    termbuf.c_lflag &= ~TCTLECH;
  else
    termbuf.c_lflag |= TCTLECH;
# endif
#endif
}

int
tty_iscrnl ()
{
#ifndef	USE_TERMIO
  return termbuf.sg.sg_flags & CRMOD;
#else
  return termbuf.c_iflag & ICRNL;
#endif
}

/*
 * Try to guess whether speeds are "encoded" (4.2BSD) or just numeric (4.4BSD).
 */
#if B4800 != 4800
#define	DECODE_BAUD
#endif

#ifdef	DECODE_BAUD

/*
 * A table of available terminal speeds
 */
struct termspeeds {
  int	speed;
  int	value;
} termspeeds[] = {
  { 0,     B0 },    { 50,    B50 },   { 75,    B75 },
  { 110,   B110 },  { 134,   B134 },  { 150,   B150 },
  { 200,   B200 },  { 300,   B300 },  { 600,   B600 },
  { 1200,  B1200 }, { 1800,  B1800 }, { 2400,  B2400 },
  { 4800,   B4800 },
#ifdef	B7200
  { 7200,  B7200 },
#endif
  { 9600,   B9600 },
#ifdef	B14400
  { 14400,  B14400 },
#endif
#ifdef	B19200
  { 19200,  B19200 },
#endif
#ifdef	B28800
  { 28800,  B28800 },
#endif
#ifdef	B38400
  { 38400,  B38400 },
#endif
#ifdef	B57600
  { 57600,  B57600 },
#endif
#ifdef	B115200
  { 115200, B115200 },
#endif
#ifdef	B230400
  { 230400, B230400 },
#endif
  { -1,     0 }
};
#endif	/* DECODE_BAUD */

void
tty_tspeed (int val)
{
#ifdef	DECODE_BAUD
  register struct termspeeds *tp;

  for (tp = termspeeds; (tp->speed != -1) && (val > tp->speed); tp++)
    ;
  if (tp->speed == -1)	/* back up to last valid value */
    --tp;
  cfsetospeed (&termbuf, tp->value);
#else  
  cfsetospeed (&termbuf, val);
#endif	/* DECODE_BAUD */
}

void
tty_rspeed (int val)
{
#ifdef	DECODE_BAUD
  register struct termspeeds *tp;

  for (tp = termspeeds; (tp->speed != -1) && (val > tp->speed); tp++)
    ;
  if (tp->speed == -1)	/* back up to last valid value */
    --tp;
  cfsetispeed (&termbuf, tp->value);
#else	/* DECODE_BAUD */
  cfsetispeed (&termbuf, val);
#endif	/* DECODE_BAUD */
}

#if defined(CRAY2) && defined(UNICOS5)
int
tty_isnewmap ()
{
  return (termbuf.c_oflag & OPOST) && (termbuf.c_oflag & ONLCR) &&
			!(termbuf.c_oflag & ONLRET);
}
#endif

char	*envinit[3];
extern char **environ;

void
init_env()
{
	extern char *getenv();
	char **envp;

	envp = envinit;
	if (*envp = getenv("TZ"))
		*envp++ -= 3;
#if	defined(CRAY) || defined(__hpux)
	else
		*envp++ = "TZ=GMT0";
#endif
	*envp = 0;
	environ = envinit;
}


