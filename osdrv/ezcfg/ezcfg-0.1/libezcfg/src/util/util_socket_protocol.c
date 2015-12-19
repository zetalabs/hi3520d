/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_socket_protocol.c
 *
 * Description  : socket protocol settings
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-24   0.1       Write it from scrach
 * ============================================================================
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

struct proto_pair {
	int index;
	char *name;
};

static struct proto_pair ezcfg_support_protocols[] = {
	{ EZCFG_PROTO_UNKNOWN, NULL },
	{ EZCFG_PROTO_CTRL, EZCFG_SOCKET_PROTO_CTRL_STRING },
	{ EZCFG_PROTO_HTTP, EZCFG_SOCKET_PROTO_HTTP_STRING },
	{ EZCFG_PROTO_HTTPS, EZCFG_SOCKET_PROTO_HTTPS_STRING },
	{ EZCFG_PROTO_SOAP_HTTP, EZCFG_SOCKET_PROTO_SOAP_HTTP_STRING },
	{ EZCFG_PROTO_IGRS, EZCFG_SOCKET_PROTO_IGRS_STRING },
	{ EZCFG_PROTO_IGRS_ISDP, EZCFG_SOCKET_PROTO_IGRS_ISDP_STRING },
	{ EZCFG_PROTO_UEVENT, EZCFG_SOCKET_PROTO_UEVENT_STRING },
#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
	{ EZCFG_PROTO_UPNP_SSDP, EZCFG_SOCKET_PROTO_UPNP_SSDP_STRING },
	{ EZCFG_PROTO_UPNP_HTTP, EZCFG_SOCKET_PROTO_UPNP_HTTP_STRING },
	{ EZCFG_PROTO_UPNP_GENA, EZCFG_SOCKET_PROTO_UPNP_GENA_STRING },
#endif
	{ EZCFG_PROTO_JSON_HTTP, EZCFG_SOCKET_PROTO_JSON_HTTP_STRING },
	{ EZCFG_PROTO_NV_JSON_HTTP, EZCFG_SOCKET_PROTO_NV_JSON_HTTP_STRING },
};

int ezcfg_util_socket_protocol_get_index(char *name)
{
	size_t i;
	struct proto_pair *pip;
	for (i = 1; i < ARRAY_SIZE(ezcfg_support_protocols); i++) {
		pip = &(ezcfg_support_protocols[i]);
		if (strcmp(pip->name, name) == 0)
			return pip->index;
	}
	return EZCFG_PROTO_UNKNOWN;
}

bool ezcfg_util_socket_is_supported_protocol(const int proto)
{
	size_t i;
	struct proto_pair *pip;
	for (i = 1; i < ARRAY_SIZE(ezcfg_support_protocols); i++) {
		pip = &(ezcfg_support_protocols[i]);
		if (proto == pip->index)
			return true;
	}
	return false;
}

