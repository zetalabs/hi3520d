/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : basic/json/json_nvram.c
 *
 * Description  : simple implementation of RFC7159
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2015-06-13   0.1       Split it from basic/json/json.c
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#include "json_local.h"

bool ezcfg_json_is_nvram_representation(struct ezcfg_json *json)
{
  struct ezcfg_binary_tree_node *np, *np_left, *np_right;
  struct ezcfg_json_value *vp, *vp_left;

  ASSERT(json != NULL);

  np = ezcfg_binary_tree_get_root(json->text);
  if (np == NULL) {
    goto exit_fail;
  }
  vp = ezcfg_binary_tree_node_get_data(np);
  if (vp == NULL) {
    goto exit_fail;
  }

  if (vp->type != EZCFG_JSON_VALUE_TYPE_OBJECT) {
    goto exit_fail;
  }

  np_left = ezcfg_binary_tree_node_get_left(np);
  np = np_left;

  while(np) {
    vp = ezcfg_binary_tree_node_get_data(np);
    if (vp == NULL) {
      goto exit_fail;
    }
    if (vp->type != EZCFG_JSON_VALUE_TYPE_NVPAIR) {
      goto exit_fail;
    }
    np_left = ezcfg_binary_tree_node_get_left(np);
    if (np_left == NULL) {
      goto exit_fail;
    }
    vp_left = ezcfg_binary_tree_node_get_data(np_left);
    if (vp_left == NULL) {
      goto exit_fail;
    }
    if (vp_left->type != EZCFG_JSON_VALUE_TYPE_STRING) {
      goto exit_fail;
    }

    /* move to next nv-pair */
    np_right = ezcfg_binary_tree_node_get_right(np);
    np = np_right;
  }
  return true;

exit_fail:
  return false;
}

/* the empty object {} means nvram result OK */
bool ezcfg_json_is_nvram_result_ok(struct ezcfg_json *json)
{
  struct ezcfg_binary_tree_node *np, *np_left, *np_right;
  struct ezcfg_json_value *vp;

  ASSERT(json != NULL);

  np = ezcfg_binary_tree_get_root(json->text);
  if (np == NULL) {
    goto exit_fail;
  }
  vp = ezcfg_binary_tree_node_get_data(np);
  if (vp == NULL) {
    goto exit_fail;
  }

  if (vp->type != EZCFG_JSON_VALUE_TYPE_OBJECT) {
    goto exit_fail;
  }

  np_left = ezcfg_binary_tree_node_get_left(np);
  np_right = ezcfg_binary_tree_node_get_right(np);
  if ((np_left == NULL) && (np_right == NULL)) {
    return true;
  }

exit_fail:
  return false;
}
