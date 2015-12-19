/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/admin/http_html_admin_setup_system.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-21   0.1       Write it from scratch
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
static int set_html_main_setup_system(
	struct ezcfg_http_html_admin *admin,
	struct ezcfg_locale *locale,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	struct ezcfg_nvram *nvram;
	struct ezcfg_html *html;
	int main_index;
	int content_index, child_index;
	int p_index, select_index;
	char buf[1024];
	char tz_area[32];
	char tz_location[64];
	char *p = NULL;
	int i;
	int ret = -1;

	ASSERT(admin != NULL);
	ASSERT(pi > 1);

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
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
	/* <h3>Localization</h3> */
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_H3_ELEMENT_NAME, ezcfg_locale_text(locale, "Localization"));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Language : </p> */
	snprintf(buf, sizeof(buf), "%s%s",
		ezcfg_locale_text(locale, "Language"),
		ezcfg_locale_text(locale, " : "));
	p_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (p_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Language : <select></select> </p> */
	child_index = -1;
	select_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_SELECT_ELEMENT_NAME, NULL);
	if (select_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, select_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(EZCFG, SYS_LANGUAGE), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <p>Language : <select><option></option></select> </p> */
	buf[0] = '\0';
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EZCFG, SYS_LANGUAGE), &p);
	if (p != NULL) {
		snprintf(buf, sizeof(buf), "%s", p);
		free(p);
	}
	child_index = -1;
	for (i = 0; i < ezcfg_util_lang_get_length(); i++) {
		child_index = ezcfg_html_add_body_child(html, select_index, child_index, EZCFG_HTML_OPTION_ELEMENT_NAME, ezcfg_locale_text(locale, ezcfg_util_lang_get_desc_by_index(i)));
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, ezcfg_util_lang_get_name_by_index(i), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		if (strcmp(buf, ezcfg_util_lang_get_name_by_index(i)) == 0) {
			ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		}
	}

	/* <p>Time Zone</p> */
	snprintf(buf, sizeof(buf), "%s",
		ezcfg_locale_text(locale, "Time Zone"));
	p_index = ezcfg_html_add_body_child(html, content_index, p_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (p_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>  (Area) : </p> */
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, " ("),
		ezcfg_locale_text(locale, "Area"),
		ezcfg_locale_text(locale, ") : "));
	p_index = ezcfg_html_add_body_child(html, content_index, p_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (p_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>  (Area) : <select></select> </p> */
	child_index = -1;
	select_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_SELECT_ELEMENT_NAME, NULL);
	if (select_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, select_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(UI, TZ_AREA), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <p>  (Area) : <select><option></option></select> </p> */
	tz_area[0] = '\0';
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(UI, TZ_AREA), &p);
	if (p == NULL) {
		/* ui_tz_area is not set, use sys_tz_area */
		ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EZCFG, SYS_TZ_AREA), &p);
	}
	if (p != NULL) {
		snprintf(tz_area, sizeof(tz_area), "%s", p);
		free(p);
	}

	child_index = -1;
	for (i = 0; i < ezcfg_util_tzdata_get_area_length(); i++) {
		child_index = ezcfg_html_add_body_child(html, select_index, child_index, EZCFG_HTML_OPTION_ELEMENT_NAME, ezcfg_locale_text(locale, ezcfg_util_tzdata_get_area_desc_by_index(i)));
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, ezcfg_util_tzdata_get_area_name_by_index(i), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		if (strcmp(tz_area, ezcfg_util_tzdata_get_area_name_by_index(i)) == 0) {
			ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		}
	}

	if (ezcfg_nvram_match_entry_value(nvram, NVRAM_SERVICE_OPTION(EZCFG, SYS_TZ_AREA), tz_area) == false) {
		/* <p>  (Warning : time zone area has been changed, please set location again!)</p> */
		snprintf(buf, sizeof(buf), "%s%s%s%s%s",
			ezcfg_locale_text(locale, " ("),
			ezcfg_locale_text(locale, "Warning"),
			ezcfg_locale_text(locale, " : "),
			ezcfg_locale_text(locale, "time zone area has been changed, please set location again!"),
			ezcfg_locale_text(locale, ")"));
		p_index = ezcfg_html_add_body_child(html, content_index, p_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
		if (p_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, p_index, EZCFG_HTML_CLASS_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_P_CLASS_WARNING, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	}

	/* <p>  (Location) : </p> */
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, " ("),
		ezcfg_locale_text(locale, "Location"),
		ezcfg_locale_text(locale, ") : "));
	p_index = ezcfg_html_add_body_child(html, content_index, p_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (p_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>  (Location) : <select></select> </p> */
	child_index = -1;
	select_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_SELECT_ELEMENT_NAME, NULL);
	if (select_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, select_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(UI, TZ_LOCATION), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <p>  (Location) : <select><option></option></select> </p> */
	tz_location[0] = '\0';
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(UI, TZ_LOCATION), &p);
	if (p == NULL) {
		/* ui_tz_location is not set, use sys_tz_location */
		ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EZCFG, SYS_TZ_LOCATION), &p);
	}
	if (p != NULL) {
		snprintf(tz_location, sizeof(tz_location), "%s", p);
		free(p);
	}

	child_index = -1;
	for (i = 0; i < ezcfg_util_tzdata_get_location_length(tz_area); i++) {
		child_index = ezcfg_html_add_body_child(html, select_index, child_index, EZCFG_HTML_OPTION_ELEMENT_NAME, ezcfg_locale_text(locale, ezcfg_util_tzdata_get_location_desc_by_index(tz_area, i)));
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, ezcfg_util_tzdata_get_location_name_by_index(tz_area, i), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		if (strcmp(tz_location, ezcfg_util_tzdata_get_location_name_by_index(tz_area, i)) == 0) {
			ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		}
	}

	/* restore index pointer */
	child_index = p_index;

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

static int build_admin_setup_system_response(struct ezcfg_http_html_admin *admin)
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
		ezcfg_locale_set_domain(locale, EZCFG_HTTP_HTML_ADMIN_SETUP_SYSTEM_DOMAIN);
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
	child_index = ezcfg_html_add_head_child(html, head_index, child_index, EZCFG_HTML_TITLE_ELEMENT_NAME, ezcfg_locale_text(locale, "Setup System"));
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
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, "setup_system", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_METHOD_ATTRIBUTE_NAME, "post", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_ACTION_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "setup_system", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* HTML div head */
	child_index = ezcfg_http_html_admin_set_html_head(admin, form_index, -1);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_head error.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML div main */
	child_index = set_html_main_setup_system(admin, locale, form_index, child_index);
	if (child_index < 0) {
		err(ezcfg, "set_html_main_setup_system error.\n");
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

static bool do_admin_setup_system_action(struct ezcfg_http_html_admin *admin)
{
	//struct ezcfg *ezcfg;
	struct ezcfg_link_list *list;
	char *tz_area, *tz_location;
	bool ret = false;

	//ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	list = ezcfg_http_html_admin_get_post_list(admin);

	if (ezcfg_http_html_admin_get_action(admin) == HTTP_HTML_ADMIN_ACT_SAVE) {
		/* set area and location */
		tz_area = ezcfg_link_list_get_node_value_by_name(list, NVRAM_SERVICE_OPTION(UI, TZ_AREA));
		tz_location = ezcfg_link_list_get_node_value_by_name(list, NVRAM_SERVICE_OPTION(UI, TZ_LOCATION));
		if ((tz_area != NULL) && (tz_location != NULL)) {
			if (ezcfg_util_tzdata_check_area_location(tz_area, tz_location) == true) {
				ret = ezcfg_link_list_insert(list, NVRAM_SERVICE_OPTION(EZCFG, SYS_TZ_AREA), tz_area);
				if (ret == false) {
					return false;
				}
				ret = ezcfg_link_list_insert(list, NVRAM_SERVICE_OPTION(EZCFG, SYS_TZ_LOCATION), tz_location);
				if (ret == false) {
					return false;
				}
			}
		}

		ret = ezcfg_http_html_admin_save_settings(admin);
	}
	return ret;
}

static bool handle_admin_setup_system_post(struct ezcfg_http_html_admin *admin)
{
	//struct ezcfg *ezcfg;
	bool ret = false;

	//ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);

	if (ezcfg_http_html_admin_handle_post_data(admin) == true) {
		ret = do_admin_setup_system_action(admin);
	}
	return ret;
}

/**
 * Public functions
 **/
int ezcfg_http_html_admin_setup_system_handler(struct ezcfg_http_html_admin *admin)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	int ret = -1;

	ASSERT(admin != NULL);

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	http = ezcfg_http_html_admin_get_http(admin);

	/* admin setup_system uri=[/admin/setup_system] */
	if (ezcfg_http_request_method_cmp(http, EZCFG_HTTP_METHOD_POST) == 0) {
		/* do post handling */
		info(ezcfg, "[%s]\n", ezcfg_http_get_message_body(http));
		handle_admin_setup_system_post(admin);
	}

	ret = build_admin_setup_system_response(admin);
	return ret;
}
