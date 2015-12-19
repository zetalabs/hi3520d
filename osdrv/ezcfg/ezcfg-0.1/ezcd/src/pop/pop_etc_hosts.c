/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_hosts.c
 *
 * Description  : ezbox /etc/hosts file generating program
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

int pop_etc_hosts(int flag)
{
        FILE *file = NULL;
	int ret;

	/* generate /etc/hosts */
	file = fopen("/etc/hosts", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	switch (flag) {
	case RC_ACT_BOOT :
		fprintf(file, "%s\t%s\n", "127.0.0.1", "localhost.");
		ret = EXIT_SUCCESS;
		break;

	case RC_ACT_RESTART :
	case RC_ACT_START :
		fprintf(file, "%s\t%s\n", "127.0.0.1", "localhost.");
		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}

	fclose(file);
	return (ret);
}
