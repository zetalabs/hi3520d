/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : basic/json/json_local.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2015 by ezbox-project
 *
 * History      Rev       Description
 * 2015-06-13   0.1       Move some local definitions here.
 * ============================================================================
 */

#ifndef _EZCFG_LOCAL_BASIC_JSON_JSON_LOCAL_H_
#define _EZCFG_LOCAL_BASIC_JSON_JSON_LOCAL_H_

#include "ezcfg-types.h"

/* basic/json/json.c */
struct ezcfg_json_value {
  int type; /* false / null / true / object / array / number / string */
  char *element;
  int visited; /* used for stack operation */
};

struct ezcfg_json {
  struct ezcfg *ezcfg;
  struct ezcfg_binary_tree *text; /* JSON-text = object / array */
  struct ezcfg_stack_list *stack;
  int msg_len;
};

#endif /* _EZCFG_LOCAL_BASIC_JSON_JSON_LOCAL_H_ */
