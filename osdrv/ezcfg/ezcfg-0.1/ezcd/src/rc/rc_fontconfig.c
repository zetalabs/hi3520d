/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_fontconfig.c
 *
 * Description  : ezbox run fontconfig service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-09-02   0.1       Write it from scratch
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

int rc_fontconfig(int flag)
{
	int rc;

	switch (flag) {
	case RC_RESTART :
	case RC_START :
		mkdir("/etc/fonts", 0777);
		mkdir("/etc/fonts/conf.d", 0777);
		pop_etc_fonts_fonts_dtd(RC_START);
		pop_etc_fonts_fonts_conf(RC_START);
		break;
	}
	return (EXIT_SUCCESS);
}
