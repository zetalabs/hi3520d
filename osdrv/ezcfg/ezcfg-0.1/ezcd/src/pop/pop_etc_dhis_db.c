/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_dhis_db.c
 *
 * Description  : ezbox /etc/dhis.db file generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-10-03   0.1       Write it from scratch
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
#include <net/if.h>

#include "ezcd.h"
#include "pop_func.h"

int pop_etc_dhis_db(int flag)
{
        FILE *file = NULL;
	char name[32];
	char buf[128];
	int entry_num = 0;
	int i, j;
	int rc;
#if 0
	char *HostID;
	char *HostPass;
	char *AuthN0, *AuthN1;
	char *AuthN2, *AuthN3;
	char *OnCmd;
	char *OffCmd;
#endif

	/* generate /etc/dhis.db */
	file = fopen("/etc/dhis.db", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	switch (flag) {
	case RC_ACT_START :
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DHISD, DB_ENTRY_NUM));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			entry_num = atoi(buf);
		}

		for (i = 0; i < entry_num; i++) {
			snprintf(name, sizeof(name), "%s.%d.%s",
				NVRAM_SERVICE_OPTION(DHISD, DB_ENTRY), i,
				EZCFG_DHISD_KEYWORD_HOST_ID);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc <= 0)
				continue;

			fprintf(file, "%s {\n", buf);

			/* HostName */
			snprintf(name, sizeof(name), "%s.%d.%s",
				NVRAM_SERVICE_OPTION(DHISD, DB_ENTRY), i,
				EZCFG_DHISD_KEYWORD_HOST_NAME);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "%s\t%s\n", EZCFG_DHISD_KEYWORD_HOST_NAME, buf);
			}

			/* authentication method */
			snprintf(name, sizeof(name), "%s.%d.%s", NVRAM_SERVICE_OPTION(DHISD, DB_ENTRY), i,
				EZCFG_DHISD_QRC_ENABLE);
			if (utils_nvram_cmp(name, "1") == 0) {
				/* AuthN */
				for (j = 0; j < 4; j++) {
					snprintf(name, sizeof(name), "%s.%d.%s%d",
						NVRAM_SERVICE_OPTION(DHISD, DB_ENTRY), i,
						EZCFG_DHISD_KEYWORD_AUTH_N, j);
					rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
					if (rc > 0) {
						fprintf(file, "%s\t%s\n", EZCFG_DHISD_KEYWORD_AUTH_N, buf);
					}
				}
			}
			else {
				/* HostPass */
				snprintf(name, sizeof(name), "%s.%d.%s", NVRAM_SERVICE_OPTION(DHISD, DB_ENTRY), i,
					EZCFG_DHISD_KEYWORD_HOST_PASS);
				rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
				if (rc > 0) {
					fprintf(file, "%s\t%s\n", EZCFG_DHISD_KEYWORD_HOST_PASS, buf);
				}
			}

			/* OnCmd */
			snprintf(name, sizeof(name), "%s.%d.%s",
				NVRAM_SERVICE_OPTION(DHISD, DB_ENTRY), i,
				EZCFG_DHISD_KEYWORD_ON_CMD);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "%s\t%s\n", EZCFG_DHISD_KEYWORD_ON_CMD, buf);
			}

			/* OffCmd */
			snprintf(name, sizeof(name), "%s.%d.%s",
				NVRAM_SERVICE_OPTION(DHISD, DB_ENTRY), i,
				EZCFG_DHISD_KEYWORD_OFF_CMD);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "%s\t%s\n", EZCFG_DHISD_KEYWORD_OFF_CMD, buf);
			}

			fprintf(file, "%s\n", "}");
		}
		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
