/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_nvram_conf.c
 *
 * Description  : ezbox /etc/nvram.conf generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-24   0.1       Write it from scratch
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

#include "ezcd.h"
#include "pop_func.h"

int pop_etc_nvram_conf(int flag)
{
	FILE *file;
	char name[64];
	char buf[256];
	int rc;
	int i;
	int socket_number = 0;
	int auth_number = 0;

	switch (flag) {
	case RC_ACT_BOOT :
	case RC_ACT_START :
	case RC_ACT_RELOAD :
	case RC_ACT_RESTART :
		/* get nvram config from nvram */
		file = fopen(NVRAM_CONFIG_FILE_PATH, "w");
		if (file == NULL)
			return (EXIT_FAILURE);

		/* setup ezcfg common info */
		fprintf(file, "[%s]\n", EZCFG_EZCFG_SECTION_COMMON);

		/* log_level */
		snprintf(name, sizeof(name), "%s%s.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_COMMON,
		         EZCFG_EZCFG_KEYWORD_LOG_LEVEL);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_LOG_LEVEL, buf);
		}

		/* rules_path */
		snprintf(name, sizeof(name), "%s%s.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_COMMON,
		         EZCFG_EZCFG_KEYWORD_RULES_PATH);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_RULES_PATH, buf);
		}

		/* socket_number */
		snprintf(name, sizeof(name), "%s%s.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_COMMON,
		         EZCFG_EZCFG_KEYWORD_SOCKET_NUMBER);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SOCKET_NUMBER, buf);
			socket_number = atoi(buf);
		}

		/* locale */
		snprintf(name, sizeof(name), "%s%s.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_COMMON,
		         EZCFG_EZCFG_KEYWORD_LOCALE);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_LOCALE, buf);
		}

		/* auth_number */
		snprintf(name, sizeof(name), "%s%s.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_COMMON,
		         EZCFG_EZCFG_KEYWORD_AUTH_NUMBER);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_AUTH_NUMBER, buf);
			auth_number = atoi(buf);
		}

		fprintf(file, "\n");

		/* setup nvram storage info */
		for(i = 0; i < EZCFG_NVRAM_STORAGE_NUM; i++) {

			fprintf(file, "[%s]\n", EZCFG_EZCFG_SECTION_NVRAM);

			snprintf(name, sizeof(name), "%s%s.%d.%s",
			         EZCFG_EZCFG_NVRAM_PREFIX,
			         EZCFG_EZCFG_SECTION_NVRAM,
			         i, EZCFG_EZCFG_KEYWORD_BUFFER_SIZE);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc >= 0) {
				fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_BUFFER_SIZE, buf);
			}

			snprintf(name, sizeof(name), "%s%s.%d.%s",
			         EZCFG_EZCFG_NVRAM_PREFIX,
			         EZCFG_EZCFG_SECTION_NVRAM,
			         i, EZCFG_EZCFG_KEYWORD_BACKEND_TYPE);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc >= 0) {
				fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_BACKEND_TYPE, buf);
			}

			snprintf(name, sizeof(name), "%s%s.%d.%s",
			         EZCFG_EZCFG_NVRAM_PREFIX,
			         EZCFG_EZCFG_SECTION_NVRAM,
			         i, EZCFG_EZCFG_KEYWORD_CODING_TYPE);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc >= 0) {
				fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_CODING_TYPE, buf);
			}

			snprintf(name, sizeof(name), "%s%s.%d.%s",
			         EZCFG_EZCFG_NVRAM_PREFIX,
			         EZCFG_EZCFG_SECTION_NVRAM,
			         i, EZCFG_EZCFG_KEYWORD_STORAGE_PATH);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc >= 0) {
				fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_STORAGE_PATH, buf);
			}
			fprintf(file, "\n");
		}

		/* setup socket info */
		for(i = 0; i < socket_number; i++) {

			fprintf(file, "[%s]\n", EZCFG_EZCFG_SECTION_SOCKET);

			snprintf(name, sizeof(name), "%s%s.%d.%s",
			         EZCFG_EZCFG_NVRAM_PREFIX,
			         EZCFG_EZCFG_SECTION_SOCKET,
			         i, EZCFG_EZCFG_KEYWORD_DOMAIN);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc >= 0) {
				fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_DOMAIN, buf);
			}

			snprintf(name, sizeof(name), "%s%s.%d.%s",
			         EZCFG_EZCFG_NVRAM_PREFIX,
			         EZCFG_EZCFG_SECTION_SOCKET,
			         i, EZCFG_EZCFG_KEYWORD_TYPE);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc >= 0) {
				fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_TYPE, buf);
			}

			snprintf(name, sizeof(name), "%s%s.%d.%s",
			         EZCFG_EZCFG_NVRAM_PREFIX,
			         EZCFG_EZCFG_SECTION_SOCKET,
			         i, EZCFG_EZCFG_KEYWORD_PROTOCOL);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc >= 0) {
				fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_PROTOCOL, buf);
			}

			snprintf(name, sizeof(name), "%s%s.%d.%s",
			         EZCFG_EZCFG_NVRAM_PREFIX,
			         EZCFG_EZCFG_SECTION_SOCKET,
			         i, EZCFG_EZCFG_KEYWORD_ADDRESS);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc >= 0) {
				fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_ADDRESS, buf);
			}
			fprintf(file, "\n");
		}

		/* setup auth info */
		for(i = 0; i < auth_number; i++) {

			fprintf(file, "[%s]\n", EZCFG_EZCFG_SECTION_AUTH);

			snprintf(name, sizeof(name), "%s%s.%d.%s",
			         EZCFG_EZCFG_NVRAM_PREFIX,
			         EZCFG_EZCFG_SECTION_AUTH,
			         i, EZCFG_EZCFG_KEYWORD_TYPE);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc >= 0) {
				fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_TYPE, buf);
			}

			snprintf(name, sizeof(name), "%s%s.%d.%s",
			         EZCFG_EZCFG_NVRAM_PREFIX,
			         EZCFG_EZCFG_SECTION_AUTH,
			         i, EZCFG_EZCFG_KEYWORD_USER);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc >= 0) {
				fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_USER, buf);
			}

			snprintf(name, sizeof(name), "%s%s.%d.%s",
			         EZCFG_EZCFG_NVRAM_PREFIX,
			         EZCFG_EZCFG_SECTION_AUTH,
			         i, EZCFG_EZCFG_KEYWORD_REALM);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc >= 0) {
				fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_REALM, buf);
			}

			snprintf(name, sizeof(name), "%s%s.%d.%s",
			         EZCFG_EZCFG_NVRAM_PREFIX,
			         EZCFG_EZCFG_SECTION_AUTH,
			         i, EZCFG_EZCFG_KEYWORD_DOMAIN);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc >= 0) {
				fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_DOMAIN, buf);
			}

			snprintf(name, sizeof(name), "%s%s.%d.%s",
			         EZCFG_EZCFG_NVRAM_PREFIX,
			         EZCFG_EZCFG_SECTION_AUTH,
			         i, EZCFG_EZCFG_KEYWORD_SECRET);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc >= 0) {
				fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SECRET, buf);
			}
			fprintf(file, "\n");
		}

		fclose(file);
		break;
	}

	return (EXIT_SUCCESS);
}
