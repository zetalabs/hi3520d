/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-http_html_apply.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-06-25   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_HTTP_HTML_APPLY_H_
#define _EZCFG_HTTP_HTML_APPLY_H_

enum {
	HTTP_HTML_APPLY_ACT_CANCEL = 0,
	HTTP_HTML_APPLY_ACT_SAVE,
	HTTP_HTML_APPLY_ACT_REFRESH
};

/* ezcfg HTTP html apply request/response mode */
#define EZCFG_HTTP_HTML_APPLY_STATUS_SYSTEM_DOMAIN     "apply_status_system"
#define EZCFG_HTTP_HTML_APPLY_STATUS_LAN_DOMAIN        "apply_status_lan"
#define EZCFG_HTTP_HTML_APPLY_VIEW_DHCP_CLIENT_TABLE_DOMAIN  "apply_view_dhcp_client_table"
#define EZCFG_HTTP_HTML_APPLY_STATUS_WAN_DOMAIN        "apply_status_wan"

#define EZCFG_HTTP_HTML_APPLY_SETUP_SYSTEM_DOMAIN      "apply_setup_system"
#define EZCFG_HTTP_HTML_APPLY_SETUP_LAN_DOMAIN         "apply_setup_lan"
#define EZCFG_HTTP_HTML_APPLY_SETUP_WAN_DOMAIN         "apply_setup_wan"

#define EZCFG_HTTP_HTML_APPLY_MANAGEMENT_AUTHZ_DOMAIN  "apply_management_authz"
#define EZCFG_HTTP_HTML_APPLY_MANAGEMENT_DEFAULT_DOMAIN  "apply_management_default"
#define EZCFG_HTTP_HTML_APPLY_MANAGEMENT_UPGRADE_DOMAIN  "apply_management_upgrade"

#define EZCFG_HTTP_HTML_APPLY_CNC_SETUP_DOMAIN         "apply_cnc_setup"
#define EZCFG_HTTP_HTML_APPLY_CNC_DEFAULT_DOMAIN       "apply_cnc_default"
#define EZCFG_HTTP_HTML_APPLY_CNC_LATENCY_DOMAIN       "apply_cnc_latency"

#define EZCFG_HTTP_HTML_APPLY_HEAD_DOMAIN              "apply_head"
#define EZCFG_HTTP_HTML_APPLY_MAIN_DOMAIN              "apply_main"
#define EZCFG_HTTP_HTML_APPLY_FOOT_DOMAIN              "apply_foot"
#define EZCFG_HTTP_HTML_APPLY_MENU_DOMAIN              "apply_menu"
#define EZCFG_HTTP_HTML_APPLY_CONTENT_DOMAIN           "apply_content"
#define EZCFG_HTTP_HTML_APPLY_BUTTON_DOMAIN            "apply_button"


#define EZCFG_HTTP_HTML_APPLY_DIV_ID_CONTAINER     "container"
#define EZCFG_HTTP_HTML_APPLY_DIV_ID_HEAD          "head"
#define EZCFG_HTTP_HTML_APPLY_DIV_ID_MAIN          "main"
#define EZCFG_HTTP_HTML_APPLY_DIV_ID_FOOT          "foot"
#define EZCFG_HTTP_HTML_APPLY_DIV_ID_MENU          "menu"
#define EZCFG_HTTP_HTML_APPLY_DIV_ID_CONTENT       "content"
#define EZCFG_HTTP_HTML_APPLY_DIV_ID_BUTTON        "button"

#define EZCFG_HTTP_HTML_APPLY_INPUT_TYPE_SUBMIT        "submit"
#define EZCFG_HTTP_HTML_APPLY_INPUT_TYPE_FILE          "file"
#define EZCFG_HTTP_HTML_APPLY_INPUT_TYPE_TEXT          "text"
#define EZCFG_HTTP_HTML_APPLY_INPUT_TYPE_PASSWORD      "password"
#define EZCFG_HTTP_HTML_APPLY_INPUT_TYPE_CHECKBOX      "checkbox"
#define EZCFG_HTTP_HTML_APPLY_INPUT_TYPE_RADIO         "radio"

#define EZCFG_HTTP_HTML_APPLY_INPUT_NAME_ACT_SAVE      "act_save"
#define EZCFG_HTTP_HTML_APPLY_INPUT_NAME_ACT_CANCEL    "act_cancel"
#define EZCFG_HTTP_HTML_APPLY_INPUT_NAME_ACT_REFRESH   "act_refresh"
#define EZCFG_HTTP_HTML_APPLY_INPUT_NAME_ACT_RELEASE   "act_release"
#define EZCFG_HTTP_HTML_APPLY_INPUT_NAME_ACT_RENEW     "act_renew"


#define EZCFG_HTTP_HTML_APPLY_OPTION_VALUE_EN_HK       "en_HK"
#define EZCFG_HTTP_HTML_APPLY_OPTION_VALUE_ZH_CN       "zh_CN"

#define EZCFG_HTTP_HTML_APPLY_P_CLASS_WARNING          "warning"

#endif /* _EZCFG_HTTP_HTML_APPLY_H_ */
