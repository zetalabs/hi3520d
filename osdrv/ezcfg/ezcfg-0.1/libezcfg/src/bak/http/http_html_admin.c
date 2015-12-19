/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/http_html_admin.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-03-25   0.1       Write it from scratch
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
#include "ezcfg-http_html_admin.h"

struct ezcfg_http_html_admin {
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_nvram *nvram;
	struct ezcfg_link_list *post_list;
	struct ezcfg_html *html;
};

struct http_html_admin_handler {
	char *section;
	int (*handler)(struct ezcfg_http_html_admin *admin);
};

struct http_html_admin_handler adm_handlers[] = {
	{ "status_system", ezcfg_http_html_admin_status_system_handler },
#if (HAVE_EZBOX_LAN_NIC == 1)
	{ "status_lan", ezcfg_http_html_admin_status_lan_handler },
#endif
	{ "view_dhcp_client_table", ezcfg_http_html_admin_view_dhcp_client_table_handler },
#if (HAVE_EZBOX_WAN_NIC == 1)
	{ "status_wan", ezcfg_http_html_admin_status_wan_handler },
#endif
	{ "setup_system", ezcfg_http_html_admin_setup_system_handler },
#if (HAVE_EZBOX_LAN_NIC == 1)
	{ "setup_lan", ezcfg_http_html_admin_setup_lan_handler },
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
	{ "setup_wan", ezcfg_http_html_admin_setup_wan_handler },
#endif
	{ "management_authz", ezcfg_http_html_admin_management_authz_handler },
	{ "management_default", ezcfg_http_html_admin_management_default_handler },
	{ "management_upgrade", ezcfg_http_html_admin_management_upgrade_handler },
#if (HAVE_EZBOX_SERVICE_EMC2 == 1)
	{ "cnc_setup", ezcfg_http_html_admin_cnc_setup_handler },
	{ "cnc_default", ezcfg_http_html_admin_cnc_default_handler },
	{ "cnc_latency", ezcfg_http_html_admin_cnc_latency_handler },
#endif
	{ "layout_css", ezcfg_http_html_admin_layout_css_handler },
	{ NULL, NULL }
};

/**
 * Private functions
 **/
static int generate_http_response(struct ezcfg_http_html_admin *admin)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_html *html;
	char *msg = NULL;
	int msg_len;
	int n;
	int rc = -1;
	bool ret = false;

	ezcfg = admin->ezcfg;
	http = admin->http;
	html = admin->html;

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

	ret = ezcfg_http_html_admin_set_http_html_common_header(admin);
	if (ret == false) {
		err(ezcfg, "ezcfg_http_html_admin_set_http_html_common_header error.\n");
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

static bool parse_post_data(struct ezcfg_http_html_admin *admin)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_link_list *list;

	ASSERT(admin != NULL);

	ezcfg = admin->ezcfg;
	http = admin->http;

	list = ezcfg_link_list_new(ezcfg);
	if (list == NULL) {
		return false;
	}
	admin->post_list = list;
	return ezcfg_http_parse_post_data(http, list);
}

static bool validate_post_data(struct ezcfg_http_html_admin *admin)
{
	//struct ezcfg *ezcfg;
	struct ezcfg_link_list *list;
	struct ezcfg_nvram *nvram;
	int i;
	int list_length;
	char *name, *value;
	bool ret;

	ASSERT(admin != NULL);

	//ezcfg = admin->ezcfg;
	nvram = admin->nvram;
	list = admin->post_list;

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
int ezcfg_http_handle_admin_request(struct ezcfg_http *http, struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http_html_admin *admin = NULL;
	char *request_uri;
	char *section;
	struct http_html_admin_handler *ah;
	int ret = -1;

	ASSERT(http != NULL);
	ASSERT(nvram != NULL);

	ezcfg = ezcfg_http_get_ezcfg(http);

	request_uri = ezcfg_http_get_request_uri(http);

	if (request_uri != NULL && 
	    strncmp(request_uri, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI,
		strlen(EZCFG_HTTP_HTML_ADMIN_PREFIX_URI)) == 0) {
		/* admin prefix uri=[/admin/] */
		section = request_uri+strlen(EZCFG_HTTP_HTML_ADMIN_PREFIX_URI);
		if (*section == '\0') {
			if (ezcfg_http_set_request_uri(http, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "status_system") == true) {
				request_uri = ezcfg_http_get_request_uri(http);
				section = request_uri+strlen(EZCFG_HTTP_HTML_ADMIN_PREFIX_URI);
			}
		}
		ah = adm_handlers;
		while(ah->section != NULL) {
			if (strcmp(ah->section, section) == 0) {
				admin = ezcfg_http_html_admin_new(ezcfg, http, nvram);
				if (admin != NULL) {
					ret = ah->handler(admin);
					if ((ret >= 0) && (admin->html != NULL)) {
						ret = generate_http_response(admin);
					}
				}
				break;
			}
			ah++;
		}
	}

	if (admin != NULL) {
		ezcfg_http_html_admin_delete(admin);
	}
	return ret;
}

struct ezcfg_http_html_admin *ezcfg_http_html_admin_new(
	struct ezcfg *ezcfg,
	struct ezcfg_http *http,
	struct ezcfg_nvram *nvram)
{
	struct ezcfg_http_html_admin *admin;

	ASSERT(ezcfg != NULL);
	ASSERT(http != NULL);
	ASSERT(nvram != NULL);

	admin = calloc(1, sizeof(struct ezcfg_http_html_admin));
	if (admin != NULL) {
		admin->ezcfg = ezcfg;
		admin->http = http;
		admin->nvram = nvram;
		admin->post_list = NULL;
		admin->html = NULL;
	}
	return admin;
}

void ezcfg_http_html_admin_delete(struct ezcfg_http_html_admin *admin)
{
	ASSERT(admin != NULL);

	if (admin->post_list != NULL) {
		ezcfg_link_list_delete(admin->post_list);
	}

	if (admin->html != NULL) {
		ezcfg_html_delete(admin->html);
	}

	free(admin);
}

bool ezcfg_http_html_admin_handle_post_data(struct ezcfg_http_html_admin *admin)
{
	//struct ezcfg *ezcfg;

	//ezcfg = admin->ezcfg;

	if (parse_post_data(admin) == true) {
		return validate_post_data(admin);
	}
	else {
		return false;
	}
}

int ezcfg_http_html_admin_get_action(struct ezcfg_http_html_admin *admin)
{
	//struct ezcfg *ezcfg;
	struct ezcfg_link_list *list;

	ASSERT(admin != NULL);

	//ezcfg = admin->ezcfg;

	list = admin->post_list;

	if (list != NULL) {
		if (ezcfg_link_list_in(list, "act_save") == true)
			return HTTP_HTML_ADMIN_ACT_SAVE;
		else if (ezcfg_link_list_in(list, "act_refresh") == true)
			return HTTP_HTML_ADMIN_ACT_REFRESH;
		else
			return HTTP_HTML_ADMIN_ACT_CANCEL;
	}
	return HTTP_HTML_ADMIN_ACT_CANCEL;
}

bool ezcfg_http_html_admin_save_settings(struct ezcfg_http_html_admin *admin)
{
	//struct ezcfg *ezcfg;
	struct ezcfg_link_list *list;
	struct ezcfg_nvram *nvram;
	int i;
	int list_length;
	char *name, *value;
	bool ret;

	ASSERT(admin != NULL);

	//ezcfg = admin->ezcfg;
	nvram = admin->nvram;
	list = admin->post_list;

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

struct ezcfg *ezcfg_http_html_admin_get_ezcfg(struct ezcfg_http_html_admin *admin)
{
	ASSERT(admin != NULL);

	return admin->ezcfg;
}

struct ezcfg_http *ezcfg_http_html_admin_get_http(struct ezcfg_http_html_admin *admin)
{
	ASSERT(admin != NULL);

	return admin->http;
}

struct ezcfg_html *ezcfg_http_html_admin_get_html(struct ezcfg_http_html_admin *admin)
{
	ASSERT(admin != NULL);

	return admin->html;
}

bool ezcfg_http_html_admin_set_html(struct ezcfg_http_html_admin *admin, struct ezcfg_html *html)
{
	ASSERT(admin != NULL);

	if (admin->html != NULL)
		ezcfg_html_delete(html);

	admin->html = html;
	return true;
}

struct ezcfg_nvram *ezcfg_http_html_admin_get_nvram(struct ezcfg_http_html_admin *admin)
{
	ASSERT(admin != NULL);

	return admin->nvram;
}

struct ezcfg_link_list *ezcfg_http_html_admin_get_post_list(struct ezcfg_http_html_admin *admin)
{
	ASSERT(admin != NULL);

	return admin->post_list;
}
