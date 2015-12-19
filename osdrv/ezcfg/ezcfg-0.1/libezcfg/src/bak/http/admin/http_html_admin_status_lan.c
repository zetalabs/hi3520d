/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/admin/http_html_admin_status_lan.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-03   0.1       Write it from scratch
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

static int set_html_main_status_lan_dhcpd(
	struct ezcfg_http_html_admin *admin,
	struct ezcfg_locale *locale,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	//struct ezcfg_http *http;
	struct ezcfg_nvram *nvram;
	struct ezcfg_html *html;
	int content_index, child_index;
	int p_index;
	char buf[1024];
	char *p = NULL;
	int i;
	bool bool_flag;
	int ret = -1;

	ASSERT(admin != NULL);
	ASSERT(pi > 1);

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	//http = ezcfg_http_html_admin_get_http(admin);
	nvram = ezcfg_http_html_admin_get_nvram(admin);
	html = ezcfg_http_html_admin_get_html(admin);

	content_index = pi;
	child_index = si;

	/* <h3>DHCP Server</h3> */
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_H3_ELEMENT_NAME, ezcfg_locale_text(locale, "DHCP Server"));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Service Switch : Enabled</p> */
	i = -1;
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_ENABLE), &p);
	if (p != NULL) {
		i = atoi(p);
		free(p);
	}
	if ((i == 0) || (i == 1)) {
		bool_flag = true;
	}
	else {
		bool_flag = false;
	}
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "Service Switch"),
		ezcfg_locale_text(locale, " : "),
		(bool_flag == true) ? ezcfg_locale_text(locale, ezcfg_util_text_get_service_switch(i == 1)) : ezcfg_locale_text(locale, "Unknown Settings"));
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	if (bool_flag == false) {
		ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_CLASS_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_P_CLASS_WARNING, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	}
	else {
		if (i == 1) {
			/* <p>Start IP Address : 192.168.1.50</p> */
			ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_START_IPADDR), &p);
			snprintf(buf, sizeof(buf), "%s%s%s",
				ezcfg_locale_text(locale, "Start IP Address"),
				ezcfg_locale_text(locale, " : "),
				(p != NULL) ? p : ezcfg_locale_text(locale, "Unknown Start IP Address"));
			if (p != NULL) {
				bool_flag = true;
				free(p);
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

			/* <p>End IP Address : 192.168.1.50</p> */
			ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_END_IPADDR), &p);
			snprintf(buf, sizeof(buf), "%s%s%s",
				ezcfg_locale_text(locale, "End IP Address"),
				ezcfg_locale_text(locale, " : "),
				(p != NULL) ? p : ezcfg_locale_text(locale, "Unknown End IP Address"));
			if (p != NULL) {
				bool_flag = true;
				free(p);
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

			/* <p>Client Lease Time : 86400 Seconds</p> */
			ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_LEASE), &p);
			snprintf(buf, sizeof(buf), "%s%s%s%s%s",
				ezcfg_locale_text(locale, "Client Lease Time"),
				ezcfg_locale_text(locale, " : "),
				(p != NULL) ? p : ezcfg_locale_text(locale, "Invalid Lease Time"),
				ezcfg_locale_text(locale, " "),
				(p != NULL) ? ezcfg_locale_text(locale, "Seconds") : "");
			if (p != NULL) {
				bool_flag = true;
				free(p);
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

			/* <p><a href="/admin/view_dhcp_client_table">View DHCP Client Table</a></p> */
			child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, NULL);
			if (child_index < 0) {
				err(ezcfg, "ezcfg_html_add_body_child error.\n");
				goto func_exit;
			}
			/* save <p> index */
			p_index = child_index;
			child_index = -1;

			child_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_A_ELEMENT_NAME, ezcfg_locale_text(locale, "View DHCP Client Table"));
			if (child_index < 0) {
				err(ezcfg, "ezcfg_html_add_body_child error.\n");
				goto func_exit;
			}

			ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_HREF_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "view_dhcp_client_table", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
			/* restore <p> index */
			child_index = p_index;
		}
	}

	si = child_index;

	/* must return main index */
	ret = si;

func_exit:
	return ret;
}

static int set_html_main_status_lan(
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
	char *p = NULL;
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
	/* <h3>LAN Information</h3> */
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_H3_ELEMENT_NAME, ezcfg_locale_text(locale, "LAN Information"));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Physical Address : 00:11:22:33:44:55</p> */
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(LAN, HWADDR), &p);
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "Physical Address"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? p : ezcfg_locale_text(locale, "Unknown Physical Address"));
	if (p != NULL) {
		bool_flag = true;
		free(p);
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

	/* <p>IP Address : 192.168.1.1</p> */
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(LAN, IPADDR), &p);
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "IP Address"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? p : ezcfg_locale_text(locale, "Unknown IP Address"));
	if (p != NULL) {
		bool_flag = true;
		free(p);
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

	/* <p>Subnet Mask : 255.255.255.0</p> */
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(LAN, NETMASK), &p);
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "Subnet Mask"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? p : ezcfg_locale_text(locale, "Unknown Subnet Mask"));
	if (p != NULL) {
		bool_flag = true;
		free(p);
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

	buf[0] = '\0';
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(RC, DNSMASQ_ENABLE), &p);
	if (p != NULL) {
		snprintf(buf, sizeof(buf), "%s", p);
		free(p);
	}

	if (strcmp(buf, "1") == 0) {
		child_index = set_html_main_status_lan_dhcpd(admin, locale, content_index, child_index);
		if (child_index < 0) {
			err(ezcfg, "set_html_main_setup_lan_dhcpd error.\n");
			goto func_exit;
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

static int build_admin_status_lan_response(struct ezcfg_http_html_admin *admin)
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
		ezcfg_locale_set_domain(locale, EZCFG_HTTP_HTML_ADMIN_STATUS_LAN_DOMAIN);
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
	child_index = ezcfg_html_add_head_child(html, head_index, child_index, EZCFG_HTML_TITLE_ELEMENT_NAME, ezcfg_locale_text(locale, "LAN Status"));
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
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, "status_lan", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_METHOD_ATTRIBUTE_NAME, "post", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_ACTION_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "status_lan", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* HTML div head */
	child_index = ezcfg_http_html_admin_set_html_head(admin, form_index, -1);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_head error.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML div main */
	child_index = set_html_main_status_lan(admin, locale, form_index, child_index);
	if (child_index < 0) {
		err(ezcfg, "set_html_main_status_lan error.\n");
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

int ezcfg_http_html_admin_status_lan_handler(struct ezcfg_http_html_admin *admin)
{
	//struct ezcfg *ezcfg;
	int ret = -1;

	ASSERT(admin != NULL);

	//ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);

	/* admin status_lan uri=[/admin/status_lan] */
	ret = build_admin_status_lan_response(admin);
	return ret;
}
