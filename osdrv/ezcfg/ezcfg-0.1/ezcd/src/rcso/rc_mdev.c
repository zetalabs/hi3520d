/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_mdev.c
 *
 * Description  : ezbox run mdev to generate /dev/ node service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-08-11   0.1       Write it from scratch
 * 2011-10-16   0.2       Modify it to use rcso framework
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
	FILE *fp = fopen("/dev/kmsg", "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_mdev(int argc, char **argv)
#endif
{
	FILE *file;
	char cmdline[256];
	int flag, ret;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "mdev")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_BOOT :
	case RC_ACT_START :
	case RC_ACT_RESTART :
		pop_etc_mdev_conf(flag);
		file = fopen("/proc/sys/kernel/hotplug", "w");
                if (file != NULL)
		{
			fprintf(file, "%s", CMD_MDEV);
			fclose(file);
		}

		if (flag == RC_ACT_BOOT) {
			snprintf(cmdline, sizeof(cmdline), "%s -s", CMD_MDEV);
			ret = utils_system(cmdline);
		}
		ret = EXIT_SUCCESS;
		break;

	case RC_ACT_STOP :
		file = fopen("/proc/sys/kernel/hotplug", "w");
                if (file != NULL)
		{
			fprintf(file, "%s", "");
			fclose(file);
		}
		ret = EXIT_SUCCESS;
		break;

	default:
		ret = EXIT_FAILURE;
		break;
	}

	return (ret);
}
