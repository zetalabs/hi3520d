/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_freeradius2_clients_conf.c
 *
 * Description  : ezbox /etc/freeradius2/clients.conf file generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-10-15   0.1       Write it from scratch
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

static int gen_client_entries(FILE *file, int num, char *prefix)
{
	int rc;
	int i;
	char name[64];
	char buf[256];

	/* client */
	for (i = 0; i < num; i++) {
		/* name */
		snprintf(name, sizeof(name), "%s.%d.%s", prefix, i,
			EZCFG_FREERADIUS2_OPT_KEYWORD_NAME);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s %s {\n", EZCFG_FREERADIUS2_OPT_KEYWORD_CLIENT, buf);
		}
		else {
			continue;
		}
		/* ipaddr */
		snprintf(name, sizeof(name), "%s.%d.%s", prefix, i,
			EZCFG_FREERADIUS2_OPT_KEYWORD_IPADDR);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_IPADDR, buf);
		}
		/* ipv6addr */
		snprintf(name, sizeof(name), "%s.%d.%s", prefix, i,
			EZCFG_FREERADIUS2_OPT_KEYWORD_IPV6ADDR);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_IPV6ADDR, buf);
		}
		/* netmask */
		snprintf(name, sizeof(name), "%s.%d.%s", prefix, i,
			EZCFG_FREERADIUS2_OPT_KEYWORD_NETMASK);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_NETMASK, buf);
		}
		/* secret */
		snprintf(name, sizeof(name), "%s.%d.%s", prefix, i,
			EZCFG_FREERADIUS2_OPT_KEYWORD_SECRET);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_SECRET, buf);
		}
		/* require_message_authenticator */
		snprintf(name, sizeof(name), "%s.%d.%s", prefix, i,
			EZCFG_FREERADIUS2_OPT_KEYWORD_REQUIRE_MESSAGE_AUTHENTICATOR);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			if ((strcmp(buf, "yes") == 0) || (strcmp(buf, "no") == 0)) {
				fprintf(file, "\t%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_REQUIRE_MESSAGE_AUTHENTICATOR, buf);
			}
		}
		/* shortname */
		snprintf(name, sizeof(name), "%s.%d.%s", prefix, i,
			EZCFG_FREERADIUS2_OPT_KEYWORD_SHORTNAME);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_SHORTNAME, buf);
		}
		/* nastype */
		snprintf(name, sizeof(name), "%s.%d.%s", prefix, i,
			EZCFG_FREERADIUS2_OPT_KEYWORD_NASTYPE);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_NASTYPE, buf);
		}
		/* virtual_server */
		snprintf(name, sizeof(name), "%s.%d.%s", prefix, i,
			EZCFG_FREERADIUS2_OPT_KEYWORD_VIRTUAL_SERVER);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_VIRTUAL_SERVER, buf);
		}
		/* coa_server */
		snprintf(name, sizeof(name), "%s.%d.%s", prefix, i,
			EZCFG_FREERADIUS2_OPT_KEYWORD_COA_SERVER);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_COA_SERVER, buf);
		}
		fprintf(file, "}\n");
	}

	return EXIT_SUCCESS;
}

int pop_etc_freeradius2_clients_conf(int flag)
{
        FILE *file = NULL;
	int rc;
	char buf[256];


	/* generate /etc/freeradius2/clients.conf */
	file = fopen("/etc/freeradius2/clients.conf", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	/* client */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, CLIENT_ENTRY_NUM), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			gen_client_entries(file, rc, NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, CLIENT_ENTRY));
		}
	}

	/* per_socket_clients */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, PER_SOCKET_CLIENTS_ENTRY_NUM), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "clients per_socket_clients {\n");
			gen_client_entries(file, rc, NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, PER_SOCKET_CLIENTS_ENTRY));
			fprintf(file, "}\n");
		}
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
