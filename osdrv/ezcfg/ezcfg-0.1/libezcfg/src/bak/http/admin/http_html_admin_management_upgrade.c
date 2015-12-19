/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/admin/http_html_admin_management_upgrade.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-06   0.1       Write it from scratch
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
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
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
static int set_html_main_management_upgrade(
	struct ezcfg_http_html_admin *admin,
	struct ezcfg_locale *locale,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	//struct ezcfg_nvram *nvram;
	struct ezcfg_html *html;
	int content_index, child_index;
	int p_index;
	int input_index;
	char buf[1024];
	int ret = -1;

	ASSERT(admin != NULL);
	ASSERT(pi > 1);

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	//nvram = ezcfg_http_html_admin_get_nvram(admin);
	html = ezcfg_http_html_admin_get_html(admin);

	/* <div id="main"> */
	si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_DIV_ELEMENT_NAME, NULL);
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, si, EZCFG_HTML_ID_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_MAIN, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <div id="menu"> */
	child_index = ezcfg_http_html_admin_set_html_menu(admin, si, -1);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_menu.\n");
		goto func_exit;
	}

	/* <div id="content"> */
	content_index = ezcfg_html_add_body_child(html, si, child_index, EZCFG_HTML_DIV_ELEMENT_NAME, NULL);
	if (content_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, content_index, EZCFG_HTML_ID_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_CONTENT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	child_index = -1;
	/* <h3>System Upgrade</h3> */
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_H3_ELEMENT_NAME, ezcfg_locale_text(locale, "System Upgrade"));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>System Image : </p> */
	snprintf(buf, sizeof(buf), "%s%s",
		ezcfg_locale_text(locale, "System Image"),
		ezcfg_locale_text(locale, " : "));
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>System Image : <input type="file" name="sys_upgrade_image"/></p> */
	/* save <p> index */
	p_index = child_index;
	child_index = -1;

	input_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
	if (input_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_FILE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(SYS, UPGRADE_IMAGE), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* restore <p> index */
	child_index = p_index;

	/* <p class="warning">Waring : Upgrading system may take a few minutes; please don't turn off the power.</p> */
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "Warning"),
		ezcfg_locale_text(locale, " : "),
		ezcfg_locale_text(locale, "Upgrading system may take a few minutes; please don't turn off the power."));
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_CLASS_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_P_CLASS_WARNING, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <br /> */
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_BR_ELEMENT_NAME, NULL);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <input> buttons part */
	child_index = ezcfg_http_html_admin_set_html_button(admin, content_index, child_index);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_button.\n");
		goto func_exit;
	}

	/* must return main index */
	ret = si;

func_exit:
	return ret;
}

static int build_admin_management_upgrade_response(struct ezcfg_http_html_admin *admin)
{
	struct ezcfg *ezcfg;
	struct ezcfg_html *html = NULL;
	struct ezcfg_locale *locale = NULL;
	int head_index, body_index, child_index;
	int container_index, form_index;
	int rc = 0;
	
	ASSERT(admin != NULL);

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);

	/* set locale info */
	locale = ezcfg_locale_new(ezcfg);
	if (locale != NULL) {
		ezcfg_locale_set_domain(locale, EZCFG_HTTP_HTML_ADMIN_MANAGEMENT_UPGRADE_DOMAIN);
		ezcfg_locale_set_dir(locale, EZCFG_HTTP_HTML_LANG_DIR);
	}

	html = ezcfg_html_new(ezcfg);

	if (html == NULL) {
		err(ezcfg, "can not alloc html.\n");
		rc = -1;
		goto func_exit;
	}

	/* set admin->html */
	ezcfg_http_html_admin_set_html(admin, html);

	/* clean HTML structure info */
	ezcfg_html_reset_attributes(html);

	/* build HTML */
	ezcfg_html_set_version_major(html, 4);
	ezcfg_html_set_version_minor(html, 1);

	/* HTML root */
	rc = ezcfg_html_set_root(html, EZCFG_HTML_HTML_ELEMENT_NAME);
	if (rc < 0) {
		err(ezcfg, "ezcfg_html_set_root.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML Head */
	head_index = ezcfg_html_set_head(html, EZCFG_HTML_HEAD_ELEMENT_NAME);
	if (head_index < 0) {
		err(ezcfg, "ezcfg_html_set_head error.\n");
		rc = -1;
		goto func_exit;
	}

	/* set admin common Head */
	child_index = ezcfg_http_html_admin_set_html_common_head(admin, head_index, -1);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_common_head error.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML Title */
	child_index = ezcfg_html_add_head_child(html, head_index, child_index, EZCFG_HTML_TITLE_ELEMENT_NAME, ezcfg_locale_text(locale, "System Upgrade"));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child error.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML Body */
	body_index = ezcfg_html_set_body(html, EZCFG_HTML_BODY_ELEMENT_NAME);
	if (body_index < 0) {
		err(ezcfg, "ezcfg_html_set_body error.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML div container */
	container_index = ezcfg_html_add_body_child(html, body_index, -1, EZCFG_HTML_DIV_ELEMENT_NAME, NULL);
	if (container_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		rc = -1;
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, container_index, EZCFG_HTML_ID_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_CONTAINER, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* HTML form */
	form_index = ezcfg_html_add_body_child(html, container_index, -1, EZCFG_HTML_FORM_ELEMENT_NAME, NULL);
	if (form_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		rc = -1;
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, "management_upgrade", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_METHOD_ATTRIBUTE_NAME, "post", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_ACTION_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "management_upgrade", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_ENCTYPE_ATTRIBUTE_NAME, "multipart/form-data", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* HTML div head */
	child_index = ezcfg_http_html_admin_set_html_head(admin, form_index, -1);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_head error.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML div main */
	child_index = set_html_main_management_upgrade(admin, locale, form_index, child_index);
	if (child_index < 0) {
		err(ezcfg, "set_html_main_management_upgrade error.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML div foot */
	child_index = ezcfg_http_html_admin_set_html_foot(admin, form_index, child_index);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_foot error.\n");
		rc = -1;
		goto func_exit;
	}

	/* set return value */
	rc = 0;
func_exit:
	if (locale != NULL)
		ezcfg_locale_delete(locale);

	return rc;
}

static bool do_admin_management_upgrade_action(struct ezcfg_http_html_admin *admin)
{
	//struct ezcfg *ezcfg;
	//struct ezcfg_link_list *list;
	bool ret = false;

	//ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	//list = ezcfg_http_html_admin_get_post_list(admin);

	if (ezcfg_http_html_admin_get_action(admin) == HTTP_HTML_ADMIN_ACT_SAVE) {
		ret = ezcfg_http_html_admin_save_settings(admin);
	}
	return ret;
}

static bool handle_admin_management_upgrade_post(struct ezcfg_http_html_admin *admin)
{
	//struct ezcfg *ezcfg;
	bool ret = false;

	//ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);

	if (ezcfg_http_html_admin_handle_post_data(admin) == true) {
		ret = do_admin_management_upgrade_action(admin);
	}
	return ret;
}

/**
 * Public functions
 **/
int ezcfg_http_html_admin_management_upgrade_handler(struct ezcfg_http_html_admin *admin)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	int ret = -1;

	ASSERT(admin != NULL);

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	http = ezcfg_http_html_admin_get_http(admin);

	/* admin management_upgrade uri=[/admin/management_upgrade] */
	if (ezcfg_http_request_method_cmp(http, EZCFG_HTTP_METHOD_POST) == 0) {
		/* do post handling */
		info(ezcfg, "[%s]\n", ezcfg_http_get_message_body(http));
		handle_admin_management_upgrade_post(admin);
	}

	ret = build_admin_management_upgrade_response(admin);
	return ret;
}
