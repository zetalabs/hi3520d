/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/http_html_apply.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-06-25   0.1       Write it from scratch
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

#if (HAVE_EZBOX_EZCFG_NLS == 1)
#include <locale.h>
#include <libintl.h>
#endif

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-http.h"
#include "ezcfg-html.h"
#include "ezcfg-http_html_apply.h"

struct ezcfg_http_html_apply {
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_nvram *nvram;
	struct ezcfg_link_list *post_list;
	struct ezcfg_html *html;
};

struct http_html_apply_handler {
	char *section;
	int (*handler)(struct ezcfg_http_html_apply *apply);
};

struct http_html_apply_handler apply_handlers[] = {
	{ NULL, NULL }
};

/**
 * Private functions
 **/
static int generate_http_response(struct ezcfg_http_html_apply *apply)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_html *html;
	char *msg = NULL;
	int msg_len;
	int n;
	int rc = -1;
	bool ret = false;

	ezcfg = apply->ezcfg;
	http = apply->http;
	html = apply->html;

	msg_len = ezcfg_html_get_message_length(html);
	if (msg_len < 0) {
		err(ezcfg, "ezcfg_html_get_message_length\n");
		rc = -1;
		goto func_exit;
	}
	msg_len++; /* one more for '\0' */
	msg = (char *)malloc(msg_len);
	if (msg == NULL) {
		err(ezcfg, "malloc error.\n");
		rc = -1;
		goto func_exit;
	}

	memset(msg, 0, msg_len);
	n = ezcfg_html_write_message(html, msg, msg_len);
	if (n < 0) {
		err(ezcfg, "ezcfg_html_write_message\n");
		rc = -1;
		goto func_exit;
	}

	/* FIXME: name point to http->request_uri !!!
         * never reset http before using name */
	/* clean http structure info */
	ezcfg_http_reset_attributes(http);
	ezcfg_http_set_status_code(http, 200);
	ezcfg_http_set_state_response(http);

	if (ezcfg_http_set_message_body(http, msg, n) == NULL) {
		err(ezcfg, "ezcfg_http_set_message_body\n");
		rc = -1;
		goto func_exit;
	}

	ret = ezcfg_http_html_apply_set_http_html_common_header(apply);
	if (ret == false) {
		err(ezcfg, "ezcfg_http_html_apply_set_http_html_common_header error.\n");
		rc = -1;
		goto func_exit;
	}

	/* set return value */
	rc = 0;
func_exit:
	if (msg != NULL)
		free(msg);

	return rc;
}

static bool parse_post_data(struct ezcfg_http_html_apply *apply)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_link_list *list;

	ASSERT(apply != NULL);

	ezcfg = apply->ezcfg;
	http = apply->http;

	list = ezcfg_link_list_new(ezcfg);
	if (list == NULL) {
		return false;
	}
	apply->post_list = list;
	return ezcfg_http_parse_post_data(http, list);
}

static bool validate_post_data(struct ezcfg_http_html_apply *apply)
{
	//struct ezcfg *ezcfg;
	struct ezcfg_link_list *list;
	struct ezcfg_nvram *nvram;
	int i;
	int list_length;
	char *name, *value;
	bool ret;

	ASSERT(apply != NULL);

	//ezcfg = apply->ezcfg;
	nvram = apply->nvram;
	list = apply->post_list;

	list_length = ezcfg_link_list_get_length(list);
	for(i = 1; i < list_length+1; i++) {
		name = ezcfg_link_list_get_node_name_by_index(list, i);
		value = ezcfg_link_list_get_node_value_by_index(list, i);
		ret = ezcfg_nvram_is_valid_entry_value(nvram, name, value);
		if (ret == false) {
			return false;
		}
	}
	return true;
}

/**
 * Public functions
 **/
int ezcfg_http_handle_apply_request(struct ezcfg_http *http, struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http_html_apply *apply = NULL;
	char *request_uri;
	char *section;
	struct http_html_apply_handler *ah;
	int ret = -1;

	ASSERT(http != NULL);
	ASSERT(nvram != NULL);

	ezcfg = ezcfg_http_get_ezcfg(http);

	request_uri = ezcfg_http_get_request_uri(http);

	if (request_uri != NULL && 
	    strncmp(request_uri, EZCFG_HTTP_HTML_APPLY_PREFIX_URI,
	            strlen(EZCFG_HTTP_HTML_APPLY_PREFIX_URI)) == 0) {
		/* apply prefix uri=[/admin/apply/] */
		section = request_uri+strlen(EZCFG_HTTP_HTML_APPLY_PREFIX_URI);
		ah = apply_handlers;
		while(ah->section != NULL) {
			if (strcmp(ah->section, section) == 0) {
				apply = ezcfg_http_html_apply_new(ezcfg, http, nvram);
				if (apply != NULL) {
					ret = ah->handler(apply);
					if ((ret >= 0) && (apply->html != NULL)) {
						ret = generate_http_response(apply);
					}
				}
				break;
			}
			ah++;
		}
	}

	if (apply != NULL) {
		ezcfg_http_html_apply_delete(apply);
	}
	return ret;
}

struct ezcfg_http_html_apply *ezcfg_http_html_apply_new(
	struct ezcfg *ezcfg,
	struct ezcfg_http *http,
	struct ezcfg_nvram *nvram)
{
	struct ezcfg_http_html_apply *apply;

	ASSERT(ezcfg != NULL);
	ASSERT(http != NULL);
	ASSERT(nvram != NULL);

	apply = calloc(1, sizeof(struct ezcfg_http_html_apply));
	if (apply != NULL) {
		apply->ezcfg = ezcfg;
		apply->http = http;
		apply->nvram = nvram;
		apply->post_list = NULL;
		apply->html = NULL;
	}
	return apply;
}

void ezcfg_http_html_apply_delete(struct ezcfg_http_html_apply *apply)
{
	ASSERT(apply != NULL);

	if (apply->post_list != NULL) {
		ezcfg_link_list_delete(apply->post_list);
	}

	if (apply->html != NULL) {
		ezcfg_html_delete(apply->html);
	}

	free(apply);
}

bool ezcfg_http_html_apply_handle_post_data(struct ezcfg_http_html_apply *apply)
{
	//struct ezcfg *ezcfg;

	//ezcfg = apply->ezcfg;

	if (parse_post_data(apply) == true) {
		return validate_post_data(apply);
	}
	else {
		return false;
	}
}

bool ezcfg_http_html_apply_save_settings(struct ezcfg_http_html_apply *apply)
{
	//struct ezcfg *ezcfg;
	struct ezcfg_link_list *list;
	struct ezcfg_nvram *nvram;
	int i;
	int list_length;
	char *name, *value;
	bool ret;

	ASSERT(apply != NULL);

	//ezcfg = apply->ezcfg;
	nvram = apply->nvram;
	list = apply->post_list;

	list_length = ezcfg_link_list_get_length(list);
	for (i = 1; i < list_length+1; i++) {
		name = ezcfg_link_list_get_node_name_by_index(list, i);
		value = ezcfg_link_list_get_node_value_by_index(list, i);
		ret = ezcfg_nvram_set_entry(nvram, name, value);
		if (ret == false) {
			return false;
		}
	}
	if (list_length > 0) {
		ezcfg_nvram_commit(nvram);
	}
	return true;
}

struct ezcfg *ezcfg_http_html_apply_get_ezcfg(struct ezcfg_http_html_apply *apply)
{
	ASSERT(apply != NULL);

	return apply->ezcfg;
}

struct ezcfg_http *ezcfg_http_html_apply_get_http(struct ezcfg_http_html_apply *apply)
{
	ASSERT(apply != NULL);

	return apply->http;
}

struct ezcfg_html *ezcfg_http_html_apply_get_html(struct ezcfg_http_html_apply *apply)
{
	ASSERT(apply != NULL);

	return apply->html;
}

bool ezcfg_http_html_apply_set_html(struct ezcfg_http_html_apply *apply, struct ezcfg_html *html)
{
	ASSERT(apply != NULL);

	if (apply->html != NULL)
		ezcfg_html_delete(html);

	apply->html = html;
	return true;
}

struct ezcfg_nvram *ezcfg_http_html_apply_get_nvram(struct ezcfg_http_html_apply *apply)
{
	ASSERT(apply != NULL);

	return apply->nvram;
}

struct ezcfg_link_list *ezcfg_http_html_apply_get_post_list(struct ezcfg_http_html_apply *apply)
{
	ASSERT(apply != NULL);

	return apply->post_list;
}
