/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_clean_dirs.c
 *
 * Description  : ezbox prepare basic directory structure
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2013-06-17   0.1       Write it from scratch
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

int utils_clean_preboot_dirs(void)
{
#if 0
	char buf[32];

	/* /proc */
	mkdir("/proc", 0555);
	mount("proc", "/proc", "proc", MS_MGC_VAL, NULL);

	/* sysfs -> /sys */
	mkdir("/sys", 0755);
	mount("sysfs", "/sys", "sysfs", MS_MGC_VAL, NULL);

	/* /dev */
	mkdir("/dev", 0755);
	mount("devfs", "/dev", "tmpfs", MS_MGC_VAL, NULL);

	/* init shms */
	mkdir("/dev/shm", 0777);

	/* mount /dev/pts */
	mkdir("/dev/pts", 0777);
	mount("devpts", "/dev/pts", "devpts", MS_MGC_VAL, NULL);

	mknod("/dev/console", S_IRUSR|S_IWUSR|S_IFCHR, makedev(5, 1));
	mknod("/dev/null", S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH|S_IFCHR, makedev(1, 3));
	mknod("/dev/kmsg", S_IRUSR|S_IWUSR|S_IFCHR, makedev(1, 11));

	/* /etc */
	mkdir("/etc", 0755);
	mount("tmpfs", "/etc", "tmpfs", MS_MGC_VAL, NULL);

	/* /boot */
	mkdir("/boot", 0777);

	/* /data */
	mkdir("/data", 0777);

	/* /var */
	mkdir("/var", 0777);
	mkdir("/var/lock", 0777);
	mkdir("/var/log", 0777);
	mkdir("/var/run", 0777);
	mkdir("/var/tmp", 0777);

	/* useful /var directories */
	mkdir("/var/lib", 0777);
	mkdir("/var/lib/misc", 0777);

	/* /tmp */
	snprintf(buf, sizeof(buf), "%s -rf /tmp", CMD_RM);
	utils_system(buf);
	if (symlink("/var/tmp", "/tmp") == -1) {
		DBG("%s-%s(%d)\n", __FILE__, __func__, __LINE__);
	}

	/* user's home directory */
	mkdir("/home", 0755);

	/* root's home directory */
	mkdir(ROOT_HOME_PATH, 0755);
#endif
	umount("/etc");
	umount("/dev/pts");
	umount("/dev");
	umount("/sys");
	umount("/proc");

	return (EXIT_SUCCESS);
}

