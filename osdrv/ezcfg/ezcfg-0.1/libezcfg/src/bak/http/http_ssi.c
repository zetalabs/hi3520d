/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/http_ssi.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-16   0.1       Write it from scratch
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
#include "ezcfg-html.h"

struct ezcfg_http_ssi {
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_nvram *nvram;
	struct ezcfg_ssi *ssi;
};

/**
 * Private functions
 **/
static int build_http_ssi_response(struct ezcfg_http *http,
	struct ezcfg_nvram *nvram,
	struct ezcfg_ssi *ssi)
{
	//struct ezcfg *ezcfg;
	int rc = 0;
	
	ASSERT(http != NULL);
	ASSERT(nvram != NULL);
	ASSERT(ssi != NULL);

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
void ezcfg_http_ssi_delete(struct ezcfg_http_ssi *hs)
{
	ASSERT(hs != NULL);

	free(hs);
}

struct ezcfg_http_ssi *ezcfg_http_ssi_new(
	struct ezcfg *ezcfg,
	struct ezcfg_http *http,
	struct ezcfg_nvram *nvram,
	struct ezcfg_ssi *ssi)
{
	struct ezcfg_http_ssi *hs;

	ASSERT(ezcfg != NULL);
	ASSERT(http != NULL);
	ASSERT(nvram != NULL);
	ASSERT(ssi != NULL);

	hs = malloc(sizeof(struct ezcfg_http_ssi));
	if (hs != NULL) {
		memset(hs, 0, sizeof(struct ezcfg_http_ssi));
		hs->ezcfg = ezcfg;
		hs->http = http;
		hs->nvram = nvram;
		hs->ssi = ssi;
        }
	return hs;
}

int ezcfg_http_handle_ssi_request(struct ezcfg_http *http,
	struct ezcfg_nvram *nvram,
	struct ezcfg_ssi *ssi)
{
	//struct ezcfg *ezcfg;

	ASSERT(http != NULL);
	ASSERT(nvram != NULL);
	ASSERT(ssi != NULL);

	//ezcfg = ezcfg_http_get_ezcfg(http);

	return build_http_ssi_response(http, nvram, ssi);
}
