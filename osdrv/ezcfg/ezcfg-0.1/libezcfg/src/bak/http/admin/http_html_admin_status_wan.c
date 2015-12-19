/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/admin/http_html_admin_status_wan.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-27   0.1       Write it from scratch
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
static int set_status_wan_common(
	struct ezcfg_http_html_admin *admin,
	struct ezcfg_locale *locale,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	//struct ezcfg_http *http;
	struct ezcfg_nvram *nvram;
	struct ezcfg_html *html;
	char buf[1024];
	char *p = NULL;
	int i;
	bool bool_flag;

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	//http = ezcfg_http_html_admin_get_http(admin);
	nvram = ezcfg_http_html_admin_get_nvram(admin);
	html = ezcfg_http_html_admin_get_html(admin);

	/* <p>IP Address : 0.0.0.0</p>*/
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, IPADDR), &p);
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "IP Address"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? p : ezcfg_locale_text(locale, "Invalid IP Address"));
	if (p != NULL) {
		bool_flag = true;
		free(p);
	}
	else {
		bool_flag = false;
	}
	si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	if (bool_flag == false) {
		ezcfg_html_add_body_child_attribute(html, si, EZCFG_HTML_CLASS_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_P_CLASS_WARNING, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	}

	/* <p>Subnet Mask : 0.0.0.0</p>*/
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, NETMASK), &p);
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "Subnet Mask"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? p : ezcfg_locale_text(locale, "Invalid Subnet Mask"));
	if (p != NULL) {
		bool_flag = true;
		free(p);
	}
	else {
		bool_flag = false;
	}
	si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	if (bool_flag == false) {
		ezcfg_html_add_body_child_attribute(html, si, EZCFG_HTML_CLASS_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_P_CLASS_WARNING, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	}

	/* <p>Default Gateway : 0.0.0.0</p>*/
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, GATEWAY), &p);
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "Default Gateway"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? p : ezcfg_locale_text(locale, "Invalid Default Gateway"));
	if (p != NULL) {
		bool_flag = true;
		free(p);
	}
	else {
		bool_flag = false;
	}
	si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	if (bool_flag == false) {
		ezcfg_html_add_body_child_attribute(html, si, EZCFG_HTML_CLASS_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_P_CLASS_WARNING, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	}

	/* <p>DNS X : 0.0.0.0</p>*/
	for (i = 1; i <= 3; i++) {
		snprintf(buf, sizeof(buf), "%s%d", NVRAM_SERVICE_OPTION(WAN, DNS), i);
		ezcfg_nvram_get_entry_value(nvram, buf, &p);
		snprintf(buf, sizeof(buf), "%s %d%s%s",
			ezcfg_locale_text(locale, "DNS"), i,
			ezcfg_locale_text(locale, " : "),
			(p != NULL) ? p : "");
		if (p != NULL) {
			bool_flag = true;
			free(p);
		}
		else {
			bool_flag = false;
		}
		si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, buf);
		if (si < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		if (bool_flag == false) {
			ezcfg_html_add_body_child_attribute(html, si, EZCFG_HTML_CLASS_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_P_CLASS_WARNING, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		}
	}

	/* <p>MTU : 1500</p>*/
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, MTU), &p);
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "MTU"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? p : ezcfg_locale_text(locale, "Invalid MTU"));
	if (p != NULL) {
		bool_flag = true;
		free(p);
	}
	else {
		bool_flag = false;
	}
	si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	if (bool_flag == false) {
		ezcfg_html_add_body_child_attribute(html, si, EZCFG_HTML_CLASS_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_P_CLASS_WARNING, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	}

func_exit:
	return si;
}

static int set_status_wan_dhcp(
	struct ezcfg_http_html_admin *admin,
	struct ezcfg_locale *locale,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	//struct ezcfg_http *http;
	struct ezcfg_nvram *nvram;
	struct ezcfg_html *html;
	int p_index;
	char buf[1024];
	char *p = NULL;
	bool bool_flag;

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	//http = ezcfg_http_html_admin_get_http(admin);
	nvram = ezcfg_http_html_admin_get_nvram(admin);
	html = ezcfg_http_html_admin_get_html(admin);

	/* <p>WAN TYPE DHCP</p>*/
	si = set_status_wan_common(admin, locale, pi, si);
	if (si < 0) {
		err(ezcfg, "set_status_wan_common.\n");
		goto func_exit;
	}

	/* <p>DHCP Lease Time : 86400</p>*/
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, DHCP_LEASE), &p);
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "DHCP Lease Time"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? p : ezcfg_locale_text(locale, "Invalid Lease Time"));
	if (p != NULL) {
		bool_flag = true;
		free(p);
	}
	else {
		bool_flag = false;
	}
	si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	if (bool_flag == false) {
		ezcfg_html_add_body_child_attribute(html, si, EZCFG_HTML_CLASS_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_P_CLASS_WARNING, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	}

	/* <p></p>*/
	si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, NULL);
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	p_index = si;
	si = -1;

	/* <input type="submit" name="act_release" value="Release IP Address" /> */
	si = ezcfg_html_add_body_child(html, p_index, si, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, si, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_SUBMIT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, si, EZCFG_HTML_NAME_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_NAME_ACT_RELEASE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, si, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, ezcfg_locale_text(locale, "Release IP Address"), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <input type="submit" name="act_renew" value="Renew IP Address" /> */
	si = ezcfg_html_add_body_child(html, p_index, si, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, si, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_SUBMIT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, si, EZCFG_HTML_NAME_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_NAME_ACT_RENEW, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, si, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, ezcfg_locale_text(locale, "Renew IP Address"), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	si = p_index;
func_exit:
	return si;
}

static int set_status_wan_static(
	struct ezcfg_http_html_admin *admin,
	struct ezcfg_locale *locale,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	//struct ezcfg_http *http;
	//struct ezcfg_nvram *nvram;
	//struct ezcfg_html *html;

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	//http = ezcfg_http_html_admin_get_http(admin);
	//nvram = ezcfg_http_html_admin_get_nvram(admin);
	//html = ezcfg_http_html_admin_get_html(admin);

	/* <p>WAN TYPE STATIC</p>*/
	si = set_status_wan_common(admin, locale, pi, si);
	if (si < 0) {
		err(ezcfg, "set_status_wan_common.\n");
		goto func_exit;
	}

func_exit:
	return si;
}

#if (HAVE_EZBOX_WAN_PPPOE == 1)
static int set_status_wan_pppoe(
	struct ezcfg_http_html_admin *admin,
	struct ezcfg_locale *locale,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_nvram *nvram;
	struct ezcfg_html *html;

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	http = ezcfg_http_html_admin_get_http(admin);
	nvram = ezcfg_http_html_admin_get_nvram(admin);
	html = ezcfg_http_html_admin_get_html(admin);

	/* <p>WAN TYPE PPPOE</p>*/
	si = set_status_wan_common(admin, locale, pi, si);
	if (si < 0) {
		err(ezcfg, "set_status_wan_common.\n");
		goto func_exit;
	}

func_exit:
	return si;
}
#endif

#if (HAVE_EZBOX_WAN_PPTP == 1)
static int set_status_wan_pptp(
	struct ezcfg_http_html_admin *admin,
	struct ezcfg_locale *locale,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_nvram *nvram;
	struct ezcfg_html *html;

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	http = ezcfg_http_html_admin_get_http(admin);
	nvram = ezcfg_http_html_admin_get_nvram(admin);
	html = ezcfg_http_html_admin_get_html(admin);

	/* <p>WAN TYPE PPTP</p>*/
	si = set_status_wan_common(admin, locale, pi, si);
	if (si < 0) {
		err(ezcfg, "set_status_wan_common.\n");
		goto func_exit;
	}

func_exit:
	return si;
}
#endif

#if (HAVE_EZBOX_WAN_L2TP == 1)
static int set_status_wan_l2tp(
	struct ezcfg_http_html_admin *admin,
	struct ezcfg_locale *locale,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_nvram *nvram;
	struct ezcfg_html *html;

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	http = ezcfg_http_html_admin_get_http(admin);
	nvram = ezcfg_http_html_admin_get_nvram(admin);
	html = ezcfg_http_html_admin_get_html(admin);

	/* <p>WAN TYPE L2TP</p>*/
	si = set_status_wan_common(admin, locale, pi, si);
	if (si < 0) {
		err(ezcfg, "set_status_wan_common.\n");
		goto func_exit;
	}

func_exit:
	return si;
}
#endif

struct type_handler {
	char *name;
	int (*handler)(
		struct ezcfg_http_html_admin *admin,
		struct ezcfg_locale *locale,
		int pi, int si);
};

static struct type_handler wan_type_handlers[] = {
	{ "dhcp", set_status_wan_dhcp },
	{ "static", set_status_wan_static },
#if (HAVE_EZBOX_WAN_PPPOE == 1)
	{ "pppoe", set_status_wan_pppoe },
#endif
#if (HAVE_EZBOX_WAN_PPTP == 1)
	{ "pptp", set_status_wan_pptp },
#endif
#if (HAVE_EZBOX_WAN_L2TP == 1)
	{ "l2tp", set_status_wan_l2tp },
#endif
};

static int set_html_main_status_wan(
	struct ezcfg_http_html_admin *admin,
	struct ezcfg_locale *locale,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	//struct ezcfg_http *http;
	struct ezcfg_nvram *nvram;
	struct ezcfg_html *html;
	int main_index;
	int content_index, child_index;
	char buf[1024];
	char wan_type[16];
	char *p = NULL;
	size_t i;
	bool bool_flag;
	int ret = -1;

	ASSERT(admin != NULL);
	ASSERT(pi > 1);

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	//http = ezcfg_http_html_admin_get_http(admin);
	nvram = ezcfg_http_html_admin_get_nvram(admin);
	html = ezcfg_http_html_admin_get_html(admin);

	/* <div id="main"> */
	main_index = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_DIV_ELEMENT_NAME, NULL);
	if (main_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, main_index, EZCFG_HTML_ID_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_MAIN, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <div id="menu"> */
	child_index = ezcfg_http_html_admin_set_html_menu(admin, main_index, -1);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_menu.\n");
		goto func_exit;
	}

	/* <div id="content"> */
	content_index = ezcfg_html_add_body_child(html, main_index, child_index, EZCFG_HTML_DIV_ELEMENT_NAME, NULL);
	if (content_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, content_index, EZCFG_HTML_ID_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_CONTENT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	child_index = -1;
	/* <h3>WAN Information</h3> */
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_H3_ELEMENT_NAME, ezcfg_locale_text(locale, "WAN Information"));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Connection Type : Automatic Configuration - DHCP </p> */
	wan_type[0] = '\0';
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, TYPE), &p);
	if (p != NULL) {
		snprintf(wan_type, sizeof(wan_type), "%s", p);
		free(p);
	}
	p = ezcfg_util_wan_get_type_desc_by_name(wan_type);
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "Connection Type"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? ezcfg_locale_text(locale, p) : ezcfg_locale_text(locale, "Unknown Type"));
	if (p != NULL) {
		bool_flag = true;
	}
	else {
		bool_flag = false;
	}
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	if (bool_flag == false) {
		ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_CLASS_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_P_CLASS_WARNING, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	}
	else {
		/* show WAN type related info */
		for (i = 0; i < ARRAY_SIZE(wan_type_handlers); i++) {
			if (strcmp(wan_type, wan_type_handlers[i].name) == 0) {
				child_index = wan_type_handlers[i].handler(admin, locale, content_index, child_index);
				if (child_index < 0) {
					err(ezcfg, "set_status_wan_%s.\n", wan_type);
					goto func_exit;
				}
				/* stop loop */
				break;
			}
		}
	}

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
	ret = main_index;

func_exit:
	return ret;
}

static int build_admin_status_wan_response(struct ezcfg_http_html_admin *admin)
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
		ezcfg_locale_set_domain(locale, EZCFG_HTTP_HTML_ADMIN_STATUS_WAN_DOMAIN);
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
	child_index = ezcfg_html_add_head_child(html, head_index, child_index, EZCFG_HTML_TITLE_ELEMENT_NAME, ezcfg_locale_text(locale, "WAN Status"));
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
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, "status_wan", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_METHOD_ATTRIBUTE_NAME, "post", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_ACTION_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "status_wan", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* HTML div head */
	child_index = ezcfg_http_html_admin_set_html_head(admin, form_index, -1);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_head error.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML div main */
	child_index = set_html_main_status_wan(admin, locale, form_index, child_index);
	if (child_index < 0) {
		err(ezcfg, "set_html_main_status_wan error.\n");
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

/**
 * Public functions
 **/

int ezcfg_http_html_admin_status_wan_handler(struct ezcfg_http_html_admin *admin)
{
	//struct ezcfg *ezcfg;
	int ret = -1;

	ASSERT(admin != NULL);

	//ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);

	/* admin status_wan uri=[/admin/status_wan] */
	ret = build_admin_status_wan_response(admin);
	return ret;
}
