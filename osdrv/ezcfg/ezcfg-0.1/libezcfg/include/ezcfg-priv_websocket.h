/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_websocket.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2013-04-14   0.1       Split it from ezcfg-private.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_WEBSOCKET_H_
#define _EZCFG_PRIV_WEBSOCKET_H_

#include "ezcfg-types.h"

/* websocket/websocket.c */
struct ezcfg_websocket *ezcfg_websocket_new(struct ezcfg *ezcfg);
void ezcfg_websocket_delete(struct ezcfg_websocket *ws);
struct ezcfg_socket *ezcfg_websocket_get_socket(struct ezcfg_websocket *wsp);
bool ezcfg_websocket_set_socket(struct ezcfg_websocket *wsp, struct ezcfg_socket *sp);
struct ezcfg_ssl *ezcfg_websocket_get_ssl(struct ezcfg_websocket *wsp);
bool ezcfg_websocket_set_ssl(struct ezcfg_websocket *wsp, struct ezcfg_ssl *sslp);

#endif /* _EZCFG_PRIV_WEBSOCKET_H_ */
