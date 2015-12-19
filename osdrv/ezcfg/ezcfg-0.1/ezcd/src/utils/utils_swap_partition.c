/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_swap_partition.c
 *
 * Description  : ezbox on/off swap partition
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2013-05-24   0.1       Write it from scratch
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

int utils_swap_partition_on(void)
{
	int ret;
	char swap_dev[64];
	char buf[128];

	ret = utils_get_swap_device_path(swap_dev, sizeof(swap_dev));
	if (ret < 1) {
		return (EXIT_FAILURE);
	}

	snprintf(buf, sizeof(buf), "%s /dev/%s", CMD_SWAPON, swap_dev);
	utils_system(buf);
	return (EXIT_SUCCESS);
}

int utils_swap_partition_off(void)
{
	int ret;
	char swap_dev[64];
	char buf[128];

	ret = utils_get_swap_device_path(swap_dev, sizeof(swap_dev));
	if (ret < 1) {
		return (EXIT_FAILURE);
	}

	snprintf(buf, sizeof(buf), "%s /dev/%s", CMD_SWAPOFF, swap_dev);
	utils_system(buf);
	return (EXIT_SUCCESS);
}

