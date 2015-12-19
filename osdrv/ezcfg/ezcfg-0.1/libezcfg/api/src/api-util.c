/* ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : api-util.c
 *
 * Description  : ezcfg API for ezcfg common utilities
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-03-29   0.1       Write it from scratch
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
#include <sys/ipc.h>
#include <sys/sem.h>
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

#include "ezcfg-api.h"

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

/**
 * ezcfg_api_util_get_conf_string:
 *
 **/
int ezcfg_api_util_get_conf_string(const char *path,
	const char *section, const int idx, const char *keyword,
	char *buf, size_t len)
{
	int rc = 0;
	char *p = NULL;

	if ((path == NULL) || (section == NULL) ||
	    (idx < 0) || (keyword == NULL) ||
	    (buf == NULL) || (len < 1)) {
		return -EZCFG_E_ARGUMENT ;
	}

	p = ezcfg_util_get_conf_string(path, section, idx, keyword);
	if (p == NULL) {
		return -EZCFG_E_RESOURCE ;
	}

	rc = snprintf(buf, len, "%s", p);
	if ((rc < 0) || (rc >= (int)len)) {
		rc = -EZCFG_E_SPACE ;
	}
	free(p);
	return rc;
}
