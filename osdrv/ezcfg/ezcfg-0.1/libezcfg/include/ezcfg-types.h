/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-types.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-12-02   0.1       Splite it from ezcfg.h
 * ============================================================================
 */

#ifndef _EZCFG_TYPES_H_
#define _EZCFG_TYPES_H_

#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

/*********************************/
/* common part types             */
/*********************************/
/* common/ezcfg.c */
struct ezcfg;

/*********************************/
/* basic object types            */
/*********************************/
/* basic/nv_pair/nv_pair.c */
struct ezcfg_nv_pair;

/* basic/linked_list/linked_list.c */
struct ezcfg_linked_list;

/* basic/stack_list/stack_list.c */
struct ezcfg_stack_list;

/* basic/binary_tree/binary_tree.c */
struct ezcfg_binary_tree;

/* basic/socket/socket.c */
struct ezcfg_socket;

/* basic/thread/thread.c */
struct ezcfg_thread;

/* basic/process/process.c */
struct ezcfg_process;

/* basic/auth/auth.c */
struct ezcfg_auth;

/* basic/http/http.c */
struct ezcfg_http;

/* json/json.c */
struct ezcfg_json_value;
struct ezcfg_json;


/*********************************/
/* composite object types        */
/*********************************/



/*********************************/
/* FIXME: legacy object types    */
/*********************************/
/* tree/binary_tree.c */
struct ezcfg_binary_tree_node;
struct ezcfg_binary_tree;

/* agent/agent_core.c */
struct ezcfg_agent_core;

/* agent/agent_master.c */
struct ezcfg_agent_master;

/* agent/agent_worker.c */
struct ezcfg_agent_worker;

/* thread/master.c */
struct ezcfg_master;

/* thread/worker.c */
struct ezcfg_worker;

/* http/http.c */
struct ezcfg_http;

/* uevent/uevent.c */
struct ezcfg_uevent;

/* igrs/igrs.c */
struct ezcfg_igrs;
struct ezcfg_igrs_msg_op;

/* upnp/upnp.c */
struct ezcfg_upnp;

/* upnp/upnp_ssdp.c */
struct ezcfg_upnp_ssdp;

/* upnp/upnp_gena.c */
struct ezcfg_upnp_gena;

/* uuid/uuid.c */
struct ezcfg_uuid;

/* ctrl/ctrl.c - daemon runtime setup */
struct ezcfg_ctrl;

/* xml/xml.c */
struct ezcfg_xml_element;
struct ezcfg_xml;

/* nvram/nvram.c */
struct ezcfg_nvram;

/* ezctp/ezctp.c */
struct ezcfg_ezctp;

/* ssl/ssl.c */
struct ezcfg_ssl;

/* websocket/websocket.c */
struct ezcfg_websocket;

/* websocket/http_websocket.c */
struct ezcfg_http_websocket;

#endif /* _EZCFG_TYPES_H_ */
