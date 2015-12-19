/* ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : api-rc.c
 *
 * Description  : ezcfg API for ezcfg rc manipulate
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

/**
 * ezcfg_api_rc_require_semaphore:
 *
 **/
int ezcfg_api_rc_require_semaphore(char *sem_ezcfg_path)
{
	int key, semid;
	struct sembuf res;

	if (sem_ezcfg_path == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	key = ftok(sem_ezcfg_path, EZCFG_SEM_PROJID_EZCFG);
	if (key == -1) {
		DBG("<6>rc:pid=[%d] ftok error.\n", getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* create a semaphore set that only includes one semaphore */
	/* rc semaphore has been initialized in ezcd */
	semid = semget(key, EZCFG_SEM_NUMBER, 00666);
	if (semid < 0) {
		DBG("<6>rc: semget error\n");
		return -EZCFG_E_RESOURCE ;
	}

	/* now require available resource */
	res.sem_num = EZCFG_SEM_RC_INDEX;
	res.sem_op = -1;
	res.sem_flg = 0;

	if (semop(semid, &res, 1) == -1) {
		DBG("<6>rc: semop require res error\n");
		return -EZCFG_E_RESOURCE ;
	}

	return 0;
}

/**
 * ezcfg_api_rc_release_semaphore:
 *
 **/
int ezcfg_api_rc_release_semaphore(char *sem_ezcfg_path)
{
	int key, semid;
	struct sembuf res;

	if (sem_ezcfg_path == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	key = ftok(sem_ezcfg_path, EZCFG_SEM_PROJID_EZCFG);
	if (key == -1) {
		DBG("<6>rc:pid=[%d] ftok error.\n", getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* create a semaphore set that only includes one semaphore */
	/* rc semaphore has been initialized in ezcd */
	semid = semget(key, EZCFG_SEM_NUMBER, 00666);
	if (semid < 0) {
		DBG("<6>rc: semget error\n");
		return -EZCFG_E_RESOURCE ;
	}

	/* now release available resource */
	res.sem_num = EZCFG_SEM_RC_INDEX;
	res.sem_op = 1;
	res.sem_flg = 0;

	if (semop(semid, &res, 1) == -1) {
		DBG("<6>rc: semop release res error\n");
		return -EZCFG_E_RESOURCE ;
	}

	return 0;
}
