/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/master_uevent.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-01   0.1       Split it from master.c
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
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <pthread.h>

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

bool ezcfg_master_handle_uevent_socket(
	struct ezcfg_master *master,
	struct ezcfg_socket *listener,
	struct ezcfg_socket *accepted)
{
	struct ezcfg *ezcfg;
	char buf[EZCFG_UEVENT_MAX_MESSAGE_SIZE];
	int len;

	ASSERT(master != NULL);
	ASSERT(listener != NULL);
	ASSERT(accepted != NULL);

	ezcfg = ezcfg_master_get_ezcfg(master);

	len = ezcfg_socket_read(listener, buf, sizeof(buf), 0);
	if (len > 0) {
		buf[len - 1] = '\0';
		if (ezcfg_socket_set_buffer(accepted, buf, len) == false) {
			err(ezcfg, "set UEVENT socket buffer error.\n");
			return false;
		}
	}
	else {
		err(ezcfg, "read UEVENT socket error.\n");
		return false;
	}

	return true;
}
