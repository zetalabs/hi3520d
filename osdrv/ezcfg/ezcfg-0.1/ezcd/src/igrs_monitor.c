/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : igrs_monitor.c
 *
 * Description  : ezbox igrs monitor daemon program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-19   0.1       Write it from scratch
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
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>
#include <dlfcn.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "ezcd.h"

#if 0
#define DBG(format, args...) do {\
	FILE *fp = fopen("/dev/kmsg", "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

int igrs_monitor_main(int argc, char **argv)
{
	pid_t pid;

	pid = fork();
	if (pid < 0) {
		DBG("<6>igrs_monitor: can't fork");
		return (EXIT_FAILURE);
	}
	if (pid > 0) {
		return (EXIT_SUCCESS); /* parent */
	}

	/* child */
	/* unset umask */
	umask(0);

	/* run main loop forever */
	while (1) {
		sleep(1);
	}

	/* should never run to this place!!! */
	return (EXIT_FAILURE);
}
