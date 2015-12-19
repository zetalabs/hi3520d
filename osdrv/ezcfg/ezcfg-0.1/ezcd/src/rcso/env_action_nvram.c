/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : env_action_nvram.c
 *
 * Description  : ezbox env agent populates nvram command config file
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-24   0.1       Write it from scratch
 * 2011-10-21   0.2       Modify it to use rcso framework
 * 2012-09-06   0.3       Add user defined command support
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
#include "pop_func.h"

#if 0
#define DBG(format, args...) do {\
	FILE *dbg_fp = fopen("/dev/kmsg", "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, arg...)
#endif

static int nvram_user_defined(int argc, char **argv)
{
	if (argc < 1) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "set") == 0) {
		if (argc != 3)
			return (EXIT_FAILURE);

		if (ezcfg_api_nvram_set(argv[1], argv[2]) < 0)
			return (EXIT_FAILURE);
		else
			return (EXIT_SUCCESS);
	}
	else if (strcmp(argv[0], "unset") == 0) {
		if (argc != 2)
			return (EXIT_FAILURE);

		if (ezcfg_api_nvram_unset(argv[1]) < 0)
			return (EXIT_FAILURE);
		else
			return (EXIT_SUCCESS);
	}
	else {
		return (EXIT_FAILURE);
	}
}

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int env_action_nvram(int argc, char **argv)
#endif
{
	int flag, ret;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "nvram")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_BOOT :
#if 0
		/* generate nvram config file */
		pop_etc_nvram_conf(RC_ACT_BOOT);
#endif
		/* update nvram with ezbox_boot.cfg */
		DBG("%s(%d)!\n", __func__, __LINE__);
		ret = utils_sync_nvram_with_cfg(BOOT_CONFIG_FILE_PATH, NULL);
		DBG("%s(%d)!\n", __func__, __LINE__);
		break;

	case RC_ACT_RELOAD :
#if 0
		/* re-generate nvram config file */
		pop_etc_nvram_conf(RC_ACT_RELOAD);
#endif
		/* stop */
		/* first make /boot writable */
		utils_remount_boot_partition_writable();
		/* update ezbox_boot.cfg with nvram not including "sys." prefix */
		ret = utils_sync_cfg_with_nvram(BOOT_CONFIG_FILE_PATH, "!" EZCFG_SYS_NVRAM_PREFIX);
		/* make /boot read-only */
		utils_remount_boot_partition_readonly();

		/* start */
		break;

	case RC_ACT_STOP :
		/* first make /boot writable */
		utils_remount_boot_partition_writable();
#if 0
		/* remove ezbox_upgrade.cfg */
		unlink(UPGRADE_CONFIG_FILE_PATH);
#endif
		/* update ezbox_boot.cfg with nvram not including "sys." prefix */
		ret = utils_sync_cfg_with_nvram(BOOT_CONFIG_FILE_PATH, "!" EZCFG_SYS_NVRAM_PREFIX);
		/* make /boot read-only */
		utils_remount_boot_partition_readonly();
		break;

	case RC_ACT_USRDEF :
		ret = nvram_user_defined(argc-2, &(argv[2]));
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}
	return (ret);
}
