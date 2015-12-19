/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/worker_igrs.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-12   0.1       Split it from worker.c
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

static bool igrs_error_handler(struct ezcfg_worker *worker)
{
	return false;
}

static void send_igrs_error(struct ezcfg_worker *worker, int status,
                            const char *reason, const char *fmt, ...)
{
	char buf[EZCFG_BUFFER_SIZE];
	va_list ap;
	int len;
	bool handled;

	//ezcfg_http_set_status_code(worker->proto_data, status);
	handled = igrs_error_handler(worker);

	if (handled == false) {
		buf[0] = '\0';
		len = 0;

		/* Errors 1xx, 204 and 304 MUST NOT send a body */
		if (status > 199 && status != 204 && status != 304) {
			len = snprintf(buf, sizeof(buf),
			               "Error %d: %s\n", status, reason);
			va_start(ap, fmt);
			len += vsnprintf(buf + len, sizeof(buf) - len, fmt, ap);
			va_end(ap);
			ezcfg_worker_set_num_bytes_sent(worker, len);
		}
		ezcfg_worker_printf(worker,
		              "HTTP/1.1 %d %s\r\n"
		              "Content-Type: text/plain\r\n"
		              "Content-Length: %d\r\n"
		              "Connection: close\r\n"
		              "\r\n%s", status, reason, len, buf);
	}
}

static void handle_igrs_request(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;
	struct ezcfg_igrs *igrs;
	struct ezcfg_http *http;
	char *request_uri;
	char *msg = NULL;
	int msg_len;

	ASSERT(worker != NULL);

	igrs = (struct ezcfg_igrs *)ezcfg_worker_get_proto_data(worker);
	ASSERT(igrs != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);
	http = ezcfg_igrs_get_http(igrs);

	request_uri = ezcfg_http_get_request_uri(http);
	if (request_uri == NULL) {
		err(ezcfg, "no request uri for IGRS action.\n");

		/* clean http structure info */
		ezcfg_http_reset_attributes(http);
		ezcfg_http_set_status_code(http, 400);
		ezcfg_http_set_state_response(http);

		/* build IGRS error response */
		msg_len = ezcfg_igrs_http_get_message_length(igrs);
		if (msg_len < 0) {
			err(ezcfg, "ezcfg_igrs_get_message_length error.\n");
			goto exit;
		}
		msg_len++; /* one more for '\0' */
		msg = (char *)malloc(msg_len);
		if (msg == NULL) {
			err(ezcfg, "malloc msg error.\n");
			goto exit;
		}
		memset(msg, 0, msg_len);
		msg_len = ezcfg_igrs_http_write_message(igrs, msg, msg_len);
		ezcfg_worker_write(worker, msg, msg_len);
		goto exit;
	}

	if (ezcfg_igrs_handle_message(igrs) < 0) {
		/* clean http structure info */
		ezcfg_http_reset_attributes(http);
		ezcfg_http_set_status_code(http, 400);
		ezcfg_http_set_state_response(http);

		/* build IGRS error response */
		msg_len = ezcfg_igrs_http_get_message_length(igrs);
		if (msg_len < 0) {
			err(ezcfg, "ezcfg_igrs_get_message_length error.\n");
			goto exit;
		}
		msg_len++; /* one more for '\0' */
		msg = (char *)malloc(msg_len);
		if (msg == NULL) {
			err(ezcfg, "malloc msg error.\n");
			goto exit;
		}
		memset(msg, 0, msg_len);
		msg_len = ezcfg_igrs_http_write_message(igrs, msg, msg_len);
		ezcfg_worker_write(worker, msg, msg_len);
	}
	else {
		/* build IGRS response */
		ezcfg_igrs_build_message(igrs);
		msg_len = ezcfg_igrs_get_message_length(igrs);
		if (msg_len < 0) {
			err(ezcfg, "ezcfg_igrs_get_message_length error.\n");
			goto exit;
		}
		msg_len++; /* one more for '\0' */
		msg = (char *)malloc(msg_len);
		if (msg == NULL) {
			err(ezcfg, "malloc msg error.\n");
			goto exit;
		}
		memset(msg, 0, msg_len);
		msg_len = ezcfg_igrs_write_message(igrs, msg, msg_len);
		ezcfg_worker_write(worker, msg, msg_len);
	}
exit:
	if (msg != NULL)
		free(msg);
}

void ezcfg_worker_process_igrs_new_connection(struct ezcfg_worker *worker)
{
	int header_len, nread;
	char *buf;
	int buf_len;
	struct ezcfg *ezcfg;
	struct ezcfg_igrs *igrs;
	struct ezcfg_http *http;

	ASSERT(worker != NULL);

	igrs = (struct ezcfg_igrs *)ezcfg_worker_get_proto_data(worker);
	ASSERT(igrs != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);
	http = ezcfg_igrs_get_http(igrs);
	buf_len = EZCFG_IGRS_HTTP_CHUNK_SIZE ;

	buf = calloc(buf_len+1, sizeof(char)); /* +1 for \0 */
	if (buf == NULL) {
		err(ezcfg, "not enough memory for processing igrs new connection\n");
		return;
	}
	memset(buf, 0, buf_len);
	nread = 0;
	header_len = ezcfg_socket_read_http_header(ezcfg_worker_get_socket(worker), http, buf, buf_len, &nread);

	ASSERT(nread >= header_len);

	if (header_len <= 0) {
		err(ezcfg, "request error\n");
		goto exit;
	}

	/* first setup message body info */
	if (nread > header_len) {
		ezcfg_igrs_set_message_body(igrs, buf + header_len, nread - header_len);
	}

	/* 0-terminate the request: parse http request uses sscanf
	 * !!! never, be careful not mangle the "\r\n\r\n" string!!!
	 */
	//buf[header_len - 1] = '\0';
	ezcfg_http_set_state_request(http);
	if (ezcfg_igrs_parse_header(igrs, buf, header_len) == true) {
		unsigned short major, minor;
		char *p;
		major = ezcfg_igrs_get_version_major(igrs);
		minor = ezcfg_igrs_get_version_minor(igrs);
		if ((major != 1) || (minor != 0)) {
			send_igrs_error(worker, 505,
			                "IGRS version not supported",
			                "%s", "Weird IGRS version");
			goto exit;
		}
		p = ezcfg_socket_read_http_content(ezcfg_worker_get_socket(worker), http, buf, header_len, &buf_len, &nread);
		if (p == NULL) {
			/* Do not put garbage in the access log */
			send_igrs_error(worker, 400, "Bad Request", "Can not parse request: %.*s", nread, buf);
			goto exit;
		}
		buf = p;
		if (nread > header_len) {
			ezcfg_igrs_set_message_body(igrs, buf + header_len, nread - header_len);
			ezcfg_igrs_parse_message_body(igrs);
		}
		ezcfg_worker_set_birth_time(worker, time(NULL));
		handle_igrs_request(worker);
	} else {
		/* Do not put garbage in the access log */
		send_igrs_error(worker, 400, "Bad Request", "Can not parse request: %.*s", nread, buf);
	}

exit:
	/* release buf memory */
	if (buf != NULL)
		free(buf);
}
