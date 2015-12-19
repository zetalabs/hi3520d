/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_dhid_conf.c
 *
 * Description  : ezbox /etc/dhid.conf file generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-06-12   0.1       Write it from scratch
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

int pop_etc_dhid_conf(int flag)
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
	char *AuthP0, *AuthP1;
	char *AuthQ0, *AuthQ1;
	char *ISAddr;
	char *Refresh;
	char *OnCmd;
	char *OffCmd;
#endif

	/* generate /etc/dhid.conf */
	file = fopen("/etc/dhid.conf", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	switch (flag) {
	case RC_ACT_START :
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DHID, ENTRY_NUM));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			entry_num = atoi(buf);
		}

		for (i = 0; i < entry_num; i++) {
			fprintf(file, "%s\n", "{");

			/* HostID */
			snprintf(name, sizeof(name), "%s.%d.%s", NVRAM_SERVICE_OPTION(DHID, ENTRY), i,
				EZCFG_DHID_KEYWORD_HOST_ID);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "%s\t%s\n", EZCFG_DHID_KEYWORD_HOST_ID, buf);

			}

			/* authentication method */
			snprintf(name, sizeof(name), "%s.%d.%s", NVRAM_SERVICE_OPTION(DHID, ENTRY), i,
				EZCFG_DHID_QRC_ENABLE);
			if (utils_nvram_cmp(name, "1") == 0) {
				/* AuthP */
				for (j = 0; j < 2; j++) {
					snprintf(name, sizeof(name), "%s.%d.%s%d",
						NVRAM_SERVICE_OPTION(DHID, ENTRY), i,
						EZCFG_DHID_KEYWORD_AUTH_P, j);
					rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
					if (rc > 0) {
						fprintf(file, "%s\t%s\n", EZCFG_DHID_KEYWORD_AUTH_P, buf);
					}
				}
				/* AuthQ */
				for (j = 0; j < 2; j++) {
					snprintf(name, sizeof(name), "%s.%d.%s%d",
						NVRAM_SERVICE_OPTION(DHID, ENTRY), i,
						EZCFG_DHID_KEYWORD_AUTH_Q, j);
					rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
					if (rc > 0) {
						fprintf(file, "%s\t%s\n", EZCFG_DHID_KEYWORD_AUTH_Q, buf);
					}
				}
			}
			else {
				/* HostPass */
				snprintf(name, sizeof(name), "%s.%d.%s", NVRAM_SERVICE_OPTION(DHID, ENTRY), i,
					EZCFG_DHID_KEYWORD_HOST_PASS);
				rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
				if (rc > 0) {
					fprintf(file, "%s\t%s\n", EZCFG_DHID_KEYWORD_HOST_PASS, buf);
				}
			}

			/* ISAddr */
			for (j = 0; j < 2; j++) {
				snprintf(name, sizeof(name), "%s.%d.%s%d",
					NVRAM_SERVICE_OPTION(DHID, ENTRY), i,
					EZCFG_DHID_KEYWORD_IS_ADDR, j);
				rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
				if (rc > 0) {
					fprintf(file, "%s\t%s\n", EZCFG_DHID_KEYWORD_IS_ADDR, buf);
				}
			}

			/* OnCmd */
			snprintf(name, sizeof(name), "%s.%d.%s",
				NVRAM_SERVICE_OPTION(DHID, ENTRY), i,
				EZCFG_DHID_KEYWORD_ON_CMD);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "%s\t%s\n", EZCFG_DHID_KEYWORD_ON_CMD, buf);
			}

			/* OffCmd */
			snprintf(name, sizeof(name), "%s.%d.%s",
				NVRAM_SERVICE_OPTION(DHID, ENTRY), i,
				EZCFG_DHID_KEYWORD_OFF_CMD);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "%s\t%s\n", EZCFG_DHID_KEYWORD_OFF_CMD, buf);
			}

			fprintf(file, "%s\n", "}");
		}
		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
