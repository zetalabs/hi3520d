/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/worker_upnp_gena.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-29   0.1       Write it from scratch
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

void ezcfg_worker_process_upnp_gena_new_connection(struct ezcfg_worker *worker)
{
	char *buf;
	int buf_len;
	struct ezcfg *ezcfg;
	struct ezcfg_upnp_gena *upnp_gena;

	ASSERT(worker != NULL);

	upnp_gena = (struct ezcfg_upnp_gena *)(ezcfg_worker_get_proto_data(worker));
	ASSERT(upnp_gena != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);
	buf = ezcfg_socket_get_buffer(ezcfg_worker_get_socket(worker));
	if (buf == NULL) {
		err(ezcfg, "not enough memory for processing upnp_gena new connection\n");
		return;
	}
	buf_len = ezcfg_socket_get_buffer_len(ezcfg_worker_get_socket(worker));

	info(ezcfg, "upnp_gena=[%s], len=%d\n", buf, buf_len);
	return;
}
