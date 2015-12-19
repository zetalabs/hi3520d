/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_nano_x.c
 *
 * Description  : ezbox run nano-X service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-31   0.1       Write it from scratch
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

int rc_nano_x(int flag)
{
	int rc;
	char buf[64];

	switch (flag) {
	case RC_START :
		rc = nvram_match(NVRAM_SERVICE_OPTION(RC, NANO_X_ENABLE), "1");
		if (rc < 0) {
			return (EXIT_FAILURE);
		}

		/* nano-X MOUSE_PORT */
		buf[0] = '\0';
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(NANO_X, MOUSE_PORT), buf, sizeof(buf));
		if (buf[0] != '\0') {
			setenv("MOUSE_PORT", buf, 1);
		}

		/* nano-X CONSOLE */
		buf[0] = '\0';
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(NANO_X, CONSOLE), buf, sizeof(buf));
		if (buf[0] != '\0') {
			setenv("CONSOLE", buf, 1);
		}

		/* start nano-X server */
		snprintf(buf, sizeof(buf), "start-stop-daemon -S -b -n nano-X -a /usr/bin/nano-X");
		system(buf);

		/* start nanowm */
		snprintf(buf, sizeof(buf), "start-stop-daemon -S -b -n nanowm -a /usr/bin/nanowm");
		system(buf);

		/* start nxterm */
		snprintf(buf, sizeof(buf), "start-stop-daemon -S -b -n nxterm -a /usr/bin/nxterm");
		system(buf);
		break;

	case RC_STOP :
		system("start-stop-daemon -K -s KILL -n nano-X");
		break;

	case RC_RESTART :
		rc = rc_nano_x(RC_STOP);
		sleep(1);
		rc = rc_nano_x(RC_START);
		break;
	}
	return (EXIT_SUCCESS);
}
