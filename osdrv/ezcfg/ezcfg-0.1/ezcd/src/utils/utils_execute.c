/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : utils/utils_execute.c
 *
 * Description  : execute command without shell, replace system() call
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-10   0.1       Write it from scrach
 * ============================================================================
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "ezcd.h"

int utils_execute(char *const argv[], char *in_path, char *out_path, int timeout, int *ppid)
{
  pid_t pid;
  int status;
  int fd;
  int flags;
  int sig;

  switch (pid = fork()) {
  case -1: /* error */
    perror("fork");
    return errno;

  case 0: /* child */
    /* reset signal handlers set for parent process */
    for (sig = 0; sig < (_NSIG-1); sig++)
      signal(sig, SIG_DFL);

    /* clean up */
    ioctl(0, TIOCNOTTY, 0);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    setsid();

    /* check if /dev/console is available */
    if ((fd = open("/dev/console", O_RDWR)) < 0) {
      (void) open("/dev/null", O_RDONLY);
      (void) open("/dev/null", O_WRONLY);
      (void) open("/dev/null", O_WRONLY);
    }
    else {
      close(fd);
      (void) open("/dev/console", O_RDONLY);
      (void) open("/dev/console", O_WRONLY);
      (void) open("/dev/console", O_WRONLY);
    }

    /* redirect stdin to <in_path> */
    if (in_path) {
      flags = O_RDONLY;
      if ((fd = open(in_path, flags, 0644)) < 0) {
	perror(in_path);
      }
      else {
	dup2(fd, STDIN_FILENO);
	close(fd);
      }
    }

    /* redirect stdout to <out_path> */
    if (out_path) {
      flags = O_WRONLY | O_CREAT;
      if (!strncmp(out_path, ">>", 2)) {
	/* append to <out_path> */
	flags |= O_APPEND;
	out_path += 2;
      } else if (!strncmp(out_path, ">", 1)) {
	/* overwrite <out_path> */
	flags |= O_TRUNC;
	out_path += 1;
      }
      if ((fd = open(out_path, flags, 0644)) < 0) {
	perror(out_path);
      }
      else {
	dup2(fd, STDOUT_FILENO);
	close(fd);
      }
    }

    /* execute command */
    setenv("PATH", "/sbin:/bin:/usr/sbin:/usr/bin", 1);
    alarm(timeout);
    execvp(argv[0], argv);
    perror(argv[0]);
    exit(errno);

  default: /* parent */
    if (ppid) {
      *ppid = pid;
      return 0;
    } else {
      if (waitpid(pid, &status, 0) == -1) {
	perror("waitpid");
	return errno;
      }
      if (WIFEXITED(status))
	return WEXITSTATUS(status);
      else
	return status;
    }
  }
}
