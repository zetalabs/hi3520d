/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_socket_type.c
 *
 * Description  : socket type settings
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

struct type_pair {
	int index;
	char *name;
};

static struct type_pair ezcfg_support_types[] = {
	{ -1, NULL },
	{ SOCK_STREAM, EZCFG_SOCKET_TYPE_STREAM_STRING },
	{ SOCK_DGRAM, EZCFG_SOCKET_TYPE_DGRAM_STRING },
	{ SOCK_RAW, EZCFG_SOCKET_TYPE_RAW_STRING },
};

int ezcfg_util_socket_type_get_index(char *name)
{
	size_t i;
	struct type_pair *tip;
	for (i = 1; i < ARRAY_SIZE(ezcfg_support_types); i++) {
		tip = &(ezcfg_support_types[i]);
		if (strcmp(tip->name, name) == 0)
			return tip->index;
	}
	return (-1);
}

bool ezcfg_util_socket_is_supported_type(const int type)
{
	size_t i;
	struct type_pair *tip;
	for (i = 1; i < ARRAY_SIZE(ezcfg_support_types); i++) {
		tip = &(ezcfg_support_types[i]);
		if (type == tip->index)
			return true;
	}
	return false;
}

