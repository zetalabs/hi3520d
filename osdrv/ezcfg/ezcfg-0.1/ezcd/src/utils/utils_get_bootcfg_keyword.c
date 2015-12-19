/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_get_bootcfg_keyword.c
 *
 * Description  : ezcfg get keyword from ezbox_boot.cfg file function
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-07-27   0.1       Write it from scratch
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
	FILE *dbg_fp = fopen("/dev/kmsg", "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

static int get_bootcfg_keyword(char *name, char *buf, int buf_len, int full_flag)
{
	int len, ret=-1;
	struct stat stat_buf;
	char *keyword=NULL, *value=NULL;

	DBG("%s(%d) name=[%s] buf==NULL[%d] buf_len=[%d]\n", __func__, __LINE__, name, (buf == NULL), buf_len);

	if ((name == NULL) || (buf == NULL) || (buf_len < 1))
		return -1;

	len = strlen(name) + 2;
	keyword = malloc(len);
	if (keyword == NULL)
		return -1;

	snprintf(keyword, len, "%s=", name);
	DBG("%s(%d) keyword=[%s] BOOT_CONFIG_FILE_PATH=[%s]\n", __func__, __LINE__, keyword, BOOT_CONFIG_FILE_PATH);

	if ((stat(BOOT_CONFIG_FILE_PATH, &stat_buf) == 0) &&
	    (S_ISREG(stat_buf.st_mode))) {
		DBG("%s(%d)\n", __func__, __LINE__);
		/* get keyword's value from boot.cfg file */
		if (full_flag == 0)
			value = utils_file_get_keyword(BOOT_CONFIG_FILE_PATH, keyword);
		else
			value = utils_file_get_keyword_full(BOOT_CONFIG_FILE_PATH, keyword);
		DBG("%s(%d) value=[%s]\n", __func__, __LINE__, value);
		if (value != NULL) {
			ret = snprintf(buf, buf_len, "%s", value);
			free(value);
		}
	}
	free(keyword);
	return ret;
}

int utils_get_bootcfg_keyword(char *name, char *buf, int buf_len)
{
	return get_bootcfg_keyword(name, buf, buf_len, 0);
}

int utils_get_bootcfg_keyword_full(char *name, char *buf, int buf_len)
{
	return get_bootcfg_keyword(name, buf, buf_len, 1);
}
