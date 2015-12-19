/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-http_html_admin.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-25   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_HTTP_HTML_ADMIN_H_
#define _EZCFG_HTTP_HTML_ADMIN_H_

enum {
	HTTP_HTML_ADMIN_ACT_CANCEL = 0,
	HTTP_HTML_ADMIN_ACT_SAVE,
	HTTP_HTML_ADMIN_ACT_REFRESH
};

/* ezcfg HTTP html admin request/response mode */
#define EZCFG_HTTP_HTML_ADMIN_STATUS_SYSTEM_DOMAIN     "admin_status_system"
#define EZCFG_HTTP_HTML_ADMIN_STATUS_LAN_DOMAIN        "admin_status_lan"
#define EZCFG_HTTP_HTML_ADMIN_VIEW_DHCP_CLIENT_TABLE_DOMAIN  "admin_view_dhcp_client_table"
#define EZCFG_HTTP_HTML_ADMIN_STATUS_WAN_DOMAIN        "admin_status_wan"

#define EZCFG_HTTP_HTML_ADMIN_SETUP_SYSTEM_DOMAIN      "admin_setup_system"
#define EZCFG_HTTP_HTML_ADMIN_SETUP_LAN_DOMAIN         "admin_setup_lan"
#define EZCFG_HTTP_HTML_ADMIN_SETUP_WAN_DOMAIN         "admin_setup_wan"

#define EZCFG_HTTP_HTML_ADMIN_MANAGEMENT_AUTHZ_DOMAIN  "admin_management_authz"
#define EZCFG_HTTP_HTML_ADMIN_MANAGEMENT_DEFAULT_DOMAIN  "admin_management_default"
#define EZCFG_HTTP_HTML_ADMIN_MANAGEMENT_UPGRADE_DOMAIN  "admin_management_upgrade"

#define EZCFG_HTTP_HTML_ADMIN_CNC_SETUP_DOMAIN         "admin_cnc_setup"
#define EZCFG_HTTP_HTML_ADMIN_CNC_DEFAULT_DOMAIN       "admin_cnc_default"
#define EZCFG_HTTP_HTML_ADMIN_CNC_LATENCY_DOMAIN       "admin_cnc_latency"

#define EZCFG_HTTP_HTML_ADMIN_HEAD_DOMAIN              "admin_head"
#define EZCFG_HTTP_HTML_ADMIN_MAIN_DOMAIN              "admin_main"
#define EZCFG_HTTP_HTML_ADMIN_FOOT_DOMAIN              "admin_foot"
#define EZCFG_HTTP_HTML_ADMIN_MENU_DOMAIN              "admin_menu"
#define EZCFG_HTTP_HTML_ADMIN_CONTENT_DOMAIN           "admin_content"
#define EZCFG_HTTP_HTML_ADMIN_BUTTON_DOMAIN            "admin_button"


#define EZCFG_HTTP_HTML_ADMIN_DIV_ID_CONTAINER     "container"
#define EZCFG_HTTP_HTML_ADMIN_DIV_ID_HEAD          "head"
#define EZCFG_HTTP_HTML_ADMIN_DIV_ID_MAIN          "main"
#define EZCFG_HTTP_HTML_ADMIN_DIV_ID_FOOT          "foot"
#define EZCFG_HTTP_HTML_ADMIN_DIV_ID_MENU          "menu"
#define EZCFG_HTTP_HTML_ADMIN_DIV_ID_CONTENT       "content"
#define EZCFG_HTTP_HTML_ADMIN_DIV_ID_BUTTON        "button"

#define EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_SUBMIT        "submit"
#define EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_FILE          "file"
#define EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_TEXT          "text"
#define EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_PASSWORD      "password"
#define EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_CHECKBOX      "checkbox"
#define EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_RADIO         "radio"

#define EZCFG_HTTP_HTML_ADMIN_INPUT_NAME_ACT_SAVE      "act_save"
#define EZCFG_HTTP_HTML_ADMIN_INPUT_NAME_ACT_CANCEL    "act_cancel"
#define EZCFG_HTTP_HTML_ADMIN_INPUT_NAME_ACT_REFRESH   "act_refresh"
#define EZCFG_HTTP_HTML_ADMIN_INPUT_NAME_ACT_RELEASE   "act_release"
#define EZCFG_HTTP_HTML_ADMIN_INPUT_NAME_ACT_RENEW     "act_renew"


#define EZCFG_HTTP_HTML_ADMIN_OPTION_VALUE_EN_HK       "en_HK"
#define EZCFG_HTTP_HTML_ADMIN_OPTION_VALUE_ZH_CN       "zh_CN"

#define EZCFG_HTTP_HTML_ADMIN_P_CLASS_WARNING          "warning"

#endif /* _EZCFG_HTTP_HTML_ADMIN_H_ */
