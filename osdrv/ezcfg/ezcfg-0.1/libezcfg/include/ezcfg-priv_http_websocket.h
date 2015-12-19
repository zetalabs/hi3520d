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

#ifndef _EZCFG_PRIV_HTTP_WEBSOCKET_H_
#define _EZCFG_PRIV_HTTP_WEBSOCKET_H_

#include "ezcfg-types.h"

/* websocket/http_websocket.c */
struct ezcfg_http_websocket *ezcfg_http_websocket_new(
	struct ezcfg *ezcfg,
	struct ezcfg_http *http,
	struct ezcfg_websocket *ws);
void ezcfg_http_websocket_delete(struct ezcfg_http_websocket *hws);
int ezcfg_http_handle_websocket_request(struct ezcfg_http_websocket *hws);

#endif /* _EZCFG_PRIV_HTTP_WEBSOCKET_H_ */
