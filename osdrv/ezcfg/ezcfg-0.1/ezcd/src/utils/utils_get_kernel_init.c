/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_get_kernel_init.c
 *
 * Description  : ezcfg get kernel "init=" parameter function
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2013-07-22   0.1       Write it from scratch
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
/*
 * Returns kernel "init=" string
 */
int utils_get_kernel_init(char *buf, int buf_len)
{
	int rc = -1;
	char *p;
	p = utils_file_get_keyword("/proc/cmdline", "init=");
	if (p != NULL) {
		rc = snprintf(buf, buf_len, "%s", p);
		free(p);
	}
	return rc;
}
