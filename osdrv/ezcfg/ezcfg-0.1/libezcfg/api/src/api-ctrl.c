/* ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : api-ctrl.c
 *
 * Description  : ezcfg API for ezcfg ctrl manipulate
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-02   0.1       Write it from scratch
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
#include "ezcfg-soap_http.h"

#include "ezcfg-api.h"

#if 0
#define DBG(format, args...) do {\
	FILE *dbg_fp = fopen("/tmp/ezcfg-api-ctrl.dbg", "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

static bool debug = false;

static void log_fn(struct ezcfg *ezcfg, int priority,
                   const char *file, int line, const char *fn,
                   const char *format, va_list args)
{
	if (debug) {
		char buf[1024];
		struct timeval tv;
		struct timezone tz;

		vsnprintf(buf, sizeof(buf), format, args);
		gettimeofday(&tv, &tz);
		fprintf(stderr, "%llu.%06u [%u] %s(%d): %s",
		        (unsigned long long) tv.tv_sec, (unsigned int) tv.tv_usec,
		        (int) getpid(), fn, line, buf);
	}
#if 0
	else {
		vsyslog(priority, format, args);
	}
#endif
}

/**
 * ezcfg_api_ctrl_exec:
 * @argv: control command arguments
 * @output: buffer to store result
 * @len: buffer size
 *
 **/
int ezcfg_api_ctrl_exec(char *const argv[], char *output, size_t len)
{
	char buf[256];
	char msg[EZCFG_CTRL_MAX_MESSAGE_SIZE];
	size_t msg_len;
	struct ezcfg *ezcfg = NULL;
	struct ezcfg_ctrl *ezctrl = NULL;
	int i, rc = 0;
	int key, semid = -1;
	struct sembuf res;

	if (argv == NULL || output == NULL || len < 1) {
		return -EZCFG_E_ARGUMENT ;
	}

	if (argv[0] == NULL || *(argv[0]) == '\0') {
		return -EZCFG_E_ARGUMENT ;
	}

	for (i = 0, msg_len = 0; argv[i] != NULL; i++) {
		msg_len += strlen(argv[i]);
		msg_len++; /* one more for blank space or '\0'-terminated */
	}

	if (msg_len > sizeof(msg)) {
		return -EZCFG_E_SPACE ;
	}

	ezcfg = ezcfg_new(ezcfg_api_common_get_config_file());
	if (ezcfg == NULL) {
		return -EZCFG_E_RESOURCE ;
	}

	ezcfg_log_init("ctrl_exec");
	ezcfg_common_set_log_fn(ezcfg, log_fn);

	/* prepare semaphore */
	//key = ftok(EZCFG_SEM_EZCFG_PATH, EZCFG_SEM_PROJID_EZCFG);
	key = ftok(ezcfg_common_get_sem_ezcfg_path(ezcfg), EZCFG_SEM_PROJID_EZCFG);
	if (key == -1) {
		DBG("<6>pid=[%d] ftok error.\n", getpid());
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	/* create a semaphore set */
	semid = semget(key, EZCFG_SEM_NUMBER, 00666);
	while (semid < 0) {
		DBG("<6>pid=[%d] try to create sem.\n", getpid());
		semid = semget(key, EZCFG_SEM_NUMBER, 00666);
	}
	DBG("<6>pid=[%d] %s(%d) create sem OK.\n", getpid(), __func__, __LINE__);

	/* now require available resource */
	res.sem_num = EZCFG_SEM_CTRL_INDEX;
	res.sem_op = -1;
	res.sem_flg = 0;

	if (semop(semid, &res, 1) == -1) {
		DBG("<6>pid=[%d] semop require_res error\n", getpid());
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	/* prepare ctrl message */
	snprintf(msg, sizeof(msg), "%s", argv[0]);
	for (i = 1; argv[i] != NULL; i++) {
		strcat(msg, " ");
		strcat(msg, argv[i]);
	}

	/* try to connect ctrl socket */
	//snprintf(buf, sizeof(buf), "%s-%d", EZCFG_SOCK_CTRL_PATH, getpid());
	snprintf(buf, sizeof(buf), "%s-%d", ezcfg_common_get_sock_ctrl_path(ezcfg), getpid());
	//ezctrl = ezcfg_ctrl_new_from_socket(ezcfg, AF_LOCAL, EZCFG_PROTO_CTRL, buf, EZCFG_SOCK_CTRL_PATH);
	ezctrl = ezcfg_ctrl_new_from_socket(ezcfg, AF_LOCAL, EZCFG_PROTO_CTRL, buf, ezcfg_common_get_sock_ctrl_path(ezcfg));

	if (ezctrl == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto sem_exit;
	}

	if (ezcfg_ctrl_connect(ezctrl) < 0) {
		rc = -EZCFG_E_CONNECTION ;
		goto sem_exit;
	}

	if (ezcfg_ctrl_write(ezctrl, msg, msg_len, 0) < 0) {
		rc = -EZCFG_E_WRITE ;
		goto sem_exit;
	}

	memset(output, 0, len);

	msg_len = ezcfg_ctrl_read(ezctrl, output, len, 0);
	if (msg_len < 1) {
		rc = -EZCFG_E_READ ;
		goto sem_exit;
	}

	rc = msg_len;

sem_exit:
	/* now release resource */
	res.sem_num = EZCFG_SEM_CTRL_INDEX;
	res.sem_op = 1;
	res.sem_flg = 0;

	if (semop(semid, &res, 1) == -1) {
		DBG("<6>pid=[%d] semop release_res error\n", getpid());
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

exit:
        if (ezctrl != NULL) {
                ezcfg_ctrl_delete(ezctrl);
	}

        if (ezcfg != NULL) {
                ezcfg_delete(ezcfg);
	}

	return rc;
}
