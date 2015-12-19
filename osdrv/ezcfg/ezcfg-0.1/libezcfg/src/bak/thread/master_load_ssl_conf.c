/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/master_load_ssl_conf.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-02-04   0.1       Write it from scratch
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
#include "ezcfg-ssl.h"

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

void ezcfg_master_load_ssl_conf(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;
	char *p = NULL;
	int i;
	int ssl_number = -1;
	struct ezcfg_ssl *sslp = NULL;
	struct ezcfg_ssl **psp = NULL;
	int role;
	int method;
	bool socket_enable;
	struct ezcfg_socket *sp;

	if (master == NULL)
		return ;

	ezcfg = ezcfg_master_get_ezcfg(master);

	/* first get the SSL number */
	p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_SSL_NUMBER);
	if (p != NULL) {
		ssl_number = atoi(p);
		free(p);
	}

	for (i = 0; i < ssl_number; i++) {
		/* check SSL role */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_SSL, i, EZCFG_EZCFG_KEYWORD_ROLE);
		if (p == NULL) {
			continue;
		}

		if (strcmp(p, EZCFG_SSL_ROLE_SERVER_STRING) == 0) {
			role = EZCFG_SSL_ROLE_SERVER;
		}
		else if (strcmp(p, EZCFG_SSL_ROLE_CLIENT_STRING) == 0) {
			role = EZCFG_SSL_ROLE_CLIENT;
		}
		else {
			/* unknown SSL role */
			role = EZCFG_SSL_ROLE_UNKNOWN;
		}
		free(p);

		/* check SSL method */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_SSL, i, EZCFG_EZCFG_KEYWORD_METHOD);
		if (p == NULL) {
			continue;
		}

		if (strcmp(p, EZCFG_SSL_METHOD_SSLV2_STRING) == 0) {
			method = EZCFG_SSL_METHOD_SSLV2;
		}
		else if (strcmp(p, EZCFG_SSL_METHOD_SSLV3_STRING) == 0) {
			method = EZCFG_SSL_METHOD_SSLV3;
		}
		else if (strcmp(p, EZCFG_SSL_METHOD_TLSV1_STRING) == 0) {
			method = EZCFG_SSL_METHOD_TLSV1;
		}
		else if (strcmp(p, EZCFG_SSL_METHOD_SSLV23_STRING) == 0) {
			method = EZCFG_SSL_METHOD_SSLV23;
		}
		else {
			/* unknown SSL method */
			free(p);
			continue;
		}
		free(p);

		/* initialize */
		sslp = ezcfg_ssl_new(ezcfg, role, method);

		if (sslp == NULL) {
			continue;
		}

		/* check socket enable */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_SSL, i, EZCFG_EZCFG_KEYWORD_SOCKET_ENABLE);
		if (p == NULL) {
			ezcfg_ssl_delete(sslp);
			continue;
		}

		if (strcmp(p, "1") == 0) {
			socket_enable = true;
		}
		else {
			socket_enable = false;
		}
		free(p);

		/* setup SSL related socket */
		if (socket_enable == true) {
			int socket_domain;
			int socket_type;
			int socket_proto;
			char socket_address[256];

			/* initialize */
			socket_domain = -1;
			socket_type = -1;
			socket_proto = EZCFG_PROTO_UNKNOWN;
			socket_address[0] = '\0';
			sp = NULL;

			/* socket domain */
			p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg),
				EZCFG_EZCFG_SECTION_SSL, i,
				EZCFG_EZCFG_KEYWORD_SOCKET_DOMAIN);
			if (p != NULL) {
				socket_domain = ezcfg_util_socket_domain_get_index(p);
				free(p);
			}

			/* socket type */
			p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg),
				EZCFG_EZCFG_SECTION_SSL, i,
				EZCFG_EZCFG_KEYWORD_SOCKET_TYPE);
			if (p != NULL) {
				socket_type = ezcfg_util_socket_type_get_index(p);
				free(p);
			}

			/* socket protocol */
			p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg),
				EZCFG_EZCFG_SECTION_SSL, i,
				EZCFG_EZCFG_KEYWORD_SOCKET_PROTOCOL);
			if (p != NULL) {
				socket_proto = ezcfg_util_socket_protocol_get_index(p);
				free(p);
			}

			/* socket address */
			p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg),
				EZCFG_EZCFG_SECTION_SSL, i,
				EZCFG_EZCFG_KEYWORD_SOCKET_ADDRESS);
			if (p != NULL) {
				snprintf(socket_address, sizeof(socket_address), "%s", p);
				free(p);
				p = NULL;
			}
			if ((socket_domain < 0) ||
			    (socket_type < 0) ||
			    (socket_proto == EZCFG_PROTO_UNKNOWN) ||
			    (socket_address[0] == '\0')) {
				err(ezcfg, "socket setting error\n");
				ezcfg_ssl_delete(sslp);
				continue;
			}

			sp = ezcfg_socket_fake_new(ezcfg,
				socket_domain,
				socket_type,
				socket_proto,
				socket_address);
			if (sp == NULL) {
				err(ezcfg, "init fake socket fail: %m\n");
				ezcfg_ssl_delete(sslp);
				continue;
			}
			ezcfg_ssl_set_socket(sslp, sp);
			sp = NULL;
		}

		/* check certificate file */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_SSL, i, EZCFG_EZCFG_KEYWORD_CERTIFICATE_FILE);
		if (p == NULL) {
			ezcfg_ssl_delete(sslp);
			continue;
		}
		if (*p != '\0') {
			ezcfg_ssl_set_certificate_file(sslp, p);
		}
		free(p);

		/* check certificate chain file */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_SSL, i, EZCFG_EZCFG_KEYWORD_CERTIFICATE_CHAIN_FILE);
		if (p == NULL) {
			ezcfg_ssl_delete(sslp);
			continue;
		}
		if (*p != '\0') {
			ezcfg_ssl_set_certificate_chain_file(sslp, p);
		}
		free(p);

		/* check private key file */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_SSL, i, EZCFG_EZCFG_KEYWORD_PRIVATE_KEY_FILE);
		if (p == NULL) {
			ezcfg_ssl_delete(sslp);
			continue;
		}
		if (*p != '\0') {
			ezcfg_ssl_set_private_key_file(sslp, p);
		}
		free(p);

		/* check if SSL is valid */
		if (ezcfg_ssl_is_valid(sslp) == false) {
			ezcfg_ssl_delete(sslp);
			continue;
		}

		/* check if SSL is already set */
		psp = ezcfg_master_get_p_ssl(master);
		if (ezcfg_ssl_list_in(psp, sslp) == true) {
			info(ezcfg, "ssl entry already set\n");
			ezcfg_ssl_delete(sslp);
			continue;
		}

		/* add new SSL */
		psp = ezcfg_master_get_p_ssl(master);
		if (ezcfg_ssl_list_insert(psp, sslp) == true) {
			info(ezcfg, "insert ssl entry successfully\n");
			/* set sslp to NULL to avoid delete it */
			sslp = NULL;
		}
		else {
			err(ezcfg, "insert ssl entry failed: %m\n");
			ezcfg_ssl_delete(sslp);
		}
	}
}
