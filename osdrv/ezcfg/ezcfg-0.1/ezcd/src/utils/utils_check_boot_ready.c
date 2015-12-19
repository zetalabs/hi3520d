/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_check_boot_ready.c
 *
 * Description  : ezbox check boot partition is ready
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2013-05-20   0.1       Write it from scratch
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

bool utils_boot_is_ready(void)
{
	char name[64];
	char buf[256];
	char boot_dev[64];
	struct stat stat_buf;
	char root_path[64];
	int ret;

	/* get boot partition device name */
	ret = utils_get_boot_device_path(boot_dev, sizeof(boot_dev));
	if (ret < 1) {
		DBG("%s(%d) utils_get_boot_device_path() error!\n", __func__, __LINE__);
		return (false);
	}

	snprintf(buf, sizeof(buf), "/dev/%s", boot_dev);
	if (utils_partition_is_mounted(buf, "/boot") == false) {
		DBG("%s(%d) utils_partition_is_mounted(%s, /boot) error!\n", __func__, __LINE__, buf);
		return false;
	}

	/* check ezbox_boot.cfg is OK */
	if ((stat(BOOT_CONFIG_FILE_PATH, &stat_buf) != 0) ||
	    (!S_ISREG(stat_buf.st_mode))) {
		DBG("%s(%d) %s is not OK!\n", __func__, __LINE__, BOOT_CONFIG_FILE_PATH);
		return false;
	}

	/* root_path */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_ROOT_PATH);
	ret = utils_get_bootcfg_keyword(name, root_path, sizeof(root_path));
	if (ret < 1) {
		DBG("%s(%d) utils_get_bootcfg_keyword(%s) error!\n", __func__, __LINE__, name);
		return false;
	}

	/* sem_ezcfg_path */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SEM_EZCFG_PATH);
	ret = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	if ((ret < 1) || (strncmp(buf, root_path, strlen(root_path)) != 0)) {
		DBG("%s(%d) utils_get_bootcfg_keyword(%s)=[%s] error!\n", __func__, __LINE__, name, buf);
		return false;
	}

	/* shm_ezcfg_path */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SHM_EZCFG_PATH);
	ret = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	if ((ret < 1) || (strncmp(buf, root_path, strlen(root_path)) != 0)) {
		DBG("%s(%d) utils_get_bootcfg_keyword(%s)=[%s] error!\n", __func__, __LINE__, name, buf);
		return false;
	}

	/* sock_ctrl_path */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SOCK_CTRL_PATH);
	ret = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	if (ret < 1) {
		DBG("%s(%d) utils_get_bootcfg_keyword(%s)=[%s] error!\n", __func__, __LINE__, name, buf);
		return false;
	}

	/* sock_nvram_path */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SOCK_NVRAM_PATH);
	ret = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	if (ret < 1) {
		DBG("%s(%d) utils_get_bootcfg_keyword(%s)=[%s] error!\n", __func__, __LINE__, name, buf);
		return false;
	}

	/* sock_master_path */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SOCK_MASTER_PATH);
	ret = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	if (ret < 1) {
		DBG("%s(%d) utils_get_bootcfg_keyword(%s)=[%s] error!\n", __func__, __LINE__, name, buf);
		return false;
	}

	return true;
}

