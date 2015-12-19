/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/worker_upnp_http.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-28   0.1       Derive from worker_http.c for UPnP
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

#define handle_error_en(en, msg) \
	do { errno = en; perror(msg); } while (0)


#define EZCFG_HTTP_HTML_SSI_EXTENSION	".shtm"

/**
 * Private functions
 **/
static bool http_error_handler(struct ezcfg_worker *worker)
{
	return false;
}

static void send_http_error(struct ezcfg_worker *worker, int status,
                            const char *reason, const char *fmt, ...)
{
	struct ezcfg_http *http;
	char buf[EZCFG_BUFFER_SIZE];
	va_list ap;
	int len;
	bool handled;

	http = (struct ezcfg_http *)ezcfg_worker_get_proto_data(worker);
	ezcfg_http_set_status_code(http, status);
	handled = http_error_handler(worker);

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

static void send_http_bad_request(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	char *msg = NULL;
	int msg_len;

	ASSERT(worker != NULL);

	http = (struct ezcfg_http *)ezcfg_worker_get_proto_data(worker);
	ASSERT(http != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);

	/* clean http structure info */
	ezcfg_http_reset_attributes(http);
	ezcfg_http_set_status_code(http, 400);
	ezcfg_http_set_state_response(http);

	/* build HTTP error response */
	msg_len = ezcfg_http_get_message_length(http);
	if (msg_len < 0) {
		err(ezcfg, "ezcfg_http_get_message_length error.\n");
		goto func_exit;
	}
	msg_len++; /* one more for '\0' */
	msg = (char *)malloc(msg_len);
	if (msg == NULL) {
		err(ezcfg, "malloc msg error.\n");
		goto func_exit;
	}
	memset(msg, 0, msg_len);
	msg_len = ezcfg_http_write_message(http, msg, msg_len);
	ezcfg_worker_write(worker, msg, msg_len);
func_exit:
	if (msg != NULL)
		free(msg);
}

static bool is_http_html_admin_request(const char *uri)
{
	if (strncmp(uri, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI, strlen(EZCFG_HTTP_HTML_ADMIN_PREFIX_URI)) == 0) {
		return true;
	}
	else {
		return false;
	}
}

static bool need_authorization(const char *uri)
{
	if (is_http_html_admin_request(uri) == true) {
		return true;
	}
	else if (strncmp(uri, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI, strlen(EZCFG_HTTP_HTML_ADMIN_PREFIX_URI)) == 0) {
		return true;
	}
	else {
		return false;
	}
}

static bool is_authorized(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_master *master;
	struct ezcfg_nvram *nvram;
	struct ezcfg_auth *auth, *auths;
	char *msg = NULL;
	int msg_len;
	bool ret = false;
	char buf[1024];
	char *p;

	ASSERT(worker != NULL);

	http = (struct ezcfg_http *)ezcfg_worker_get_proto_data(worker);
	ASSERT(http != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);
	master = ezcfg_worker_get_master(worker);
	nvram = ezcfg_master_get_nvram(master);

	auth = ezcfg_auth_new(ezcfg);
	if (auth == NULL) {
		err(ezcfg, "ezcfg_auth_new error.\n");
		goto func_exit;
	}

	if (ezcfg_http_parse_auth(http, auth) == false) {
		err(ezcfg, "ezcfg_http_parse_auth error.\n");
		goto try_auth;
	}

	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_REALM), &p);
	if (p == NULL) {
		err(ezcfg, "ezcfg_nvram_get_entry_value error.\n");
		goto try_auth;
	}
	ret = ezcfg_auth_set_realm(auth, p);
	free(p);
	if (ret == false) {
		err(ezcfg, "ezcfg_auth_set_realm error.\n");
		goto try_auth;
	}

	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_DOMAIN), &p);
	if (p == NULL) {
		err(ezcfg, "ezcfg_nvram_get_entry_value error.\n");
		goto try_auth;
	}
	ret = ezcfg_auth_set_domain(auth, p);
	free(p);
	if (ret == false) {
		err(ezcfg, "ezcfg_auth_set_domain error.\n");
		goto try_auth;
	}

	/* lock auths */
	if (ezcfg_master_auth_mutex_lock(master) == 0) {
		auths = ezcfg_master_get_auths(master);
		ret = ezcfg_auth_check_authorized(&auths, auth);
		/* unlock auths */
		ezcfg_master_auth_mutex_unlock(master);
	}

	if (ret == false) {
try_auth:
		/* clean http structure info */
		ezcfg_http_reset_attributes(http);
		ezcfg_http_set_status_code(http, 401);
		ezcfg_http_set_state_response(http);

		/* http WWW-Authenticate */
		ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_REALM), &p);
		if (p == NULL) {
			err(ezcfg, "ezcfg_nvram_get_entry_value error.\n");
			goto func_exit;
		}
		snprintf(buf, sizeof(buf), "Basic realm=\"%s\"", p);
		free(p);
		ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_WWW_AUTHENTICATE, buf);

		/* build HTTP Unauthorized response */
		msg_len = ezcfg_http_get_message_length(http);
		if (msg_len < 0) {
			err(ezcfg, "ezcfg_http_get_message_length error.\n");
			goto func_exit;
		}
		msg_len++; /* one more for '\0' */
		msg = (char *)malloc(msg_len);
		if (msg == NULL) {
			err(ezcfg, "malloc msg error.\n");
			goto func_exit;
		}
		memset(msg, 0, msg_len);
		msg_len = ezcfg_http_write_message(http, msg, msg_len);
		ezcfg_worker_write(worker, msg, msg_len);
		goto func_exit;
	}

	/* OK, it's authenticated */
	ret = true;

func_exit:
	if (msg != NULL)
		free(msg);
	return (ret);
}

static void handle_auth_request(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_master *master;
	struct ezcfg_nvram *nvram;
	char *msg = NULL;
	int msg_len;
	char buf[1024];
	char *p;

	ASSERT(worker != NULL);

	http = (struct ezcfg_http *)ezcfg_worker_get_proto_data(worker);
	ASSERT(http != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);
	master = ezcfg_worker_get_master(worker);
	nvram = ezcfg_master_get_nvram(master);

	/* clean http structure info */
	ezcfg_http_reset_attributes(http);
	ezcfg_http_set_status_code(http, 401);
	ezcfg_http_set_state_response(http);

	/* http WWW-Authenticate */
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_REALM), &p);
	if (p == NULL) {
		err(ezcfg, "ezcfg_nvram_get_entry_value error.\n");
		goto func_exit;
	}
	snprintf(buf, sizeof(buf), "Basic realm=\"%s\"", p);
	free(p);
	ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_WWW_AUTHENTICATE, buf);

	/* build HTTP Unauthorized response */
	msg_len = ezcfg_http_get_message_length(http);
	if (msg_len < 0) {
		err(ezcfg, "ezcfg_http_get_message_length error.\n");
		goto func_exit;
	}
	msg_len++; /* one more for '\0' */
	msg = (char *)malloc(msg_len);
	if (msg == NULL) {
		err(ezcfg, "malloc msg error.\n");
		goto func_exit;
	}
	memset(msg, 0, msg_len);
	msg_len = ezcfg_http_write_message(http, msg, msg_len);
	ezcfg_worker_write(worker, msg, msg_len);

func_exit:
	if (msg != NULL)
		free(msg);
}

static size_t url_decode(const char *src, size_t src_len,
	char *dst, size_t dst_len, int is_form_url_encoded) {
	size_t i, j;
	int a, b;
#define HEXTOI(x) (isdigit(x) ? x - '0' : x - 'W')

	for (i = j = 0; i < src_len && j < dst_len - 1; i++, j++) {
		if (src[i] == '%' &&
		    isxdigit(* (const unsigned char *) (src + i + 1)) &&
		    isxdigit(* (const unsigned char *) (src + i + 2))) {
			a = tolower(* (const unsigned char *) (src + i + 1));
			b = tolower(* (const unsigned char *) (src + i + 2));
			dst[j] = (char) ((HEXTOI(a) << 4) | HEXTOI(b));
			i += 2;
		} else if (is_form_url_encoded && src[i] == '+') {
			dst[j] = ' ';
		} else {
			dst[j] = src[i];
		}
	}

	dst[j] = '\0'; // Null-terminate the destination

	return j;
}

static void remove_double_dots_and_double_slashes(char *s)
{
	char *p = s;

	while (*s != '\0') {
		*p++ = *s++;
		if (s[-1] == '/' || s[-1] == '\\') {
			/* skip all following slashes and backslashes */
			while (*s == '/' || *s == '\\') {
				s++;
			}

			/* skip all double-dots */
			while (*s == '.' && s[1] == '.') {
				s += 2;
			}
		}
	}
	*p = '\0';
}

static void handle_ssi_request(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_master *master;
	struct ezcfg_nvram *nvram;
	struct ezcfg_ssi *ssi = NULL;
	char buf[1024];
	char *request_uri;
	char *accept_language = NULL;
	size_t uri_len;
	char *msg = NULL;
	int msg_len;
	char *p;
	int len;
	time_t t;
	struct tm *tmp;

	ASSERT(worker != NULL);

	http = (struct ezcfg_http *)ezcfg_worker_get_proto_data(worker);
	ASSERT(http != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);
	master = ezcfg_worker_get_master(worker);
	nvram = ezcfg_master_get_nvram(master);

	/* HTTP header has been parsed */
	p = ezcfg_http_get_header_value(http, EZCFG_HTTP_HEADER_ACCEPT_LANGUAGE);
	if (p != NULL) {
		accept_language = strdup(p);
		if (accept_language == NULL)
			goto func_exit;
	}

	ssi = ezcfg_ssi_new(ezcfg, nvram);
	if (ssi == NULL) {
		send_http_error(worker, 500,
		                "Internal Server Error",
		                "%s", "Not enough memory");
		goto func_exit;
	}

	request_uri = ezcfg_http_get_request_uri(http);
	uri_len = strlen(request_uri);

	/* set default document root */
	if (ezcfg_ssi_set_document_root(ssi, "/etc/ezcfg_upnpd") == false) {
		send_http_error(worker, 500,
		                "Internal Server Error",
		                "%s", "Not enough memory");
		goto func_exit;
	}

	/* set file path */
	uri_len = strlen(request_uri);
	if (uri_len+2 > sizeof(buf)) {
		send_http_error(worker, 505,
		                "Bad Request",
		                "%s", "File name is too large");
		goto func_exit;
	}

	snprintf(buf, sizeof(buf), "%s", *request_uri == '/' ? "" : "/");
	url_decode(request_uri, uri_len, buf+strlen(buf), uri_len+1, 0);
	remove_double_dots_and_double_slashes(buf);
	if (ezcfg_ssi_set_path(ssi, buf) == false) {
		send_http_error(worker, 500,
		                "Internal Server Error",
		                "%s", "Not enough memory");
		goto func_exit;
	}

	if (ezcfg_ssi_open_file(ssi, "r") == NULL) {
		send_http_error(worker, 500,
		                "Internal Server Error",
		                "%s", "Cannot open file");
		goto func_exit;
	}

	if (ezcfg_http_handle_ssi_request(http, nvram, ssi) < 0) {
		send_http_bad_request(worker);
		goto func_exit;
	}
	else {
		/* process SSI file */
		msg_len = 0;
		msg = NULL;
		len = ezcfg_ssi_file_get_line(ssi, buf, sizeof(buf));
		while(len >= 0) {
			if (len > 0) {
				p = realloc(msg, msg_len+len);
				if (p == NULL) {
					goto func_exit;
				}
				msg = p;
				p += msg_len;
				strncpy(p, buf, len);
				msg_len += len;
			}
			len = ezcfg_ssi_file_get_line(ssi, buf, sizeof(buf));
		}

		p = ezcfg_http_set_message_body(http, msg, msg_len);
		if (p == NULL) {
			goto func_exit;
		}

		/* clean msg */
		free(msg);
		msg = NULL;

		/* build HTTP response */
		/* HTTP header Content-Language */
		if (accept_language != NULL) {
			if (ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CONTENT_LANGUAGE, accept_language) == false) {
				err(ezcfg, "HTTP add header error.\n");
				goto func_exit;
			}
		}

		/* HTTP header Content-Length */
		snprintf(buf, sizeof(buf), "%d", msg_len);
		if (ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CONTENT_LENGTH, buf) == false) {
			err(ezcfg, "HTTP add header error.\n");
			goto func_exit;
		}

		/* HTTP header Content-Type */
		snprintf(buf, sizeof(buf), "%s; %s=%s", EZCFG_HTTP_MIME_TEXT_XML, EZCFG_HTTP_CHARSET_NAME, EZCFG_HTTP_CHARSET_UTF8);
		if (ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CONTENT_TYPE, buf) == false) {
			err(ezcfg, "HTTP add header error.\n");
			goto func_exit;
		}

		/* HTTP header Date */
		/* Date: Thu, 01 Jan 1970 00:00:45 GMT */
		t = time(NULL);
		tmp = localtime(&t);
		if (tmp == NULL) {
			err(ezcfg, "localtime error.\n");
			goto func_exit;
		}

		if (strftime(buf, sizeof(buf), "%a, %d %b %Y %T GMT", tmp) == 0) {
			err(ezcfg, "strftime returned 0\n");
			goto func_exit;
		}

		if (ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_DATE, buf) == false) {
			err(ezcfg, "HTTP add header error.\n");
			goto func_exit;
		}

		msg_len = ezcfg_http_get_message_length(http);
		if (msg_len < 0) {
			err(ezcfg, "ezcfg_http_get_message_length error.\n");
			goto func_exit;
		}
		msg_len++; /* one more for '\0' */
		msg = (char *)malloc(msg_len);
		if (msg == NULL) {
			err(ezcfg, "malloc msg error.\n");
			goto func_exit;
		}
		memset(msg, 0, msg_len);
		msg_len = ezcfg_http_write_message(http, msg, msg_len);
		ezcfg_worker_write(worker, msg, msg_len);

		goto func_exit;
	}

func_exit:
	if (accept_language != NULL)
		free(accept_language);

	if (msg != NULL)
		free(msg);

	if (ssi != NULL)
		ezcfg_ssi_delete(ssi);
}

static void handle_upnp_http_request(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	char *request_uri;

	ASSERT(worker != NULL);

	http = (struct ezcfg_http *)ezcfg_worker_get_proto_data(worker);
	ASSERT(http != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);

	request_uri = ezcfg_http_get_request_uri(http);
	if (request_uri == NULL) {
		err(ezcfg, "no request uri for HTTP GET method.\n");
		send_http_bad_request(worker);
		return;
	}

	if (need_authorization(request_uri) == true &&
	    is_authorized(worker) == false) {
		/* handle authorization */
		handle_auth_request(worker);
		return ;
	}

	/* don't need authenticate or has been authenticated */
	/* handle SSI enabled web page */
	handle_ssi_request(worker);
}

/**
 * Public functions
 **/
void ezcfg_worker_process_upnp_http_new_connection(struct ezcfg_worker *worker)
{
	int header_len, nread;
	char *buf;
	int buf_len;
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;

	ASSERT(worker != NULL);

	http = (struct ezcfg_http *)ezcfg_worker_get_proto_data(worker);
	ASSERT(http != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);
	buf_len = EZCFG_HTTP_CHUNK_SIZE;

	buf = calloc(buf_len+1, sizeof(char)); /* +1 for \0 */
	if (buf == NULL) {
		err(ezcfg, "not enough memory for processing http new connection\n");
		return;
	}
	nread = 0;
	header_len = ezcfg_socket_read_http_header(ezcfg_worker_get_socket(worker), http, buf, buf_len, &nread);

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
	if (ezcfg_http_parse_header(http, buf, header_len) == true) {
		unsigned short major, minor;
		char *p;
		major = ezcfg_http_get_version_major(http);
		minor = ezcfg_http_get_version_minor(http);
		if (major != 1 || minor != 1) {
			send_http_error(worker, 505,
			                "HTTP version not supported",
			                "%s", "Weird HTTP version");
			goto exit;
		}
		p = ezcfg_socket_read_http_content(ezcfg_worker_get_socket(worker), http, buf, header_len, &buf_len, &nread);
		if (p == NULL) {
			send_http_error(worker, 400, "Bad Request", "");
			goto exit;
		}
		buf = p;
		if (nread > header_len) {
			ezcfg_http_set_message_body(http, buf + header_len, nread - header_len);
		}
		ezcfg_worker_set_birth_time(worker, time(NULL));
		handle_upnp_http_request(worker);
	} else {
		/* Do not put garbage in the access log */
		send_http_error(worker, 400, "Bad Request", "Can not parse request: %.*s", nread, buf);
	}

exit:
	/* release buf memory */
	free(buf);
}
