/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_basic_json.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2014-03-08   0.1       Write it from scratch
 * 2015-06-10   0.2       Rename it to ezcfg-priv_basic_json.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_BASIC_JSON_H_
#define _EZCFG_PRIV_BASIC_JSON_H_

#include "ezcfg-types.h"

/* basic/json/json.c */
struct ezcfg_json_value *ezcfg_json_value_create(char *buf);
int ezcfg_json_value_del(struct ezcfg_json_value *vp);
int ezcfg_json_value_del_wrap(void *data);

int ezcfg_json_del(struct ezcfg_json *json);
struct ezcfg_json *ezcfg_json_new(struct ezcfg *ezcfg);
int ezcfg_json_reset(struct ezcfg_json *json);
int ezcfg_json_get_text_type(struct ezcfg_json *json);
int ezcfg_json_parse_text(struct ezcfg_json *json, char *text, int len);
int ezcfg_json_get_msg_len(struct ezcfg_json *json);
int ezcfg_json_write_message(struct ezcfg_json *json, char *buf, int len);
struct ezcfg_linked_list *ezcfg_json_build_nvram_node_list(struct ezcfg_json *json);

/* basic/json/json_nvram.c */
bool ezcfg_json_is_nvram_representation(struct ezcfg_json *json);
bool ezcfg_json_is_nvram_result_ok(struct ezcfg_json *json);

#endif /* _EZCFG_PRIV_BASIC_JSON_H_ */
