/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_cleanup_rootfs.c
 *
 * Description  : ezcfg cleanup rootfs files before mounting overlayfs
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-06-19   0.1       Write it from scratch
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

int utils_cleanup_rootfs(char *path)
{
	FILE *file = NULL;
	char buf[FILE_LINE_BUFFER_SIZE];
	int buf_len;
	char *p=NULL;
	int rc;

	if (path == NULL)
		return EXIT_FAILURE;

	file = fopen(path, "r");
	if (file == NULL)
		return EXIT_FAILURE;

	rc = snprintf(buf, sizeof(buf), "%s -rf", CMD_RM);
	if (rc < 0) {
		rc = EXIT_FAILURE;
		goto func_exit;
	}
	p = buf + rc;
	buf_len = sizeof(buf) - rc;

	while (utils_file_get_line(file, p, buf_len, "#", LINE_TAIL_STRING) == true) {
		if (strncmp(p, "d ", 2) == 0) {
			/* remove following file/directory */
			*p = ' '; /* change 'd' to ' ' */
			utils_system(buf);
		}
	}
	rc = EXIT_SUCCESS;

func_exit:
	fclose(file);
	return rc;
}
