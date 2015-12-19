/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : args/arg_nvram_ssl.c
 *
 * Description  : implement nvram ssl argument
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-02-13   0.1       Write it from scratch
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

bool ezcfg_arg_nvram_ssl_init(struct ezcfg_arg_nvram_ssl *ap)
{
	ASSERT(ap != NULL);

	memset(ap, 0, sizeof(struct ezcfg_arg_nvram_ssl));
	return true;
}

bool ezcfg_arg_nvram_ssl_clean(struct ezcfg_arg_nvram_ssl *ap)
{
	ASSERT(ap != NULL);

	if (ap->role != NULL) {
		free(ap->role);
	}

	if (ap->method != NULL) {
		free(ap->method);
	}

	if (ap->socket_enable != NULL) {
		free(ap->socket_enable);
	}

	if (ap->socket_domain != NULL) {
		free(ap->socket_domain);
	}

	if (ap->socket_type != NULL) {
		free(ap->socket_type);
	}

	if (ap->socket_protocol != NULL) {
		free(ap->socket_protocol);
	}

	if (ap->socket_address != NULL) {
		free(ap->socket_address);
	}

	return true;
}

bool ezcfg_arg_nvram_ssl_delete(struct ezcfg_arg_nvram_ssl *ap)
{
	ASSERT(ap != NULL);

	ezcfg_arg_nvram_ssl_clean(ap);
	free(ap);

	return true;
}

