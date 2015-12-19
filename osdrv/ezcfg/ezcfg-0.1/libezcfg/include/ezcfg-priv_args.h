/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_auth.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-02-08   0.1       Split it from ezcfg-private.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_ARGS_H_
#define _EZCFG_PRIV_ARGS_H_

#include "ezcfg-types.h"

/* args/arg_nvram_socket.c */
bool ezcfg_arg_nvram_socket_init(struct ezcfg_arg_nvram_socket *ap);
bool ezcfg_arg_nvram_socket_clean(struct ezcfg_arg_nvram_socket *ap);
bool ezcfg_arg_nvram_socket_delete(struct ezcfg_arg_nvram_socket *ap);

/* args/arg_nvram_ssl.c */
bool ezcfg_arg_nvram_ssl_init(struct ezcfg_arg_nvram_ssl *ap);
bool ezcfg_arg_nvram_ssl_clean(struct ezcfg_arg_nvram_ssl *ap);
bool ezcfg_arg_nvram_ssl_delete(struct ezcfg_arg_nvram_ssl *ap);

#endif /* _EZCFG_PRIV_ARGS_H_ */
