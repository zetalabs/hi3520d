/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_init_ezcfg_api.c
 *
 * Description  : initialize ezcfg-api settings
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-03-07   0.1       Write it from scratch
 * ============================================================================
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcd.h"

bool utils_init_ezcfg_api(const char *path)
{
	int ret;
	if (path == NULL) {
		return false;
	}

	/* ezcfg_api_common */
	ret = ezcfg_api_common_set_config_file(path);
	if (ret < 0) {
		return false;
	}

	return true;
}
