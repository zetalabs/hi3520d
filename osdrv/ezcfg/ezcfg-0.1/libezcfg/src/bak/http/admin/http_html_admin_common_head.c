/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/admin/http_html_admin_common_head.c
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

int ezcfg_http_html_admin_set_html_common_head(
	struct ezcfg_http_html_admin *admin,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	struct ezcfg_html *html;
	int child_index;
	char buf[1024];

	ASSERT(admin != NULL);
	ASSERT(pi > 0);

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	html = ezcfg_http_html_admin_get_html(admin);

	/* <meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/> */
	child_index = ezcfg_html_add_head_child(html, pi, si, EZCFG_HTML_META_ELEMENT_NAME, NULL);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child err.\n");
		goto func_exit;
	}
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_HTTP_EQUIV_ATTRIBUTE_NAME, EZCFG_HTTP_HEADER_CONTENT_TYPE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	snprintf(buf, sizeof(buf), "%s; %s=%s", EZCFG_HTTP_MIME_TEXT_HTML, EZCFG_HTTP_CHARSET_NAME, EZCFG_HTTP_CHARSET_UTF8);
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_CONTENT_ATTRIBUTE_NAME, buf, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <meta http-equiv="Cache-Control" content="no-cache"/> */
	child_index = ezcfg_html_add_head_child(html, pi, child_index, EZCFG_HTML_META_ELEMENT_NAME, NULL);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child err.\n");
		goto func_exit;
	}
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_HTTP_EQUIV_ATTRIBUTE_NAME, EZCFG_HTTP_HEADER_CACHE_CONTROL, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_CONTENT_ATTRIBUTE_NAME, EZCFG_HTTP_CACHE_REQUEST_NO_CACHE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <meta http-equiv="Expires" content="0"/> */
	child_index = ezcfg_html_add_head_child(html, pi, child_index, EZCFG_HTML_META_ELEMENT_NAME, NULL);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child err.\n");
		goto func_exit;
	}
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_HTTP_EQUIV_ATTRIBUTE_NAME, EZCFG_HTTP_HEADER_EXPIRES, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_CONTENT_ATTRIBUTE_NAME, "0", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <meta http-equiv="Pragma" content="no-cache"/> */
	child_index = ezcfg_html_add_head_child(html, pi, child_index, EZCFG_HTML_META_ELEMENT_NAME, NULL);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child err.\n");
		goto func_exit;
	}
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_HTTP_EQUIV_ATTRIBUTE_NAME, EZCFG_HTTP_HEADER_PRAGMA, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_CONTENT_ATTRIBUTE_NAME, EZCFG_HTTP_PRAGMA_NO_CACHE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <link rel="stylesheet" type="text/css" href="/admin/layout_css" /> */
	child_index = ezcfg_html_add_head_child(html, pi, child_index, EZCFG_HTML_LINK_ELEMENT_NAME, NULL);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child err.\n");
		goto func_exit;
	}
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_REL_ATTRIBUTE_NAME, "stylesheet", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, "text/css", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_HREF_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "layout_css", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

func_exit:
	return child_index;
}

bool ezcfg_http_html_admin_set_http_html_common_header(struct ezcfg_http_html_admin *admin)
{
	//struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	char buf[1024];
	bool ret;

	ASSERT(admin != NULL);

	//ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	http = ezcfg_http_html_admin_get_http(admin);

	/* HTML http-equiv content-type */
	snprintf(buf, sizeof(buf), "%s; %s=%s", EZCFG_HTTP_MIME_TEXT_HTML, EZCFG_HTTP_CHARSET_NAME, EZCFG_HTTP_CHARSET_UTF8);
	ret = ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CONTENT_TYPE, buf);
	if (ret == false) {
		return false;
	}

	/* HTML http-equiv cache-control */
	ret = ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CACHE_CONTROL, EZCFG_HTTP_CACHE_REQUEST_NO_CACHE);
	if (ret == false) {
		goto func_exit;
	}

	/* HTML http-equiv expires */
	ret = ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_EXPIRES, "0");
	if (ret == false) {
		goto func_exit;
	}

	/* HTML http-equiv pragma */
	ret = ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_PRAGMA, EZCFG_HTTP_PRAGMA_NO_CACHE);
	if (ret == false) {
		goto func_exit;
	}

	snprintf(buf, sizeof(buf), "%u", ezcfg_http_get_message_body_len(http));
	ret = ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CONTENT_LENGTH , buf);
	if (ret == false) {
		goto func_exit;
	}

func_exit:
	return ret;
}

bool ezcfg_http_html_admin_set_http_css_common_header(struct ezcfg_http_html_admin *admin)
{
	//struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	char buf[1024];
	bool ret;

	ASSERT(admin != NULL);

	//ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	http = ezcfg_http_html_admin_get_http(admin);

	/* text/css charset=UTF-8 */
	snprintf(buf, sizeof(buf), "%s; %s=%s", EZCFG_HTTP_MIME_TEXT_CSS, EZCFG_HTTP_CHARSET_NAME, EZCFG_HTTP_CHARSET_UTF8);
	ret = ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CONTENT_TYPE, buf);
	if (ret == false) {
		return false;
	}

	/* HTML http-equiv cache-control */
	ret = ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CACHE_CONTROL, EZCFG_HTTP_CACHE_REQUEST_NO_CACHE);
	if (ret == false) {
		goto func_exit;
	}

	/* HTML http-equiv expires */
	ret = ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_EXPIRES, "0");
	if (ret == false) {
		goto func_exit;
	}

	/* HTML http-equiv pragma */
	ret = ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_PRAGMA, EZCFG_HTTP_PRAGMA_NO_CACHE);
	if (ret == false) {
		goto func_exit;
	}

	snprintf(buf, sizeof(buf), "%u", ezcfg_http_get_message_body_len(http));
	ret = ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CONTENT_LENGTH , buf);
	if (ret == false) {
		goto func_exit;
	}

func_exit:
	return ret;
}

