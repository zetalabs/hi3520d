/* ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : api-arg_nvram_ssl.c
 *
 * Description  : ezcfg API for function arguments for nvram ssl manipulate
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-02-15   0.1       Write it from scratch
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

struct ezcfg_arg_nvram_ssl *ezcfg_api_arg_nvram_ssl_new(void)
{
	struct ezcfg_arg_nvram_ssl *ap;

	ap = malloc(sizeof(struct ezcfg_arg_nvram_ssl));
	if (ap != NULL) {
		ezcfg_arg_nvram_ssl_init(ap);
	}

	return ap;
}

int ezcfg_api_arg_nvram_ssl_delete(struct ezcfg_arg_nvram_ssl *ap)
{
	int rc = 0;
	if (ap == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	ezcfg_arg_nvram_ssl_delete(ap);

	return rc;
}

int ezcfg_api_arg_nvram_ssl_get_role(struct ezcfg_arg_nvram_ssl *ap,char **pp)
{
	int rc = 0;

	if (ap == NULL || pp == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	*pp = ap->role;

	return rc;
}

int ezcfg_api_arg_nvram_ssl_set_role(struct ezcfg_arg_nvram_ssl *ap, const char *role)
{
	int rc = 0;
	char *p;

	if (ap == NULL || role == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	p = strdup(role);
	if (p == NULL) {
		return -EZCFG_E_SPACE ;
	}

	if (ap->role != NULL) {
		free(ap->role);
	}
	ap->role = p;

	return rc;
}

int ezcfg_api_arg_nvram_ssl_get_method(struct ezcfg_arg_nvram_ssl *ap,char **pp)
{
	int rc = 0;

	if (ap == NULL || pp == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	*pp = ap->method;

	return rc;
}

int ezcfg_api_arg_nvram_ssl_set_method(struct ezcfg_arg_nvram_ssl *ap, const char *method)
{
	int rc = 0;
	char *p;

	if (ap == NULL || method == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	p = strdup(method);
	if (p == NULL) {
		return -EZCFG_E_SPACE ;
	}

	if (ap->method != NULL) {
		free(ap->method);
	}
	ap->method = p;

	return rc;
}

int ezcfg_api_arg_nvram_ssl_get_socket_enable(struct ezcfg_arg_nvram_ssl *ap,char **pp)
{
	int rc = 0;

	if (ap == NULL || pp == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	*pp = ap->socket_enable;

	return rc;
}

int ezcfg_api_arg_nvram_ssl_set_socket_enable(struct ezcfg_arg_nvram_ssl *ap, const char *socket_enable)
{
	int rc = 0;
	char *p;

	if (ap == NULL || socket_enable == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	p = strdup(socket_enable);
	if (p == NULL) {
		return -EZCFG_E_SPACE ;
	}

	if (ap->socket_enable != NULL) {
		free(ap->socket_enable);
	}
	ap->socket_enable = p;

	return rc;
}

int ezcfg_api_arg_nvram_ssl_get_socket_domain(struct ezcfg_arg_nvram_ssl *ap,char **pp)
{
	int rc = 0;

	if (ap == NULL || pp == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	*pp = ap->socket_domain;

	return rc;
}

int ezcfg_api_arg_nvram_ssl_set_socket_domain(struct ezcfg_arg_nvram_ssl *ap, const char *socket_domain)
{
	int rc = 0;
	char *p;

	if (ap == NULL || socket_domain == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	p = strdup(socket_domain);
	if (p == NULL) {
		return -EZCFG_E_SPACE ;
	}

	if (ap->socket_domain != NULL) {
		free(ap->socket_domain);
	}
	ap->socket_domain = p;

	return rc;
}

int ezcfg_api_arg_nvram_ssl_get_socket_type(struct ezcfg_arg_nvram_ssl *ap,char **pp)
{
	int rc = 0;

	if (ap == NULL || pp == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	*pp = ap->socket_type;

	return rc;
}

int ezcfg_api_arg_nvram_ssl_set_socket_type(struct ezcfg_arg_nvram_ssl *ap, const char *socket_type)
{
	int rc = 0;
	char *p;

	if (ap == NULL || socket_type == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	p = strdup(socket_type);
	if (p == NULL) {
		return -EZCFG_E_SPACE ;
	}

	if (ap->socket_type != NULL) {
		free(ap->socket_type);
	}
	ap->socket_type = p;

	return rc;
}

int ezcfg_api_arg_nvram_ssl_get_socket_protocol(struct ezcfg_arg_nvram_ssl *ap,char **pp)
{
	int rc = 0;

	if (ap == NULL || pp == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	*pp = ap->socket_protocol;

	return rc;
}

int ezcfg_api_arg_nvram_ssl_set_socket_protocol(struct ezcfg_arg_nvram_ssl *ap, const char *socket_protocol)
{
	int rc = 0;
	char *p;

	if (ap == NULL || socket_protocol == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	p = strdup(socket_protocol);
	if (p == NULL) {
		return -EZCFG_E_SPACE ;
	}

	if (ap->socket_protocol != NULL) {
		free(ap->socket_protocol);
	}
	ap->socket_protocol = p;

	return rc;
}

int ezcfg_api_arg_nvram_ssl_get_socket_address(struct ezcfg_arg_nvram_ssl *ap,char **pp)
{
	int rc = 0;

	if (ap == NULL || pp == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	*pp = ap->socket_address;

	return rc;
}

int ezcfg_api_arg_nvram_ssl_set_socket_address(struct ezcfg_arg_nvram_ssl *ap, const char *socket_address)
{
	int rc = 0;
	char *p;

	if (ap == NULL || socket_address == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	p = strdup(socket_address);
	if (p == NULL) {
		return -EZCFG_E_SPACE ;
	}

	if (ap->socket_address != NULL) {
		free(ap->socket_address);
	}
	ap->socket_address = p;

	return rc;
}

int ezcfg_api_arg_nvram_ssl_get_certificate_file(struct ezcfg_arg_nvram_ssl *ap,char **pp)
{
	int rc = 0;

	if (ap == NULL || pp == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	*pp = ap->certificate_file;

	return rc;
}

int ezcfg_api_arg_nvram_ssl_set_certificate_file(struct ezcfg_arg_nvram_ssl *ap, const char *file)
{
	int rc = 0;
	char *p;

	if (ap == NULL || file == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	p = strdup(file);
	if (p == NULL) {
		return -EZCFG_E_SPACE ;
	}

	if (ap->certificate_file != NULL) {
		free(ap->certificate_file);
	}
	ap->certificate_file = p;

	return rc;
}

int ezcfg_api_arg_nvram_ssl_get_certificate_chain_file(struct ezcfg_arg_nvram_ssl *ap,char **pp)
{
	int rc = 0;

	if (ap == NULL || pp == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	*pp = ap->certificate_chain_file;

	return rc;
}

int ezcfg_api_arg_nvram_ssl_set_certificate_chain_file(struct ezcfg_arg_nvram_ssl *ap, const char *file)
{
	int rc = 0;
	char *p;

	if (ap == NULL || file == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	p = strdup(file);
	if (p == NULL) {
		return -EZCFG_E_SPACE ;
	}

	if (ap->certificate_chain_file != NULL) {
		free(ap->certificate_chain_file);
	}
	ap->certificate_chain_file = p;

	return rc;
}

int ezcfg_api_arg_nvram_ssl_get_private_key_file(struct ezcfg_arg_nvram_ssl *ap,char **pp)
{
	int rc = 0;

	if (ap == NULL || pp == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	*pp = ap->private_key_file;

	return rc;
}

int ezcfg_api_arg_nvram_ssl_set_private_key_file(struct ezcfg_arg_nvram_ssl *ap, const char *file)
{
	int rc = 0;
	char *p;

	if (ap == NULL || file == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	p = strdup(file);
	if (p == NULL) {
		return -EZCFG_E_SPACE ;
	}

	if (ap->private_key_file != NULL) {
		free(ap->private_key_file);
	}
	ap->private_key_file = p;

	return rc;
}

