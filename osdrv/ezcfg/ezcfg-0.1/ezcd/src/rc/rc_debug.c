/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_debug.c
 *
 * Description  : ezbox debug rc running state service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-08-05   0.1       Write it from scratch
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

#include "ezcd.h"
#include "pop_func.h"

static int dump_rc_functions(void)
{
	int i;
	char *p;
	for (i = 1; i <= utils_get_rc_funcs_number(); i++) {
		p = utils_get_rc_func_name(i);
		if (p != NULL) {
			printf("%s\n", p);
		}
	}
	return (EXIT_SUCCESS);
}

int rc_debug(int flag)
{
	if (flag == RC_DEBUG_DUMP)
		return dump_rc_functions();

	return (EXIT_FAILURE);
}
