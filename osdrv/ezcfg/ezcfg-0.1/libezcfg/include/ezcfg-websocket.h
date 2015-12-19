/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-websocket.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2013-04-15   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_WEBSOCKET_H_
#define _EZCFG_WEBSOCKET_H_

/* ezcfg HTTP websocket headers, in RFC 6455 */
#define EZCFG_WS_HTTP_HEADER_SEC_WEBSOCKET_KEY \
	"Sec-WebSocket-Key"
#define EZCFG_WS_HTTP_HEADER_SEC_WEBSOCKET_VERSION \
	"Sec-WebSocket-Version"
#define EZCFG_WS_HTTP_HEADER_SEC_WEBSOCKET_PROTOCOL \
	"Sec-WebSocket-Protocol"
#define EZCFG_WS_HTTP_HEADER_SEC_WEBSOCKET_EXTENSIONS \
	"Sec-WebSocket-Extensions"

#define EZCFG_WS_HTTP_HEADER_KEYWORD_WEBSOCKET \
	"websocket"

#endif /* _EZCFG_WEBSOCKET_H_ */
