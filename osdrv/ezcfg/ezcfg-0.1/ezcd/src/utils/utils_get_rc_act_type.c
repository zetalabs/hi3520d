/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_get_rc_act_type.c
 *
 * Description  : ezcfg get rc action type function
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-10-19   0.1       Write it from scratch
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
 * Returns the rc action type enum in utils.h
 */
int utils_get_rc_act_type(char *act)
{
	int ret;

	if (act == NULL) {
		ret = RC_ACT_UNKNOWN;
	}
	else if (strcmp(act, "boot") == 0) {
		ret = RC_ACT_BOOT;
	}
	else if (strcmp(act, "start") == 0) {
		ret = RC_ACT_START;
	}
	else if (strcmp(act, "stop") == 0) {
		ret = RC_ACT_STOP;
	}
	else if (strcmp(act, "restart") == 0) {
		ret = RC_ACT_RESTART;
	}
	else if (strcmp(act, "reload") == 0) {
		ret = RC_ACT_RELOAD;
	}
	else if (strcmp(act, "combined") == 0) {
		ret = RC_ACT_COMBINED;
	}
	else if (strcmp(act, "usrdef") == 0) {
		ret = RC_ACT_USRDEF;
	}
	else {
		ret = RC_ACT_UNKNOWN;
        }
	return (ret);
}
