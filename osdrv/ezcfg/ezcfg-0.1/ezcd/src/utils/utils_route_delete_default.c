/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_route_delete_default.c
 *
 * Description  : delete default route entries on special interface
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-11   0.1       Write it from scratch
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
#include <dlfcn.h>

#include "ezcd.h"

/*
 * example of /proc/net/route
 ****************************
Iface   Destination     Gateway         Flags   RefCnt  Use     Metric  Mask            MTU     Window  IRTT 
eth1    006AA8C0        00000000        0001    0       0       0       00FFFFFF        0       0       0
eth0    0038A8C0        00000000        0001    0       0       0       00FFFFFF        0       0       0
eth1    00000000        016AA8C0        0003    0       0       1       00000000        0       0       0                 
 ****************************
 */

/* return deleted entries number */
int utils_route_delete_default(char *iface)
{
        FILE *file = NULL;
	char buf[128];
	char *p, *q;
	int ret = 0;
	char *argv[] = { CMD_IP, "route", "del", "default", "dev", iface, NULL };

	if ((iface == NULL) || (*iface == '\0')){
		argv[4] = NULL;	/* "dev" to NULL */
		argv[5] = NULL;	/* iface to NULL */
	}

	/* get route info from /proc/net/route */
	file = fopen("/proc/net/route", "r");
	if (file != NULL) {
		while (utils_file_get_line(file,
			buf, sizeof(buf), "#", LINE_TAIL_STRING) == true) {

			/* first check Iface */
			p = strchr(buf, '\t');
			if (p == NULL)
				continue;
			*p = '\0';
			if ((iface != NULL) &&
			    (*iface != '\0') &&
			    (strcmp(iface, buf) != 0))
				continue;

			/* then check Destination */
			p++;
			q = strchr(p, '\t');
			if (q == NULL)
				continue;

			/* find a default routing entry on iface */
			if (strcmp(p, "00000000") == 0) {
				utils_execute(argv, NULL, NULL, 0, NULL);
				ret++;
			}
		}
		fclose(file);
	}

	return ret;
}
