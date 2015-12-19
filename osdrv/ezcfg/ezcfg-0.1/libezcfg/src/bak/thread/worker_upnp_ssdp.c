/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/worker_upnp_ssdp.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-13   0.1       Split it from worker.c
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
#include <time.h>
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

/**
 * Private functions
 **/
static bool check_valid_msearch_request(struct ezcfg_http *http)
{
	char *p;

	/* check HOST: */
	p = ezcfg_http_get_header_value(http, EZCFG_UPNP_HTTP_HEADER_HOST);
	if (p == NULL) {
		return false;
	}
	if (strcmp(p, EZCFG_PROTO_UPNP_SSDP_MCAST_IPADDR_STRING \
		":" \
		EZCFG_PROTO_UPNP_SSDP_PORT_NUMBER_STRING) != 0) {
		return false;
	}

	/* check MAN: */
	p = ezcfg_http_get_header_value(http, EZCFG_UPNP_HTTP_HEADER_MAN);
	if (p == NULL) {
		return false;
	}
	if (strcmp(p, "\"ssdp:discover\"") != 0) {
		return false;
	}

	/* check MX: */
	p = ezcfg_http_get_header_value(http, EZCFG_UPNP_HTTP_HEADER_MX);
	if (p == NULL) {
		return false;
	}

	/* check ST: */
	p = ezcfg_http_get_header_value(http, EZCFG_UPNP_HTTP_HEADER_ST);
	if (p == NULL) {
		return false;
	}

	return true;
}

/**
 * Public functions
 **/
void ezcfg_worker_process_upnp_ssdp_new_connection(struct ezcfg_worker *worker)
{
	char *buf;
	int len;
	struct ezcfg *ezcfg;
	struct ezcfg_socket *sp;
	struct ezcfg_upnp_ssdp *ssdp;
	struct ezcfg_upnp *upnp;
	struct ezcfg_http *http;
	struct ezcfg_master *master;

	ASSERT(worker != NULL);

	ssdp = (struct ezcfg_upnp_ssdp *)(ezcfg_worker_get_proto_data(worker));
	ASSERT(ssdp != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);
	master = ezcfg_worker_get_master(worker);

	sp = ezcfg_worker_get_socket(worker);
	buf = ezcfg_socket_get_buffer(sp);
	if (buf == NULL) {
		err(ezcfg, "not enough memory for processing ssdp new connection\n");
		return;
	}
	len = ezcfg_socket_get_buffer_len(sp);
	http = ezcfg_upnp_ssdp_get_http(ssdp);
	ezcfg_http_set_state_request(http);
	/* FIXME: socket->buffer will mangle by ezcfg_http_parse_header() */
	if (ezcfg_http_parse_header(http, buf, len) == false) {
		err(ezcfg, "SSDP packet format error.\n");
		return;
	}

	/* check valid SSDP M-SEARCH packet */
	if (check_valid_msearch_request(http) == true) {
		int mx;
		struct timespec ts;
		mx = atoi(ezcfg_http_get_header_value(http, EZCFG_UPNP_HTTP_HEADER_MX));
		if (mx > 0) {
			ts.tv_sec = random() % mx;
			ts.tv_nsec = random() % 1000*1000*1000;
			nanosleep(&ts, NULL);
		}
		if (ezcfg_master_upnp_mutex_lock(master) == 0) {
			upnp = ezcfg_master_get_upnp(master);
			if (upnp != NULL) {
				ezcfg_upnp_ssdp_set_upnp(ssdp, upnp);
				ezcfg_upnp_ssdp_msearch_response(ssdp, sp);
			}
			ezcfg_master_upnp_mutex_unlock(master);
		}
		return;
	}
}
