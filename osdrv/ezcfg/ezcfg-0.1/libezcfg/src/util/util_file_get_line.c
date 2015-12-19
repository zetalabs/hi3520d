/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : util/util_file_get_line.c
 *
 * Description  : get a line from file
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-01-18   0.1       Copy it from ezcd/src/utils/utils_file_get_line.c
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

#include "ezcfg.h"
#include "ezcfg-private.h"

bool ezcfg_util_file_get_line(FILE *fp, char *buf, size_t size, char *comment, char *rm)
{
	if ((fp == NULL) || (buf == NULL) || (size < 1)) {
		return false;
	}

	while(fgets(buf, size, fp) != NULL)
	{
		if(strchr(comment, buf[0]) == NULL)
		{
			int len = strlen(buf);

			while((len > 0) && 
			      (buf[len] == '\0' || 
			       (strchr(rm, buf[len]) != NULL)))
			{
				buf[len] = '\0';
				len --;
			}

			if (len > 0)
				return true;
		}
	}
	return false;
}
