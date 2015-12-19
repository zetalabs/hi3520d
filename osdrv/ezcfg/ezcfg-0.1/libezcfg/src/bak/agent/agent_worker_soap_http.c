/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : agent/agent_worker_soap_http.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2013-08-01   0.1       Modify it from thread/worker_soap_http.c
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
	FILE *dbg_fp; \
	snprintf(path, 256, "/tmp/%d-debug.txt", getpid()); \
	dbg_fp = fopen(path, "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, "tid=[%d] ", (int)gettid()); \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

static bool soap_http_error_handler(struct ezcfg_agent_worker *worker)
{
	return false;
}

static void send_soap_http_error(struct ezcfg_agent_worker *worker, int status,
                            const char *reason, const char *fmt, ...)
{
	char buf[EZCFG_BUFFER_SIZE];
	va_list ap;
	int len;
	bool handled;

	//ezcfg_http_set_status_code(worker->proto_data, status);
	handled = soap_http_error_handler(worker);

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
			ezcfg_agent_worker_set_num_bytes_sent(worker, len);
		}
		ezcfg_agent_worker_printf(worker,
		              "HTTP/1.1 %d %s\r\n"
		              "Content-Type: text/plain\r\n"
		              "Content-Length: %d\r\n"
		              "Connection: close\r\n"
		              "\r\n%s", status, reason, len, buf);
	}
}

static bool is_soap_http_nvram_request(const char *uri)
{
	if (strncmp(uri, EZCFG_SOAP_HTTP_NVRAM_GET_URI"?name=", strlen(EZCFG_SOAP_HTTP_NVRAM_GET_URI) + 6) == 0) {
		return true;
	}
	else if (strcmp(uri, EZCFG_SOAP_HTTP_NVRAM_SET_URI) == 0) {
		return true;
	}
	else if (strncmp(uri, EZCFG_SOAP_HTTP_NVRAM_UNSET_URI"?name=", strlen(EZCFG_SOAP_HTTP_NVRAM_UNSET_URI) + 6) == 0) {
		return true;
	}
	else if (strcmp(uri, EZCFG_SOAP_HTTP_NVRAM_LIST_URI) == 0) {
		return true;
	}
	else if (strcmp(uri, EZCFG_SOAP_HTTP_NVRAM_INFO_URI) == 0) {
		return true;
	}
	else if (strcmp(uri, EZCFG_SOAP_HTTP_NVRAM_COMMIT_URI) == 0) {
		return true;
	}
	else if (strcmp(uri, EZCFG_SOAP_HTTP_NVRAM_INSERT_SOCKET_URI) == 0) {
		return true;
	}
	else if (strcmp(uri, EZCFG_SOAP_HTTP_NVRAM_REMOVE_SOCKET_URI) == 0) {
		return true;
	}
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	else if (strcmp(uri, EZCFG_SOAP_HTTP_NVRAM_INSERT_SSL_URI) == 0) {
		return true;
	}
	else if (strcmp(uri, EZCFG_SOAP_HTTP_NVRAM_REMOVE_SSL_URI) == 0) {
		return true;
	}
#endif
	else {
		return false;
	}
}

static void handle_soap_http_request(struct ezcfg_agent_worker *worker)
{
	struct ezcfg *ezcfg;
	struct ezcfg_soap_http *sh;
	struct ezcfg_http *http;
	struct ezcfg_agent_master *master;
	struct ezcfg_nvram *nvram;
	char *request_uri;
	char *msg = NULL;
	int msg_len;

	ASSERT(worker != NULL);

	sh = (struct ezcfg_soap_http *)ezcfg_agent_worker_get_proto_data(worker);
	ASSERT(sh != NULL);

	ezcfg = ezcfg_agent_worker_get_ezcfg(worker);
	http = ezcfg_soap_http_get_http(sh);
	master = ezcfg_agent_worker_get_master(worker);
	nvram = ezcfg_agent_master_get_nvram(master);

	request_uri = ezcfg_http_get_request_uri(http);
	if (request_uri == NULL) {
		err(ezcfg, "no request uri for SOAP/HTTP binding GET method.\n");

		/* clean http structure info */
		ezcfg_http_reset_attributes(http);
		ezcfg_http_set_status_code(http, 400);
		ezcfg_http_set_state_response(http);

		/* build SOAP/HTTP binding error response */
		msg_len = ezcfg_soap_http_get_message_length(sh);
		if (msg_len < 0) {
			err(ezcfg, "ezcfg_soap_http_get_message_length error.\n");
			goto exit;
		}
		msg_len++; /* one more for '\0' */
		msg = (char *)malloc(msg_len);
		if (msg == NULL) {
			err(ezcfg, "malloc msg error.\n");
			goto exit;
		}
		memset(msg, 0, msg_len);
		msg_len = ezcfg_soap_http_write_message(sh, msg, msg_len);
		ezcfg_agent_worker_write(worker, msg, msg_len);
		goto exit;
	}

	if (is_soap_http_nvram_request(request_uri) == true) {
		if (ezcfg_soap_http_handle_nvram_request(sh, nvram) < 0) {
			/* clean http structure info */
			ezcfg_http_reset_attributes(http);
			ezcfg_http_set_status_code(http, 400);
			ezcfg_http_set_state_response(http);

			/* build SOAP/HTTP error response */
			msg_len = ezcfg_soap_http_get_message_length(sh);
			if (msg_len < 0) {
				err(ezcfg, "ezcfg_soap_http_get_message_length error.\n");
				goto exit;
			}
			msg_len++; /* one more for '\0' */
			msg = (char *)malloc(msg_len);
			if (msg == NULL) {
				err(ezcfg, "malloc msg error.\n");
				goto exit;
			}
			memset(msg, 0, msg_len);
			msg_len = ezcfg_soap_http_write_message(sh, msg, msg_len);
			ezcfg_agent_worker_write(worker, msg, msg_len);
			goto exit;
		}
		else {
			/* build SOAP/HTTP binding response */
			msg_len = ezcfg_soap_http_get_message_length(sh);
			if (msg_len < 0) {
				err(ezcfg, "ezcfg_soap_http_get_message_length error.\n");
				goto exit;
			}
			msg_len++; /* one more for '\0' */
			msg = (char *)malloc(msg_len);
			if (msg == NULL) {
				err(ezcfg, "malloc msg error.\n");
				goto exit;
			}
			memset(msg, 0, msg_len);
			msg_len = ezcfg_soap_http_write_message(sh, msg, msg_len);
			ezcfg_agent_worker_write(worker, msg, msg_len);
			goto exit;
		}
	}
exit:
	if (msg != NULL)
		free(msg);
}

void ezcfg_agent_worker_process_soap_http_new_connection(struct ezcfg_agent_worker *worker)
{
	int header_len, nread;
	char *buf;
	int buf_len;
	struct ezcfg *ezcfg;
	struct ezcfg_soap_http *sh;
	struct ezcfg_http *http;

	ASSERT(worker != NULL);

	sh = (struct ezcfg_soap_http *)ezcfg_agent_worker_get_proto_data(worker);
	ASSERT(sh != NULL);

	ezcfg = ezcfg_agent_worker_get_ezcfg(worker);
	http = ezcfg_soap_http_get_http(sh);
	buf_len = EZCFG_SOAP_HTTP_CHUNK_SIZE ;

	buf = malloc(buf_len+1); /* +1 for \0 */
	if (buf == NULL) {
		err(ezcfg, "not enough memory for processing SOAP/HTTP new connection\n");
		return;
	}
	memset(buf, 0, buf_len+1);
	nread = 0;
	header_len = ezcfg_socket_read_http_header(ezcfg_agent_worker_get_socket(worker), http, buf, buf_len, &nread);

	ASSERT(nread >= header_len);

	if (header_len <= 0) {
		err(ezcfg, "request error\n");
		free(buf);
		return; /* Request is too large or format is not correct */
	}

	/* 0-terminate the request: parse http request uses sscanf
	 * !!! never, be careful not mangle the "\r\n\r\n" string!!!
	 */
	//buf[header_len - 1] = '\0';
	ezcfg_http_set_state_request(http);
	if (ezcfg_soap_http_parse_header(sh, buf, header_len) == true) {
		unsigned short major, minor;
		char *p;
		major = ezcfg_soap_http_get_http_version_major(sh);
		minor = ezcfg_soap_http_get_http_version_minor(sh);
		if ((major != 1) || (minor != 1)) {
			send_soap_http_error(worker, 505,
			                "SOAP/HTTP binding version not supported",
			                "%s", "Weird HTTP version");
			goto exit;
		}
		p = ezcfg_socket_read_http_content(ezcfg_agent_worker_get_socket(worker), http, buf, header_len, &buf_len, &nread);
		if (p == NULL) {
			/* Do not put garbage in the access log */
			send_soap_http_error(worker, 400, "Bad Request", "Can not parse request: %.*s", nread, buf);
			goto exit;
		}
		buf = p;
		if (nread > header_len) {
			ezcfg_soap_http_set_message_body(sh, buf + header_len, nread - header_len);
			ezcfg_soap_http_parse_message_body(sh);
		}
		ezcfg_agent_worker_set_birth_time(worker, time(NULL));
		handle_soap_http_request(worker);
	} else {
		/* Do not put garbage in the access log */
		send_soap_http_error(worker, 400, "Bad Request", "Can not parse request: %.*s", nread, buf);
	}

exit:
	/* release buf memory */
	free(buf);
}
