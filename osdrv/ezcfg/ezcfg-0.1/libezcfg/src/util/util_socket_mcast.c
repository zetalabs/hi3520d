/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_socket_mcast.c
 *
 * Description  : socket multicast settings
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-01   0.1       Write it from scrach
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
#include <sys/socket.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

bool ezcfg_util_socket_is_multicast_address(const int proto, const char *addr)
{
#if (HAVE_EZBOX_SERVICE_EZCFG_IGRSD == 1)
	if (proto == EZCFG_PROTO_IGRS_ISDP) {
		if (strcmp(addr, EZCFG_PROTO_IGRS_ISDP_MCAST_IPADDR_STRING) == 0)
			return true;
#if (HAVE_EZBOX_EZCFG_IPV6 == 1)
		if (strcasecmp(addr, EZCFG_PROTO_IGRS_ISDP_MCAST_IPV6_LINK_LOCAL_ADDR_STRING) == 0)
			return true;
		if (strcasecmp(addr, EZCFG_PROTO_IGRS_ISDP_MCAST_IPV6_SITE_LOCAL_ADDR_STRING) == 0)
			return true;
		if (strcasecmp(addr, EZCFG_PROTO_IGRS_ISDP_MCAST_IPV6_GLOBAL_ADDR_STRING) == 0)
			return true;
#endif
	}
#endif
#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
	if (proto == EZCFG_PROTO_UPNP_SSDP) {
		if (strcmp(addr, EZCFG_PROTO_UPNP_SSDP_MCAST_IPADDR_STRING) == 0)
			return true;
#if (HAVE_EZBOX_EZCFG_IPV6 == 1)
		if (strcasecmp(addr, EZCFG_PROTO_UPNP_SSDP_MCAST_IPV6_LINK_LOCAL_ADDR_STRING) == 0)
			return true;
		if (strcasecmp(addr, EZCFG_PROTO_UPNP_SSDP_MCAST_IPV6_SITE_LOCAL_ADDR_STRING) == 0)
			return true;
		if (strcasecmp(addr, EZCFG_PROTO_UPNP_SSDP_MCAST_IPV6_GLOBAL_ADDR_STRING) == 0)
			return true;
#endif
	}
#endif
	return false;
}
