/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : env_action_overlay_rootfs.c
 *
 * Description  : ezbox env agent prepares overlaying extend system on core one.
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-06-14   0.1       Write it from scratch
 * 2012-12-25   0.2       Modify it to use agent action framework
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
int env_action_overlay_rootfs(int argc, char **argv)
#endif
{
	char buf[FILE_LINE_BUFFER_SIZE];
	struct stat stat_buf;
	int ret;
	int flag = RC_ACT_UNKNOWN;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "overlay_rootfs")) {
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
		/* first cleanup duplicated files, release some memory */
		/* if data partition does not mount, the config file does not exist */
		utils_cleanup_rootfs(DATA_ROOTFS_DIR_PATH ROOTFS_CONFIG_FILE_PATH);

		/* overlay /bin */
		if (stat(DATA_ROOTFS_BIN_DIR_PATH, &stat_buf) == 0) {
			if (S_ISDIR(stat_buf.st_mode)) {
				snprintf(buf, sizeof(buf), "-o lowerdir=%s,upperdir=%s", "/bin", DATA_ROOTFS_BIN_DIR_PATH);
				utils_mount_partition("overlayfs", "/bin", "overlayfs", buf);
			}
		}

		/* overlay /lib */
		if (stat(DATA_ROOTFS_LIB_DIR_PATH, &stat_buf) == 0) {
			if (S_ISDIR(stat_buf.st_mode)) {
				snprintf(buf, sizeof(buf), "-o lowerdir=%s,upperdir=%s", "/lib", DATA_ROOTFS_LIB_DIR_PATH);
				utils_mount_partition("overlayfs", "/lib", "overlayfs", buf);
			}
		}

		/* overlay /sbin */
		if (stat(DATA_ROOTFS_SBIN_DIR_PATH, &stat_buf) == 0) {
			if (S_ISDIR(stat_buf.st_mode)) {
				snprintf(buf, sizeof(buf), "-o lowerdir=%s,upperdir=%s", "/sbin", DATA_ROOTFS_SBIN_DIR_PATH);
				utils_mount_partition("overlayfs", "/sbin", "overlayfs", buf);
			}
		}

		/* overlay /usr */
		if (stat(DATA_ROOTFS_USR_DIR_PATH, &stat_buf) == 0) {
			if (S_ISDIR(stat_buf.st_mode)) {
				snprintf(buf, sizeof(buf), "-o lowerdir=%s,upperdir=%s", "/usr", DATA_ROOTFS_USR_DIR_PATH);
				utils_mount_partition("overlayfs", "/usr", "overlayfs", buf);
			}
		}

		/* overlay /var */
		if (stat(DATA_ROOTFS_VAR_DIR_PATH, &stat_buf) == 0) {
			if (S_ISDIR(stat_buf.st_mode)) {
				snprintf(buf, sizeof(buf), "-o lowerdir=%s,upperdir=%s", "/var", DATA_ROOTFS_VAR_DIR_PATH);
				utils_mount_partition("overlayfs", "/var", "overlayfs", buf);
			}
		}

		/* overlay /home */
		if (stat(DATA_ROOTFS_HOME_DIR_PATH, &stat_buf) == 0) {
			if (S_ISDIR(stat_buf.st_mode)) {
				snprintf(buf, sizeof(buf), "-o lowerdir=%s,upperdir=%s", "/home", DATA_ROOTFS_HOME_DIR_PATH);
				utils_mount_partition("overlayfs", "/home", "overlayfs", buf);
			}
		}

		/* overlay /root */
		if ((strncmp(ROOT_HOME_PATH, "/home/", 6) != 0) &&
		    (stat(DATA_ROOTFS_HOME_DIR_PATH, &stat_buf) == 0)) {
			if (S_ISDIR(stat_buf.st_mode)) {
				snprintf(buf, sizeof(buf), "-o lowerdir=%s,upperdir=%s", ROOT_HOME_PATH, DATA_ROOTFS_ROOT_DIR_PATH);
				utils_mount_partition("overlayfs", ROOT_HOME_PATH, "overlayfs", buf);
			}
		}

		ret = EXIT_SUCCESS;
		break;

	case RC_ACT_STOP :
		/* overlay /bin */
		if (stat(DATA_ROOTFS_BIN_DIR_PATH, &stat_buf) == 0) {
			if (S_ISDIR(stat_buf.st_mode)) {
				utils_umount_partition("/bin");
			}
		}

		/* overlay /lib */
		if (stat(DATA_ROOTFS_LIB_DIR_PATH, &stat_buf) == 0) {
			if (S_ISDIR(stat_buf.st_mode)) {
				utils_umount_partition("/lib");
			}
		}

		/* overlay /sbin */
		if (stat(DATA_ROOTFS_SBIN_DIR_PATH, &stat_buf) == 0) {
			if (S_ISDIR(stat_buf.st_mode)) {
				utils_umount_partition("/sbin");
			}
		}

		/* overlay /usr */
		if (stat(DATA_ROOTFS_USR_DIR_PATH, &stat_buf) == 0) {
			if (S_ISDIR(stat_buf.st_mode)) {
				utils_umount_partition("/usr");
			}
		}

		/* overlay /var */
		if (stat(DATA_ROOTFS_VAR_DIR_PATH, &stat_buf) == 0) {
			if (S_ISDIR(stat_buf.st_mode)) {
				utils_umount_partition("/var");
			}
		}

		/* overlay /home */
		if (stat(DATA_ROOTFS_HOME_DIR_PATH, &stat_buf) == 0) {
			if (S_ISDIR(stat_buf.st_mode)) {
				utils_umount_partition("/home");
			}
		}

		/* overlay /root */
		if ((strncmp(ROOT_HOME_PATH, "/home/", 6) != 0) &&
		    (stat(DATA_ROOTFS_HOME_DIR_PATH, &stat_buf) == 0)) {
			if (S_ISDIR(stat_buf.st_mode)) {
				utils_umount_partition(ROOT_HOME_PATH);
			}
		}

		ret = EXIT_SUCCESS;
		break;

	default:
		ret = EXIT_FAILURE;
		break;
	}

	return (ret);
}
