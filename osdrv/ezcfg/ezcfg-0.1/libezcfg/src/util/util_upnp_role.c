/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_upnp_role.c
 *
 * Description  : upnp role settings
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-09   0.1       Write it from scrach
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

int ezcfg_util_upnp_role(char *name)
{
	if (name == NULL)
		return EZCFG_UPNP_ROLE_UNKNOWN;

	if (strcmp(name, EZCFG_UPNP_ROLE_DEVICE_STRING) == 0)
		return EZCFG_UPNP_ROLE_DEVICE;
	else if (strcmp(name, EZCFG_UPNP_ROLE_CONTROL_POINT_STRING) == 0)
		return EZCFG_UPNP_ROLE_CONTROL_POINT;
	else
		return EZCFG_UPNP_ROLE_UNKNOWN;
}
