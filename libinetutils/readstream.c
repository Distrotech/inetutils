#include <stropts.h>
#include <termios.h>
#include <termio.h>
#include <sys/tty.h>
#include <sys/ptyvar.h>
#include <errno.h>

/* readstream - convert SYSV pty packets to BSD pty packets */
int readstream(int p, char *ibuf, int bufsize)
{
	int flags = 0;
	int ret = 0;
	struct termios *tsp;
	struct termio *tp;
	struct iocblk *ip;
	char vstop, vstart;
	int ixon;
	int newflow;
	int flowison = -1;  /* current state of flow: -1 is unknown */
	struct strbuf strbufc, strbufd;
#define BUFFERSIZE 100
	char ctlbuf[BUFFERSIZE];  /* control buffer */

	strbufc.maxlen = sizeof (ctlbuf);
	strbufc.buf = (char *)ctlbuf;

	strbufd.maxlen = bufsize - 1;
	strbufd.len = 0;
	strbufd.buf = ibuf + 1;
	ibuf[0] = 0;

	ret = getmsg(p, &strbufc, &strbufd, &flags);
	if (ret < 0)  /* error of some sort -- probably EAGAIN */
		return(ret);

	if (strbufc.len <= 0 || ctlbuf[0] == M_DATA) {
		/* data message */
		if (strbufd.len > 0) {			/* real data */
			return(strbufd.len + 1);	/* count header char */
		} else {
			/* nothing there */
			errno = EAGAIN;
			return(-1);
		}
	}

	/*
	 * It's a control message.  Return 1, to look at the flag we set
	 */

	switch (ctlbuf[0] & 0377) {
		case M_FLUSH:
			if (ibuf[1] & FLUSHW)
				ibuf[0] = TIOCPKT_FLUSHWRITE;
			return(1);

		case M_IOCTL:
			ip = (struct iocblk *) (ibuf+1);

			switch (ip->ioc_cmd) {
				case TCSETS:
				case TCSETSW:
				case TCSETSF:
					tsp = (struct termios *)
						(ibuf+1+ sizeof(struct iocblk));
					vstop = tsp->c_cc[VSTOP];
					vstart = tsp->c_cc[VSTART];
					ixon = tsp->c_iflag & IXON;
					break;
				case TCSETA:
				case TCSETAW:
				case TCSETAF:
					tp = (struct termio *)
						(ibuf+1+ sizeof(struct iocblk));
					vstop = tp->c_cc[VSTOP];
					vstart = tp->c_cc[VSTART];
					ixon = tp->c_iflag & IXON;
					break;
				default:
					errno = EAGAIN;
					return(-1);
			}

		newflow =  (ixon && (vstart == 021) &&
				(vstop == 023)) ? 1 : 0;
		if (newflow != flowison) {  /* it's a change */
			flowison = newflow;
			ibuf[0] = newflow ? TIOCPKT_DOSTOP :
				TIOCPKT_NOSTOP;
			return(1);
		}
	}

	/* nothing worth doing anything about */
	errno = EAGAIN;
	return(-1);
}
