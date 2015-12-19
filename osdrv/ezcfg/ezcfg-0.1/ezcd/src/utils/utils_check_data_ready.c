/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_check_data_ready.c
 *
 * Description  : ezbox check data partition is ready
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2013-05-23   0.1       Write it from scratch
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

bool utils_data_is_ready(void)
{
	char buf[256];
	char data_dev[64];
	char data_mount_entry[128];
	FILE *fp;
	int ret;

	/* get boot partition device name */
	ret = utils_get_data_device_path(data_dev, sizeof(data_dev));
	if (ret < 1) {
		return (false);
	}

	/* get mounts info from /proc/mounts */
	fp = fopen("/proc/mounts", "r");
	if (fp == NULL) {
		return (false);
	}

	ret = -1;
	snprintf(data_mount_entry, sizeof(data_mount_entry), "/dev/%s /data ", data_dev);

	while (utils_file_get_line(fp, buf, sizeof(buf), "", LINE_TAIL_STRING) == true) {
		if (strncmp(buf, data_mount_entry, strlen(data_mount_entry)) == 0) {
			ret = 0;
			break;
		}
	}

	fclose(fp);

	if (ret != 0) {
		return false;
	}

	return true;
}

