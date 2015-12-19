/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_nv_json_http.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2015 by ezbox-project
 *
 * History      Rev       Description
 * 2015-06-07   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_NV_JSON_HTTP_H_
#define _EZCFG_PRIV_NV_JSON_HTTP_H_

#include "ezcfg-types.h"

/* nv_json_http/nv_json_http.c */
struct ezcfg_nv_json_http;

void ezcfg_nv_json_http_delete(struct ezcfg_nv_json_http *njh);
struct ezcfg_nv_json_http *ezcfg_nv_json_http_new(struct ezcfg *ezcfg);
struct ezcfg_json *ezcfg_nv_json_http_get_json(struct ezcfg_nv_json_http *njh);
struct ezcfg_http *ezcfg_nv_json_http_get_http(struct ezcfg_nv_json_http *njh);
unsigned short ezcfg_nv_json_http_get_http_version_major(struct ezcfg_nv_json_http *njh);
unsigned short ezcfg_nv_json_http_get_http_version_minor(struct ezcfg_nv_json_http *njh);
bool ezcfg_nv_json_http_set_http_version_major(struct ezcfg_nv_json_http *njh, unsigned short major);
bool ezcfg_nv_json_http_set_http_version_minor(struct ezcfg_nv_json_http *njh, unsigned short minor);
char *ezcfg_nv_json_http_get_http_header_value(struct ezcfg_nv_json_http *njh, char *name);
void ezcfg_nv_json_http_reset_attributes(struct ezcfg_nv_json_http *njh);
bool ezcfg_nv_json_http_parse_header(struct ezcfg_nv_json_http *njh, char *buf, int len);
bool ezcfg_nv_json_http_parse_message_body(struct ezcfg_nv_json_http *njh);
char *ezcfg_nv_json_http_set_message_body(struct ezcfg_nv_json_http *njh, const char *body, int len);
void ezcfg_nv_json_http_dump(struct ezcfg_nv_json_http *njh);
int ezcfg_nv_json_http_get_message_length(struct ezcfg_nv_json_http *njh);
int ezcfg_nv_json_http_write_message(struct ezcfg_nv_json_http *njh, char *buf, int len);
int ezcfg_nv_json_http_handle_request(struct ezcfg_nv_json_http *njh);

#endif /* _EZCFG_PRIV_NV_JSON_HTTP_H_ */
