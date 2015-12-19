/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : common/shm.c
 *
 * Description  : wrap system V IPC shared memory operations
 * Warning      : must exclusively use it, say lock EZCFG_SEM_SHM_INDEX before using it.
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-03-16   0.1       Write it from scratch
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
	FILE *dbg_fp; \
	snprintf(path, 256, "/tmp/%d-debug.txt", getpid()); \
	dbg_fp = fopen(path, "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

struct ezcfg_shm {
	int ezcfg_sem_id;
	int ezcfg_shm_id;
	size_t ezcfg_shm_size;
	size_t ezcfg_nvram_queue_length;
	size_t ezcfg_nvram_queue_num;
	size_t ezcfg_rc_queue_length;
	size_t ezcfg_rc_queue_num;
#if (HAVE_EZBOX_SERVICE_EZCTP == 1)
	/* ezctp shared memory */
	int ezctp_shm_id;
	void *ezctp_shm_addr;
	size_t ezctp_shm_size;
	/* ezctp circular queue */
	size_t ezctp_cq_unit_size; /* the size per unit in the queue */
	size_t ezctp_cq_length; /* the length of queue */
	size_t ezctp_cq_free; /* the free units of queue */
	int ezctp_cq_head;
	int ezctp_cq_tail;
#endif
};

/*
 * Private functions
 */

/*
 * Public functions
 */
size_t ezcfg_shm_get_size(void)
{
	return sizeof(struct ezcfg_shm);
}

int ezcfg_shm_get_ezcfg_sem_id(const struct ezcfg_shm *shm)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	return shm->ezcfg_sem_id;
}

void ezcfg_shm_set_ezcfg_sem_id(struct ezcfg_shm *shm, int sem_id)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	shm->ezcfg_sem_id = sem_id;
}

int ezcfg_shm_get_ezcfg_shm_id(const struct ezcfg_shm *shm)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	return shm->ezcfg_shm_id;
}

void ezcfg_shm_set_ezcfg_shm_id(struct ezcfg_shm *shm, int shm_id)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	shm->ezcfg_shm_id = shm_id;
}

size_t ezcfg_shm_get_ezcfg_shm_size(const struct ezcfg_shm *shm)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	return shm->ezcfg_shm_size;
}

void ezcfg_shm_set_ezcfg_shm_size(struct ezcfg_shm *shm, size_t shm_size)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	shm->ezcfg_shm_size = shm_size;
}

size_t ezcfg_shm_get_ezcfg_nvram_queue_length(const struct ezcfg_shm *shm)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	return shm->ezcfg_nvram_queue_length;
}

void ezcfg_shm_set_ezcfg_nvram_queue_length(struct ezcfg_shm *shm, size_t length)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	shm->ezcfg_nvram_queue_length = length;
}

size_t ezcfg_shm_get_ezcfg_nvram_queue_num(const struct ezcfg_shm *shm)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	return shm->ezcfg_nvram_queue_num;
}

void ezcfg_shm_set_ezcfg_nvram_queue_num(struct ezcfg_shm *shm, size_t num)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	shm->ezcfg_nvram_queue_num = num;
}

size_t ezcfg_shm_get_ezcfg_rc_queue_length(const struct ezcfg_shm *shm)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	return shm->ezcfg_rc_queue_length;
}

void ezcfg_shm_set_ezcfg_rc_queue_length(struct ezcfg_shm *shm, size_t length)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	shm->ezcfg_rc_queue_length = length;
}

size_t ezcfg_shm_get_ezcfg_rc_queue_num(const struct ezcfg_shm *shm)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	return shm->ezcfg_rc_queue_num;
}

void ezcfg_shm_set_ezcfg_rc_queue_num(struct ezcfg_shm *shm, size_t num)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	shm->ezcfg_rc_queue_num = num;
}

#if (HAVE_EZBOX_SERVICE_EZCTP == 1)
void ezcfg_shm_delete_ezctp_shm(struct ezcfg_shm *shm)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	/* detach system V shared memory from system */
	if (shm->ezctp_shm_addr != (void *)-1) {
		if (shmdt(shm->ezctp_shm_addr) == -1) {
			DBG("<6>pid=[%d] shmdt error\n", getpid());
		}
		else {
			DBG("<6>pid=[%d] shm detach OK.\n", getpid());
		}
	}
	if (shm->ezctp_shm_id != -1) {
		/* remove system V shared memory from system */
		if (shmctl(shm->ezctp_shm_id, IPC_RMID, NULL) == -1) {
			DBG("<6>pid=[%d] shmctl IPC_RMID error\n", getpid());
		}
		else {
			DBG("<6>pid=[%d] remove shm OK.\n", getpid());
		}
	}
}

int ezcfg_shm_get_ezctp_shm_id(const struct ezcfg_shm *shm)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	return shm->ezctp_shm_id;
}

void ezcfg_shm_set_ezctp_shm_id(struct ezcfg_shm *shm, int shm_id)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	shm->ezctp_shm_id = shm_id;
}

void *ezcfg_shm_get_ezctp_shm_addr(const struct ezcfg_shm *shm)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	return shm->ezctp_shm_addr;
}

void ezcfg_shm_set_ezctp_shm_addr(struct ezcfg_shm *shm, void *shm_addr)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	shm->ezctp_shm_addr = shm_addr;
}

size_t ezcfg_shm_get_ezctp_shm_size(const struct ezcfg_shm *shm)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	return shm->ezctp_shm_size;
}

void ezcfg_shm_set_ezctp_shm_size(struct ezcfg_shm *shm, size_t shm_size)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	shm->ezctp_shm_size = shm_size;
}

size_t ezcfg_shm_get_ezctp_cq_unit_size(const struct ezcfg_shm *shm)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	return shm->ezctp_cq_unit_size;
}

void ezcfg_shm_set_ezctp_cq_unit_size(struct ezcfg_shm *shm, size_t unit_size)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	shm->ezctp_cq_unit_size = unit_size;
}

size_t ezcfg_shm_get_ezctp_cq_length(const struct ezcfg_shm *shm)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	return shm->ezctp_cq_length;
}

void ezcfg_shm_set_ezctp_cq_length(struct ezcfg_shm *shm, size_t length)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	shm->ezctp_cq_length = length;
}

size_t ezcfg_shm_get_ezctp_cq_free(const struct ezcfg_shm *shm)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	return shm->ezctp_cq_free;
}

void ezcfg_shm_set_ezctp_cq_free(struct ezcfg_shm *shm, size_t length)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	shm->ezctp_cq_free = length;
}

bool ezcfg_shm_insert_ezctp_market_data(struct ezcfg_shm *shm, const void *data, size_t n, size_t size)
{
	bool insert_flag = false;
	size_t i;
	char *src, *dst;
	void *ezctp_shm_addr;

	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);
	ASSERT(data != NULL);
	ASSERT(n > 0);
	ASSERT(size > 0);

	if ((shm->ezctp_cq_free >= n) && (shm->ezctp_shm_id != -1)) {
		ezctp_shm_addr = shmat(shm->ezctp_shm_id, NULL, 0);
		if (ezctp_shm_addr != (void *)-1) {
			/* there's enough free units in CQ */
			for (i = 0; i < n; i++) {
				src = (char *)data + (i * size);
				dst = (char *)ezctp_shm_addr + (shm->ezctp_cq_tail * shm->ezctp_cq_unit_size);
				/* copy the data to CQ */
				memcpy(dst, src, size);
				shm->ezctp_cq_tail = (shm->ezctp_cq_tail + 1) % shm->ezctp_cq_length;
			}
			shmdt(ezctp_shm_addr);
			shm->ezctp_cq_free -= n;
			insert_flag = true;
		}
	}

	return insert_flag;
}

/*
 * if data == NULL, just remove market data
 * else store the market data in (*data)
 */
bool ezcfg_shm_remove_ezctp_market_data(struct ezcfg_shm *shm, void **data, size_t *n, size_t *size)
{
	bool insert_flag = false;
	size_t i;
	char *src, *dst;
	void *ezctp_shm_addr;

	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);
	ASSERT(n != NULL);
	ASSERT(size != NULL);

	*n = shm->ezctp_cq_length - shm->ezctp_cq_free;
	*size = shm->ezctp_cq_unit_size;

	if (((*size) * (*n)) > 0) {
		ezctp_shm_addr = shmat(shm->ezctp_shm_id, NULL, 0);
		if (ezctp_shm_addr != (void *)-1) {
			if (data != NULL) {
				*data = malloc((*size) * (*n));
				if ((*data) != NULL) {
					for (i = 0; i < (*n); i++) {
						src = (char *)ezctp_shm_addr + (shm->ezctp_cq_head * shm->ezctp_cq_unit_size);
						dst = (char *)(*data) + (i * (*size));
						/* copy the data from CQ */
						memcpy(dst, src, (*size));
						shm->ezctp_cq_head = (shm->ezctp_cq_head + 1) % shm->ezctp_cq_length;
					}
					shm->ezctp_cq_free += (*n);
					insert_flag = true;
				}
			}
			else {
				shm->ezctp_cq_head = shm->ezctp_cq_tail;
				shm->ezctp_cq_free += (*n);
				insert_flag = true;
			}
			shmdt(ezctp_shm_addr);
		}
	}

	return insert_flag;
}

bool ezcfg_shm_save_ezctp_market_data(struct ezcfg_shm *shm, FILE *fp, size_t size, int flag)
{
	bool save_flag = false;
	size_t i, n;
	char *src;
	void *ezctp_shm_addr;

	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);
	ASSERT(fp != NULL);
	ASSERT(size > 0);

	n = shm->ezctp_cq_length - shm->ezctp_cq_free;

	if ((flag & 0) == flag) {
		ezctp_shm_addr = shmat(shm->ezctp_shm_id, NULL, 0);
		if (ezctp_shm_addr != (void *)-1) {
			for (i = 0; i < n; i++) {
				src = (char *)ezctp_shm_addr + (shm->ezctp_cq_head * shm->ezctp_cq_unit_size);
				/* copy the data from CQ */
				if (fwrite(src, size, 1, fp) < 1) {
					goto write_exit;
				}
				shm->ezctp_cq_head = (shm->ezctp_cq_head + 1) % shm->ezctp_cq_length;
				shm->ezctp_cq_free ++;
			}
			save_flag = true;
		write_exit:
			shmdt(ezctp_shm_addr);
		}
	}

	return save_flag;
}
#endif
