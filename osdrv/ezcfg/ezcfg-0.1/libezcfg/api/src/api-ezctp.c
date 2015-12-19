/* ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : api-ezctp.c
 *
 * Description  : ezcfg API for ezctp manipulate
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-03-14   0.1       Write it from scratch
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
	FILE *dbg_fp = fopen("/dev/kmsg", "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

/**
 * ezcfg_api_ezctp_insert_market_data:
 *
 **/
bool ezcfg_api_ezctp_insert_market_data(void *shm_ezcfg_addr, const void *data, size_t n, size_t size)
{
	int sem_id;
	struct sembuf res;
	bool insert_flag = false;

	if ((shm_ezcfg_addr == NULL) || (shm_ezcfg_addr == (void *) -1)) {
		return false;
	}

	if ((data == NULL) || (n < 1) || (size < 1)) {
		return false;
	}

	/* create a semaphore set that only includes one semaphore */
	/* shm semaphore has been initialized in ezcd */
	sem_id = ezcfg_shm_get_ezcfg_sem_id(shm_ezcfg_addr);
	if (sem_id == -1) {
		DBG("<6>ezctp: ezcfg_shm_get_ezcfg_sem_id error\n");
		return false;
	}

	/* now require available resource */
	res.sem_num = EZCFG_SEM_EZCTP_INDEX;
	res.sem_op = -1;
	res.sem_flg = 0;

	if (semop(sem_id, &res, 1) == -1) {
		DBG("<6>ezctp: semop require res error\n");
		return false;
	}

	/* insert the data to ezctp shared memory */
	insert_flag = ezcfg_shm_insert_ezctp_market_data(shm_ezcfg_addr, data, n, size);

	/* now release available resource */
	res.sem_num = EZCFG_SEM_EZCTP_INDEX;
	res.sem_op = 1;
	res.sem_flg = 0;

	if (semop(sem_id, &res, 1) == -1) {
		DBG("<6>ezctp: semop release res error\n");
		//return false;
	}

	return insert_flag;
}

/**
 * ezcfg_api_ezctp_remove_market_data:
 *
 **/
bool ezcfg_api_ezctp_remove_market_data(void *shm_ezcfg_addr, void **pdata, size_t *n, size_t *psize)
{
	int sem_id;
	struct sembuf res;
	bool insert_flag = false;

	if ((shm_ezcfg_addr == NULL) || (shm_ezcfg_addr == (void *) -1)){
		return false;
	}

	if ((pdata == NULL) || (n == NULL) || (psize == NULL)) {
		return false;
	}

	/* create a semaphore set that only includes one semaphore */
	/* shm semaphore has been initialized in ezcd */
	sem_id = ezcfg_shm_get_ezcfg_sem_id(shm_ezcfg_addr);
	if (sem_id == -1) {
		DBG("<6>ezctp: ezcfg_shm_get_ezcfg_sem_id error\n");
		return false;
	}

	/* now require available resource */
	res.sem_num = EZCFG_SEM_EZCTP_INDEX;
	res.sem_op = -1;
	res.sem_flg = 0;

	if (semop(sem_id, &res, 1) == -1) {
		DBG("<6>ezctp: semop require res error\n");
		return false;
	}

	/* insert the data to ezctp shared memory */
	insert_flag = ezcfg_shm_remove_ezctp_market_data(shm_ezcfg_addr, pdata, n, psize);

	/* now release available resource */
	res.sem_num = EZCFG_SEM_EZCTP_INDEX;
	res.sem_op = 1;
	res.sem_flg = 0;

	if (semop(sem_id, &res, 1) == -1) {
		DBG("<6>ezctp: semop release res error\n");
		//return false;
	}

	return insert_flag;
}

/**
 * ezcfg_api_ezctp_save_market_data:
 *
 **/
bool ezcfg_api_ezctp_save_market_data(void *shm_ezcfg_addr, FILE *fp, size_t size, int flag)
{
	int sem_id;
	struct sembuf res;
	bool save_flag = false;

	if ((shm_ezcfg_addr == NULL) || (shm_ezcfg_addr == (void *) -1)){
		return false;
	}

	if ((fp == NULL) || (size < 1)) {
		return false;
	}

	/* create a semaphore set that only includes one semaphore */
	/* shm semaphore has been initialized in ezcd */
	sem_id = ezcfg_shm_get_ezcfg_sem_id(shm_ezcfg_addr);
	if (sem_id == -1) {
		DBG("<6>ezctp: ezcfg_shm_get_ezcfg_sem_id error\n");
		return false;
	}

	/* now require available resource */
	res.sem_num = EZCFG_SEM_EZCTP_INDEX;
	res.sem_op = -1;
	res.sem_flg = 0;

	if (semop(sem_id, &res, 1) == -1) {
		DBG("<6>ezctp: semop require res error\n");
		return false;
	}

	/* save the data in ezctp shared memory to file */
	save_flag = ezcfg_shm_save_ezctp_market_data(shm_ezcfg_addr, fp, size, flag);

	/* now release available resource */
	res.sem_num = EZCFG_SEM_EZCTP_INDEX;
	res.sem_op = 1;
	res.sem_flg = 0;

	if (semop(sem_id, &res, 1) == -1) {
		DBG("<6>ezctp: semop release res error\n");
		//return false;
	}

	return save_flag;
}
