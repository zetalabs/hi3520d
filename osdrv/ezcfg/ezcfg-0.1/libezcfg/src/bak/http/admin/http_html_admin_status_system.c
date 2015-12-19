/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/admin/http_html_admin_status_system.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-11   0.1       Write it from scratch
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
static int set_html_main_status_system(
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
	char lang[16];
	char tz_area[32];
	char tz_location[64];
	char *p = NULL, *q = NULL;
	struct sysinfo info;
	time_t tm;
	struct tm tms;
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
	/* <h3>Device Information</h3> */
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_H3_ELEMENT_NAME, ezcfg_locale_text(locale, "Device Information"));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Device Name : ezbox </p> */
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(SYS, DEVICE_NAME), &p);
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "Device Name"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? p : ezcfg_locale_text(locale, "Unknown Device"));
	if (p != NULL) {
		free(p);
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

	/* <p>Serial Number : 0123456789 </p> */
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(SYS, SERIAL_NUMBER), &p);
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "Serial Number"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? p : ezcfg_locale_text(locale, "Invalid Serial Number"));
	if (p != NULL) {
		free(p);
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

	/* <p>Hardware Version : 1.0 </p> */
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(SYS, HARDWARE_VERSION), &p);
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "Hardware Version"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? p : ezcfg_locale_text(locale, "Invalid Version"));
	if (p != NULL) {
		free(p);
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

	/* <p>Software Version : 1.0 </p> */
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(SYS, SOFTWARE_VERSION), &p);
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "Software Version"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? p : ezcfg_locale_text(locale, "Invalid Version"));
	if (p != NULL) {
		free(p);
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

	/* <h3>System Configurations</h3> */
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_H3_ELEMENT_NAME, ezcfg_locale_text(locale, "System Configurations"));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Language : Chinese (China) </p> */
	lang[0] = '\0';
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EZCFG, SYS_LANGUAGE), &p);
	if (p != NULL) {
		snprintf(lang, sizeof(lang), "%s", p);
		free(p);
	}
	p = ezcfg_util_lang_get_desc_by_name(lang);
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "Language"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? p : ezcfg_locale_text(locale, "Unknown Language"));
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

	/* <p>Time Zone : Asia/Shanghai</p> */
	tz_area[0] = '\0';
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EZCFG, SYS_TZ_AREA), &p);
	if (p != NULL) {
		snprintf(tz_area, sizeof(tz_area), "%s", p);
		free(p);
	}
	tz_location[0] = '\0';
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EZCFG, SYS_TZ_LOCATION), &p);
	if (p != NULL) {
		snprintf(tz_location, sizeof(tz_location), "%s", p);
		free(p);
	}
	p = ezcfg_util_tzdata_get_area_desc_by_name(tz_area);
	q = ezcfg_util_tzdata_get_location_desc_by_name(tz_area, tz_location);
	if ((p != NULL) && (q != NULL)) {
		bool_flag = true;
	}
	else {
		bool_flag = false;
	}
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "Time Zone"),
		(bool_flag == true) ? "" : ezcfg_locale_text(locale, " : "),
		(bool_flag == true) ? "" : ezcfg_locale_text(locale, "Unknown Time Zone"));
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	if (bool_flag == true) {
		snprintf(buf, sizeof(buf), "%s%s%s%s",
			ezcfg_locale_text(locale, " ("),
			ezcfg_locale_text(locale, "Area"),
			ezcfg_locale_text(locale, ") : "),
			ezcfg_locale_text(locale, p));
		child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		snprintf(buf, sizeof(buf), "%s%s%s%s",
			ezcfg_locale_text(locale, " ("),
			ezcfg_locale_text(locale, "Location"),
			ezcfg_locale_text(locale, ") : "),
			ezcfg_locale_text(locale, q));
		child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
	}
	else {
		ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_CLASS_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_P_CLASS_WARNING, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	}

	/* <p>Current Time : Thu Apr 21 18:10:50 2011</p> */
	time(&tm);
	memcpy(&tms, localtime(&tm), sizeof(struct tm));
	p = NULL;
	if ((bool_flag == true) &&
	    (time(0) > (time_t)60*60*24*365)) {
		p = malloc(256);
		if (p != NULL) {
			strftime(p, 255, "%Y-%m-%d %H:%M:%S", &tms);
		}
	}
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "Current Time"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? p : ezcfg_locale_text(locale, "Not Available"));
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (p != NULL) {
		free(p);
		bool_flag = true;
	}
	else {
		bool_flag = false;
	}
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	if (bool_flag == false) {
		ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_CLASS_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_P_CLASS_WARNING, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	}

	/* <h3>Running Status</h3> */
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_H3_ELEMENT_NAME, ezcfg_locale_text(locale, "Running Status"));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	if (sysinfo(&info) == -1) {
		/* access system info error */
		/* <p>System running status is not available right now...</p> */
		child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, ezcfg_locale_text(locale, "System running status is not available right now, please check it later."));
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_CLASS_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_P_CLASS_WARNING, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	}
	else {
		if (info.mem_unit == 1) {
			info.mem_unit = 1024;
			/* TODO: Make all this stuff not overflow when mem >= 4 GiB */
			info.totalram /= info.mem_unit;
			info.freeram /= info.mem_unit;
			info.sharedram /= info.mem_unit;
			info.bufferram /= info.mem_unit;
			info.totalswap /= info.mem_unit;
			info.freeswap /= info.mem_unit;
		}
		else {
			info.mem_unit /= 1024;
			/* TODO: Make all this stuff not overflow when mem >= 4 GiB */
			info.totalram *= info.mem_unit;
			info.freeram *= info.mem_unit;
			info.sharedram *= info.mem_unit;
			info.bufferram *= info.mem_unit;
			info.totalswap *= info.mem_unit;
			info.freeswap *= info.mem_unit;
		}

		/* <p>Up Time : 10000 Seconds </p> */
		snprintf(buf, sizeof(buf), "%s%s%ld%s%s",
			ezcfg_locale_text(locale, "Up Time"),
			ezcfg_locale_text(locale, " : "),
			info.uptime,
			ezcfg_locale_text(locale, " "),
			ezcfg_locale_text(locale, "Seconds"));
		child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}

		/* <p>Total Memory : 65536 KB </p> */
		snprintf(buf, sizeof(buf), "%s%s%lu%s%s",
			ezcfg_locale_text(locale, "Total Memory"),
			ezcfg_locale_text(locale, " : "),
			info.totalram,
			ezcfg_locale_text(locale, " "),
			ezcfg_locale_text(locale, "KB"));
		child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}

		/* <p>Free Memory : 16384 KB </p> */
		snprintf(buf, sizeof(buf), "%s%s%lu%s%s",
			ezcfg_locale_text(locale, "Free Memory"),
			ezcfg_locale_text(locale, " : "),
			info.freeram,
			ezcfg_locale_text(locale, " "),
			ezcfg_locale_text(locale, "KB"));
		child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}

		/* <p>Total Swap : 65536 KB </p> */
		snprintf(buf, sizeof(buf), "%s%s%lu%s%s",
			ezcfg_locale_text(locale, "Total Swap"),
			ezcfg_locale_text(locale, " : "),
			info.totalswap,
			ezcfg_locale_text(locale, " "),
			ezcfg_locale_text(locale, "KB"));
		child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}

		/* <p>Free Swap : 16384 KB </p> */
		snprintf(buf, sizeof(buf), "%s%s%lu%s%s",
			ezcfg_locale_text(locale, "Free Swap"),
			ezcfg_locale_text(locale, " : "),
			info.freeswap,
			ezcfg_locale_text(locale, " "),
			ezcfg_locale_text(locale, "KB"));
		child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
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

static int build_admin_status_system_response(struct ezcfg_http_html_admin *admin)
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
		ezcfg_locale_set_domain(locale, EZCFG_HTTP_HTML_ADMIN_STATUS_SYSTEM_DOMAIN);
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
	child_index = ezcfg_html_add_head_child(html, head_index, child_index, EZCFG_HTML_TITLE_ELEMENT_NAME, ezcfg_locale_text(locale, "System Status"));
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
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, "status_system", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_METHOD_ATTRIBUTE_NAME, "post", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_ACTION_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "status_system", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* HTML div head */
	child_index = ezcfg_http_html_admin_set_html_head(admin, form_index, -1);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_head error.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML div main */
	child_index = set_html_main_status_system(admin, locale, form_index, child_index);
	if (child_index < 0) {
		err(ezcfg, "set_html_main_status_system error.\n");
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

#if 0
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
#endif

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

int ezcfg_http_html_admin_status_system_handler(struct ezcfg_http_html_admin *admin)
{
	//struct ezcfg *ezcfg;
	int ret = -1;

	ASSERT(admin != NULL);

	//ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);

	/* admin status_system uri=[/admin/status_system] */
	ret = build_admin_status_system_response(admin);
	return ret;
}
