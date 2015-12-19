/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_composite_json_http.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2015 by ezbox-project
 *
 * History      Rev       Description
 * 2015-06-13   0.1       Split it from ezcfg-private.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_COMPOSITE_JSON_HTTP_H_
#define _EZCFG_PRIV_COMPOSITE_JSON_HTTP_H_

#include "ezcfg-types.h"
#include "ezcfg-http.h"

/* composite/json_http/json_http.c */
struct ezcfg_json_http;
int ezcfg_json_http_del(struct ezcfg_json_http *jh);
struct ezcfg_json_http *ezcfg_json_http_new(struct ezcfg *ezcfg);

struct ezcfg_json *ezcfg_json_http_get_json(struct ezcfg_json_http *jh);
struct ezcfg_http *ezcfg_json_http_get_http(struct ezcfg_json_http *jh);
unsigned short ezcfg_json_http_get_http_version_major(struct ezcfg_json_http *jh);
unsigned short ezcfg_json_http_get_http_version_minor(struct ezcfg_json_http *jh);
bool ezcfg_json_http_set_http_version_major(struct ezcfg_json_http *jh, unsigned short major);
bool ezcfg_json_http_set_http_version_minor(struct ezcfg_json_http *jh, unsigned short minor);
char *ezcfg_json_http_get_http_header_value(struct ezcfg_json_http *jh, char *name);
int ezcfg_json_http_reset_attributes(struct ezcfg_json_http *jh);
bool ezcfg_json_http_parse_header(struct ezcfg_json_http *jh, char *buf, int len);
bool ezcfg_json_http_parse_message_body(struct ezcfg_json_http *jh);
char *ezcfg_json_http_set_message_body(struct ezcfg_json_http *jh, const char *body, int len);
int ezcfg_json_http_dump(struct ezcfg_json_http *jh);
int ezcfg_json_http_get_message_length(struct ezcfg_json_http *jh);
int ezcfg_json_http_write_message(struct ezcfg_json_http *jh, char *buf, int len);

#endif /* _EZCFG_PRIV_COMPOSITE_JSON_HTTP_H_ */
