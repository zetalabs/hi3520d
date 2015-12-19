/* ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : util_upnp_monitor.c
 *
 * Description  : ezcfg API for ezcfg upnp manipulate
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-25   0.1       Copy from api-upnp.c
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

#if 1
#define DBG(format, args...) do {\
	char path[256]; \
	FILE *fp; \
	snprintf(path, 256, "/tmp/%d-debug.txt", getpid()); \
	fp = fopen(path, "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

static char task_file[EZCFG_PATH_MAX] = EZCFG_UPNP_TASK_FILE_PATH;

/**
 * ezcfg_util_upnp_monitor_set_task_file:
 *
 **/
int ezcfg_util_upnp_monitor_set_task_file(const char *path)
{
	int rc = 0;
	if (path == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	rc = strlen(path);
	if (rc >= (int)sizeof(task_file)) {
		return -EZCFG_E_ARGUMENT ;
	}

	rc = snprintf(task_file, sizeof(task_file), "%s", path);
	if (rc < 0) {
		rc = -EZCFG_E_SPACE ;
	}
	return rc;
}

/**
 * ezcfg_util_upnp_monitor_get_task_file:
 *
 **/
int ezcfg_util_upnp_monitor_get_task_file(char *path, size_t len)
{
	int rc = 0;
	if (path == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	if (len < (strlen(task_file)+1)) {
		return -EZCFG_E_SPACE ;
	}

	rc = snprintf(path, len, "%s", task_file);
	if (rc < 0) {
		rc = -EZCFG_E_SPACE ;
	}
	return rc;
}

/**
 * ezcfg_util_upnp_monitor_lock_task_file:
 *
 **/
bool ezcfg_util_upnp_monitor_lock_task_file(char *sem_ezcfg_path)
{
	int key, semid;
	struct sembuf res;

	//key = ftok(EZCFG_SEM_EZCFG_PATH, EZCFG_SEM_PROJID_EZCFG);
	key = ftok(sem_ezcfg_path, EZCFG_SEM_PROJID_EZCFG);
	if (key == -1) {
		return false;
	}

	/* create a semaphore set that only includes one semaphore */
	/* upnp semaphore has been initialized in ezcd */
	semid = semget(key, EZCFG_SEM_NUMBER, 00666);
	if (semid < 0) {
		return false;
	}

	/* now require available resource */
	res.sem_num = EZCFG_SEM_UPNP_INDEX;
	res.sem_op = -1;
	res.sem_flg = 0;

	if (semop(semid, &res, 1) == -1) {
		return false;
	}

	return true;
}

/**
 * ezcfg_util_upnp_monitor_unlock_task_file:
 *
 **/
bool ezcfg_util_upnp_monitor_unlock_task_file(char *sem_ezcfg_path)
{
	int key, semid;
	struct sembuf res;

	//key = ftok(EZCFG_SEM_EZCFG_PATH, EZCFG_SEM_PROJID_EZCFG);
	key = ftok(sem_ezcfg_path, EZCFG_SEM_PROJID_EZCFG);
	if (key == -1) {
		return false;
	}

	/* create a semaphore set that only includes one semaphore */
	/* upnp semaphore has been initialized in ezcd */
	semid = semget(key, EZCFG_SEM_NUMBER, 00666);
	if (semid < 0) {
		return false;
	}

	res.sem_num = EZCFG_SEM_UPNP_INDEX;
	res.sem_op = 1;
	res.sem_flg = 0;

	if (semop(semid, &res, 1) == -1) {
		return false;
	}

	return true;
}
