/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_service_binding.c
 *
 * Description  : system service binding interface checking
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-12   0.1       Write it from scrach
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

int ezcfg_util_service_binding(char *interface)
{
	if (strcmp(interface, "lan") == 0) {
		return EZCFG_SERVICE_BINDING_LAN ;
	}
	else if (strcmp(interface, "wan") == 0) {
		return EZCFG_SERVICE_BINDING_WAN ;
	}
	else {
		return EZCFG_SERVICE_BINDING_UNKNOWN ;
	}
}
