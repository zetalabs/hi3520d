/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_hotplug2.c
 *
 * Description  : ezbox run hotplug2 service
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

int rc_hotplug2(int flag)
{
	FILE *file;
	int ret;
	char cmdline[256];

	pop_etc_hotplug2_rules(flag);

	switch (flag) {
	case RC_BOOT :
		file = fopen("/proc/sys/kernel/hotplug", "w");
		if (file != NULL)
		{
			fprintf(file, "%s", "");
			fclose(file);
		}

		snprintf(cmdline, sizeof(cmdline), "%s --set-worker /lib/hotplug2/worker_fork.so --set-rules-file /etc/hotplug2.rules --no-persistent --set-coldplug-cmd %s", CMD_HOTPLUG2, CMD_UDEVTRIGGER);
		ret = system(cmdline);

#if 0
		snprintf(cmdline, sizeof(cmdline), "%s --set-worker /lib/hotplug2/worker_fork.so --set-rules-file /etc/hotplug2.rules --persistent &", CMD_HOTPLUG2);
		ret = system(cmdline);

		break;
#endif

	case RC_START :
		snprintf(cmdline, sizeof(cmdline), "%s --set-worker /lib/hotplug2/worker_fork.so --set-rules-file /etc/hotplug2.rules --persistent --override --max-children 1 >/dev/null 2>&1 &", CMD_HOTPLUG2);
		ret = system(cmdline);

		break;

	case RC_STOP :
		snprintf(cmdline, sizeof(cmdline), "%s -q hotplug2", CMD_KILLALL);
		ret = system(cmdline);

		break;

	case RC_RESTART :
		rc_hotplug2(RC_STOP);
		rc_hotplug2(RC_START);
		break;
	}
	return (EXIT_SUCCESS);
}
