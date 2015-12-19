/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_socket_domain.c
 *
 * Description  : socket domain settings
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-29   0.1       Write it from scrach
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

struct domain_pair {
	int index;
	char *name;
};

static struct domain_pair ezcfg_support_domains[] = {
	{ -1, NULL },
	{ AF_LOCAL, EZCFG_SOCKET_DOMAIN_LOCAL_STRING },
	{ AF_INET, EZCFG_SOCKET_DOMAIN_INET_STRING },
	{ AF_NETLINK, EZCFG_SOCKET_DOMAIN_NETLINK_STRING },
#if (HAVE_EZBOX_EZCFG_IPV6 == 1)
	{ AF_INET6, EZCFG_SOCKET_DOMAIN_INET6_STRING },
#endif
};

int ezcfg_util_socket_domain_get_index(char *name)
{
	size_t i;
	struct domain_pair *dip;
	for (i = 1; i < ARRAY_SIZE(ezcfg_support_domains); i++) {
		dip = &(ezcfg_support_domains[i]);
		if (strcmp(dip->name, name) == 0)
			return dip->index;
	}
	return -1;
}

bool ezcfg_util_socket_is_supported_domain(const int domain)
{
	size_t i;
	struct domain_pair *dip;
	for (i = 1; i < ARRAY_SIZE(ezcfg_support_domains); i++) {
		dip = &(ezcfg_support_domains[i]);
		if (domain == dip->index)
			return true;
	}
	return false;
}

