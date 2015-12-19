/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_ssl.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-02-08   0.1       Split it from ezcfg-private.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_SSL_H_
#define _EZCFG_PRIV_SSL_H_

#include "ezcfg-types.h"

/* ssl/ssl.c */
struct ezcfg_ssl *ezcfg_ssl_new(struct ezcfg *ezcfg, const int role, const int method);
void ezcfg_ssl_delete(struct ezcfg_ssl *sslp);
struct ezcfg_socket *ezcfg_ssl_get_socket(struct ezcfg_ssl *sslp);
bool ezcfg_ssl_set_socket(struct ezcfg_ssl *sslp, struct ezcfg_socket *sp);
bool ezcfg_ssl_is_valid(struct ezcfg_ssl *sslp);
bool ezcfg_ssl_open_session(struct ezcfg_ssl *sslp, const int sock);
bool ezcfg_ssl_close_session(struct ezcfg_ssl *sslp);
const char *ezcfg_ssl_get_certificate_file(struct ezcfg_ssl *sslp);
bool ezcfg_ssl_set_certificate_file(struct ezcfg_ssl *sslp, const char *file);
const char *ezcfg_ssl_get_certificate_chain_file(struct ezcfg_ssl *sslp);
bool ezcfg_ssl_set_certificate_chain_file(struct ezcfg_ssl *sslp, const char *file);
const char *ezcfg_ssl_get_private_key_file(struct ezcfg_ssl *sslp);
bool ezcfg_ssl_set_private_key_file(struct ezcfg_ssl *sslp, const char *file);
int ezcfg_ssl_read(struct ezcfg_ssl *sslp, void *buf, int len, int flags);
int ezcfg_ssl_write(struct ezcfg_ssl *sslp, const void *buf, int len, int flags);
bool ezcfg_ssl_list_in(struct ezcfg_ssl **list, struct ezcfg_ssl *sslp);
bool ezcfg_ssl_list_insert(struct ezcfg_ssl **list, struct ezcfg_ssl *sslp);
void ezcfg_ssl_list_delete(struct ezcfg_ssl **list);
struct ezcfg_ssl *ezcfg_ssl_list_find_by_socket(struct ezcfg_ssl **list, struct ezcfg_socket *sp);

#endif /* _EZCFG_PRIV_SSL_H_ */
