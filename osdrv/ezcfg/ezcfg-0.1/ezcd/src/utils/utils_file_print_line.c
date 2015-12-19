/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_file_print_line.c
 *
 * Description  : print a line to file
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-01-08   0.1       Write it from scratch
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

bool utils_file_print_line(FILE *fp, char *buf, size_t size, const char *fmt, ...)
{
	va_list args;

	if ((fp == NULL) || (buf == NULL) || (size < 1)) {
		return false;
	}

	va_start(args, fmt);
	vsnprintf(buf, size, fmt, args);
	va_end(args);
	va_start(args, fmt);
	fprintf(fp, "%s", buf);
	va_end(args);

	return true;
}
