/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_modules.c
 *
 * Description  : ezbox /etc/modules generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-02   0.1       Write it from scratch
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

int pop_etc_modules(int flag)
{
	FILE *file;
	char *p, *q;
	char buf[KERNEL_COMMAND_LINE_SIZE];
	int rc;

	file = fopen("/etc/modules", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	switch (flag) {
	case RC_ACT_BOOT :
	case RC_ACT_RESTART :
	case RC_ACT_START :
		if (flag == RC_ACT_BOOT) {
			/* get the kernel module name from boot.cfg */
			rc = utils_get_bootcfg_keyword(NVRAM_SERVICE_OPTION(SYS, MODULES), buf, sizeof(buf));
		}
		else {
			/* get the kernel module name from nvram */
			rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(SYS, MODULES), buf, sizeof(buf));
		}

		if (rc > 0) {
			p = buf;
			while(p != NULL) {
				q = strchr(p, ',');
				if (q != NULL)
					*q = '\0';
				fprintf(file, "%s\n", p);
				if (q != NULL)
					p = q+1;
				else
					p = NULL;
			}
		}
		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
