/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : json/json_http_nvram.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2014-03-24   0.1       Write it from scratch
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
#include "ezcfg-json_http.h"

/**
 * Private functions
 **/
/* it must be nvram has a state coherence with list */
static int build_nvram_set_response(struct ezcfg_nvram *nvram,
	struct ezcfg_linked_list * list,
	struct ezcfg_http *http)
{
	int rc = -1;
	
	ASSERT(http != NULL);

	if (ezcfg_nvram_set_entries(nvram, list) == true) {
		/* FIXME: name point to http->request_uri !!!
	         * never reset http before using name */
		/* clean http structure info */
		ezcfg_http_reset_attributes(http);
		ezcfg_http_set_status_code(http, 200);
		ezcfg_http_set_state_response(http);

		/* set return value */
		rc = 0;
	}

	return rc;
}

/**
 * Public functions
 **/
int ezcfg_json_http_handle_nvram_request(struct ezcfg_json_http *jh, struct ezcfg_nvram *nvram)
{
	struct ezcfg_http *http;
	struct ezcfg_json *json;
	struct ezcfg_linked_list *list;
	int ret = -1;

	ASSERT(jh != NULL);
	ASSERT(jh->http != NULL);
	ASSERT(jh->json != NULL);
	ASSERT(nvram != NULL);

	http = jh->http;
	json = jh->json;

	/* json contains parsed text info */
	list = ezcfg_json_build_nvram_node_list(json);
	if (list != NULL) {
		ret = build_nvram_set_response(nvram, list, http);
		ezcfg_linked_list_delete(list);
	}

	return ret;
}
