/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_http_socket.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2015 by ezbox-project
 *
 * History      Rev       Description
 * 2015-06-08   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_HTTP_SOCKET_H_
#define _EZCFG_PRIV_HTTP_SOCKET_H_

#include "ezcfg-types.h"

/* http_socket/http_socket.c */
struct ezcfg_http_socket;

int ezcfg_http_socket_read_http_header(struct ezcfg_http_socket *hs, char *buf, int bufsiz, int *nread);
int ezcfg_http_socket_read_http_content(struct ezcfg_http_socket *hs, char **pbuf, int header_len, int *bufsiz, int *nread);

#endif /* _EZCFG_PRIV_NV_JSON_HTTP_H_ */
