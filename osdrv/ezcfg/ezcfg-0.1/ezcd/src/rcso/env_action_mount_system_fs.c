/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : env_action_mount_system_fs.c
 *
 * Description  : ezbox env agent mount system supported fs
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-08-07   0.1       Write it from scratch
 * 2011-10-20   0.2       Modify it to use rcso framework
 * 2012-12-25   0.3       Modify it to use agent action framework
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

static bool support_fs(char *fs)
{
	FILE *file = NULL;
	char buf[64];
	char *p;
	bool ret = false;

	file = fopen("/proc/filesystems", "r");
	if (file == NULL)
		return ret;

	while (utils_file_get_line(file,
		buf, sizeof(buf), "#", LINE_TAIL_STRING) == true) {
		p = strchr(buf, '\t');
		if (p != NULL) {
			p++;
			if (strcmp(fs, p) == 0) {
				ret = true;
				break;
			}
		}
	}

	fclose(file);
	return ret;
}

static bool fs_not_mounted(char *fs)
{
	FILE *file = NULL;
	char buf[64];
	char *p;
	bool ret = true;

	file = fopen("/proc/mounts", "r");
	if (file == NULL)
		return ret;

	while (utils_file_get_line(file,
		buf, sizeof(buf), "#", LINE_TAIL_STRING) == true) {
		p = strchr(buf, ' ');
		if (p != NULL) {
			*p = '\0';
			if (strcmp(fs, buf) == 0) {
				ret = false;
				break;
			}
		}
	}

	fclose(file);
	return ret;
}

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int env_action_mount_system_fs(int argc, char **argv)
#endif
{
	int flag, ret;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "mount_system_fs")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_BOOT :
	case RC_ACT_START :
		if ((support_fs("debugfs") == true) &&
		    (fs_not_mounted("debugfs") == true)) {
			mount("debugfs", "/sys/kernel/debug", "debugfs", MS_MGC_VAL, NULL);
		}

		if ((support_fs("usbfs") == true) &&
		    (fs_not_mounted("usbfs") == true)) {
			mount("none", "/proc/bus/usb", "usbfs", MS_MGC_VAL, NULL);
		}

#if (HAVE_EZBOX_EZCFG_CGROUP == 1)
#if 0
		if ((support_fs("cgroup") == true) &&
		    (fs_not_mounted("cgroups") == true)) {
			mount("cgroups", "/sys/fs/cgroup", "tmpfs", MS_MGC_VAL, NULL);
		}
#else
		if ((support_fs("cgroup") == true) &&
		    (fs_not_mounted("cgroup") == true)) {
			mount("cgroup", "/sys/fs/cgroup", "cgroup", MS_MGC_VAL, NULL);
		}
#endif
#endif

		ret = EXIT_SUCCESS;
		break;

	default:
		ret = EXIT_FAILURE;
		break;
	}

	return (ret);
}
