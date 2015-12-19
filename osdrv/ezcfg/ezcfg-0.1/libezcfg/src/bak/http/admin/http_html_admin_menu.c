/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/admin/http_html_admin_menu.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-12   0.1       Write it from scratch
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

/**
 * Private functions
 **/

/**
 * Public functions
 **/

int ezcfg_http_html_admin_set_html_menu(
	struct ezcfg_http_html_admin *admin,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	//struct ezcfg_http *http;
	//struct ezcfg_nvram *nvram;
	struct ezcfg_html *html;
	struct ezcfg_locale *locale = NULL;
	int menu_index;
	int ul_index, li_index;
	int ret = -1;

	ASSERT(admin != NULL);
	ASSERT(pi > 1);

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	//http = ezcfg_http_html_admin_get_http(admin);
	//nvram = ezcfg_http_html_admin_get_nvram(admin);
	html = ezcfg_http_html_admin_get_html(admin);

        /* set locale info */
	locale = ezcfg_locale_new(ezcfg);
	if (locale != NULL) {
		ezcfg_locale_set_domain(locale, EZCFG_HTTP_HTML_ADMIN_MENU_DOMAIN);
		ezcfg_locale_set_dir(locale, EZCFG_HTTP_HTML_LANG_DIR);
	}

	/* <div id="menu"> */
	menu_index = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_DIV_ELEMENT_NAME, NULL);
	if (menu_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, menu_index, EZCFG_HTML_ID_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_MENU, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* menu <ul> */
	ul_index = ezcfg_html_add_body_child(html, menu_index, -1, EZCFG_HTML_UL_ELEMENT_NAME, NULL);
	if (ul_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child err.\n");
		goto func_exit;
	}

	/* menu <ul> <li> */
	/* menu Status */
	li_index = ezcfg_http_html_admin_html_menu_status(admin, ul_index, -1);
	if (li_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child err.\n");
		goto func_exit;
	}

	/* menu <ul> <li> */
	/* menu Setup */
	li_index = ezcfg_http_html_admin_html_menu_setup(admin, ul_index, li_index);
	if (li_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child err.\n");
		goto func_exit;
	}

	/* menu <ul> <li> */
	/* menu Management */
	li_index = ezcfg_http_html_admin_html_menu_management(admin, ul_index, li_index);
	if (li_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child err.\n");
		goto func_exit;
	}

	/* menu <ul> <li> */
	/* menu CNC */
#if (HAVE_EZBOX_SERVICE_EMC2 == 1)
	li_index = ezcfg_http_html_admin_html_menu_cnc(admin, ul_index, li_index);
	if (li_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child err.\n");
		goto func_exit;
	}
#endif

	/* must return menu index */
	ret = menu_index;
func_exit:
	if (locale != NULL)
		ezcfg_locale_delete(locale);

	return ret;
}

