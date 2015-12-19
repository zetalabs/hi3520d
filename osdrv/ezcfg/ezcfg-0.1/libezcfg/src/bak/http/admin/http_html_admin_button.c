/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/admin/http_html_admin_button.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-19   0.1       Write it from scratch
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

char *refresh_buttons[] = {
	"status_system",
#if (HAVE_EZBOX_LAN_NIC == 1)
	"status_lan",
#endif
	"view_dhcp_client_table",
#if (HAVE_EZBOX_WAN_NIC == 1)
	"status_wan",
#endif
#if (HAVE_EZBOX_SERVICE_EMC2 == 1)
	"cnc_latency",
#endif
};

char *save_cancel_buttons[] = {
	"setup_system",
#if (HAVE_EZBOX_LAN_NIC == 1)
	"setup_lan",
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
	"setup_wan",
#endif
	"management_authz",
	"management_default",
	"management_upgrade",
#if (HAVE_EZBOX_SERVICE_EMC2 == 1)
	"cnc_setup",
	"cnc_default",
	"cnc_latency",
#endif
};

/**
 * Private functions
 **/

/**
 * Public functions
 **/

int ezcfg_http_html_admin_set_html_button(
	struct ezcfg_http_html_admin *admin,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_html *html;
	struct ezcfg_locale *locale = NULL;
	int button_index;
	int input_index;
	char *p;
	size_t i;
	char *section;
	int ret = -1;

	ASSERT(admin != NULL);
	ASSERT(pi > 1);

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	http = ezcfg_http_html_admin_get_http(admin);
	html = ezcfg_http_html_admin_get_html(admin);

	p = ezcfg_http_get_request_uri(http);
	section = p+strlen(EZCFG_HTTP_HTML_ADMIN_PREFIX_URI);

        /* set locale info */
	locale = ezcfg_locale_new(ezcfg);
	if (locale != NULL) {
		ezcfg_locale_set_domain(locale, EZCFG_HTTP_HTML_ADMIN_BUTTON_DOMAIN);
		ezcfg_locale_set_dir(locale, EZCFG_HTTP_HTML_LANG_DIR);
	}

	/* <div id="button"> */
	button_index = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_DIV_ELEMENT_NAME, NULL);
	if (button_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, button_index, EZCFG_HTML_ID_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_BUTTON, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	input_index = -1;

	for (i = 0; i < ARRAY_SIZE(save_cancel_buttons); i++) {
		p = save_cancel_buttons[i];
		if (strcmp(p, section) == 0) {
			/* <input type="submit" name="act_save" value="Save Settings"> */
			input_index = ezcfg_html_add_body_child(html, button_index, input_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
			if (input_index < 0) {
				err(ezcfg, "ezcfg_html_add_body_child err.\n");
				goto func_exit;
			}
			ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_SUBMIT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
			ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_NAME_ACT_SAVE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
			ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, ezcfg_locale_text(locale, "Save Settings"), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

			/* <input type="submit" name="act_cancel" value="Cancel Changes"> */
			input_index = ezcfg_html_add_body_child(html, button_index, input_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
			if (input_index < 0) {
				err(ezcfg, "ezcfg_html_add_body_child err.\n");
				goto func_exit;
			}
			ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_SUBMIT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
			ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_NAME_ACT_CANCEL, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
			ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, ezcfg_locale_text(locale, "Cancel Changes"), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

			break;
		}
	}
	
	for (i = 0; i < ARRAY_SIZE(refresh_buttons); i++) {
		p = refresh_buttons[i];
		if (strcmp(p, section) == 0) {
			input_index = ezcfg_html_add_body_child(html, button_index, input_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
			if (input_index < 0) {
				err(ezcfg, "ezcfg_html_add_body_child err.\n");
				goto func_exit;
			}
			ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_SUBMIT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
			ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_NAME_ACT_REFRESH, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
			ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, ezcfg_locale_text(locale, "Refresh"), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
			break;
		}
	}

	/* must return menu index */
	ret = button_index;
func_exit:
	if (locale != NULL)
		ezcfg_locale_delete(locale);

	return ret;
}

