/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : util/util_javascript_var_escaped.c
 *
 * Description  : replace var text escaped char in javascript
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-01-19   0.1       Write it from scratch
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

#include "ezcfg.h"
#include "ezcfg-private.h"

#define IS_ESCAPED_CHAR(c)	(((c) == '\\') || ((c) == '\''))

static int var_escaped_length(const char *s)
{
	int len;

	len = 0;
	while(*s != '\0') {
		if (IS_ESCAPED_CHAR(*s)) len++;
		s++;
		len++;
	}

	/* one more for '\0' */
	len++;

	return len;
}

char *ezcfg_util_javascript_var_escaped(const char *src)
{
	int len;
	char *dst;
	char *p;

	if (src == NULL) {
		return NULL;
	}

	len = var_escaped_length(src);
	if (len < 1) {
		return NULL;
	}

	dst = malloc(len);
	if (dst == NULL) {
		return NULL;
	}

	p = dst;
	while(*src != '\0') {
		if (IS_ESCAPED_CHAR(*src)) {
			*p = '\\';
			p++;
		}
		*p++ = *src++;
	}

	/* 0-terminated string */
	*p = '\0';

	return dst;
}
