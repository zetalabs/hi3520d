/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : util/util_file_extension.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-01-16   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#if 1
#define DBG(format, args...) do { \
	char path[256]; \
	FILE *fp; \
	snprintf(path, 256, "/tmp/%d-debug.txt", getpid()); \
	fp = fopen(path, "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

struct file_extension_pair {
	int index;
	char *name;
};

static struct file_extension_pair ezcfg_support_file_extensions[] = {
	{ EZCFG_FILE_EXT_UNKNOWN, NULL },
	{ EZCFG_FILE_EXT_SHTM, EZCFG_FILE_EXT_SHTM_STRING },
	{ EZCFG_FILE_EXT_JS, EZCFG_FILE_EXT_JS_STRING },
};

int ezcfg_util_file_get_type_extension_index(const char *path)
{
	size_t i;
	struct file_extension_pair *p;
	for (i = 1; i < ARRAY_SIZE(ezcfg_support_file_extensions); i++) {
		p = &(ezcfg_support_file_extensions[i]);
		if (strstr(path, p->name) != NULL)
			return p->index;
	}
	return EZCFG_FILE_EXT_UNKNOWN;
}

