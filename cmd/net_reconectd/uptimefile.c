/*
    pidfile.c - interact with pidfiles
    Copyright (c) 1995  Martin Schulze <Martin.Schulze@Linux.DE>

    This file is part of the sysklogd package, a kernel and system log daemon.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111, USA
*/

/*
 * Sat Aug 19 13:24:33 MET DST 1995: Martin Schulze
 *	First version (v0.2) released
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>

/* read_pid
 *
 * Reads the specified pidfile and returns the read pid.
 * 0 is returned if either there's no pidfile, it's empty
 * or no pid can be read.
 */
int read_uptime (char *pidfile)
{
  FILE *f;
  int pid;

  if (!(f=fopen(pidfile,"r")))
    return 0;
  fscanf(f,"%d", &pid);
  fclose(f);
  return pid;
}

/* write_pid
 *
 * Writes the pid to the specified file. If that fails 0 is
 * returned, otherwise the pid.
 */
int write_uptime (char *pidfile)
{
  FILE *f;
  int fd;
  int pid;

  if ( ((fd = open(pidfile, O_RDWR|O_CREAT|O_TRUNC, 0644)) == -1)
       || ((f = fdopen(fd, "r+")) == NULL) ) {
      fprintf(stderr, "Can't open or create %s.\n", pidfile);
      return 0;
  }

  if (flock(fd, LOCK_EX|LOCK_NB) == -1) {
      fscanf(f, "%d", &pid);
      fclose(f);
      printf("Can't lock, lock is held by pid %d.\n", pid);
      return 0;
  }

  pid = getpid();
  if (!fprintf(f,"%d\n", pid)) {
      printf("Can't write pid , %s.\n", strerror(errno));
      close(fd);
      return 0;
  }
  fflush(f);

  if (flock(fd, LOCK_UN) == -1) {
      printf("Can't unlock pidfile %s, %s.\n", pidfile, strerror(errno));
      close(fd);
      return 0;
  }
  close(fd);

  return pid;
}
  
