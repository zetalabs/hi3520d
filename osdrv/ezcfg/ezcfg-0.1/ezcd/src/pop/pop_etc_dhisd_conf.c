/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_dhisd_conf.c
 *
 * Description  : ezbox /etc/dhisd.conf file generating program
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

int pop_etc_dhisd_conf(int flag)
{
        FILE *file = NULL;
	char buf[128];
	int rc;

	/* generate /etc/dhisd.conf */
	file = fopen("/etc/dhisd.conf", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	switch (flag) {
	case RC_ACT_START :
		/* LogFile */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(DHISD, LOG_FILE), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s %s\n", EZCFG_DHISD_KEYWORD_LOG_FILE, buf);
		}

		/* PidFile */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(DHISD, PID_FILE), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s %s\n", EZCFG_DHISD_KEYWORD_PID_FILE, buf);
		}

		/* BindAddress */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(DHISD, BIND_ADDRESS), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s %s\n", EZCFG_DHISD_KEYWORD_BIND_ADDRESS, buf);
		}

		/* BindPort */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(DHISD, BIND_PORT), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s %s\n", EZCFG_DHISD_KEYWORD_BIND_PORT, buf);
		}

		/* DBFile */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(DHISD, DB_FILE), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s %s\n", EZCFG_DHISD_KEYWORD_DB_FILE, buf);
		}

		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DHISD, DB_FILE), "mysql") == 0) {
			/* MySQLServer */
			rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(DHISD, MYSQL_SERVER), buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "%s %s\n", EZCFG_DHISD_KEYWORD_MYSQL_SERVER, buf);
			}

			/* MySQLUser */
			rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(DHISD, MYSQL_USER), buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "%s %s\n", EZCFG_DHISD_KEYWORD_MYSQL_USER, buf);
			}

			/* MySQLPassword */
			rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(DHISD, MYSQL_PASSWORD), buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "%s %s\n", EZCFG_DHISD_KEYWORD_MYSQL_PASSWORD, buf);
			}

			/* MySQLDBase */
			rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(DHISD, MYSQL_DBASE), buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "%s %s\n", EZCFG_DHISD_KEYWORD_MYSQL_DBASE, buf);
			}
		}

		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
