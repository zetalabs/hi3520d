/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : args/arg_nvram_socket.c
 *
 * Description  : implement nvram ssl argument
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-02-14   0.1       Write it from scratch
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

/*
 * Private functions
 */

/*
 * Public functions
 */

bool ezcfg_arg_nvram_socket_init(struct ezcfg_arg_nvram_socket *ap)
{
	ASSERT(ap != NULL);

	memset(ap, 0, sizeof(struct ezcfg_arg_nvram_socket));
	return true;
}

bool ezcfg_arg_nvram_socket_clean(struct ezcfg_arg_nvram_socket *ap)
{
	ASSERT(ap != NULL);

	if (ap->domain != NULL) {
		free(ap->domain);
	}

	if (ap->type != NULL) {
		free(ap->type);
	}

	if (ap->protocol != NULL) {
		free(ap->protocol);
	}

	if (ap->address != NULL) {
		free(ap->address);
	}

	return true;
}

bool ezcfg_arg_nvram_socket_delete(struct ezcfg_arg_nvram_socket *ap)
{
	ASSERT(ap != NULL);

	ezcfg_arg_nvram_socket_clean(ap);
	free(ap);

	return true;
}

