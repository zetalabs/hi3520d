/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_wan.c
 *
 * Description  : system WAN connection settings
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-27   0.1       Write it from scrach
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

struct wan_pair {
	char *name;
	char *desc;
};

struct wan_pair ezcfg_wan_types[] = {
	{ "static", "Static IP" },
	{ "dhcp", "Automatic Configuration - DHCP" },
#if (HAVE_EZBOX_WAN_PPPOE == 1)
	{ "pppoe", "PPPoE" },
#endif
#if (HAVE_EZBOX_WAN_PPTP == 1)
	{ "pptp", "PPTP" },
#endif
#if (HAVE_EZBOX_WAN_L2TP == 1)
	{ "l2tp", "L2TP" },
#endif
	{ "disabled", "Disabled" },
};

size_t ezcfg_util_wan_get_type_length(void)
{
	return ARRAY_SIZE(ezcfg_wan_types);
}

char *ezcfg_util_wan_get_type_name_by_index(int i)
{
	return ezcfg_wan_types[i].name;
}

char *ezcfg_util_wan_get_type_desc_by_index(int i)
{
	return ezcfg_wan_types[i].desc;
}

char *ezcfg_util_wan_get_type_desc_by_name(char *name)
{
	size_t i;
	for (i = 0; i < ARRAY_SIZE(ezcfg_wan_types); i++) {
		if (strcmp(ezcfg_wan_types[i].name, name) == 0)
			return ezcfg_wan_types[i].desc;
	}
	return NULL;
}

