/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_switch_root.c
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
#include <sys/vfs.h>
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

#ifndef RAMFS_MAGIC
# define RAMFS_MAGIC ((unsigned)0x858458f6)
#endif
#ifndef TMPFS_MAGIC
# define TMPFS_MAGIC ((unsigned)0x01021994)
#endif

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

bool utils_switch_root_is_ready(char *init_path)
{
	char buf[32];
	struct stat st;
	struct statfs stfs;
	int ret;

	if (getpid() != 1) {
		DBG("huedebug %s(%d) pid=[%d]\n", __func__, __LINE__, getpid());
		return (false);
	}

	if (init_path == NULL) {
		DBG("huedebug %s(%d) init_path is NULL!\n", __func__, __LINE__);
		return (false);
	}

	ret = utils_get_root_device_path(buf, sizeof(buf));
	if (ret < 3) {
		DBG("huedebug %s(%d) pid=[%d]\n", __func__, __LINE__, getpid());
		return (false);
	}

	if (stat(init_path, &st) != 0 || !S_ISREG(st.st_mode)) {
		DBG("huedebug %s(%d) pid=[%d]\n", __func__, __LINE__, getpid());
		return (false);
	}

	statfs("/", &stfs);
	if (((unsigned)stfs.f_type != RAMFS_MAGIC) &&
	    ((unsigned)stfs.f_type != TMPFS_MAGIC)) {
		DBG("huedebug %s(%d) pid=[%d]\n", __func__, __LINE__, getpid());
		return (false);
	}

	ret = utils_mount_root_partition_writable();
	if (ret != EXIT_SUCCESS) {
		DBG("huedebug %s(%d) pid=[%d]\n", __func__, __LINE__, getpid());
		return (false);
	}

	return (true);
}

int utils_switch_root_device(char *init_path)
{
	int ret;
	char *argv[] = { CMD_SWITCH_ROOT, "/root", init_path, NULL };

	DBG("huedebug %s(%d) pid=[%d]\n", __func__, __LINE__, getpid());

	if (init_path == NULL) {
		DBG("huedebug %s(%d) init_path is NULL!\n", __func__, __LINE__);
		return (EXIT_FAILURE);
	}

	ret = execv(argv[0], argv);

	/* should never return if switch_root succeed */
	if (ret == -1) {
		DBG("huedebug %s(%d) pid=[%d] ret=[%d] errno=[%d]\n", __func__, __LINE__, getpid(), ret, errno);
	}
	DBG("huedebug %s(%d) pid=[%d] ret=[%d]\n", __func__, __LINE__, getpid(), ret);
	return (EXIT_FAILURE);
}

