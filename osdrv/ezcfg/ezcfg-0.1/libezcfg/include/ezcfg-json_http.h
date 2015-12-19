/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-json_http.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2014-03-14   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_JSON_HTTP_H_
#define _EZCFG_JSON_HTTP_H_

#include "ezcfg.h"
#include "ezcfg-http.h"

struct ezcfg_json_http {
  struct ezcfg *ezcfg;
  struct ezcfg_http *http;
  struct ezcfg_json *json;
};

/* json_http/json_http.c */

/* ezcfg JSON over HTTP socket handler */

#endif
