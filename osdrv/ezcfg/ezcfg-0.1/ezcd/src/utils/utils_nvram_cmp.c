/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_check_ezcd.c
 *
 * Description  : ezbox check ezcfg daemon service status
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-09   0.1       Write it from scratch
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

int utils_nvram_cmp(const char *name, const char *value)
{
	int rc = 0;
	char *buf;
	int buf_len;

	if (name == NULL || value == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	buf_len = EZCFG_NVRAM_BUFFER_SIZE ;
	buf = (char *)malloc(buf_len);
	if (buf == NULL) {
		return -EZCFG_E_SPACE ;
	}

	rc = ezcfg_api_nvram_get(name, buf, buf_len);
	if (rc < 0) {
		rc = -EZCFG_E_RESULT ;
		goto exit;
	}

	if (strcmp(value, buf) != 0) {
		rc = -EZCFG_E_RESULT ;
		goto exit;
	}
exit:
	if (buf != NULL)
		free(buf);
	return rc;
}

