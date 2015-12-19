/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/http_nvram.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-01-13   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-http.h"
#include "ezcfg-html.h"

struct ezcfg_http_nvram {
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_nvram *nvram;
	int content_type;
	char *root;
	char *path;
};

/**
 * Private functions
 **/
static int build_http_nvram_response(struct ezcfg_http_nvram *hn)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_nvram *nvram;
	char buf[1024];
	char *p, *q;
	char *nv_name, *js_name;
	FILE *fp = NULL;
	int len = 0;
	int msg_len = 0;
	char *msg = NULL;
	int rc = -1;
	
	ezcfg = hn->ezcfg;
	http = hn->http;
	nvram = hn->nvram;

	/* FIXME: name point to http->request_uri !!!
         * never reset http before using name */
	/* clean http structure info */
	ezcfg_http_reset_attributes(http);
	ezcfg_http_set_status_code(http, 200);
	ezcfg_http_set_state_response(http);

	/* open nvram-js mapping list file */
	len = strlen(hn->root) + strlen(hn->path);
	if ((len - strlen(EZCFG_FILE_EXT_JS_STRING) +
	     strlen(EZCFG_FILE_EXT_NVJS_STRING)) < sizeof(buf)) {
		snprintf(buf, sizeof(buf), "%s%s", hn->root, hn->path);
		p = buf + len - strlen(EZCFG_FILE_EXT_JS_STRING);
		snprintf(p, sizeof(buf)-(p-buf), "%s", EZCFG_FILE_EXT_NVJS_STRING);
		fp = fopen(buf, "r");
		if (fp == NULL) {
			goto func_exit;
		}
	}

	/* build HTTP response */
	/* HTTP header content-type */
	snprintf(buf, sizeof(buf), "%s; %s=%s",
		EZCFG_HTTP_MIME_APPLICATION_X_JAVASCRIPT,
		EZCFG_HTTP_CHARSET_NAME,
		EZCFG_HTTP_CHARSET_UTF8);
	if (ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CONTENT_TYPE, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		goto func_exit;
	}

	/* read nvram-js mapping file */
	while (ezcfg_util_file_get_line(fp, buf, sizeof(buf), "#", EZCFG_FILE_LINE_TAIL_STRING) == true) {
		/* convert nvram to js variable */
		p = strchr(buf, ':');
		if (p != NULL) {
			*p = '\0';
			nv_name = buf;
			js_name = p+1;
			ezcfg_nvram_get_entry_value(nvram, nv_name, &p);
			if (p == NULL)
				continue;

			/* get escaped line */
			q = ezcfg_util_javascript_var_escaped(p);
			free(p);
			if (q == NULL)
				continue;

			len = strlen(q);
			len += strlen(js_name);
			len += 5; /* for ='...';\n */

			/* one more for 0-terminated string */
			p = realloc(msg, msg_len+len+1);
			if (p == NULL) {
				free(q);
				goto func_exit;
			}
			msg = p;
			p += msg_len;
			snprintf(p, len+1, "%s='%s';\n", js_name, q);
			msg_len += len;
			free(q);
		}
	}

	p = ezcfg_http_set_message_body(http, msg, msg_len);
	if (p == NULL) {
		goto func_exit;
	}

	/* clean msg */
	free(msg);
	msg = NULL;

	/* set return value */
	rc = 0;

func_exit:
	if (fp != NULL)
		fclose(fp);

	if (msg != NULL)
		free(msg);

	return rc;
}

/**
 * Public functions
 **/
void ezcfg_http_nvram_delete(struct ezcfg_http_nvram *hn)
{
	ASSERT(hn != NULL);

	if (hn->root != NULL)
		free(hn->root);

	if (hn->path != NULL)
		free(hn->path);

	free(hn);
}

struct ezcfg_http_nvram *ezcfg_http_nvram_new(struct ezcfg *ezcfg)
{
	struct ezcfg_http_nvram *hn;

	ASSERT(ezcfg != NULL);

	hn = malloc(sizeof(struct ezcfg_http_nvram));
	if (hn != NULL) {
		memset(hn, 0, sizeof(struct ezcfg_http_nvram));
		hn->ezcfg = ezcfg;
        }
	return hn;
}

bool ezcfg_http_nvram_set_http(struct ezcfg_http_nvram *hn, struct ezcfg_http *http)
{
	ASSERT(hn != NULL);
	ASSERT(http != NULL);

	hn->http = http;
	return true;
}

bool ezcfg_http_nvram_set_nvram(struct ezcfg_http_nvram *hn, struct ezcfg_nvram *nvram)
{
	ASSERT(hn != NULL);
	ASSERT(nvram != NULL);

	hn->nvram = nvram;
	return true;
}

int ezcfg_http_nvram_get_content_type(struct ezcfg_http_nvram *hn)
{
	ASSERT(hn != NULL);

	return hn->content_type;
}

bool ezcfg_http_nvram_set_content_type(struct ezcfg_http_nvram *hn, const int type)
{
	ASSERT(hn != NULL);
	ASSERT(type >= 0);

	hn->content_type = type;
	return true;
}

bool ezcfg_http_nvram_set_root(struct ezcfg_http_nvram *hn, const char *root)
{
	char *p;
	ASSERT(hn != NULL);
	ASSERT(root != NULL);

	p = strdup(root);
	if (p == NULL)
		return false;

	if (hn->root != NULL)
		free(hn->root);

	hn->root = p;
	return true;
}

bool ezcfg_http_nvram_set_path(struct ezcfg_http_nvram *hn, const char *path)
{
	char *p;
	ASSERT(hn != NULL);
	ASSERT(path != NULL);

	p = strdup(path);
	if (p == NULL)
		return false;

	if (hn->path != NULL)
		free(hn->path);

	hn->path = p;
	return true;
}

int ezcfg_http_handle_nvram_request(struct ezcfg_http_nvram *hn)
{
	//struct ezcfg *ezcfg;

	ASSERT(hn != NULL);
	ASSERT(hn->http != NULL);
	ASSERT(hn->nvram != NULL);

	//ezcfg = hn->ezcfg;

	return build_http_nvram_response(hn);
}
