/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/admin/http_html_admin_menu_management.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-05   0.1       Write it from scratch
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
static int submenu_management_authz(struct ezcfg_html *html, int pi, int si, struct ezcfg_locale *locale)
{
	struct ezcfg *ezcfg;
	int li2_index, a2_index;
	int ret = -1;

	ASSERT(html != NULL);
	ASSERT(pi > 1);

	ezcfg = ezcfg_html_get_ezcfg(html);

	/* management_authz */
	/* submenu <ul> <li> */
	li2_index = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_LI_ELEMENT_NAME, NULL);
	if (li2_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child err.\n");
		goto func_exit;
	}

	a2_index = -1;
	/* submenu <ul> <li> <a> */
	a2_index = ezcfg_html_add_body_child(html, li2_index, a2_index, EZCFG_HTML_A_ELEMENT_NAME, ezcfg_locale_text(locale, "Authorization"));
	if (a2_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child err.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, a2_index, EZCFG_HTML_HREF_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "management_authz", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* must return menu index */
	ret = li2_index;
func_exit:

	return ret;
}

static int submenu_management_default(struct ezcfg_html *html, int pi, int si, struct ezcfg_locale *locale)
{
	struct ezcfg *ezcfg;
	int li2_index, a2_index;
	int ret = -1;

	ASSERT(html != NULL);
	ASSERT(pi > 1);

	ezcfg = ezcfg_html_get_ezcfg(html);

	/* management_default */
	/* submenu <ul> <li> */
	li2_index = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_LI_ELEMENT_NAME, NULL);
	if (li2_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child err.\n");
		goto func_exit;
	}

	a2_index = -1;
	/* submenu <ul> <li> <a> */
	a2_index = ezcfg_html_add_body_child(html, li2_index, a2_index, EZCFG_HTML_A_ELEMENT_NAME, ezcfg_locale_text(locale, "Restore Defaults"));
	if (a2_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child err.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, a2_index, EZCFG_HTML_HREF_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "management_default", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* must return menu index */
	ret = li2_index;
func_exit:

	return ret;
}

static int submenu_management_upgrade(struct ezcfg_html *html, int pi, int si, struct ezcfg_locale *locale)
{
	struct ezcfg *ezcfg;
	int li2_index, a2_index;
	int ret = -1;

	ASSERT(html != NULL);
	ASSERT(pi > 1);

	ezcfg = ezcfg_html_get_ezcfg(html);

	/* management_upgrade */
	/* submenu <ul> <li> */
	li2_index = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_LI_ELEMENT_NAME, NULL);
	if (li2_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child err.\n");
		goto func_exit;
	}

	a2_index = -1;
	/* submenu <ul> <li> <a> */
	a2_index = ezcfg_html_add_body_child(html, li2_index, a2_index, EZCFG_HTML_A_ELEMENT_NAME, ezcfg_locale_text(locale, "System Upgrade"));
	if (a2_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child err.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, a2_index, EZCFG_HTML_HREF_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "management_upgrade", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* must return menu index */
	ret = li2_index;
func_exit:

	return ret;
}


/**
 * Public functions
 **/
int ezcfg_http_html_admin_html_menu_management(
	struct ezcfg_http_html_admin *admin,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	//struct ezcfg_nvram *nvram;
	struct ezcfg_html *html;
	struct ezcfg_locale *locale = NULL;
	int li_index, h3_index, a_index;
	int ul2_index, li2_index;
	char *request_uri, *section;
	int ret = -1;

	ASSERT(admin != NULL);
	ASSERT(pi > 1);

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	http = ezcfg_http_html_admin_get_http(admin);
	//nvram = ezcfg_http_html_admin_get_nvram(admin);
	html = ezcfg_http_html_admin_get_html(admin);

	request_uri = ezcfg_http_get_request_uri(http);
	section = request_uri+strlen(EZCFG_HTTP_HTML_ADMIN_PREFIX_URI);

        /* set locale info */
	locale = ezcfg_locale_new(ezcfg);
	if (locale != NULL) {
		ezcfg_locale_set_domain(locale, EZCFG_HTTP_HTML_ADMIN_MENU_DOMAIN);
		ezcfg_locale_set_dir(locale, EZCFG_HTTP_HTML_LANG_DIR);
	}

	/* menu <ul> <li> */
	li_index = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_LI_ELEMENT_NAME, NULL);
	if (li_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child err.\n");
		goto func_exit;
	}

	h3_index = -1;
	/* menu <ul> <li> <h3> */
	h3_index = ezcfg_html_add_body_child(html, li_index, h3_index, EZCFG_HTML_H3_ELEMENT_NAME, NULL);
	if (h3_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child err.\n");
		goto func_exit;
	}

	a_index = -1;
	/* menu <ul> <li> <a> */
	a_index = ezcfg_html_add_body_child(html, h3_index, a_index, EZCFG_HTML_A_ELEMENT_NAME, ezcfg_locale_text(locale, "Management"));
	if (a_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child err.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, a_index, EZCFG_HTML_HREF_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "management_authz", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	if ((strcmp(section, "management_authz") == 0) ||
	    (strcmp(section, "management_default") == 0) ||
	    (strcmp(section, "management_upgrade") == 0)) {
		/* submenu <ul> */
		ul2_index = ezcfg_html_add_body_child(html, li_index, h3_index, EZCFG_HTML_UL_ELEMENT_NAME, NULL);
		if (ul2_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child err.\n");
			goto func_exit;
		}

		li2_index = -1;
		/* management_authz */
		/* submenu <ul> <li> */
		li2_index = submenu_management_authz(html, ul2_index, li2_index, locale);
		if (li2_index < 0) {
			err(ezcfg, "submenu_management_authz err.\n");
			goto func_exit;
		}

		/* management_default */
		/* submenu <ul> <li> */
		li2_index = submenu_management_default(html, ul2_index, li2_index, locale);
		if (li2_index < 0) {
			err(ezcfg, "submenu_management_default err.\n");
			goto func_exit;
		}

		/* management_upgrade */
		/* submenu <ul> <li> */
		li2_index = submenu_management_upgrade(html, ul2_index, li2_index, locale);
		if (li2_index < 0) {
			err(ezcfg, "submenu_management_upgrade err.\n");
			goto func_exit;
		}
	}

	/* must return menu index */
	ret = li_index;
func_exit:
	if (locale != NULL)
		ezcfg_locale_delete(locale);

	return ret;
}
