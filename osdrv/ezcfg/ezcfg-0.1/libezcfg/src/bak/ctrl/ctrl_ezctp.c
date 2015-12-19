/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ctrl/ctrl_ezctp.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-03-08   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <pthread.h>
#include <sys/sysinfo.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#if 1
#define DBG(format, args...) do { \
	char path[256]; \
	FILE *fp; \
	snprintf(path, 256, "/tmp/%d-debug.txt", getpid()); \
	fp = fopen(path, "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

int ezcfg_ctrl_handle_ezctp_message(char **argv, char *output, int len, void *rte)
{
	//struct ezcfg_worker *worker;

	if (argv == NULL || argv[0] == NULL) {
		return -1;
	}

	if (strcmp(argv[0], "ezctp") != 0) {
		return -1;
	}

	if (argv[1] == NULL) {
		return -1;
	}

	//worker = rte;

	if (strcmp(argv[1], "mduser") == 0) {
		return 0;
	}
	else if (strcmp(argv[1], "trader") == 0) {
		return 0;
	}
	return -1;
}
