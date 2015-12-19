/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezctp/ezctp.c
 *
 * Description  : implement ezctp shared memory/circular queue
 * Warning      : must exclusively use it, say lock SHM/CQ before using it.
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-03-13   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#if 0
#define DBG(format, args...) do { \
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

/* FIXME: struct ezcfg_ezctp must be placed in shm !!!*/
struct ezcfg_ezctp {
	struct ezcfg *ezcfg;

	/* Shared Memory */
	int shm_id;
	size_t shm_size;
	void *shm_addr;

	/* Circular Queue */
	size_t cq_unit_size; /* the size per unit in the queue */
	size_t cq_length; /* the length of queue */
	size_t cq_free; /* the free units of queue */
	int cq_head;
	int cq_tail;
	pthread_mutex_t cq_mutex; /* Protects circular queue operations */
};

/*
 * Private functions
 */
static bool fill_ezctp_info(struct ezcfg_ezctp *ezctp, const char *conf_path)
{
	char *p;

	if (conf_path == NULL) {
		return false;
	}

	/* shared memory size */
	p = ezcfg_util_get_conf_string(conf_path, EZCFG_EZCFG_SECTION_EZCTP, 0, EZCFG_EZCFG_KEYWORD_SHM_SIZE);
	if (p == NULL) {
		return false;
	}
	else {
		ezctp->shm_size = atoi(p);
		free(p);
	}

	/* cq size per unit */
	p = ezcfg_util_get_conf_string(conf_path, EZCFG_EZCFG_SECTION_EZCTP, 0, EZCFG_EZCFG_KEYWORD_CQ_UNIT_SIZE);
	if (p == NULL) {
		return false;
	}
	else {
		ezctp->cq_unit_size = atoi(p);
		free(p);
	}

	/* cq_length = shm_size / cq_unit_size ; */
	ezctp->cq_length = ezctp->shm_size / ezctp->cq_unit_size;
	if (ezctp->cq_length < 1) {
		return false;
	}

	/* cq_free = cq_length at the beginning */
	ezctp->cq_free = ezctp->cq_length;

	return true;
}

/*
 * Public functions
 */

bool ezcfg_ezctp_delete(struct ezcfg_ezctp *ezctp)
{
	struct ezcfg *ezcfg;

	ASSERT(ezctp != NULL);

	ezcfg = ezctp->ezcfg;

	/* lock ezctp access */
	pthread_mutex_lock(&ezctp->cq_mutex);

	if ((void *) -1 != ezctp->shm_addr) {
		/* detach system V shared memory from system */
		if (shmdt(ezctp->shm_addr) == -1) {
			DBG("<6>pid=[%d] shmdt error\n", getpid());
		}
		else {
			DBG("<6>pid=[%d] shm detach OK.\n", getpid());
		}
		ezctp->shm_addr = (void *) -1;
	}

	if (ezctp->shm_id >= 0) {
		/* remove system V shared memory from system */
		if (shmctl(ezctp->shm_id, IPC_RMID, NULL) == -1) {
			DBG("<6>pid=[%d] shmctl IPC_RMID error\n", getpid());
		}
		else {
			DBG("<6>pid=[%d] remove shm OK.\n", getpid());
		}
		ezctp->shm_id = -1;
	}

	/* unlock ezctp access */
	pthread_mutex_unlock(&ezctp->cq_mutex);

	pthread_mutex_destroy(&ezctp->cq_mutex);

	free(ezctp);

	return true;
}

struct ezcfg_ezctp *ezcfg_ezctp_new(struct ezcfg *ezcfg)
{
	struct ezcfg_ezctp *ezctp;
	key_t key;

	ASSERT(ezcfg != NULL);

	ezctp = malloc(sizeof(struct ezcfg_ezctp));
	if (ezctp == NULL) {
		err(ezcfg, "can not malloc ezctp\n");
		return NULL;
	}

	memset(ezctp, 0, sizeof(struct ezcfg_ezctp));

	ezctp->ezcfg = ezcfg;
	ezctp->shm_addr = (void *) -1;

	/* initialize ezctp/cq_mutex */
	pthread_mutex_init(&ezctp->cq_mutex, NULL);

	if (fill_ezctp_info(ezctp, ezcfg_common_get_config_file(ezcfg)) == false) {
                DBG("<6>pid=[%d] %s(%d) fill ezctp info error.\n", getpid(), __func__, __LINE__);
                goto fail_exit;
	}

	/* prepare shared memory */
	key = ftok(ezcfg_common_get_shm_ezctp_path(ezcfg), EZCFG_SHM_PROJID_EZCTP);
	if (key == -1) {
		DBG("<6>pid=[%d] ftok error.\n", getpid());
		goto fail_exit;
	}

	/* create shared memory */
	/* this is the first place to create the shared memory, must IPC_EXCL */
	ezctp->shm_id = shmget(key, ezctp->shm_size, IPC_CREAT|IPC_EXCL|00666);
	if (ezctp->shm_id < 0) {
		DBG("<6>pid=[%d] %s(%d) try to create sem error.\n", getpid(), __func__, __LINE__);
		goto fail_exit;
	}

	ezctp->shm_addr = shmat(ezctp->shm_id, NULL, 0);
	if ((void *) -1 == ezctp->shm_addr) {
		DBG("<6>pid=[%d] %s(%d) shmat error with errono=[%d]\n", getpid(), __func__, __LINE__, errno);
		goto fail_exit;
	}

	return ezctp;

fail_exit:
	ezcfg_ezctp_delete(ezctp);
	return NULL;
}

int ezcfg_ezctp_get_shm_id(struct ezcfg_ezctp *ezctp)
{
	struct ezcfg *ezcfg;

	ASSERT(ezctp != NULL);

	ezcfg = ezctp->ezcfg;

	return ezctp->shm_id;
}

size_t ezcfg_ezctp_get_cq_unit_size(struct ezcfg_ezctp *ezctp)
{
	struct ezcfg *ezcfg;

	ASSERT(ezctp != NULL);

	ezcfg = ezctp->ezcfg;

	return ezctp->cq_unit_size;
}

bool ezcfg_ezctp_insert_data(struct ezcfg_ezctp *ezctp, void *data, size_t n, size_t size)
{
	struct ezcfg *ezcfg;
	bool insert_flag = false;
	size_t i;
	char *src, *dst;

	ASSERT(ezctp != NULL);
	ASSERT(data != NULL);
	ASSERT(n > 0);
	ASSERT(size > 0);

	ezcfg = ezctp->ezcfg;

	pthread_mutex_lock(&(ezctp->cq_mutex));

	if (ezctp->cq_free >= n) {
		/* there's enough free units in CQ */
		for (i=0; i<n; i++) {
			src = (char *)data + (i * size);
			dst = (char *)ezctp->shm_addr + (ezctp->cq_tail * ezctp->cq_unit_size);
			/* copy the data to CQ */
			memcpy(dst, src, size);
			ezctp->cq_tail = (ezctp->cq_tail + 1) % ezctp->cq_length;
		}
		ezctp->cq_free -= n;
		insert_flag = true;
	}

	pthread_mutex_unlock(&(ezctp->cq_mutex));

	return insert_flag;
}
