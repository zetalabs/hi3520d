/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_exec.c
 *
 * Description  : implement execute application rcso
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-10-18   0.1       Write it from scratch
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
	FILE *dbg2_fp = fopen("/dev/kmsg", "a"); \
	if (fp) { \
		char buf[32]; \
		FILE *dbg2_fp2; \
		int i; \
		for(i=pid; i<pid+30; i++) { \
			snprintf(buf, sizeof(buf), "/proc/%d/stat", i); \
			dbg2_fp2 = fopen(buf, "r"); \
			if (dbg2_fp2) { \
				if (fgets(buf, sizeof(buf)-1, dbg2_fp2) != NULL) { \
					fprintf(dbg2_fp, "pid=[%d] buf=%s\n", i, buf); \
				} \
				fclose(dbg2_fp2); \
			} \
		} \
		fclose(dbg2_fp); \
	} \
} while(0)

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_exec(int argc, char **argv)
#endif
{
	char path[128];
	struct stat stat_buf;
	int ret;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "exec")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	/* check first place */
	snprintf(path, sizeof(path), "%s/%s", EXEC_PATH_PREFIX, argv[1]);
	if ((stat(path, &stat_buf) != 0) ||
	    (S_ISREG(stat_buf.st_mode) == 0 && S_ISLNK(stat_buf.st_mode))) {
		/* check second place */
		snprintf(path, sizeof(path), "%s/%s", EXEC_PATH_PREFIX2, argv[1]);
		if ((stat(path, &stat_buf) != 0) ||
	    	(S_ISREG(stat_buf.st_mode) == 0 && S_ISLNK(stat_buf.st_mode))) {
			return (EXIT_FAILURE);
		}
	}

	ret = execv(path, argv + 1);
	if (ret  == -1)
		return (EXIT_FAILURE);

	return (EXIT_SUCCESS);
}
