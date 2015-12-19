/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_get_wan_type.c
 *
 * Description  : ezcfg get WAN type function
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-03-01   0.1       Write it from scratch
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
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>
#include <dirent.h>

#include "ezcd.h"

/*
 * Returns the WAN type enum in utils.h
 */
int utils_get_wan_type(void)
{
	char buf[64];
	int rc;

	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, TYPE), buf, sizeof(buf));
	if (rc < 0) {
		rc = WAN_TYPE_UNKNOWN;
	}
	else if (strcmp(buf, "dhcp") == 0) {
		rc = WAN_TYPE_DHCP;
	}
	else if (strcmp(buf, "static") == 0) {
		rc = WAN_TYPE_STATIC;
	}
	else if (strcmp(buf, "pppoe") == 0) {
		rc = WAN_TYPE_PPPOE;
	}
	else {
		rc = WAN_TYPE_UNKNOWN;
        }
	return rc;
}
