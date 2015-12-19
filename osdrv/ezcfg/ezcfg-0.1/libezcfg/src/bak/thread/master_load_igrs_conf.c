/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/master_load_igrs_conf.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-30   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#if 1
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

/* setup IGRS devices and groups */
void ezcfg_master_load_igrs_conf(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;
	char *p = NULL;
#if 0
	int domain, type, proto;
	char address[256];
#endif
	int i;
	int igrs_number = -1;

	if (master == NULL)
		return ;

	ezcfg = ezcfg_master_get_ezcfg(master);

	/* first get the igrs number */
	p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_IGRS_NUMBER);
	if (p != NULL) {
		igrs_number = atoi(p);
		free(p);
	}
	for (i = 0; i < igrs_number; i++) {

		/* initialize */
#if 0
		domain = -1;
		type = -1;
		proto = EZCFG_PROTO_UNKNOWN;
		address[0] = '\0';
#endif
	}

	/* delete all sockets taged need_delete = true in need_listening_sockets */
}
