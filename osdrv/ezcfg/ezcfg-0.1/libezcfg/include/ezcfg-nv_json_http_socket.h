/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-nv_json_http_socket.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2015 by ezbox-project
 *
 * History      Rev       Description
 * 2014-06-07   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_NV_JSON_HTTP_SOCKET_H_
#define _EZCFG_NV_JSON_HTTP_SOCKET_H_

#include "ezcfg.h"
#include "ezcfg-types.h"
#include "ezcfg-http.h"

struct ezcfg_nv_json_http_socket {
  struct ezcfg *ezcfg;
  struct ezcfg_socket *sp;
  struct ezcfg_http *http;
  struct ezcfg_json *json;
};

/* nv_json_http_socket/nv_json_http_socket.c */
int ezcfg_nv_json_http_socket_handle_nvram_request(struct ezcfg_nv_json_http_socket *njhs);

/* ezcfg NVRAM on JSON over HTTP handler */
#define EZCFG_NV_JSON_HTTP_URI                   "/ezcfg/nv_json"

/* ezcfg JSON over HTTP socket handler */

#endif
