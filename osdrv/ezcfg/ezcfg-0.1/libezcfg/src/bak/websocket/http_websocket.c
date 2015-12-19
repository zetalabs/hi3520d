/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/http_websocket.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2013-04-14   0.1       Write it from scratch
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
#include "ezcfg-http.h"

struct ezcfg_http_websocket {
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_websocket *ws;
};

/**
 * Private functions
 **/
static int build_http_websocket_response(struct ezcfg_http *http,
	struct ezcfg_websocket *ws)
{
	//struct ezcfg *ezcfg;
	int rc = 0;
	
	ASSERT(http != NULL);
	ASSERT(ws != NULL);

	//ezcfg = ezcfg_http_get_ezcfg(http);

	/* FIXME: name point to http->request_uri !!!
         * never reset http before using name */
	/* clean http structure info */
	ezcfg_http_reset_attributes(http);
	ezcfg_http_set_status_code(http, 200);
	ezcfg_http_set_state_response(http);

	/* set return value */
	rc = 0;
	return rc;
}

/**
 * Public functions
 **/
void ezcfg_http_websocket_delete(struct ezcfg_http_websocket *hws)
{
	ASSERT(hws != NULL);

	free(hws);
}

struct ezcfg_http_websocket *ezcfg_http_websocket_new(
	struct ezcfg *ezcfg,
	struct ezcfg_http *http,
	struct ezcfg_websocket *ws)
{
	struct ezcfg_http_websocket *hws;

	ASSERT(ezcfg != NULL);
	ASSERT(http != NULL);
	ASSERT(ws != NULL);

	hws = malloc(sizeof(struct ezcfg_http_websocket));
	if (hws != NULL) {
		memset(hws, 0, sizeof(struct ezcfg_http_websocket));
		hws->ezcfg = ezcfg;
		hws->http = http;
		hws->ws = ws;
        }
	return hws;
}

int ezcfg_http_handle_websocket_request(struct ezcfg_http_websocket *hws)
{
	//struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_websocket *ws;

	ASSERT(hws != NULL);

	http = hws->http;
	ws = hws->ws;

	ASSERT(http != NULL);
	ASSERT(ws != NULL);

	//ezcfg = ezcfg_http_get_ezcfg(http);

	return build_http_websocket_response(http, ws);
}
