/* ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : api-uuid.c
 *
 * Description  : ezcfg API for ezcfg UUID manipulate
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-01-04   0.1       Write it from scratch
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

static bool debug = false;

static void log_fn(struct ezcfg *ezcfg, int priority,
                   const char *file, int line, const char *fn,
                   const char *format, va_list args)
{
        if (debug) {
		char buf[1024];
		struct timeval tv;
		struct timezone tz;

		vsnprintf(buf, sizeof(buf), format, args);
		gettimeofday(&tv, &tz);
		fprintf(stderr, "%llu.%06u [%u] %s(%d): %s",
			(unsigned long long) tv.tv_sec, (unsigned int) tv.tv_usec,
			(int) getpid(), fn, line, buf);
	}
#if 0
	else {
		vsyslog(priority, format, args);
	}
#endif
}

/**
 * ezcfg_api_uuid_string:
 *
 **/
int ezcfg_api_uuid1_string(char *str, int len)
{
	int rc = 0;
	struct ezcfg *ezcfg = NULL;
	struct ezcfg_uuid *uuid = NULL;

	if ((str == NULL) || (len < EZCFG_UUID_STRING_LEN+1)) {
		return -EZCFG_E_ARGUMENT ;
	}

	ezcfg = ezcfg_new(ezcfg_api_common_get_config_file());
	if (ezcfg == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	ezcfg_log_init("uuid_string");
	ezcfg_common_set_log_fn(ezcfg, log_fn);

	uuid = ezcfg_uuid_new(ezcfg, 1);
	if (uuid == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	if (ezcfg_uuid_generate(uuid) == false) {
		rc = -EZCFG_E_RESOURCE;
		goto exit;
	}

	if (ezcfg_uuid_export_str(uuid, str, len) == false) {
		rc = -EZCFG_E_RESOURCE;
		goto exit;
	}

	/* mark rc success */
	rc = EZCFG_UUID_STRING_LEN;
exit:
	if (uuid != NULL) {
		ezcfg_uuid_delete(uuid);
	}

	if (ezcfg != NULL) {
		ezcfg_delete(ezcfg);
	}

	return rc;
}
