/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-private.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-12   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_PRIVATE_H_
#define _EZCFG_PRIVATE_H_

#include <syslog.h>
#include <signal.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/syscall.h>

#include "ezcfg.h"
#include "ezcfg-types.h"

#define gettid() syscall(__NR_gettid)

typedef void *(*ezcfg_thread_func_t)(void *);

/* name-value pair struct */
struct ezcfg_nv_pair {
  char *n; /* null-terminated name string */
  char *v; /* null-terminated value string */
  int nlen; /* name string length including null */
  int vlen; /* value string length including null */
};

/* rc service invocation struct */
struct ezcfg_rc_triple {
  char *service;
  char *action;
  int wait;
};

/* args/arg_nvram_socket.c */
struct ezcfg_arg_nvram_socket {
  char *domain;
  char *type;
  char *protocol;
  char *address;
};

/* args/arg_nvram_ssl.c */
struct ezcfg_arg_nvram_ssl {
  char *role;
  char *method;
  char *socket_enable;
  char *socket_domain;
  char *socket_type;
  char *socket_protocol;
  char *socket_address;
  char *certificate_file;
  char *certificate_chain_file;
  char *private_key_file;
};

/*****************************/
/* common part header files  */
/*****************************/
/* common/ezcfg.c */
#include "ezcfg-priv_common.h"

/*****************************/
/* basic object header files */
/*****************************/
/* basic/nv_pair/nv_pair.c */
#include "ezcfg-priv_basic_nv_pair.h"

/* basic/linked_list/linked_list.c */
#include "ezcfg-priv_basic_linked_list.h"

/* basic/stack_list/stack_list.c */
#include "ezcfg-priv_basic_stack_list.h"

/* basic/binary_tree/binary_tree.c */
#include "ezcfg-priv_basic_binary_tree.h"

/* basic/nv_linked_list/nv_linked_list.c */
#include "ezcfg-priv_basic_nv_linked_list.h"

/* basic/socket/socket.c */
#include "ezcfg-priv_basic_socket.h"

/* basic/thread/thread.c */
#include "ezcfg-priv_basic_thread.h"

/* basic/process/process.c */
#include "ezcfg-priv_basic_process.h"

/* basic/auth/auth.c */
#include "ezcfg-priv_basic_auth.h"

/* basic/http/http.c */
#include "ezcfg-priv_basic_http.h"

/* basic/json/json.c */
#include "ezcfg-priv_basic_json.h"

/*********************************/
/* composite object header files */
/*********************************/
/* composite/json_http/json_http.c */
#include "ezcfg-priv_composite_json_http.h"

/* composite/nv_json_http_socket/nv_json_http_socket.c */
#include "ezcfg-priv_composite_nv_json_http_socket.h"




/* nvram/nvram-defaults.c */
extern struct ezcfg_nv_pair default_nvram_settings[];
extern char *default_nvram_savings[];
int ezcfg_nvram_get_num_default_nvram_settings(void);
int ezcfg_nvram_get_num_default_nvram_savings(void);


/* nvram/nvram-validators.c */
bool ezcfg_nvram_validate_value(struct ezcfg *ezcfg, char *name, char *value);
int ezcfg_nvram_get_num_default_nvram_validators(void);


/* nvram/nvram-propagators.c */
bool ezcfg_nvram_propagate_value(struct ezcfg *ezcfg, char *name, char *value);
int ezcfg_nvram_get_num_default_nvram_propagators(void);


/* nvram/nvram.c */
#include "ezcfg-priv_nvram.h"


/* args/arg_nvram_ssl.c */
#include "ezcfg-priv_args.h"



/* locale/locale.c */
#include "ezcfg-priv_locale.h"

/* uuid/uuid.c */
struct ezcfg_uuid;
struct ezcfg_uuid *ezcfg_uuid_new(struct ezcfg *ezcfg, int version);
bool ezcfg_uuid_delete(struct ezcfg_uuid *uuid);
bool ezcfg_uuid_generate(struct ezcfg_uuid *uuid);
bool ezcfg_uuid_export_str(struct ezcfg_uuid *uuid, char *buf, int len);
int ezcfg_uuid_get_version(struct ezcfg_uuid *uuid);
bool ezcfg_uuid_set_store_name(struct ezcfg_uuid *uuid, const char *store_name);
char *ezcfg_uuid_get_store_name(struct ezcfg_uuid *uuid);
bool ezcfg_uuid_v1_set_mac(struct ezcfg_uuid *uuid, unsigned char *mac, int len);
bool ezcfg_uuid_v1_enforce_multicast_mac(struct ezcfg_uuid *uuid);


/* ezctp/ezctp.c */
#include "ezcfg-priv_ezctp.h"


/* html/html.c */
struct ezcfg_html;
void ezcfg_html_delete(struct ezcfg_html *html);
struct ezcfg_html *ezcfg_html_new(struct ezcfg *ezcfg);
void ezcfg_html_reset_attributes(struct ezcfg_html *html);
struct ezcfg *ezcfg_html_get_ezcfg(struct ezcfg_html *html);
unsigned short ezcfg_html_get_version_major(struct ezcfg_html *html);
unsigned short ezcfg_html_get_version_minor(struct ezcfg_html *html);
bool ezcfg_html_set_version_major(struct ezcfg_html *html, unsigned short major);
bool ezcfg_html_set_version_minor(struct ezcfg_html *html, unsigned short minor);
int ezcfg_html_get_max_nodes(struct ezcfg_html *html);
bool ezcfg_html_set_max_nodes(struct ezcfg_html *html, const int max_nodes);
int ezcfg_html_set_root(struct ezcfg_html *html, const char *name);
bool ezcfg_html_add_root_attribute(struct ezcfg_html *html, const char *name, const char *value, int pos);
int ezcfg_html_set_head(struct ezcfg_html *html, const char *name);
int ezcfg_html_get_head_index(struct ezcfg_html *html);
int ezcfg_html_add_head_child(struct ezcfg_html *html, int pi, int si, const char *name, const char *content);
bool ezcfg_html_add_head_child_attribute(struct ezcfg_html *html, int ei, const char *name, const char *value, int pos);
int ezcfg_html_set_body(struct ezcfg_html *html, const char *name);
int ezcfg_html_get_body_index(struct ezcfg_html *html);
int ezcfg_html_add_body_child(struct ezcfg_html *html, int pi, int si, const char *name, const char *content);
bool ezcfg_html_add_body_child_attribute(struct ezcfg_html *html, int ei, const char *name, const char *value, int pos);
int ezcfg_html_get_element_index(struct ezcfg_html *html, const int pi, const int si, char *name);
char *ezcfg_html_get_element_content_by_index(struct ezcfg_html *html, const int index);
bool ezcfg_html_parse(struct ezcfg_html *html, char *buf, int len);
int ezcfg_html_get_message_length(struct ezcfg_html *html);
int ezcfg_html_write_message(struct ezcfg_html *html, char *buf, int len);


/* css/css.c */
struct ezcfg_css;
void ezcfg_css_delete(struct ezcfg_css *css);
struct ezcfg_css *ezcfg_css_new(struct ezcfg *ezcfg);
unsigned short ezcfg_css_get_version_major(struct ezcfg_css *css);
unsigned short ezcfg_css_get_version_minor(struct ezcfg_css *css);
bool ezcfg_css_set_version_major(struct ezcfg_css *css, unsigned short major);
bool ezcfg_css_set_version_minor(struct ezcfg_css *css, unsigned short minor);
int ezcfg_css_add_rule_set(struct ezcfg_css *css, char *selector, char *name, char *value);
int ezcfg_css_get_message_length(struct ezcfg_css *css);
int ezcfg_css_write_message(struct ezcfg_css *css, char *buf, int len);


/* ssl/ssl.c */
#include "ezcfg-priv_ssl.h"


/* websocket/websocket.c */
#include "ezcfg-priv_websocket.h"
/* websocket/http_websocket.c */
#include "ezcfg-priv_http_websocket.h"


/* ssi/ssi.c */
struct ezcfg_ssi;
struct ezcfg_ssi *ezcfg_ssi_new(struct ezcfg *ezcfg, struct ezcfg_nvram *nvram);
void ezcfg_ssi_delete(struct ezcfg_ssi *ssi);
bool ezcfg_ssi_set_document_root(struct ezcfg_ssi *ssi, const char *root);
bool ezcfg_ssi_set_path(struct ezcfg_ssi *ssi, const char *path);
FILE *ezcfg_ssi_open_file(struct ezcfg_ssi *ssi, const char *mode);
int ezcfg_ssi_file_get_line(struct ezcfg_ssi *ssi, char *buf, size_t size);


/* http/http_ssi.c */
struct ezcfg_http_ssi;
void ezcfg_http_ssi_delete(struct ezcfg_http_ssi *hs);
struct ezcfg_http_ssi *
ezcfg_http_ssi_new(struct ezcfg *ezcfg,
                   struct ezcfg_http *http,
                   struct ezcfg_nvram *nvram,
                   struct ezcfg_ssi *ssi);

int ezcfg_http_handle_ssi_request(struct ezcfg_http *http,
                                  struct ezcfg_nvram *nvram,
                                  struct ezcfg_ssi *ssi);


/* http/http_nvram.c */
#include "ezcfg-priv_http_nvram.h"


/* http/http_html_index.c */
int ezcfg_http_handle_index_request(struct ezcfg_http *http,
                                    struct ezcfg_nvram *nvram);


/* http/http_html_apply.c */
int ezcfg_http_handle_apply_request(struct ezcfg_http *http,
                                    struct ezcfg_nvram *nvram);

struct ezcfg_http_html_apply;
struct ezcfg_http_html_apply *
ezcfg_http_html_apply_new(struct ezcfg *ezcfg,
                          struct ezcfg_http *http,
                          struct ezcfg_nvram *nvram);
void ezcfg_http_html_apply_delete(struct ezcfg_http_html_apply *apply);
bool ezcfg_http_html_apply_handle_post_data(struct ezcfg_http_html_apply *apply);
bool ezcfg_http_html_apply_save_settings(struct ezcfg_http_html_apply *apply);
struct ezcfg *ezcfg_http_html_apply_get_ezcfg(struct ezcfg_http_html_apply *apply);
struct ezcfg_http *ezcfg_http_html_apply_get_http(struct ezcfg_http_html_apply *apply);
struct ezcfg_html *ezcfg_http_html_apply_get_html(struct ezcfg_http_html_apply *apply);
bool ezcfg_http_html_apply_set_html(struct ezcfg_http_html_apply *apply, struct ezcfg_html *html);
struct ezcfg_nvram *ezcfg_http_html_apply_get_nvram(struct ezcfg_http_html_apply *apply);
struct ezcfg_link_list *ezcfg_http_html_apply_get_post_list(struct ezcfg_http_html_apply *apply);

/* http/apply/http_html_apply_common_head.c */
int ezcfg_http_html_apply_set_html_common_head(struct ezcfg_http_html_apply *apply, int pi, int si);
bool ezcfg_http_html_apply_set_http_html_common_header(struct ezcfg_http_html_apply *apply);
bool ezcfg_http_html_apply_set_http_css_common_header(struct ezcfg_http_html_apply *apply);


/* http/http_html_admin.c */
int ezcfg_http_handle_admin_request(struct ezcfg_http *http, struct ezcfg_nvram *nvram);

struct ezcfg_http_html_admin;
struct ezcfg_http_html_admin *
ezcfg_http_html_admin_new(struct ezcfg *ezcfg,
                          struct ezcfg_http *http,
                          struct ezcfg_nvram *nvram);
void ezcfg_http_html_admin_delete(struct ezcfg_http_html_admin *admin);
int ezcfg_http_html_admin_get_action(struct ezcfg_http_html_admin *admin);
bool ezcfg_http_html_admin_handle_post_data(struct ezcfg_http_html_admin *admin);
bool ezcfg_http_html_admin_save_settings(struct ezcfg_http_html_admin *admin);
struct ezcfg *ezcfg_http_html_admin_get_ezcfg(struct ezcfg_http_html_admin *admin);
struct ezcfg_http *ezcfg_http_html_admin_get_http(struct ezcfg_http_html_admin *admin);
struct ezcfg_html *ezcfg_http_html_admin_get_html(struct ezcfg_http_html_admin *admin);
bool ezcfg_http_html_admin_set_html(struct ezcfg_http_html_admin *admin, struct ezcfg_html *html);
struct ezcfg_nvram *ezcfg_http_html_admin_get_nvram(struct ezcfg_http_html_admin *admin);
struct ezcfg_link_list *ezcfg_http_html_admin_get_post_list(struct ezcfg_http_html_admin *admin);


/* http/admin/http_html_admin_common_head.c */
int ezcfg_http_html_admin_set_html_common_head(struct ezcfg_http_html_admin *admin, int pi, int si);
bool ezcfg_http_html_admin_set_http_html_common_header(struct ezcfg_http_html_admin *admin);
bool ezcfg_http_html_admin_set_http_css_common_header(struct ezcfg_http_html_admin *admin);

/* http/admin/http_html_admin_head.c */
int ezcfg_http_html_admin_set_html_head(
	struct ezcfg_http_html_admin *admin,
	int pi, int si);

/* http/admin/http_html_admin_foot.c */
int ezcfg_http_html_admin_set_html_foot(
	struct ezcfg_http_html_admin *admin,
	int pi, int si);

/* http/admin/http_html_admin_button.c */
int ezcfg_http_html_admin_set_html_button(
	struct ezcfg_http_html_admin *admin,
	int pi, int si);

/* http/admin/http_html_admin_menu.c */
int ezcfg_http_html_admin_set_html_menu(
	struct ezcfg_http_html_admin *admin,
	int pi, int si);

/* http/admin/http_html_admin_menu_status.c */
int ezcfg_http_html_admin_html_menu_status(
	struct ezcfg_http_html_admin *admin,
	int pi, int si);

/* http/admin/http_html_admin_menu_setup.c */
int ezcfg_http_html_admin_html_menu_setup(
	struct ezcfg_http_html_admin *admin,
	int pi, int si);

/* http/admin/http_html_admin_menu_management.c */
int ezcfg_http_html_admin_html_menu_management(
	struct ezcfg_http_html_admin *admin,
	int pi, int si);

/* http/admin/http_html_admin_menu_cnc.c */
#if (HAVE_EZBOX_SERVICE_EMC2 == 1)
int ezcfg_http_html_admin_html_menu_cnc(
	struct ezcfg_http_html_admin *admin,
	int pi, int si);
#endif

/* http/admin/http_html_admin_layout_css.c */
int ezcfg_http_html_admin_layout_css_handler(struct ezcfg_http_html_admin *admin);

/* http/admin/http_html_admin_status_system.c */
int ezcfg_http_html_admin_status_system_handler(struct ezcfg_http_html_admin *admin);
/* http/admin/http_html_admin_status_lan.c */
#if (HAVE_EZBOX_LAN_NIC == 1)
int ezcfg_http_html_admin_status_lan_handler(struct ezcfg_http_html_admin *admin);
#endif
/* http/admin/http_html_admin_view_dhcp_client_table.c */
int ezcfg_http_html_admin_view_dhcp_client_table_handler(struct ezcfg_http_html_admin *admin);
/* http/admin/http_html_admin_status_wan.c */
#if (HAVE_EZBOX_WAN_NIC == 1)
int ezcfg_http_html_admin_status_wan_handler(struct ezcfg_http_html_admin *admin);
#endif


/* http/admin/http_html_admin_setup_system.c */
int ezcfg_http_html_admin_setup_system_handler(struct ezcfg_http_html_admin *admin);
/* http/admin/http_html_admin_setup_lan.c */
#if (HAVE_EZBOX_LAN_NIC == 1)
int ezcfg_http_html_admin_setup_lan_handler(struct ezcfg_http_html_admin *admin);
#endif
/* http/admin/http_html_admin_setup_wan.c */
#if (HAVE_EZBOX_WAN_NIC == 1)
int ezcfg_http_html_admin_setup_wan_handler(struct ezcfg_http_html_admin *admin);
#endif


/* http/admin/http_html_admin_management_authz.c */
int ezcfg_http_html_admin_management_authz_handler(struct ezcfg_http_html_admin *admin);
/* http/admin/http_html_admin_setup_lan.c */
int ezcfg_http_html_admin_management_default_handler(struct ezcfg_http_html_admin *admin);
/* http/admin/http_html_admin_setup_wan.c */
int ezcfg_http_html_admin_management_upgrade_handler(struct ezcfg_http_html_admin *admin);

#if (HAVE_EZBOX_SERVICE_EMC2 == 1)
int ezcfg_http_html_admin_cnc_setup_handler(struct ezcfg_http_html_admin *admin);
int ezcfg_http_html_admin_cnc_default_handler(struct ezcfg_http_html_admin *admin);
int ezcfg_http_html_admin_cnc_latency_handler(struct ezcfg_http_html_admin *admin);
#endif

/* xml/xml.c */
#include "ezcfg-priv_xml.h"

/* soap/soap.c */
struct ezcfg_soap;
void ezcfg_soap_delete(struct ezcfg_soap *soap);
struct ezcfg_soap *ezcfg_soap_new(struct ezcfg *ezcfg);
void ezcfg_soap_reset_attributes(struct ezcfg_soap *soap);
unsigned short ezcfg_soap_get_version_major(struct ezcfg_soap *soap);
unsigned short ezcfg_soap_get_version_minor(struct ezcfg_soap *soap);
bool ezcfg_soap_set_version_major(struct ezcfg_soap *soap, unsigned short major);
bool ezcfg_soap_set_version_minor(struct ezcfg_soap *soap, unsigned short minor);
int ezcfg_soap_get_max_nodes(struct ezcfg_soap *soap);
bool ezcfg_soap_set_max_nodes(struct ezcfg_soap *soap, const int max_nodes);
int ezcfg_soap_set_envelope(struct ezcfg_soap *soap, const char *name);
bool ezcfg_soap_add_envelope_attribute(struct ezcfg_soap *soap, const char *name, const char *value, int pos);
int ezcfg_soap_set_body(struct ezcfg_soap *soap, const char *name);
int ezcfg_soap_get_body_index(struct ezcfg_soap *soap);
int ezcfg_soap_add_body_child(struct ezcfg_soap *soap, int pi, int si, const char *name, const char *content);
bool ezcfg_soap_add_body_child_attribute(struct ezcfg_soap *soap, int ei, const char *name, const char *value, int pos);
int ezcfg_soap_get_element_index(struct ezcfg_soap *soap, const int pi, const int si, char *name);
char *ezcfg_soap_get_element_content_by_index(struct ezcfg_soap *soap, const int index);
bool ezcfg_soap_parse(struct ezcfg_soap *soap, char *buf, int len);
int ezcfg_soap_get_message_length(struct ezcfg_soap *soap);
int ezcfg_soap_write_message(struct ezcfg_soap *soap, char *buf, int len);

/* soap/soap_http.c */
struct ezcfg_soap_http;
void ezcfg_soap_http_delete(struct ezcfg_soap_http *sh);
struct ezcfg_soap_http *ezcfg_soap_http_new(struct ezcfg *ezcfg);
struct ezcfg_soap *ezcfg_soap_http_get_soap(struct ezcfg_soap_http *sh);
struct ezcfg_http *ezcfg_soap_http_get_http(struct ezcfg_soap_http *sh);
unsigned short ezcfg_soap_http_get_soap_version_major(struct ezcfg_soap_http *sh);
unsigned short ezcfg_soap_http_get_soap_version_minor(struct ezcfg_soap_http *sh);
bool ezcfg_soap_http_set_soap_version_major(struct ezcfg_soap_http *sh, unsigned short major);
bool ezcfg_soap_http_set_soap_version_minor(struct ezcfg_soap_http *sh, unsigned short minor);
unsigned short ezcfg_soap_http_get_http_version_major(struct ezcfg_soap_http *sh);
unsigned short ezcfg_soap_http_get_http_version_minor(struct ezcfg_soap_http *sh);
bool ezcfg_soap_http_set_http_version_major(struct ezcfg_soap_http *sh, unsigned short major);
bool ezcfg_soap_http_set_http_version_minor(struct ezcfg_soap_http *sh, unsigned short minor);
char *ezcfg_soap_http_get_http_header_value(struct ezcfg_soap_http *sh, char *name);
void ezcfg_soap_http_reset_attributes(struct ezcfg_soap_http *sh);
bool ezcfg_soap_http_parse_header(struct ezcfg_soap_http *sh, char *buf, int len);
bool ezcfg_soap_http_parse_message_body(struct ezcfg_soap_http *sh);
char *ezcfg_soap_http_set_message_body(struct ezcfg_soap_http *sh, const char *body, int len);
void ezcfg_soap_http_dump(struct ezcfg_soap_http *sh);
int ezcfg_soap_http_get_message_length(struct ezcfg_soap_http *sh);
int ezcfg_soap_http_write_message(struct ezcfg_soap_http *sh, char *buf, int len);

/* igrs/igrs.c */
#include "ezcfg-priv_igrs.h"

/* igrs/igrs_isdp.c */
struct ezcfg_igrs_isdp;
void ezcfg_igrs_isdp_delete(struct ezcfg_igrs_isdp *isdp);
struct ezcfg_igrs_isdp *ezcfg_igrs_isdp_new(struct ezcfg *ezcfg);
void ezcfg_igrs_isdp_dump(struct ezcfg_igrs_isdp *isdp);

/* upnp/upnp_gena.c */
struct ezcfg_upnp_gena;
void ezcfg_upnp_gena_delete(struct ezcfg_upnp_gena *upnp_gena);
struct ezcfg_upnp_gena *ezcfg_upnp_gena_new(struct ezcfg *ezcfg);
void ezcfg_upnp_gena_reset_attributes(struct ezcfg_upnp_gena *upnp_gena);

/* agent/agent_core.c */
#include "ezcfg-priv_agent.h"

/* thread/master.c */
#include "ezcfg-priv_master.h"

/* thread/worker.c */
#include "ezcfg-priv_worker.h"

/* ctrl/ctrl.c - daemon runtime setup */
#include "ezcfg-priv_ctrl.h"

/* util/util.c */
#include "ezcfg-priv_util.h"

/* uevent/uevent.c */
struct ezcfg_uevent;
void ezcfg_uevent_delete(struct ezcfg_uevent *uevent);
struct ezcfg_uevent *ezcfg_uevent_new(struct ezcfg *ezcfg);

/* upnp/upnp.c */
#include "ezcfg-priv_upnp.h"


/*********************************************/
/* following files are for complex structure */
/*********************************************/

/* http_socket/http_socket.c */
#include "ezcfg-priv_http_socket.h"

#endif /* _EZCFG_PRIVATE_H_ */
