/*-
 * Copyright (c) 1990, 1993
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

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)daemon.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */

#include <config.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

void
waitdaemon_timeout (int signo)
{
  int left;

  left = alarm(0);
  signal(SIGALRM, SIG_DFL);
  if (left == 0)
    errx(1, "timed out waiting for child");
  else
    _exit(0);
}

/* waitdaemon is like daemon, but optionally waits until the parent
   receives a SIGALRM from the child. MAXWAIT specifies a timeout,
   after which waitdaemon will return -1. otherwise waitdaemon will
   return the pid of the parent. If MAXWAIT is 0, waitdaemon returns
   immediately.  */
int
waitdaemon (int nochdir, int noclose, int maxwait)
{
  int fd;
  pid_t childpid;
  
  switch (childpid = fork()) {
  case -1:
    return (-1);
  case 0:
    break;
  default:
    if (maxwait > 0)
      {
	int status;
	pid_t pid;

	signal(SIGALRM, waitdaemon_timeout);
	alarm(maxwait);
#ifdef HAVE_WAITPID
	while ((pid = waitpid(-1, &status, 0)) != -1)
#else
        while ((pid = wait3(&status, 0, NULL)) != -1)
#endif
	  {
	    if (WIFEXITED(status))
	      errx (1, "child pid %d exited with return code %d",
		    pid, WEXITSTATUS(status));
	    if (WIFSIGNALED(status))
	      errx (1, "child pid %d exited on signal %d%s",
		    pid, WTERMSIG(status),
		    WCOREDUMP(status) ? " (core dumped)" : "");
	    if (pid == childpid)
	      break;
	  }
	_exit(0);
      }
  }

  if (setsid() == -1)
    return (-1);
  
  if (!nochdir)
    (void)chdir("/");
  
  if (!noclose && (fd = open(PATH_DEVNULL, O_RDWR, 0)) != -1) {
    (void)dup2(fd, STDIN_FILENO);
    (void)dup2(fd, STDOUT_FILENO);
    (void)dup2(fd, STDERR_FILENO);
    if (fd > 2)
      (void)close (fd);
  }
  return (getppid());
}

int
daemon (int nochdir, int noclose)
{
  return (waitdaemon(nochdir, noclose, 0) == -1) ? -1 : 0;
}
