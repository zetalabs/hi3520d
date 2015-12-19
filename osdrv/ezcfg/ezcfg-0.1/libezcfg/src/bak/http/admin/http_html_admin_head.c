/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/admin/http_html_admin_head.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-18   0.1       Write it from scratch
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

int ezcfg_http_html_admin_set_html_head(
	struct ezcfg_http_html_admin *admin,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	struct ezcfg_html *html;
	struct ezcfg_locale *locale = NULL;
	int head_index, child_index;
	int h1_index;
	int ret = -1;

	ASSERT(admin != NULL);
	ASSERT(pi > 1);

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	html = ezcfg_http_html_admin_get_html(admin);

        /* set locale info */
	locale = ezcfg_locale_new(ezcfg);
	if (locale != NULL) {
		ezcfg_locale_set_domain(locale, EZCFG_HTTP_HTML_ADMIN_HEAD_DOMAIN);
		ezcfg_locale_set_dir(locale, EZCFG_HTTP_HTML_LANG_DIR);
	}

	/* <div id="head"> */
	head_index = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_DIV_ELEMENT_NAME, NULL);
	if (head_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, head_index, EZCFG_HTML_ID_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_HEAD, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <h1><a href="http://code.google.com/p/ezbox/">ezbox</a></h1> */
	h1_index = ezcfg_html_add_body_child(html, head_index, -1, EZCFG_HTML_H1_ELEMENT_NAME, NULL);
	if (h1_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	child_index = ezcfg_html_add_body_child(html, h1_index, -1, EZCFG_HTML_A_ELEMENT_NAME, ezcfg_locale_text(locale, "ezbox"));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_HREF_ATTRIBUTE_NAME, EZBOX_PROJECT_HOME_PAGE_URI, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <h2>Open Platform for Embedded System Study</h2> */
	child_index = ezcfg_html_add_body_child(html, head_index, h1_index, EZCFG_HTML_H2_ELEMENT_NAME, ezcfg_locale_text(locale, "Open Platform for Embedded System Study"));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* must return menu index */
	ret = head_index;
func_exit:
	if (locale != NULL)
		ezcfg_locale_delete(locale);

	return ret;
}

