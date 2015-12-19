/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/worker_http.c
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
#include "ezcfg-websocket.h"

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

#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_WEBSOCKET == 1)
static bool is_http_websocket_request(struct ezcfg_http *http)
{
	char *val;

	/* The requirements for this handshake are as follows.*/
	/* 2.   The method of the request MUST be GET, and the HTTP version MUST
	 *      be at least 1.1.
	 */
	if (ezcfg_http_request_method_cmp(http, EZCFG_HTTP_METHOD_GET) != 0)
		return false;
	if (ezcfg_http_get_version_major(http) < 1)
		return false;
	if ((ezcfg_http_get_version_major(http) == 1) && (ezcfg_http_get_version_minor(http) < 1))
		return false;

	/* 5.   The request MUST contain an |Upgrade| header field whose value
	 *      MUST include the "websocket" keyword.
	 */
	val = ezcfg_http_get_header_value(http, EZCFG_HTTP_HEADER_UPGRADE);
	if ((val == NULL) || (strcmp(val, EZCFG_WS_HTTP_HEADER_KEYWORD_WEBSOCKET) != 0))
		return false;

	/* 6.   The request MUST contain a |Connection| header field whose value
	 *      MUST include the "Upgrade" token.
	 */
	val = ezcfg_http_get_header_value(http, EZCFG_HTTP_HEADER_CONNECTION);
	if ((val == NULL) || (strcmp(val, EZCFG_HTTP_HEADER_UPGRADE) != 0))
		return false;

	/* 7.   The request MUST include a header field with the name
	 *      |Sec-WebSocket-Key|.
	 */
	val = ezcfg_http_get_header_value(http, EZCFG_WS_HTTP_HEADER_SEC_WEBSOCKET_KEY);
	if (val == NULL)
		return false;

	/* 9.   The request MUST include a header field with the name
	 *      |Sec-WebSocket-Version|.  The value of this header field MUST be 13.
	 */
	val = ezcfg_http_get_header_value(http, EZCFG_WS_HTTP_HEADER_SEC_WEBSOCKET_VERSION);
	if ((val == NULL) || strcmp(val, "13"))
		return false;

	return true;
}
#endif

#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_SSI == 1)
static bool is_http_html_ssi_request(const char *uri)
{
	size_t uri_len;

	uri_len = strlen(uri);
	if (uri_len > strlen(EZCFG_FILE_EXT_SHTM_STRING)) {
		uri_len -= strlen(EZCFG_FILE_EXT_SHTM_STRING);
		if (strcmp(uri+uri_len, EZCFG_FILE_EXT_SHTM_STRING) == 0) {
			return true;
		}
	}

	return false;
}
#endif

#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_CGI_NVRAM == 1)
static bool is_http_html_nvram_request(const char *uri)
{
	int uri_len, len;
	if (strstr(uri, EZCFG_HTTP_HTML_NVRAM_PREFIX_URI) != NULL) {
		uri_len = strlen(uri);
		len = uri_len - strlen(EZCFG_FILE_EXT_JS_STRING);
		if ((len > 0) && (strcmp(uri+len, EZCFG_FILE_EXT_JS_STRING) == 0)) {
			return true;
		}
	}

	return false;
}
#endif

#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_CGI_APPLY == 1)
static bool is_http_html_apply_request(const char *uri)
{
	if (strncmp(uri, EZCFG_HTTP_HTML_APPLY_PREFIX_URI, strlen(EZCFG_HTTP_HTML_APPLY_PREFIX_URI)) == 0) {
		return true;
	}
	else {
		return false;
	}
}
#endif

#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_CGI_ADMIN == 1)
static bool is_http_html_admin_request(const char *uri)
{
	if (strncmp(uri, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI, strlen(EZCFG_HTTP_HTML_ADMIN_PREFIX_URI)) == 0) {
		return true;
	}
	else {
		return false;
	}
}
#endif

static bool need_authorization(const char *uri)
{
#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_CGI_ADMIN == 1)
	if (is_http_html_admin_request(uri) == true) {
		return true;
	}
	else
#endif
#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_CGI_NVRAM == 1)
	if (is_http_html_nvram_request(uri) == true) {
		return true;
	}
	else
#endif
#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_CGI_APPLY == 1)
	if (is_http_html_apply_request(uri) == true) {
		return true;
	}
	else
#endif
	{
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
		if (ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_WWW_AUTHENTICATE, buf) == false) {
			err(ezcfg, "HTTP add header error.\n");
			goto func_exit;
		}

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
	if (ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_WWW_AUTHENTICATE, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		goto func_exit;
	}

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

#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_WEBSOCKET == 1)
static void handle_websocket_request(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	//struct ezcfg_master *master;
	//struct ezcfg_nvram *nvram;
	struct ezcfg_websocket *ws = NULL;
	struct ezcfg_http_websocket *hws = NULL;
	char buf[1024];
	//char *request_uri;
	//size_t uri_len;
	char *msg = NULL;
	int msg_len;

	ASSERT(worker != NULL);

	http = (struct ezcfg_http *)ezcfg_worker_get_proto_data(worker);
	ASSERT(http != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);
	//master = ezcfg_worker_get_master(worker);
	//nvram = ezcfg_master_get_nvram(master);

	ws = ezcfg_websocket_new(ezcfg);
	if (ws == NULL) {
		send_http_error(worker, 500,
		                "Internal Server Error",
		                "%s", "Not enough memory");
		goto func_exit;
	}

	hws = ezcfg_http_websocket_new(ezcfg, http, ws);
	if (hws == NULL) {
		send_http_error(worker, 500,
		                "Internal Server Error",
		                "%s", "Not enough memory");
		goto func_exit;
	}

	if (ezcfg_http_handle_websocket_request(hws) < 0) {
		send_http_bad_request(worker);
		goto func_exit;
	}
	else {
		/* build HTTP response */
		/* HTTP header content-type */
		snprintf(buf, sizeof(buf), "%s; %s=%s", EZCFG_HTTP_MIME_TEXT_HTML, EZCFG_HTTP_CHARSET_NAME, EZCFG_HTTP_CHARSET_UTF8);
		if (ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CONTENT_TYPE, buf) == false) {
			err(ezcfg, "HTTP add header error.\n");
			goto func_exit;
		}

		/* HTTP header cache-control */
		if (ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CACHE_CONTROL, EZCFG_HTTP_CACHE_REQUEST_NO_CACHE) == false) {
			err(ezcfg, "HTTP add header error.\n");
			goto func_exit;
		}

		/* HTTP header expires */
		if (ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_EXPIRES, "0") == false) {
			err(ezcfg, "HTTP add header error.\n");
			goto func_exit;
		}

		/* HTTP header pragma */
		if (ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_PRAGMA, EZCFG_HTTP_PRAGMA_NO_CACHE) == false) {
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
	if (msg != NULL)
		free(msg);

	if (hws != NULL)
		ezcfg_http_websocket_delete(hws);

	if (ws != NULL)
		ezcfg_websocket_delete(ws);
}
#endif

#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_SSI == 1)
static void handle_ssi_request(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_master *master;
	struct ezcfg_nvram *nvram;
	struct ezcfg_ssi *ssi = NULL;
	char buf[1024];
	char *request_uri;
	size_t uri_len;
	char *msg = NULL;
	int msg_len;

	ASSERT(worker != NULL);

	http = (struct ezcfg_http *)ezcfg_worker_get_proto_data(worker);
	ASSERT(http != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);
	master = ezcfg_worker_get_master(worker);
	nvram = ezcfg_master_get_nvram(master);

	ssi = ezcfg_ssi_new(ezcfg, nvram);
	if (ssi == NULL) {
		send_http_error(worker, 500,
		                "Internal Server Error",
		                "%s", "Not enough memory");
		goto func_exit;
	}

	/* set default document root */
	if (ezcfg_ssi_set_document_root(ssi, ezcfg_common_get_web_document_root_path(ezcfg)) == false) {
		send_http_error(worker, 500,
		                "Internal Server Error",
		                "%s", "Not enough memory");
		goto func_exit;
	}

	/* set file path */
	request_uri = ezcfg_http_get_request_uri(http);
	uri_len = strlen(request_uri);
	if (uri_len+2 > sizeof(buf)) {
		send_http_error(worker, 505,
		                "Bad Request",
		                "%s", "File name is too large");
		goto func_exit;
	}

	snprintf(buf, sizeof(buf), "%s", *request_uri == '/' ? "" : "/");
	ezcfg_util_url_decode(request_uri, uri_len, buf+strlen(buf), uri_len+1, 0);
	ezcfg_util_url_remove_double_dots_and_double_slashes(buf);

	/* if uri is pointed to directory, add index.shtm */
	uri_len = strlen(buf);
	if (buf[uri_len - 1] == '/') {
		if ((uri_len+strlen(EZCFG_HTTP_HTML_INDEX_FILE_SHTM)+1) > sizeof(buf)) {
			send_http_error(worker, 505,
			                "Bad Request",
			                "%s", "File name is too large");
			goto func_exit;
		}
		strcat(buf, EZCFG_HTTP_HTML_INDEX_FILE_SHTM);
	}

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
		/* build HTTP response */
		/* HTTP header content-type */
		snprintf(buf, sizeof(buf), "%s; %s=%s", EZCFG_HTTP_MIME_TEXT_HTML, EZCFG_HTTP_CHARSET_NAME, EZCFG_HTTP_CHARSET_UTF8);
		if (ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CONTENT_TYPE, buf) == false) {
			err(ezcfg, "HTTP add header error.\n");
			goto func_exit;
		}

		/* HTTP header cache-control */
		if (ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CACHE_CONTROL, EZCFG_HTTP_CACHE_REQUEST_NO_CACHE) == false) {
			err(ezcfg, "HTTP add header error.\n");
			goto func_exit;
		}

		/* HTTP header expires */
		if (ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_EXPIRES, "0") == false) {
			err(ezcfg, "HTTP add header error.\n");
			goto func_exit;
		}

		/* HTTP header pragma */
		if (ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_PRAGMA, EZCFG_HTTP_PRAGMA_NO_CACHE) == false) {
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

		/* process SSI file */
		msg_len = ezcfg_ssi_file_get_line(ssi, buf, sizeof(buf));
		while(msg_len >= 0) {
			if (msg_len > 0) {
				ezcfg_worker_write(worker, buf, msg_len);
			}
			msg_len = ezcfg_ssi_file_get_line(ssi, buf, sizeof(buf));
		}

		goto func_exit;
	}

func_exit:
	if (msg != NULL)
		free(msg);

	if (ssi != NULL)
		ezcfg_ssi_delete(ssi);
}
#endif

#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_CGI_NVRAM == 1)
static void handle_nvram_request(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_master *master;
	struct ezcfg_nvram *nvram;
	struct ezcfg_http_nvram *hn;
	char buf[1024];
	char *request_uri;
	size_t uri_len;
	int type;
	char *msg = NULL;
	int msg_len;

	ASSERT(worker != NULL);

	http = (struct ezcfg_http *)ezcfg_worker_get_proto_data(worker);
	ASSERT(http != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);
	master = ezcfg_worker_get_master(worker);
	nvram = ezcfg_master_get_nvram(master);

	hn = ezcfg_http_nvram_new(ezcfg);
	if (hn == NULL) {
		send_http_error(worker, 500,
		                "Internal Server Error",
		                "%s", "Not enough memory");
		goto func_exit;
	}

	/* set content type */
	request_uri = ezcfg_http_get_request_uri(http);
	type = ezcfg_util_file_get_type_extension_index(request_uri);
	if (type != EZCFG_FILE_EXT_JS) {
		send_http_error(worker, 500,
		                "Internal Server Error",
		                "%s", "Not support file type");
		goto func_exit;
	}
	ezcfg_http_nvram_set_content_type(hn, type);

	/* set default root */
	if (ezcfg_http_nvram_set_root(hn, ezcfg_common_get_web_document_root_path(ezcfg)) == false) {
		send_http_error(worker, 500,
		                "Internal Server Error",
		                "%s", "Not enough memory");
		goto func_exit;
	}

	/* set file path */
	request_uri = ezcfg_http_get_request_uri(http);
	uri_len = strlen(request_uri);
	if (uri_len+2 > sizeof(buf)) {
		send_http_error(worker, 505,
		                "Bad Request",
		                "%s", "File name is too large");
		goto func_exit;
	}

	snprintf(buf, sizeof(buf), "%s", *request_uri == '/' ? "" : "/");
	ezcfg_util_url_decode(request_uri, uri_len, buf+strlen(buf), uri_len+1, 0);
	ezcfg_util_url_remove_double_dots_and_double_slashes(buf);
	if (ezcfg_http_nvram_set_path(hn, buf) == false) {
		send_http_error(worker, 500,
		                "Internal Server Error",
		                "%s", "Not enough memory");
		goto func_exit;
	}

	/* must setup http and nvram before handling request */
	ezcfg_http_nvram_set_http(hn, http);
	ezcfg_http_nvram_set_nvram(hn, nvram);

	if (ezcfg_http_handle_nvram_request(hn) < 0) {
		send_http_bad_request(worker);
		goto func_exit;
	}
	else {
		/* build HTTP response */
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
	if (msg != NULL)
		free(msg);

	if (hn != NULL)
		ezcfg_http_nvram_delete(hn);
}
#endif

#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_CGI_APPLY == 1)
static void handle_apply_request(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_master *master;
	struct ezcfg_nvram *nvram;
	char *msg = NULL;
	int msg_len;

	ASSERT(worker != NULL);

	http = (struct ezcfg_http *)ezcfg_worker_get_proto_data(worker);
	ASSERT(http != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);
	master = ezcfg_worker_get_master(worker);
	nvram = ezcfg_master_get_nvram(master);

	if (ezcfg_http_handle_apply_request(http, nvram) < 0) {
		send_http_bad_request(worker);
		goto func_exit;
	}
	else {
		/* build HTTP response */
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
	if (msg != NULL)
		free(msg);
}
#endif

#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_CGI_ADMIN == 1)
static void handle_admin_request(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_master *master;
	struct ezcfg_nvram *nvram;
	char *msg = NULL;
	int msg_len;

	ASSERT(worker != NULL);

	http = (struct ezcfg_http *)ezcfg_worker_get_proto_data(worker);
	ASSERT(http != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);
	master = ezcfg_worker_get_master(worker);
	nvram = ezcfg_master_get_nvram(master);

	if (ezcfg_http_handle_admin_request(http, nvram) < 0) {
		send_http_bad_request(worker);
		goto func_exit;
	}
	else {
		/* build HTTP response */
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
	if (msg != NULL)
		free(msg);
}
#endif

#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_CGI_INDEX == 1)
static void handle_index_request(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_master *master;
	struct ezcfg_nvram *nvram;
	char *msg = NULL;
	int msg_len;

	ASSERT(worker != NULL);

	http = (struct ezcfg_http *)ezcfg_worker_get_proto_data(worker);
	ASSERT(http != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);
	master = ezcfg_worker_get_master(worker);
	nvram = ezcfg_master_get_nvram(master);

	if (ezcfg_http_handle_index_request(http, nvram) < 0) {
		send_http_bad_request(worker);
		goto func_exit;
	}
	else {
		/* build HTTP response */
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
	if (msg != NULL)
		free(msg);
}
#endif

static void handle_http_request(struct ezcfg_worker *worker)
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
#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_WEBSOCKET == 1)
	if (is_http_websocket_request(http) == true) {
		/* handle SSI enabled web page */
		handle_websocket_request(worker);
	}
	else
#endif
#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_SSI == 1)
	if (is_http_html_ssi_request(request_uri) == true) {
		/* handle SSI enabled web page */
		handle_ssi_request(worker);
	}
	else
#endif
#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_CGI_NVRAM == 1)
	if (is_http_html_nvram_request(request_uri) == true) {
		/* handle SSI enabled web page */
		handle_nvram_request(worker);
	}
	else
#endif
#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_CGI_APPLY == 1)
	if (is_http_html_apply_request(request_uri) == true) {
		/* handle administration web page */
		handle_apply_request(worker);
	}
	else
#endif
#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_CGI_ADMIN == 1)
	if (is_http_html_admin_request(request_uri) == true) {
		/* handle administration web page */
		handle_admin_request(worker);
	}
	else
#endif
	{
#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_CGI_INDEX == 1)
		/* will always return index page if not find the uri */
		handle_index_request(worker);
#elif (HAVE_EZBOX_SERVICE_EZCFG_HTTPD_SSI == 1)
		/* handle SSI enabled web page */
		handle_ssi_request(worker);
#else
		send_http_bad_request(worker);
#endif
	}
}

void ezcfg_worker_process_http_new_connection(struct ezcfg_worker *worker)
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
		/* Request is too large or format is not correct */
		err(ezcfg, "request error\n");
		goto func_exit;
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
			goto func_exit;
		}
		p = ezcfg_socket_read_http_content(ezcfg_worker_get_socket(worker), http, buf, header_len, &buf_len, &nread);
		if (p == NULL) {
			send_http_error(worker, 400, "Bad Request", "");
			goto func_exit;
		}
		buf = p;
		if (nread > header_len) {
			ezcfg_http_set_message_body(http, buf + header_len, nread - header_len);
		}
		ezcfg_worker_set_birth_time(worker, time(NULL));
		handle_http_request(worker);
	} else {
		/* Do not put garbage in the access log */
		send_http_error(worker, 400, "Bad Request", "Can not parse request: %.*s", nread, buf);
	}

func_exit:
	/* release buf memory */
	free(buf);
}
