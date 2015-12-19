/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ctrl/ctrl_ipc.c
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

int ezcfg_ctrl_handle_ipc_message(char **argv, char *output, int len, void *rte)
{
	struct ezcfg_worker *worker;
	struct ezcfg_master *master;
	int rc = 0;

	if (argv == NULL || argv[0] == NULL) {
		return -1;
	}

	if (strcmp(argv[0], "ipc") != 0) {
		return -1;
	}

	if (argv[1] == NULL) {
		return -1;
	}

	worker = rte;
	master = ezcfg_worker_get_master(worker);

	if (strcmp(argv[1], "get") == 0) {
		if (argv[2] == NULL) {
			return -1;
		}

		if (strcmp(argv[2], "shm_id") == 0) {
			rc = snprintf(output, len, "%d", ezcfg_master_get_shm_id(master));
			if (rc >= len) {
				return -1;
			}
		}

		return rc;
	}
	return -1;
}
