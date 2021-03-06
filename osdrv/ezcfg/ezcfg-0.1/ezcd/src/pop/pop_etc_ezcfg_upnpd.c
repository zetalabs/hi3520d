/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_ezcfg_upnpd.c
 *
 * Description  : ezbox /etc/ezcfg_upnpd/ files generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-27   0.1       Write it from scratch
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

#if 0
#define DBG(format, args...) do {\
	FILE *fp = fopen("/dev/kmsg", "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

int pop_etc_ezcfg_upnpd(int flag)
{

	switch (flag) {
	case RC_ACT_BOOT :
	case RC_ACT_START :
	case RC_ACT_RELOAD :
	case RC_ACT_RESTART :
		mkdir(EZCFG_UPNP_CONF_ROOT_PATH, 0755);
#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD_IGD1 == 1)
		/* IGD1 root device */
		if (pop_etc_ezcfg_upnpd_igd1(flag) == EXIT_FAILURE)
			return (EXIT_FAILURE);
#endif
		break;

	default :
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}
