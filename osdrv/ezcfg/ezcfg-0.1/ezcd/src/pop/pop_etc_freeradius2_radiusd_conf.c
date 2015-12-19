/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_freeradius2_radiusd_conf.c
 *
 * Description  : ezbox /etc/freeradius2/radiusd.conf file generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-10-11   0.1       Write it from scratch
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

static int gen_radiusd_general_confs(FILE *file)
{
	int rc;
	int i;
	int listen_num;
	char name[64];
	char buf[256];

	/* general options */
	/* prefix = /usr */
	fprintf(file, "prefix = %s\n", "/usr");

	/* exec_prefix = /usr */
	fprintf(file, "exec_prefix = %s\n", "/usr");

	/* sysconfdir = /etc */
	fprintf(file, "sysconfdir = %s\n", "/etc");

	/* localstatedir = /var */
	fprintf(file, "localstatedir = %s\n", "/var");

	/* sbindir = /usr/sbin */
	fprintf(file, "sbindir = %s\n", "/usr/sbin");

	/* logdir = /var/log */
	fprintf(file, "logdir = %s\n", "/var/log");

	/* raddbdir = /etc/freeradius2 */
	fprintf(file, "raddbdir = %s\n", "/etc/freeradius2");

	/* radacctdir = /var/db/radacct */
	fprintf(file, "radacctdir = %s\n", "/var/db/radacct");

	/* name */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, NAME), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_NAME, buf);
	}

	/* confdir = ${raddbdir} */
	fprintf(file, "confdir = %s\n", "${raddbdir}");

	/* run_dir = ${localstatedir}/run */
	fprintf(file, "run_dir = %s\n", "${localstatedir}");

	/* db_dir = ${raddbdir} */
	fprintf(file, "db_dir = %s\n", "${raddbdir}");

	/* libdir = /usr/lib/freeradius2 */
	fprintf(file, "libdir = %s\n", "/usr/lib/freeradius2");

	/* pidfile = ${run_dir}/${name}.pid */
	fprintf(file, "pidfile = %s\n", "${run_dir}/${name}.pid");

	/* max_request_time = 30 */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, MAX_REQUEST_TIME), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s = %d\n", EZCFG_FREERADIUS2_OPT_KEYWORD_MAX_REQUEST_TIME, rc);
		}
	}

	/* cleanup_delay = 5 */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, CLEANUP_DELAY), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s = %d\n", EZCFG_FREERADIUS2_OPT_KEYWORD_CLEANUP_DELAY, rc);
		}
	}

	/* max_requests = 1024 */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, MAX_REQUESTS), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s = %d\n", EZCFG_FREERADIUS2_OPT_KEYWORD_MAX_REQUESTS, rc);
		}
	}

	/* listen {} */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, LISTEN_NUM), buf, sizeof(buf));
	if (rc > 0) {
		listen_num = atoi(buf);
		for (i = 0; i < listen_num; i++) {
			fprintf(file, "%s {\n", EZCFG_FREERADIUS2_OPT_KEYWORD_LISTEN);
			/* type */
			snprintf(name, sizeof(name), "%s.%d.%s",
				NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, LISTEN), i,
				EZCFG_FREERADIUS2_OPT_KEYWORD_TYPE);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "\t%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_TYPE, buf);
			}
			/* ipaddr */
			snprintf(name, sizeof(name), "%s.%d.%s",
				NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, LISTEN), i,
				EZCFG_FREERADIUS2_OPT_KEYWORD_IPADDR);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "\t%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_IPADDR, buf);
			}
			/* ipv6addr */
			snprintf(name, sizeof(name), "%s.%d.%s",
				NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, LISTEN), i,
				EZCFG_FREERADIUS2_OPT_KEYWORD_IPV6ADDR);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "\t%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_IPV6ADDR, buf);
			}
			/* port */
			snprintf(name, sizeof(name), "%s.%d.%s",
				NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, LISTEN), i,
				EZCFG_FREERADIUS2_OPT_KEYWORD_PORT);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "\t%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_PORT, buf);
			}
			/* interface */
			snprintf(name, sizeof(name), "%s.%d.%s",
				NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, LISTEN), i,
				EZCFG_FREERADIUS2_OPT_KEYWORD_INTERFACE);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "\t%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_INTERFACE, buf);
			}
			/* clients */
			snprintf(name, sizeof(name), "%s.%d.%s",
				NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, LISTEN), i,
				EZCFG_FREERADIUS2_OPT_KEYWORD_CLIENTS);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "\t%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_CLIENTS, buf);
			}
			fprintf(file, "}\n");
		}
	}

	/* hostname_lookups = no */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, HOSTNAME_LOOKUPS), buf, sizeof(buf));
	if (rc > 0) {
		if ((strcmp(buf, "yes") == 0) || (strcmp(buf, "no") == 0)) {
			fprintf(file, "%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_HOSTNAME_LOOKUPS, buf);
		}
	}

	/* allow_core_dumps = no */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, ALLOW_CORE_DUMPS), buf, sizeof(buf));
	if (rc > 0) {
		if ((strcmp(buf, "yes") == 0) || (strcmp(buf, "no") == 0)) {
			fprintf(file, "%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_ALLOW_CORE_DUMPS, buf);
		}
	}

	/* regular_expressions = yes */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, REGULAR_EXPRESSIONS), buf, sizeof(buf));
	if (rc > 0) {
		if ((strcmp(buf, "yes") == 0) || (strcmp(buf, "no") == 0)) {
			fprintf(file, "%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_REGULAR_EXPRESSIONS, buf);
		}
	}

	/* extended_expressions = yes */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, EXTENDED_EXPRESSIONS), buf, sizeof(buf));
	if (rc > 0) {
		if ((strcmp(buf, "yes") == 0) || (strcmp(buf, "no") == 0)) {
			fprintf(file, "%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_EXTENDED_EXPRESSIONS, buf);
		}
	}

	/* logging section */
	fprintf(file, "log {\n");
	/* destination = files */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, LOG_DESTINATION), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_DESTINATION, buf);
	}
	/* file = ${logdir}/radius.log */
	fprintf(file, "%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_FILE, "${logdir}/radius.log");
	/* syslog_facility = daemon */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, LOG_SYSLOG_FACILITY), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_SYSLOG_FACILITY, buf);
	}
	/* stripped_names = no */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, LOG_STRIPPED_NAMES), buf, sizeof(buf));
	if (rc > 0) {
		if ((strcmp(buf, "yes") == 0) || (strcmp(buf, "no") == 0)) {
			fprintf(file, "%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_STRIPPED_NAMES, buf);
		}
	}
	/* auth = no */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, LOG_AUTH2), buf, sizeof(buf));
	if (rc > 0) {
		if ((strcmp(buf, "yes") == 0) || (strcmp(buf, "no") == 0)) {
			fprintf(file, "%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_AUTH, buf);
		}
	}
	/* auth_badpass = no */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, LOG_AUTH_BADPASS), buf, sizeof(buf));
	if (rc > 0) {
		if ((strcmp(buf, "yes") == 0) || (strcmp(buf, "no") == 0)) {
			fprintf(file, "%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_AUTH_BADPASS, buf);
		}
	}
	/* auth_goodpass = no */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, LOG_AUTH_GOODPASS), buf, sizeof(buf));
	if (rc > 0) {
		if ((strcmp(buf, "yes") == 0) || (strcmp(buf, "no") == 0)) {
			fprintf(file, "%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_AUTH_GOODPASS, buf);
		}
	}
	fprintf(file, "}\n");

	/* checkrad = ${sbindir}/checkrad */
	fprintf(file, "%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_CHECKRAD, "${sbindir}/checkrad");

	/* SECURITY CONFIGURATION */
	fprintf(file, "security {\n");
	/* max_attributes = 200 */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, SECURITY_MAX_ATTRIBUTES), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s = %d\n", EZCFG_FREERADIUS2_OPT_KEYWORD_MAX_ATTRIBUTES, rc);
		}
	}
	/* reject_delay = 1 [range 1 to 5]*/
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, SECURITY_REJECT_DELAY), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if ((rc > 0) && (rc < 6)){
			fprintf(file, "%s = %d\n", EZCFG_FREERADIUS2_OPT_KEYWORD_REJECT_DELAY, rc);
		}
	}
	/* status_server = yes */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, SECURITY_REJECT_DELAY), buf, sizeof(buf));
	if (rc > 0) {
		if ((strcmp(buf, "yes") == 0) || (strcmp(buf, "no") == 0)) {
			fprintf(file, "%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_REJECT_DELAY, buf);
		}
	}
	fprintf(file, "}\n");

	/* PROXY CONFIGURATION */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, PROXY_REQUESTS), buf, sizeof(buf));
	if (rc > 0) {
		if (strcmp(buf, "no") == 0) {
			fprintf(file, "%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_PROXY_REQUESTS, buf);
		}
		else if (strcmp(buf, "yes") == 0) {
			fprintf(file, "%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_PROXY_REQUESTS, buf);
			fprintf(file, "%s\n", "$INCLUDE proxy.conf");
		}
	}

	/* CLIENTS CONFIGURATION */
	fprintf(file, "%s\n", "$INCLUDE clients.conf");

	/* THREAD POOL CONFIGURATION */
	fprintf(file, "thread pool {\n");
	/* start_servers = 5 */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, START_SERVERS), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s = %d\n", EZCFG_FREERADIUS2_OPT_KEYWORD_START_SERVERS, rc);
		}
	}
	/* max_servers = 32 */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, MAX_SERVERS), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s = %d\n", EZCFG_FREERADIUS2_OPT_KEYWORD_MAX_SERVERS, rc);
		}
	}
	/* min_spare_servers = 3 */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, MIN_SPARE_SERVERS), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s = %d\n", EZCFG_FREERADIUS2_OPT_KEYWORD_MIN_SPARE_SERVERS, rc);
		}
	}
	/* max_spare_servers = 10 */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, MAX_SPARE_SERVERS), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s = %d\n", EZCFG_FREERADIUS2_OPT_KEYWORD_MAX_SPARE_SERVERS, rc);
		}
	}
	/* max_queue_size = 65536 */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, MAX_QUEUE_SIZE), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s = %d\n", EZCFG_FREERADIUS2_OPT_KEYWORD_MAX_QUEUE_SIZE, rc);
		}
	}
	/* max_requests_per_server = 0 */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, MAX_REQUESTS_PER_SERVER), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc >= 0) {
			fprintf(file, "%s = %d\n", EZCFG_FREERADIUS2_OPT_KEYWORD_MAX_REQUESTS_PER_SERVER, rc);
		}
	}
	fprintf(file, "}\n");

	/* MODULE CONFIGURATION */
	fprintf(file, "modules {\n");
	fprintf(file, "%s\n", "$INCLUDE ${confdir}/modules/");
	fprintf(file, "%s\n", "$INCLUDE eap.conf");
	fprintf(file, "}\n");

	/* Instantiation */
	fprintf(file, "instantiate {\n");
	fprintf(file, "}\n");

	/* Load virtual servers. */
	fprintf(file, "%s\n", "$INCLUDE sites/");

	return EXIT_SUCCESS;
}

int pop_etc_freeradius2_radiusd_conf(int flag)
{
        FILE *file = NULL;

	/* first change umask for process */
	umask(S_IWGRP | S_IWOTH);

	/* generate /etc/freeradius2/radiusd.conf */
	file = fopen("/etc/freeradius2/radiusd.conf", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	/* general configs */
	gen_radiusd_general_confs(file);

	fclose(file);
	return (EXIT_SUCCESS);
}
