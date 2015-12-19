/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_freeradius2_eap_conf.c
 *
 * Description  : ezbox /etc/freeradius2/eap.conf file generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-10-28   0.1       Write it from scratch
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


#define fprintf_indent(IND_FP, INDENT, IND_STR, IND_FMT, IND_ARG...) do { \
	int indent_count; \
	for (indent_count = 0; indent_count < INDENT; indent_count++) \
		{ fprintf(IND_FP, "%s", IND_STR); } \
	fprintf(IND_FP, IND_FMT, ## IND_ARG); \
} while(0)


static int gen_tls_conf(FILE *file, char *prefix, int indent)
{
	return EXIT_SUCCESS;
}

static int gen_eap_conf(FILE *file, char *prefix, int indent)
{
	int rc;
	char name[64];
	char buf[256];

	/* eap */

	/* default_eap_type */
	snprintf(name, sizeof(name), "%s.%s", prefix,
		EZCFG_FREERADIUS2_OPT_KEYWORD_DEFAULT_EAP_TYPE);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf_indent(file, indent, "\t", "%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_DEFAULT_EAP_TYPE, buf);
	}

	/* timer_expire */
	snprintf(name, sizeof(name), "%s.%s", prefix,
		EZCFG_FREERADIUS2_OPT_KEYWORD_TIMER_EXPIRE);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc >= 0) {
			fprintf_indent(file, indent, "\t", "%s = %d\n", EZCFG_FREERADIUS2_OPT_KEYWORD_TIMER_EXPIRE, rc);
		}
	}

	/* ignore_unknown_eap_types */
	snprintf(name, sizeof(name), "%s.%s", prefix,
		EZCFG_FREERADIUS2_OPT_KEYWORD_IGNORE_UNKNOWN_EAP_TYPES);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		if ((strcmp(buf, "yes") == 0) || (strcmp(buf, "no") == 0)) {
			fprintf_indent(file, indent, "\t", "%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_IGNORE_UNKNOWN_EAP_TYPES, buf);
		}
	}

	/* cisco_accounting_username_bug */
	snprintf(name, sizeof(name), "%s.%s", prefix,
		EZCFG_FREERADIUS2_OPT_KEYWORD_CISCO_ACCOUNTING_USERNAME_BUG);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		if ((strcmp(buf, "yes") == 0) || (strcmp(buf, "no") == 0)) {
			fprintf_indent(file, indent, "\t", "%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_CISCO_ACCOUNTING_USERNAME_BUG, buf);
		}
	}

	/* max_sessions */
	snprintf(name, sizeof(name), "%s.%s", prefix,
		EZCFG_FREERADIUS2_OPT_KEYWORD_MAX_SESSIONS);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc >= 0) {
			fprintf_indent(file, indent, "\t", "%s = %d\n", EZCFG_FREERADIUS2_OPT_KEYWORD_MAX_SESSIONS, rc);
		}
	}

	/* tls {} */
	snprintf(name, sizeof(name), "%s.%s.%s", prefix,
		EZCFG_FREERADIUS2_OPT_KEYWORD_TLS, EZCFG_FREERADIUS2_OPT_KEYWORD_ENABLE);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc == 1) {
			fprintf_indent(file, indent, "\t", "%s {\n", EZCFG_FREERADIUS2_OPT_KEYWORD_TLS);

			snprintf(name, sizeof(name), "%s.%s", prefix, EZCFG_FREERADIUS2_OPT_KEYWORD_TLS);
			gen_tls_conf(file, name, indent+1);

			fprintf_indent(file, indent, "\t", "}\n");
		}
	}

	return EXIT_SUCCESS;
}

int pop_etc_freeradius2_eap_conf(int flag)
{
        FILE *file = NULL;

	/* generate /etc/freeradius2/eap.conf */
	file = fopen("/etc/freeradius2/eap.conf", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	fprintf(file, "%s {\n", EZCFG_FREERADIUS2_OPT_KEYWORD_EAP);

	gen_eap_conf(file, NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, EAP), 1);

	fprintf(file, "}\n");

	fclose(file);
	return (EXIT_SUCCESS);
}
