/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : upnp/upnp_gena.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-09   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-upnp.h"

struct ezcfg_upnp_gena {
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_upnp *upnp;
};

/**
 * private functions
 **/

/**
 * Public functions
 **/
void ezcfg_upnp_gena_delete(struct ezcfg_upnp_gena *gena)
{
	//struct ezcfg *ezcfg;

	ASSERT(gena != NULL);

	//ezcfg = gena->ezcfg;

	if (gena->http != NULL) {
		ezcfg_http_delete(gena->http);
	}

	free(gena);
}

/**
 * ezcfg_upnp_gena_new:
 * Create ezcfg upnp gena protocol data structure
 * Returns: a new ezcfg upnp gena protocol data structure
 **/
struct ezcfg_upnp_gena *ezcfg_upnp_gena_new(struct ezcfg *ezcfg)
{
	struct ezcfg_upnp_gena *gena;

	ASSERT(ezcfg != NULL);

	/* initialize upnp gena protocol data structure */
	gena = calloc(1, sizeof(struct ezcfg_upnp_gena));
	if (gena == NULL) {
		return NULL;
	}

	memset(gena, 0, sizeof(struct ezcfg_upnp_gena));
	gena->ezcfg = ezcfg;
	gena->http = ezcfg_http_new(ezcfg);
	if (gena->http == NULL) {
		goto fail_exit;
	}

	//gena->upnp = upnp;

	return gena;

fail_exit:
	ezcfg_upnp_gena_delete(gena);
	return NULL;
}

void ezcfg_upnp_gena_reset_attributes(struct ezcfg_upnp_gena *gena)
{
	return;
}
