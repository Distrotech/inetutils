	int
getpty(ptynum)
int *ptynum;
{
	register int p;
#ifdef	STREAMSPTY
	int t;
	char *ptsname();

	p = open("/dev/ptmx", O_RDWR);
	if (p > 0) {
		grantpt(p);
		unlockpt(p);
		strcpy(line, ptsname(p));
		return(p);
	}

#else	/* ! STREAMSPTY */
#ifndef CRAY
	register char *cp, *p1, *p2;
	register int i;
#if defined(sun) && defined(TIOCGPGRP) && BSD < 199207
	int dummy;
#endif

#ifndef	__hpux
	(void) sprintf(line, "/dev/ptyXX");
	p1 = &line[8];
	p2 = &line[9];
#else
	(void) sprintf(line, "/dev/ptym/ptyXX");
	p1 = &line[13];
	p2 = &line[14];
#endif

	for (cp = "pqrstuvwxyzPQRST"; *cp; cp++) {
		struct stat stb;

		*p1 = *cp;
		*p2 = '0';
		/*
		 * This stat() check is just to keep us from
		 * looping through all 256 combinations if there
		 * aren't that many ptys available.
		 */
		if (stat(line, &stb) < 0)
			break;
		for (i = 0; i < 16; i++) {
			*p2 = "0123456789abcdef"[i];
			p = open(line, O_RDWR);
			if (p > 0) {
#ifndef	__hpux
				line[5] = 't';
#else
				for (p1 = &line[8]; *p1; p1++)
					*p1 = *(p1+1);
				line[9] = 't';
#endif
				chown(line, 0, 0);
				chmod(line, 0600);
#if defined(sun) && defined(TIOCGPGRP) && BSD < 199207
				if (ioctl(p, TIOCGPGRP, &dummy) == 0
				    || errno != EIO) {
					chmod(line, 0666);
					close(P);
					line[5] = 'p';
				} else
#endif /* defined(sun) && defined(TIOCGPGRP) && BSD < 199207 */
					return(p);
			}
		}
	}
#else	/* CRAY */
	extern lowpty, highpty;
	struct stat sb;

	for (*ptynum = lowpty; *ptynum <= highpty; (*ptynum)++) {
		(void) sprintf(myline, "/dev/pty/%03d", *ptynum);
		p = open(myline, O_RDWR);
		if (p < 0)
			continue;
		(void) sprintf(line, "/dev/ttyp%03d", *ptynum);
		/*
		 * Here are some shenanigans to make sure that there
		 * are no listeners lurking on the line.
		 */
		if(stat(line, &sb) < 0) {
			(void) close(p);
			continue;
		}
		if(sb.st_uid || sb.st_gid || sb.st_mode != 0600) {
			chown(line, 0, 0);
			chmod(line, 0600);
			(void)close(p);
			p = open(myline, O_RDWR);
			if (p < 0)
				continue;
		}
		/*
		 * Now it should be safe...check for accessability.
		 */
		if (access(line, 6) == 0)
			return(p);
		else {
			/* no tty side to pty so skip it */
			(void) close(p);
		}
	}
#endif	/* CRAY */
#endif	/* STREAMSPTY */
	return(-1);
}
