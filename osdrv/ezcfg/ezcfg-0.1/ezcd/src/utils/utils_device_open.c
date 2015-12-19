/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_device_open.c
 *
 * Description  : ezbox device open routing
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-04   0.1       Write it from scratch
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
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcd.h"

/* try to open up the specified device */
int utils_device_open(const char *device, int mode)
{
	int m, f, fd;

	m = mode | O_NONBLOCK;

	/* retry up to 5 times */
	for (f = 0; f < 5; f++) {
		fd = open(device, m, 0600);
		if (fd >= 0)
			break;
	}
	if (fd < 0)
		return fd;
	/* reset original flags. */
	if (m != mode)
		fcntl(fd, F_SETFL, mode);
	return fd;
}
