/* ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : api-nvram_json.c
 *
 * Description  : ezcfg API for nvram manipulate on JSON representation
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2014-03-25   0.1       Write it from scratch
 * ============================================================================
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/mount.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-json_http.h"

#include "ezcfg-api.h"

#if 0
#define DBG(format, args...) do {\
	FILE *dbg_fp = fopen("/tmp/api-nvram.log", "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

static bool debug = false;

static void log_fn(struct ezcfg *ezcfg, int priority,
                   const char *file, int line, const char *fn,
                   const char *format, va_list args)
{
	if (debug) {
		char buf[1024];
		struct timeval tv;
		struct timezone tz;

		vsnprintf(buf, sizeof(buf), format, args);
		gettimeofday(&tv, &tz);
		fprintf(stderr, "%llu.%06u [%u] %s(%d): %s",
		        (unsigned long long) tv.tv_sec, (unsigned int) tv.tv_usec,
		        (int) getpid(), fn, line, buf);
	}
#if 0
	else {
		vsyslog(priority, format, args);
	}
#endif
}

/**
 * ezcfg_api_nvram_json_set:
 * @name: nvram name
 * @value: buffer stored nvram value
 *
 **/
int ezcfg_api_nvram_json_set(const char *text)
{
	char buf[1024];
	char *msg = NULL;
	int msg_len;
	struct ezcfg *ezcfg = NULL;
	struct ezcfg_json_http *jh = NULL;
	struct ezcfg_json *json = NULL;
	struct ezcfg_http *http = NULL;
	struct ezcfg_socket *sp = NULL;
	char *result;
	char *p;
	int header_len;
	int n;
	int rc = 0;
	int key, semid = -1;
	struct sembuf res;

	if (text == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	ezcfg = ezcfg_new(ezcfg_api_common_get_config_file());
	if (ezcfg == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

 	ezcfg_log_init("nvram_json_set");
	ezcfg_common_set_log_fn(ezcfg, log_fn);

	jh = ezcfg_json_http_new(ezcfg);
	if (jh == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	json = ezcfg_json_http_get_json(jh);
	http = ezcfg_json_http_get_http(jh);

	/* verify JSON text */
	if (ezcfg_json_parse_text(json, text) == false) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}
	if (ezcfg_json_is_nvram_representation(json) == false) {
		rc = -EZCFG_E_ARGUMENT ;
		goto exit;
	}

	/* build HTTP message body */
	msg_len = ezcfg_json_get_message_length(json);
	msg_len++; /* '\0' */
	msg = (char *)malloc(msg_len);
	if (msg == NULL) {
		rc = -EZCFG_E_SPACE ;
		goto exit;
	}
	memset(msg, 0, msg_len);

	n = ezcfg_json_write_message(json, msg, msg_len);
	ezcfg_http_set_message_body(http, msg, n);

	/* build HTTP request line */
	ezcfg_http_set_request_method(http, EZCFG_JSON_HTTP_METHOD_POST);
	snprintf(buf, sizeof(buf), "%s", EZCFG_JSON_HTTP_NVRAM_URI);
	ezcfg_http_set_request_uri(http, buf);
	ezcfg_http_set_version_major(http, 1);
	ezcfg_http_set_version_minor(http, 1);
	ezcfg_http_set_state_request(http);

	/* build HTTP headers */
	snprintf(buf, sizeof(buf), "%s", EZCFG_HTTP_MIME_APPLICATION_JSON);
	ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_ACCEPT, buf);

	msg_len = ezcfg_json_http_get_message_length(jh);
	p = (char *)realloc(msg, msg_len);
	if (p == NULL) {
		rc = -EZCFG_E_SPACE ;
		goto exit;
	}
	msg = p;
	memset(msg, 0, msg_len);
	n = ezcfg_json_http_write_message(jh, msg, msg_len);

	/* prepare semaphore */
	key = ftok(ezcfg_common_get_sem_ezcfg_path(ezcfg), EZCFG_SEM_PROJID_EZCFG);
	if (key == -1) {
		DBG("<6>pid=[%d] ftok error.\n", getpid());
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	/* create a semaphore set */
	semid = semget(key, EZCFG_SEM_NUMBER, 00666);
	while (semid < 0) {
		DBG("<6>pid=[%d] try to create sem.\n", getpid());
		semid = semget(key, EZCFG_SEM_NUMBER, 00666);
	}

	/* now require available resource */
	res.sem_num = EZCFG_SEM_NVRAM_INDEX;
	res.sem_op = -1;
	res.sem_flg = 0;

	if (semop(semid, &res, 1) == -1) {
		DBG("<6>pid=[%d] semop require_res error\n", getpid());
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	snprintf(buf, sizeof(buf), "%s-%d", ezcfg_common_get_sock_nvram_path(ezcfg), getpid());
	sp = ezcfg_socket_new(ezcfg, AF_LOCAL, EZCFG_PROTO_SOAP_HTTP, buf, ezcfg_common_get_sock_nvram_path(ezcfg));
	ezctrl = ezcfg_ctrl_new_from_socket(ezcfg, AF_LOCAL, EZCFG_PROTO_SOAP_HTTP, buf, ezcfg_common_get_sock_nvram_path(ezcfg));

	if (ezctrl == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto sem_exit;
	}

	if (ezcfg_ctrl_connect(ezctrl) < 0) {
		rc = -EZCFG_E_CONNECTION ;
		goto sem_exit;
	}

	if (ezcfg_ctrl_write(ezctrl, msg, msg_len, 0) < 0) {
		rc = -EZCFG_E_WRITE ;
		goto sem_exit;
	}

	ezcfg_soap_http_reset_attributes(sh);

	n = 0;
	sp = ezcfg_ctrl_get_socket(ezctrl);
	header_len = ezcfg_socket_read_http_header(sp, http, msg, msg_len, &n);

	if (header_len <= 0) {
		rc = -EZCFG_E_READ ;
		goto sem_exit;
	}

	ezcfg_http_set_state_response(http);
	if (ezcfg_soap_http_parse_header(sh, msg, header_len) == false) {
		rc = -EZCFG_E_PARSE ;
		goto sem_exit;
	}

	p = ezcfg_socket_read_http_content(sp, http, msg, header_len, &msg_len, &n);
	if ((p == NULL) || (n <= header_len)){
		rc = -EZCFG_E_READ ;
		goto sem_exit;
	}
	msg = p;

	ezcfg_soap_http_set_message_body(sh, msg + header_len, n - header_len);
	if (ezcfg_soap_http_parse_message_body(sh) == false) {
		rc = -EZCFG_E_PARSE ;
		goto sem_exit;
	}

	/* get setNvramResponse part */
	body_index = ezcfg_soap_get_body_index(soap);
	setnv_index = ezcfg_soap_get_element_index(soap, body_index, -1, EZCFG_SOAP_NVRAM_SETNV_RESPONSE_ELEMENT_NAME);
	if (setnv_index < 2) {
		rc = -EZCFG_E_PARSE ;
		goto sem_exit;
	}

	/* get nvram result part */
	child_index = ezcfg_soap_get_element_index(soap, setnv_index, -1, EZCFG_SOAP_NVRAM_RESULT_ELEMENT_NAME);
	if (child_index < 2) {
		rc = -EZCFG_E_PARSE ;
		goto sem_exit;
	}

	result = ezcfg_soap_get_element_content_by_index(soap, child_index);
	if (result == NULL) {
		rc = -EZCFG_E_RESULT ;
		goto sem_exit;
	}

	if (strcmp(result, EZCFG_SOAP_NVRAM_RESULT_VALUE_OK) == 0) {
		rc = 0;
	}
	else {
		rc = -EZCFG_E_RESULT ;
	}

sem_exit:
	/* now release resource */
	res.sem_num = EZCFG_SEM_NVRAM_INDEX;
	res.sem_op = 1;
	res.sem_flg = 0;

	if (semop(semid, &res, 1) == -1) {
		DBG("<6>pid=[%d] semop release_res error\n", getpid());
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

exit:
	if (msg != NULL) {
		free(msg);
	}

        if (sh != NULL) {
		ezcfg_soap_http_delete(sh);
	}

        if (ezctrl != NULL) {
		ezcfg_ctrl_delete(ezctrl);
	}

        if (ezcfg != NULL) {
		ezcfg_delete(ezcfg);
	}

	return rc;
}
