/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_netmask_to_prefix.c
 *
 * Description  : convert netmask 255.255.255.0 to prefix-length 24
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-06-28   0.1       Write it from scratch
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

int utils_netmask_to_prefix(char *nm_str, int *prefix)
{
	int i, j, rc;
	int nm[4];
	bool check_nm;

	if (nm_str == NULL || prefix == NULL)
		return EXIT_FAILURE;

	rc = sscanf(nm_str, "%d.%d.%d.%d", &nm[0], &nm[1], &nm[2], &nm[3]);
	if (rc != 4)
		return EXIT_FAILURE;

	if (nm[0] > 255 || nm[0] < 0 ||
	    nm[1] > 255 || nm[1] < 0 ||
	    nm[2] > 255 || nm[2] < 0 ||
	    nm[3] > 255 || nm[3] < 0)
		return EXIT_FAILURE;

	rc = 0;
	check_nm = false;
	for(i=0; i<4; i++) {
		for (j=7; j>=0; j--) {
			if (check_nm == false) {
				if (nm[i] & (1<<j)) {
					rc++;
				}
				else {
					check_nm = true;
				}
			}
			else {
				if (nm[i] & (1<<j)) {
					return EXIT_FAILURE;
				}
			}
		}
	}

	*prefix = rc;
	return EXIT_SUCCESS;
}
