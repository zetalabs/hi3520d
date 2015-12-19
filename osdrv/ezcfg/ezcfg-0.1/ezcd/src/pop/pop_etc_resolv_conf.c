/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_resolv_conf.c
 *
 * Description  : ezbox /etc/resolv.conf file generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-03   0.1       Write it from scratch
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

int pop_etc_resolv_conf(int flag)
{
        FILE *file = NULL;
	char buf[128];
	//char *p, *token, *savep;
	char name[32];
	int i, rc, ret;

	switch(flag) {
	case RC_ACT_BOOT :
	case RC_ACT_START :
	case RC_ACT_RESTART :
		/* generate /etc/resolv.conf */
		file = fopen("/etc/resolv.conf", "w");
		if (file == NULL) {
			return (EXIT_FAILURE);
		}

		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, DOMAIN), buf, sizeof(buf));
		if ((rc == 0) && (*buf != '\0')) {
			fprintf(file, "domain %s\n", buf);
		}

		for (i = 1; i <= 3; i++) {
			snprintf(name, sizeof(name), "%s%d",
				NVRAM_SERVICE_OPTION(WAN, DNS), i);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "nameserver %s\n", buf);
			}
		}
		fclose(file);
		ret = EXIT_SUCCESS;
		break;

	case RC_ACT_STOP :
		unlink("/etc/resolv.conf");
		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}

	return ret;
}
