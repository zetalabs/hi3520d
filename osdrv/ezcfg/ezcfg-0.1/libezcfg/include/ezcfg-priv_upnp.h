/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_upnp.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-09   0.1       Split it from ezcfg-private.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_UPNP_H_
#define _EZCFG_PRIV_UPNP_H_

#include "ezcfg-types.h"
/* for IFNAMSIZ */
#include <net/if.h>

/* upnp/upnp.c */
void ezcfg_upnp_delete(struct ezcfg_upnp *upnp);
struct ezcfg_upnp *ezcfg_upnp_new(struct ezcfg *ezcfg);
bool ezcfg_upnp_set_role(struct ezcfg_upnp *upnp, int role);
bool ezcfg_upnp_set_device_type(struct ezcfg_upnp *upnp, int type);
bool ezcfg_upnp_if_list_insert(struct ezcfg_upnp *upnp, char ifname[IFNAMSIZ], int life_time);
bool ezcfg_upnp_parse_description(struct ezcfg_upnp *upnp, const char *path);
bool ezcfg_upnp_list_insert(struct ezcfg_upnp **list, struct ezcfg_upnp *upnp);
void ezcfg_upnp_list_delete(struct ezcfg_upnp **list);

/* upnp/upnp_ssdp.c */
void ezcfg_upnp_ssdp_delete(struct ezcfg_upnp_ssdp *ssdp);
struct ezcfg_upnp_ssdp *ezcfg_upnp_ssdp_new(struct ezcfg *ezcfg);
bool ezcfg_upnp_ssdp_set_upnp(struct ezcfg_upnp_ssdp *ssdp, struct ezcfg_upnp *upnp);
struct ezcfg_http *ezcfg_upnp_ssdp_get_http(struct ezcfg_upnp_ssdp *ssdp);
bool ezcfg_upnp_ssdp_set_priv_data(struct ezcfg_upnp_ssdp *ssdp, const char *data);
bool ezcfg_upnp_ssdp_notify_alive(struct ezcfg_upnp_ssdp *ssdp);
bool ezcfg_upnp_ssdp_notify_byebye(struct ezcfg_upnp_ssdp *ssdp);
bool ezcfg_upnp_ssdp_msearch_request(struct ezcfg_upnp_ssdp *ssdp);
bool ezcfg_upnp_ssdp_msearch_response(struct ezcfg_upnp_ssdp *ssdp, struct ezcfg_socket *sp);
void ezcfg_upnp_ssdp_reset_attributes(struct ezcfg_upnp_ssdp *ssdp);

#endif /* _EZCFG_PRIV_UPNP_H_ */
