/* ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : api-upnp.c
 *
 * Description  : ezcfg API for ezcfg upnp manipulate
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-23   0.1       Write it from scratch
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
#include <sys/ipc.h>
#include <sys/sem.h>
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

#include "ezcfg-api.h"

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

//static char task_file[EZCFG_PATH_MAX] = EZCFG_UPNP_TASK_FILE_PATH;

/**
 * ezcfg_api_upnp_set_task_file:
 *
 **/
int ezcfg_api_upnp_set_task_file(const char *path)
{
	return ezcfg_util_upnp_monitor_set_task_file(path);
}

/**
 * ezcfg_api_upnp_get_task_file:
 *
 **/
int ezcfg_api_upnp_get_task_file(char *path, int len)
{
	return ezcfg_util_upnp_monitor_get_task_file(path, len);
}

/**
 * ezcfg_api_upnp_lock_task_file:
 *
 **/
bool ezcfg_api_upnp_lock_task_file(char *sem_ezcfg_path)
{
	return ezcfg_util_upnp_monitor_lock_task_file(sem_ezcfg_path);
}

/**
 * ezcfg_api_upnp_unlock_task_file:
 *
 **/
bool ezcfg_api_upnp_unlock_task_file(char *sem_ezcfg_path)
{
	return ezcfg_util_upnp_monitor_unlock_task_file(sem_ezcfg_path);
}
