/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : env_action_data_storage.c
 *
 * Description  : ezbox env agent prepares dynamic data storage
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-07-28   0.1       Write it from scratch
 * 2011-10-20   0.2       Modify it to use rcso framework
 * 2012-08-06   0.3       Split partition mount/umount to a single action.
 * 2012-12-25   0.4       Modify it to use agent action framework
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
#include "utils.h"

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

#define DBG2() do {\
	pid_t pid = getpid(); \
	FILE *fp = fopen("/dev/kmsg", "a"); \
	if (fp) { \
		char buf[32]; \
		FILE *fp2; \
		int i; \
		for(i=pid; i<pid+30; i++) { \
			snprintf(buf, sizeof(buf), "/proc/%d/stat", i); \
			fp2 = fopen(buf, "r"); \
			if (fp2) { \
				if (fgets(buf, sizeof(buf)-1, fp2) != NULL) { \
					fprintf(fp, "pid=[%d] buf=%s\n", i, buf); \
				} \
				fclose(fp2); \
			} \
		} \
		fclose(fp); \
	} \
} while(0)

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int env_action_data_storage(int argc, char **argv)
#endif
{
	char buf[64];
	int flag, ret;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "data_storage")) {
		return (EXIT_FAILURE);
	}

	if (utils_data_partition_is_ready() == false) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_BOOT :
		/* FIXME: warning data partition must be mounted as writable before this */
		snprintf(buf, sizeof(buf), "%s a+rwx %s", CMD_CHMOD, DATA_ROOTFS_VAR_DIR_PATH);
		utils_system(buf);
		snprintf(buf, sizeof(buf), "%s -rf %s/lock", CMD_RM, DATA_ROOTFS_VAR_DIR_PATH);
		utils_system(buf);
		snprintf(buf, sizeof(buf), "%s -rf %s/run", CMD_RM, DATA_ROOTFS_VAR_DIR_PATH);
		utils_system(buf);
		snprintf(buf, sizeof(buf), "%s -rf %s/tmp", CMD_RM, DATA_ROOTFS_VAR_DIR_PATH);
		utils_system(buf);
		mkdir(DATA_ROOTFS_VAR_DIR_PATH "/lock", 0777);
		mkdir(DATA_ROOTFS_VAR_DIR_PATH "/log", 0777);
		mkdir(DATA_ROOTFS_VAR_DIR_PATH "/run", 0777);
		mkdir(DATA_ROOTFS_VAR_DIR_PATH "/tmp", 0777);

		/* some useful directories */
		mkdir(DATA_ROOTFS_VAR_DIR_PATH "/lib", 0777);
		mkdir(DATA_ROOTFS_VAR_DIR_PATH "/lib/misc", 0777);

		ret = EXIT_SUCCESS;
		break;

	default:
		ret = EXIT_FAILURE;
		break;
	}

	return (ret);
}
