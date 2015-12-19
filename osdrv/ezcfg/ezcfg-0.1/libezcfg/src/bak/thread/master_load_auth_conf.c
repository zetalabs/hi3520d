/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/master_load_auth_conf.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-10-10   0.1       Split it from master.c
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

void ezcfg_master_load_auth_conf(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;
	char *p = NULL;
	int i;
	int auth_number = -1;
	struct ezcfg_auth *ap = NULL;
	struct ezcfg_auth **pap = NULL;

	if (master == NULL)
		return ;

	ezcfg = ezcfg_master_get_ezcfg(master);

	/* first get the auth number */
	p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_AUTH_NUMBER);
	if (p != NULL) {
		auth_number = atoi(p);
		free(p);
	}

	for (i = 0; i < auth_number; i++) {
		/* initialize */
		ap = ezcfg_auth_new(ezcfg);

		if (ap == NULL) {
			continue;
		}
		/* authentication type */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_AUTH, i, EZCFG_EZCFG_KEYWORD_TYPE);
		if (p == NULL) {
			ezcfg_auth_delete(ap);
			continue;
		}

		/* check auth type */
		if (strcmp(p, EZCFG_AUTH_TYPE_HTTP_BASIC_STRING) == 0) {
			if (ezcfg_auth_set_type(ap, p) == false) {
				ezcfg_auth_delete(ap);
				free(p);
				continue;
			}
		}
		else if (strcmp(p, EZCFG_AUTH_TYPE_HTTP_DIGEST_STRING) == 0) {
			if (ezcfg_auth_set_type(ap, p) == false) {
				ezcfg_auth_delete(ap);
				free(p);
				continue;
			}
		}
		else {
			/* unknown auth type */
			ezcfg_auth_delete(ap);
			free(p);
			continue;
		}
		free(p);

		/* authentication user */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_AUTH, i, EZCFG_EZCFG_KEYWORD_USER);
		if (p == NULL) {
			ezcfg_auth_delete(ap);
			continue;
		}

		/* check auth user */
		if (ezcfg_auth_set_user(ap, p) == false) {
			ezcfg_auth_delete(ap);
			free(p);
			continue;
		}
		free(p);

		/* authentication realm */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_AUTH, i, EZCFG_EZCFG_KEYWORD_REALM);
		if (p == NULL) {
			ezcfg_auth_delete(ap);
			continue;
		}

		/* check auth realm */
		if (ezcfg_auth_set_realm(ap, p) == false) {
			ezcfg_auth_delete(ap);
			free(p);
			continue;
		}
		free(p);

		/* authentication domain */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_AUTH, i, EZCFG_EZCFG_KEYWORD_DOMAIN);
		if (p == NULL) {
			ezcfg_auth_delete(ap);
			continue;
		}

		/* check auth domain */
		if (ezcfg_auth_set_domain(ap, p) == false) {
			ezcfg_auth_delete(ap);
			free(p);
			continue;
		}
		free(p);

		/* authentication secret */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_AUTH, i, EZCFG_EZCFG_KEYWORD_SECRET);
		if (p == NULL) {
			ezcfg_auth_delete(ap);
			continue;
		}

		/* check auth secret */
		if (ezcfg_auth_set_secret(ap, p) == false) {
			ezcfg_auth_delete(ap);
			free(p);
			continue;
		}
		free(p);

		/* check if auth is valid */
		if (ezcfg_auth_is_valid(ap) == false) {
			ezcfg_auth_delete(ap);
			continue;
		}

		/* check if auth is already set */
		pap = ezcfg_master_get_p_auths(master);
		if (ezcfg_auth_list_in(pap, ap) == true) {
			info(ezcfg, "auth entry already set\n");
			ezcfg_auth_delete(ap);
			continue;
		}

		/* add new authentication */
		pap = ezcfg_master_get_p_auths(master);
		if (ezcfg_auth_list_insert(pap, ap) == true) {
			info(ezcfg, "insert auth entry successfully\n");
			/* set ap to NULL to avoid delete it */
			ap = NULL;
		}
		else {
			err(ezcfg, "insert auth entry failed: %m\n");
			ezcfg_auth_delete(ap);
		}
	}
}
