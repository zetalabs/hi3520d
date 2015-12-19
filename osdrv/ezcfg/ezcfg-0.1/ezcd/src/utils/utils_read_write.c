/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_read_write.c
 *
 * Description  : ezbox read/write routing
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
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcd.h"

ssize_t utils_safe_read(int fd, void *buf, size_t count)
{
	ssize_t n;

	do {
		n = read(fd, buf, count);
	} while (n < 0 && errno == EINTR);

	return n;
}

ssize_t utils_safe_write(int fd, const void *buf, size_t count)
{
	ssize_t n;

	do {
		n = write(fd, buf, count);
	} while (n < 0 && errno == EINTR);

	return n;
}

ssize_t utils_full_write(int fd, const void *buf, size_t len)
{
	ssize_t cc;
	ssize_t total;

	total = 0;

	while (len) {
		cc = utils_safe_write(fd, buf, len);
		if (cc < 0) {
			if (total) {
				return total;
			}
			return cc;
		}

		total += cc;
		buf = ((const char *)buf) + cc;
		len -= cc;
	}

	return total;
}
