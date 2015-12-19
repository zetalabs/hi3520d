/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_inittab.c
 *
 * Description  : ezbox /etc/inittab file generating program
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

int pop_etc_inittab(int flag)
{
        FILE *file = NULL;
	char buf[64];

	/* generate /etc/inittab */
	file = fopen("/etc/inittab", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	snprintf(buf, sizeof(buf), "%s %s %s", CMD_RC, "action", "system_start");
	fprintf(file, "::sysinit:%s\n", buf);
	snprintf(buf, sizeof(buf), "%s %s", CMD_UMOUNT, "-a -f");
	fprintf(file, "::shutdown:%s\n", buf);
	fprintf(file, "tts/0::askfirst:%s\n", CMD_LOGIN);
	fprintf(file, "ttyS0::askfirst:%s\n", CMD_LOGIN);
	fprintf(file, "ttyAMA0::askfirst:%s\n", CMD_LOGIN);
	fprintf(file, "tty1::askfirst:%s\n", CMD_LOGIN);
	fprintf(file, "tty2::askfirst:%s\n", CMD_LOGIN);

	fclose(file);
	return (EXIT_SUCCESS);
}
