/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_hostname.c
 *
 * Description  : ezbox /etc/hostname file generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-03   0.1       Write it from scratch
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

#include "ezcd.h"
#include "pop_func.h"

int pop_etc_hostname(int flag)
{
        FILE *file = NULL;

	/* generate /etc/hostname */
	file = fopen("/etc/hostname", "w");
	if (file) {
		fprintf(file, "%s", "ezbox");
		fclose(file);
	}

	/* also update /proc/sys/kernel/hostname */
	file = fopen("/proc/sys/kernel/hostname", "w");
	if (file) {
		fprintf(file, "%s", "ezbox");
		fclose(file);
	}

	return (EXIT_SUCCESS);
}
