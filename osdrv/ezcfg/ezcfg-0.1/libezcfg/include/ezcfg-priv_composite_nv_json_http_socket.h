/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_composite_nv_json_http_socket.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2015 by ezbox-project
 *
 * History      Rev       Description
 * 2015-06-07   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_COMPOSITE_NV_JSON_HTTP_SOCKET_H_
#define _EZCFG_PRIV_COMPOSITE_NV_JSON_HTTP_SOCKET_H_

#include "ezcfg-types.h"

/* composite/nv_json_http_socket/nv_json_http_socket.c */
struct ezcfg_nv_json_http_socket;

void ezcfg_nv_json_http_socket_delete(struct ezcfg_nv_json_http_socket *njhs);
struct ezcfg_nv_json_http *ezcfg_nv_json_http_socket_new(struct ezcfg *ezcfg);
struct ezcfg_json *ezcfg_nv_json_http_socket_get_json(struct ezcfg_nv_json_http_socket *njhs);
struct ezcfg_http *ezcfg_nv_json_http_socket_get_http(struct ezcfg_nv_json_http_socket *njhs);
unsigned short ezcfg_nv_json_http_socket_get_http_version_major(struct ezcfg_nv_json_http_socket *njhs);
unsigned short ezcfg_nv_json_http_socket_get_http_version_minor(struct ezcfg_nv_json_http_socket *njhs);
bool ezcfg_nv_json_http_socket_set_http_version_major(struct ezcfg_nv_json_http_socket *njhs, unsigned short major);
bool ezcfg_nv_json_http_socket_set_http_version_minor(struct ezcfg_nv_json_http_socket *njhs, unsigned short minor);
char *ezcfg_nv_json_http_socket_get_http_header_value(struct ezcfg_nv_json_http_socket *njhs, char *name);
void ezcfg_nv_json_http_socket_reset_attributes(struct ezcfg_nv_json_http_socket *njhs);
bool ezcfg_nv_json_http_socket_parse_header(struct ezcfg_nv_json_http_socket *njhs, char *buf, int len);
bool ezcfg_nv_json_http_socket_parse_message_body(struct ezcfg_nv_json_http_socket *njhs);
char *ezcfg_nv_json_http_socket_set_message_body(struct ezcfg_nv_json_http_socket *njhs, const char *body, int len);
void ezcfg_nv_json_http_socket_dump(struct ezcfg_nv_json_http_socket *njhs);
int ezcfg_nv_json_http_socket_get_message_length(struct ezcfg_nv_json_http_socket *njhs);
int ezcfg_nv_json_http_socket_write_message(struct ezcfg_nv_json_http_socket *njhs, char *buf, int len);
int ezcfg_nv_json_http_socket_handle_request(struct ezcfg_nv_json_http_socket *njhs);

#endif /* _EZCFG_PRIV_COMPOSITE_NV_JSON_HTTP_H_ */
