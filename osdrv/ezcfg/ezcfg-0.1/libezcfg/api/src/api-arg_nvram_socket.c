/* ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : api-arg_nvram_socket.c
 *
 * Description  : ezcfg API for function arguments for nvram socket manipulate
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-02-14   0.1       Write it from scratch
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

struct ezcfg_arg_nvram_socket *ezcfg_api_arg_nvram_socket_new(void)
{
	struct ezcfg_arg_nvram_socket *ap;

	ap = malloc(sizeof(struct ezcfg_arg_nvram_socket));
	if (ap != NULL) {
		ezcfg_arg_nvram_socket_init(ap);
	}

	return ap;
}

int ezcfg_api_arg_nvram_socket_delete(struct ezcfg_arg_nvram_socket *ap)
{
	int rc = 0;
	if (ap == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	ezcfg_arg_nvram_socket_delete(ap);

	return rc;
}

int ezcfg_api_arg_nvram_socket_get_domain(struct ezcfg_arg_nvram_socket *ap, char **pp)
{
	int rc = 0;

	if (ap == NULL || pp == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	*pp = ap->domain;

	return rc;
}

int ezcfg_api_arg_nvram_socket_set_domain(struct ezcfg_arg_nvram_socket *ap, const char *domain)
{
	int rc = 0;
	char *p;

	if (ap == NULL || domain == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	p = strdup(domain);
	if (p == NULL) {
		return -EZCFG_E_SPACE ;
	}

	if (ap->domain != NULL) {
		free(ap->domain);
	}
	ap->domain = p;

	return rc;
}

int ezcfg_api_arg_nvram_socket_get_type(struct ezcfg_arg_nvram_socket *ap, char **pp)
{
	int rc = 0;

	if (ap == NULL || pp == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	*pp = ap->type;

	return rc;
}

int ezcfg_api_arg_nvram_socket_set_type(struct ezcfg_arg_nvram_socket *ap, const char *type)
{
	int rc = 0;
	char *p;

	if (ap == NULL || type == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	p = strdup(type);
	if (p == NULL) {
		return -EZCFG_E_SPACE ;
	}

	if (ap->type != NULL) {
		free(ap->type);
	}
	ap->type = p;

	return rc;
}

int ezcfg_api_arg_nvram_socket_get_protocol(struct ezcfg_arg_nvram_socket *ap, char **pp)
{
	int rc = 0;

	if (ap == NULL || pp == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	*pp = ap->protocol;

	return rc;
}

int ezcfg_api_arg_nvram_socket_set_protocol(struct ezcfg_arg_nvram_socket *ap, const char *protocol)
{
	int rc = 0;
	char *p;

	if (ap == NULL || protocol == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	p = strdup(protocol);
	if (p == NULL) {
		return -EZCFG_E_SPACE ;
	}

	if (ap->protocol != NULL) {
		free(ap->protocol);
	}
	ap->protocol = p;

	return rc;
}

int ezcfg_api_arg_nvram_socket_get_address(struct ezcfg_arg_nvram_socket *ap, char **pp)
{
	int rc = 0;

	if (ap == NULL || pp == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	*pp = ap->address;

	return rc;
}

int ezcfg_api_arg_nvram_socket_set_address(struct ezcfg_arg_nvram_socket *ap, const char *address)
{
	int rc = 0;
	char *p;

	if (ap == NULL || address == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	p = strdup(address);
	if (p == NULL) {
		return -EZCFG_E_SPACE ;
	}

	if (ap->address != NULL) {
		free(ap->address);
	}
	ap->address = p;

	return rc;
}

