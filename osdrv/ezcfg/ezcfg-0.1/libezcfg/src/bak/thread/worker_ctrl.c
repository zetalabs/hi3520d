/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/worker_ctrl.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-01   0.1       Write it from scratch
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

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-soap_http.h"

#if 0
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

static bool ctrl_error_handler(struct ezcfg_worker *worker)
{
	return false;
}

static void send_ctrl_error(struct ezcfg_worker *worker, const char *reason)
{
	bool handled;

	handled = ctrl_error_handler(worker);

	if (handled == false) {
		if (reason != NULL) {
			ezcfg_worker_printf(worker, "BAD %s", reason);
		}
		else {
			ezcfg_worker_printf(worker, "BAD");
		}
	}
}

void ezcfg_worker_process_ctrl_new_connection(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;
	struct ezcfg_ctrl *ctrl;
	char buf[EZCFG_CTRL_MAX_MESSAGE_SIZE];
	int len;

	ASSERT(worker != NULL);

	ctrl = (struct ezcfg_ctrl *)ezcfg_worker_get_proto_data(worker);
	ASSERT(ctrl != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);
	len = ezcfg_socket_read(ezcfg_worker_get_socket(worker), buf, sizeof(buf), 0);
	if (len > 0) {
		buf[len - 1] = '\0';
		if (ezcfg_ctrl_set_buffer(ctrl, buf, len) == false) {
			err(ezcfg, "set CTRL socket buffer error.\n");
			send_ctrl_error(worker, NULL);
			return;
                }
	}
	else {
		err(ezcfg, "read CTRL socket error.\n");
		send_ctrl_error(worker, NULL);
		return;
	}

	len = ezcfg_ctrl_handle_message(ctrl, buf, sizeof(buf), worker);
	if (len < 0) {
		err(ezcfg, "handle CTRL message error.\n");
		send_ctrl_error(worker, NULL);
		return;
	}
	else if (len == 0) {
		ezcfg_worker_printf(worker, "OK");
	}
	else {
		ezcfg_worker_printf(worker, "OK %s", buf);
	}
}
