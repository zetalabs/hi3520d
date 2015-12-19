/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/admin/http_html_admin_cnc_latency.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-07-29   0.1       Write it from scratch
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

static int set_html_main_cnc_latency(
	struct ezcfg_http_html_admin *admin,
	struct ezcfg_locale *locale,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_nvram *nvram;
	struct ezcfg_html *html;
	int main_index;
	int content_index, child_index;
	int p_index, input_index;
	char buf[1024];
	int ret = -1;
	char *p = NULL;

	ASSERT(admin != NULL);
	ASSERT(pi > 1);

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	http = ezcfg_http_html_admin_get_http(admin);
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
	/* <h3>Latency Test Control</h3> */
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_H3_ELEMENT_NAME, ezcfg_locale_text(locale, "Latency Test Control"));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	if (ezcfg_nvram_match_entry_value(nvram, NVRAM_SERVICE_OPTION(EMC2, LAT_TEST_START), "1") == true) {
		/* <p>Test has been started : <input type="radio" name="emc2_lat_test_start" value="0">Stop</input></p> */
		snprintf(buf, sizeof(buf), "%s%s",
			ezcfg_locale_text(locale, "Test has been started"),
			ezcfg_locale_text(locale, " : "));
		child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}

		/* save <p> index */
		p_index = child_index;
		child_index = -1;

		/* <input /> */
		input_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
		if (input_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_RADIO, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(EMC2, LAT_TEST_START), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, "0", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

		/* <i> Stop </i> */
		snprintf(buf, sizeof(buf), " %s ",
			ezcfg_locale_text(locale, "Stop"));
			child_index = ezcfg_html_add_body_child(html, p_index, input_index, EZCFG_HTML_I_ELEMENT_NAME, buf);
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}

		/* restore <p> index */
		child_index = p_index;

		/* <p>Reset Test : <input type="radio" name="emc2_lat_test_reset" value="1">Yes</input></p> */
		snprintf(buf, sizeof(buf), "%s%s",
			ezcfg_locale_text(locale, "Reset Test"),
			ezcfg_locale_text(locale, " : "));
		child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}

		/* save <p> index */
		p_index = child_index;
		child_index = -1;

		/* <input /> */
		input_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
		if (input_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_RADIO, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(EMC2, LAT_TEST_RESET), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, "1", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

		/* <i> Yes </i> */
		snprintf(buf, sizeof(buf), " %s ",
			ezcfg_locale_text(locale, "Yes"));
			child_index = ezcfg_html_add_body_child(html, p_index, input_index, EZCFG_HTML_I_ELEMENT_NAME, buf);
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}

		/* <input /> */
		input_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
		if (input_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_RADIO, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(EMC2, LAT_TEST_RESET), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, "0", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_CHECKED_ATTRIBUTE_NAME, EZCFG_HTML_CHECKED_ATTRIBUTE_NAME, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

		/* <i> No </i> */
		snprintf(buf, sizeof(buf), " %s ",
			ezcfg_locale_text(locale, "No"));
			child_index = ezcfg_html_add_body_child(html, p_index, input_index, EZCFG_HTML_I_ELEMENT_NAME, buf);
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}

		/* restore <p> index */
		child_index = p_index;
	}
	else {
		/* <p>Test has been stopped : <input type="radio" name="emc2_lat_test_start" value="1">Start</input></p> */
		snprintf(buf, sizeof(buf), "%s%s",
			ezcfg_locale_text(locale, "Test has been stopped"),
			ezcfg_locale_text(locale, " : "));
		child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}

		/* save <p> index */
		p_index = child_index;
		child_index = -1;

		/* <input /> */
		input_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
		if (input_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_RADIO, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(EMC2, LAT_TEST_START), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, "1", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

		/* <i> Start </i> */
		snprintf(buf, sizeof(buf), " %s ",
			ezcfg_locale_text(locale, "Start"));
		child_index = ezcfg_html_add_body_child(html, p_index, input_index, EZCFG_HTML_I_ELEMENT_NAME, buf);
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}

		/* restore <p> index */
		child_index = p_index;

		if (ezcfg_nvram_match_entry_value(nvram, NVRAM_SERVICE_OPTION(RC, EMC2_ENABLE), "1") == true) {
			/* <p>Warning : Machine is running, will stop it automatically!</p> */
			snprintf(buf, sizeof(buf), "%s%s%s",
				ezcfg_locale_text(locale, "Warning"),
				ezcfg_locale_text(locale, " : "),
				ezcfg_locale_text(locale, "Machine is running, will stop it automatically!"));
			child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
			if (child_index < 0) {
				err(ezcfg, "ezcfg_html_add_body_child error.\n");
				goto func_exit;
			}
			ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_CLASS_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_P_CLASS_WARNING, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
                }
	}

	/* <h3>Latency Test Information</h3> */
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_H3_ELEMENT_NAME, ezcfg_locale_text(locale, "Latency Test Information"));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Let this test run for a few minutes, then note the maximum Jitter.</p> */
	snprintf(buf, sizeof(buf), "%s",
		ezcfg_locale_text(locale, "Let this test run for a few minutes, then note the maximum Jitter."));
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>You will use it while configuring CNC.</p> */
	snprintf(buf, sizeof(buf), "%s",
		ezcfg_locale_text(locale, "You will use it while configuring CNC."));
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <h3>Servo Thread (1.0ms)</h3> */
	snprintf(buf, sizeof(buf), "%s (%.2f%s)",
		ezcfg_locale_text(locale, "Servo Thread"),
		1.00, ezcfg_locale_text(locale, "ms"));
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_H3_ELEMENT_NAME, buf);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Max Interval (ns) : 86400</p> */
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EMC2, LAT_SERVO_MAX_INTERVAL), &p);
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "Max Interval (ns)"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? p : ezcfg_locale_text(locale, "Not Ready"));
	if (p != NULL) {
		free(p);
	}
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Max Jitter (ns) : 86400</p> */
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EMC2, LAT_SERVO_MAX_JITTER), &p);
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "Max Jitter (ns)"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? p : ezcfg_locale_text(locale, "Not Ready"));
	if (p != NULL) {
		free(p);
	}
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_CLASS_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_P_CLASS_WARNING, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <p>Last Interval (ns) : 86400</p> */
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EMC2, LAT_SERVO_LAST_INTERVAL), &p);
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "Last Interval (ns)"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? p : ezcfg_locale_text(locale, "Not Ready"));
	if (p != NULL) {
		free(p);
	}
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <h3>Base Thread (25.0µs)</h3> */
	snprintf(buf, sizeof(buf), "%s (%.2f%s)",
		ezcfg_locale_text(locale, "Base Thread"),
		25.00, ezcfg_locale_text(locale, "µs"));
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_H3_ELEMENT_NAME, buf);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Max Interval (ns) : 86400</p> */
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EMC2, LAT_BASE_MAX_INTERVAL), &p);
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "Max Interval (ns)"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? p : ezcfg_locale_text(locale, "Not Ready"));
	if (p != NULL) {
		free(p);
	}
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Max Jitter (ns) : 86400</p> */
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EMC2, LAT_BASE_MAX_JITTER), &p);
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "Max Jitter (ns)"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? p : ezcfg_locale_text(locale, "Not Ready"));
	if (p != NULL) {
		free(p);
	}
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_CLASS_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_P_CLASS_WARNING, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <p>Last Interval (ns) : 86400</p> */
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EMC2, LAT_BASE_LAST_INTERVAL), &p);
	snprintf(buf, sizeof(buf), "%s%s%s",
		ezcfg_locale_text(locale, "Last Interval (ns)"),
		ezcfg_locale_text(locale, " : "),
		(p != NULL) ? p : ezcfg_locale_text(locale, "Not Ready"));
	if (p != NULL) {
		free(p);
	}
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
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

static bool do_admin_cnc_latency_action(struct ezcfg_http_html_admin *admin)
{
	struct ezcfg *ezcfg;
	struct ezcfg_link_list *list;
	bool ret = false;

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	list = ezcfg_http_html_admin_get_post_list(admin);

	if (ezcfg_http_html_admin_get_action(admin) == HTTP_HTML_ADMIN_ACT_SAVE) {
		ret = ezcfg_http_html_admin_save_settings(admin);
	}
	return ret;
}

static bool handle_admin_cnc_latency_post(struct ezcfg_http_html_admin *admin)
{
	struct ezcfg *ezcfg;
	bool ret = false;

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);

	if (ezcfg_http_html_admin_handle_post_data(admin) == true) {
		ret = do_admin_cnc_latency_action(admin);
	}
	return ret;
}

static int build_admin_cnc_latency_response(struct ezcfg_http_html_admin *admin)
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
		ezcfg_locale_set_domain(locale, EZCFG_HTTP_HTML_ADMIN_CNC_LATENCY_DOMAIN);
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
	child_index = ezcfg_html_add_head_child(html, head_index, child_index, EZCFG_HTML_TITLE_ELEMENT_NAME, ezcfg_locale_text(locale, "CNC Latency Test"));
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
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, "cnc_latency", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_METHOD_ATTRIBUTE_NAME, "post", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_ACTION_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "cnc_latency", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* HTML div head */
	child_index = ezcfg_http_html_admin_set_html_head(admin, form_index, -1);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_head error.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML div main */
	child_index = set_html_main_cnc_latency(admin, locale, form_index, child_index);
	if (child_index < 0) {
		err(ezcfg, "set_html_main_cnc_latency error.\n");
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

int ezcfg_http_html_admin_cnc_latency_handler(struct ezcfg_http_html_admin *admin)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_nvram *nvram;
	int ret = -1;
	char old_start[2];
	char *p;

	ASSERT(admin != NULL);

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	http = ezcfg_http_html_admin_get_http(admin);
	nvram = ezcfg_http_html_admin_get_nvram(admin);

	/* admin cnc_latency uri=[/admin/cnc_latency] */
	if (ezcfg_http_request_method_cmp(http, EZCFG_HTTP_METHOD_POST) == 0) {
		/* get old emc2_lat_test_start status */
		old_start[0] = '\0';
		ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EMC2, LAT_TEST_START), &p);
		if (p != NULL) {
			snprintf(old_start, sizeof(old_start), "%s", p);
			free(p);
		}

		/* do post handling */
		handle_admin_cnc_latency_post(admin);

		/* do service actions */
		/* first check start/stop test */
		if (ezcfg_nvram_match_entry_value(nvram, NVRAM_SERVICE_OPTION(EMC2, LAT_TEST_START), "1") == true) {
			if (strcmp(old_start, "1") != 0) {
				/* first check if the machine is running */
				if (ezcfg_nvram_match_entry_value(nvram, NVRAM_SERVICE_OPTION(RC, EMC2_ENABLE), "1") == true) {
					ezcfg_util_rc(EZCFG_RC_SERVICE_EMC2, EZCFG_RC_ACT_STOP, 0);
					ezcfg_nvram_set_entry(nvram, NVRAM_SERVICE_OPTION(RC, EMC2_ENABLE), "0");
				}
				/* then start machine latency test */
				ezcfg_util_rc(EZCFG_RC_SERVICE_EMC2_LATENCY_TEST, EZCFG_RC_ACT_START, 0);
				/* sleep a while */
				sleep(5);
			}
			/* now check reset test */
			if (ezcfg_nvram_match_entry_value(nvram, NVRAM_SERVICE_OPTION(EMC2, LAT_TEST_RESET), "1") == true) {
				ezcfg_util_rc(EZCFG_RC_SERVICE_EMC2_LATENCY_TEST, EZCFG_RC_ACT_RELOAD, 0);
				ezcfg_nvram_set_entry(nvram, NVRAM_SERVICE_OPTION(EMC2, LAT_TEST_RESET), "0");
				/* sleep a while */
				sleep(3);
			}
		}
		else if (ezcfg_nvram_match_entry_value(nvram, NVRAM_SERVICE_OPTION(EMC2, LAT_TEST_START), "0") == true) {
			if (strcmp(old_start, "1") == 0) {
				ezcfg_util_rc(EZCFG_RC_SERVICE_EMC2_LATENCY_TEST, EZCFG_RC_ACT_STOP, 0);
			}
		}
	}

	ret = build_admin_cnc_latency_response(admin);
	return ret;
}
