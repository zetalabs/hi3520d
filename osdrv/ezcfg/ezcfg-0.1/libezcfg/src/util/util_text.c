/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_text.c
 *
 * Description  : convert variables to text string
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

char *ezcfg_util_text_get_mtu_enable(int i)
{
	if (i == 0) {
		return "Auto";
	}
	else {
		return "Manual";
	}
}

char *ezcfg_util_text_get_keep_enable(int i)
{
	if (i == 0) {
		return "Dial on Demand";
	}
	else {
		return "Keep Alive";
	}
}

char *ezcfg_util_text_get_service_switch(bool v)
{
	if (v == true) {
		return "Enabled";
	}
	else {
		return "Disabled";
	}
}
