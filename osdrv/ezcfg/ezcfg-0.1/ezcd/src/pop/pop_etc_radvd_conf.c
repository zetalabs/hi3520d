/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_radvd_conf.c
 *
 * Description  : ezbox /etc/radvd.conf file generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-08-21   0.1       Write it from scratch
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

static int gen_interface_specific_options(FILE *file, int ifn)
{
	char name[64];
	char buf[256];
	int rc;

	/* interface IgnoreIfMissing option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_IGNORE_IF_MISSING);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_IGNORE_IF_MISSING, buf);
	}

	/* interface AdvSendAdvert option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_ADV_SEND_ADVERT);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_SEND_ADVERT, buf);
	}

	/* interface UnicastOnly option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_UNICAST_ONLY);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_UNICAST_ONLY, buf);
	}

	/* interface MaxRtrAdvInterval option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_MAX_RTR_ADV_INTERVAL);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_MAX_RTR_ADV_INTERVAL, buf);
	}

	/* interface MinRtrAdvInterval option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_MIN_RTR_ADV_INTERVAL);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_MIN_RTR_ADV_INTERVAL, buf);
	}

	/* interface MinDelayBetweenRAs option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_MIN_DELAY_BETWEEN_RAS);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_MIN_DELAY_BETWEEN_RAS, buf);
	}

	/* interface AdvManagedFlag option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_ADV_MANAGED_FLAG);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_MANAGED_FLAG, buf);
	}

	/* interface AdvOtherConfigFlag option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_ADV_OTHER_CONFIG_FLAG);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_OTHER_CONFIG_FLAG, buf);
	}

	/* interface AdvLinkMTU option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_ADV_LINK_MTU);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_LINK_MTU, buf);
	}

	/* interface AdvReachableTime option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_ADV_REACHABLE_TIME);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_REACHABLE_TIME, buf);
	}

	/* interface AdvRetransTimer option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_ADV_RETRANS_TIMER);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_RETRANS_TIMER, buf);
	}

	/* interface AdvCurHopLimit option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_ADV_CUR_HOP_LIMIT);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_CUR_HOP_LIMIT, buf);
	}

	/* interface AdvDefaultLifetime option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_ADV_DEFAULT_LIFETIME);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_DEFAULT_LIFETIME, buf);
	}

	/* interface AdvDefaultPreference option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_ADV_DEFAULT_PREFERENCE);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_DEFAULT_PREFERENCE, buf);
	}

	/* interface AdvSourceLLAddress option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_ADV_SOURCE_LL_ADDRESS);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_SOURCE_LL_ADDRESS, buf);
	}

	/* interface AdvHomeAgentFlag option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_ADV_HOME_AGENT_FLAG);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_HOME_AGENT_FLAG, buf);
	}

	/* interface AdvHomeAgentInfo option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_ADV_HOME_AGENT_INFO);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_HOME_AGENT_INFO, buf);
	}

	/* interface HomeAgentLifetime option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_HOME_AGENT_LIFETIME);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_HOME_AGENT_LIFETIME, buf);
	}

	/* interface HomeAgentPreference option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_HOME_AGENT_PREFERENCE);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_HOME_AGENT_PREFERENCE, buf);
	}

	/* interface AdvMobRtrSupportFlag option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_ADV_MOB_RTR_SUPPORT_FLAG);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_MOB_RTR_SUPPORT_FLAG, buf);
	}

	/* interface AdvIntervalOpt option */
	snprintf(name, sizeof(name), "%s%d.%s", NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CONF_KEYWORD_ADV_INTERVAL_OPT);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_INTERVAL_OPT, buf);
	}

	return (EXIT_SUCCESS);
}

static int gen_prefix_specific_options(FILE *file, int ifn, int idx)
{
	char name[64];
	char buf[256];
	int rc;

	/* prefix AdvOnLink option */
	snprintf(name, sizeof(name), "%s%d.%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_P, idx,
		EZCFG_RADVD_CONF_KEYWORD_ADV_ON_LINK);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_ON_LINK, buf);
	}

	/* prefix AdvAutonomous option */
	snprintf(name, sizeof(name), "%s%d.%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_P, idx,
		EZCFG_RADVD_CONF_KEYWORD_ADV_AUTONOMOUS);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_AUTONOMOUS, buf);
	}

	/* prefix AdvRouterAddr option */
	snprintf(name, sizeof(name), "%s%d.%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_P, idx,
		EZCFG_RADVD_CONF_KEYWORD_ADV_ROUTER_ADDR);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_ROUTER_ADDR, buf);
	}

	/* prefix AdvValidLifetime option */
	snprintf(name, sizeof(name), "%s%d.%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_P, idx,
		EZCFG_RADVD_CONF_KEYWORD_ADV_VALID_LIFETIME);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_VALID_LIFETIME, buf);
	}

	/* prefix AdvPreferredLifetime option */
	snprintf(name, sizeof(name), "%s%d.%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_P, idx,
		EZCFG_RADVD_CONF_KEYWORD_ADV_PREFERRED_LIFETIME);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_PREFERRED_LIFETIME, buf);
	}

	/* prefix AdvDeprecatePrefix option */
	snprintf(name, sizeof(name), "%s%d.%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_P, idx,
		EZCFG_RADVD_CONF_KEYWORD_DEPRECATE_PREFIX);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_DEPRECATE_PREFIX, buf);
	}

	/* prefix DecrementLifetimes option */
	snprintf(name, sizeof(name), "%s%d.%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_P, idx,
		EZCFG_RADVD_CONF_KEYWORD_DECREMENT_LIFETIMES);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_DECREMENT_LIFETIMES, buf);
	}

	/* prefix Base6Interface option */
	snprintf(name, sizeof(name), "%s%d.%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_P, idx,
		EZCFG_RADVD_CONF_KEYWORD_BASE_6_INTERFACE);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_BASE_6_INTERFACE, buf);
	}

	/* prefix Base6to4Interface option */
	snprintf(name, sizeof(name), "%s%d.%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_P, idx,
		EZCFG_RADVD_CONF_KEYWORD_BASE_6TO4_INTERFACE);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_BASE_6TO4_INTERFACE, buf);
	}

	return (EXIT_SUCCESS);
}

static int gen_prefix_definitions(FILE *file, int ifn)
{
	char name[64];
	char buf[256];
	int rc;
	int i;
	int prefix_number = 0;

	/* interface prefix numbers */
	snprintf(name, sizeof(name), "%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_PREFIX_NUMBER);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc >= 0) {
		prefix_number = atoi(buf);
	}

	for (i = 0; i < prefix_number; i++) {
		/* prefix */
		snprintf(name, sizeof(name), "%s%d.%s%d.%s",
			NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
			EZCFG_RADVD_P, i+1,
			EZCFG_RADVD_CONF_KEYWORD_PREFIX);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc <= 0)
			continue;
		/* prefix begin */
		fprintf(file, "\t%s %s\n{\n", EZCFG_RADVD_CONF_KEYWORD_PREFIX, buf);

		gen_prefix_specific_options(file, ifn, i+1);

		/* prefix end */
		fprintf(file, "\t};\n");
	}

	return (EXIT_SUCCESS);
}

static int gen_clients(FILE *file, int ifn)
{
	char name[64];
	char buf[256];
	int rc;
	int i, j;
	int clients_number = 0;
	int ipv6_address_number = 0;

	/* interface clients numbers */
	snprintf(name, sizeof(name), "%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_CLIENTS_NUMBER);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc >= 0) {
		clients_number = atoi(buf);
	}

	for (i = 0; i < clients_number; i++) {
		/* clients */
		snprintf(name, sizeof(name), "%s%d.%s%d.%s",
			NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
			EZCFG_RADVD_C, i+1,
			EZCFG_RADVD_IPV6_ADDRESS_NUMBER);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc >= 0) {
			ipv6_address_number = atoi(buf);
		}

		if (ipv6_address_number <= 0)
			continue;

		/* clients begin */
		fprintf(file, "\t%s\n{\n", EZCFG_RADVD_CONF_KEYWORD_CLIENTS);

		for (j = 0; j < ipv6_address_number; j++) {
			snprintf(name, sizeof(name), "%s%d.%s%d.%s%d",
				NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
				EZCFG_RADVD_C, i+1,
				EZCFG_RADVD_IPV6_ADDRESS, j+1);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc <= 0)
				continue;
			fprintf(file, "\t\t%s;\n", buf);
		}

		/* clients end */
		fprintf(file, "\t};\n");
	}

	return (EXIT_SUCCESS);
}

static int gen_route_specific_options(FILE *file, int ifn, int idx)
{
	char name[64];
	char buf[256];
	int rc;

	/* route AdvRouteLifetime option */
	snprintf(name, sizeof(name), "%s%d.%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_R, idx,
		EZCFG_RADVD_CONF_KEYWORD_ADV_ROUTE_LIFETIME);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_ROUTE_LIFETIME, buf);
	}

	/* route AdvRoutePreference option */
	snprintf(name, sizeof(name), "%s%d.%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_R, idx,
		EZCFG_RADVD_CONF_KEYWORD_ADV_ROUTE_PREFERENCE);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_ROUTE_PREFERENCE, buf);
	}

	/* route RemoveRoute option */
	snprintf(name, sizeof(name), "%s%d.%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_R, idx,
		EZCFG_RADVD_CONF_KEYWORD_REMOVE_ROUTE);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_REMOVE_ROUTE, buf);
	}

	return (EXIT_SUCCESS);
}

static int gen_route_definitions(FILE *file, int ifn)
{
	char name[64];
	char buf[256];
	int rc;
	int i;
	int route_number = 0;

	/* interface route numbers */
	snprintf(name, sizeof(name), "%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_ROUTE_NUMBER);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc >= 0) {
		route_number = atoi(buf);
	}

	for (i = 0; i < route_number; i++) {
		/* route */
		snprintf(name, sizeof(name), "%s%d.%s%d.%s",
			NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
			EZCFG_RADVD_R, i+1,
			EZCFG_RADVD_CONF_KEYWORD_PREFIX);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc <= 0)
			continue;
		/* route begin */
		fprintf(file, "\t%s %s\n{\n", EZCFG_RADVD_CONF_KEYWORD_ROUTE, buf);

		gen_route_specific_options(file, ifn, i+1);

		/* route end */
		fprintf(file, "\t};\n");
	}

	return (EXIT_SUCCESS);
}

static int gen_RDNSS_specific_options(FILE *file, int ifn, int idx)
{
	char name[64];
	char buf[256];
	int rc;

	/* RDNSS AdvRDNSSLifetime option */
	snprintf(name, sizeof(name), "%s%d.%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_S, idx,
		EZCFG_RADVD_CONF_KEYWORD_ADV_RDNSS_LIFETIME);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_RDNSS_LIFETIME, buf);
	}

	/* RDNSS FlushRDNSS option */
	snprintf(name, sizeof(name), "%s%d.%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_S, idx,
		EZCFG_RADVD_CONF_KEYWORD_FLUSH_RDNSS);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_FLUSH_RDNSS, buf);
	}

	return (EXIT_SUCCESS);
}

static int gen_RDNSS_definitions(FILE *file, int ifn)
{
	char name[64];
	char buf[256];
	int rc;
	int i;
	int rdnss_number = 0;

	/* interface RDNSS numbers */
	snprintf(name, sizeof(name), "%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_RDNSS_NUMBER);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc >= 0) {
		rdnss_number = atoi(buf);
	}

	for (i = 0; i < rdnss_number; i++) {
		/* RDNSS */
		snprintf(name, sizeof(name), "%s%d.%s%d.%s",
			NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
			EZCFG_RADVD_S, i+1,
			EZCFG_RADVD_IPS);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc <= 0)
			continue;
		/* RDNSS begin */
		fprintf(file, "\t%s %s\n{\n", EZCFG_RADVD_CONF_KEYWORD_RDNSS, buf);

		gen_RDNSS_specific_options(file, ifn, i+1);

		/* RDNSS end */
		fprintf(file, "\t};\n");
	}

	return (EXIT_SUCCESS);
}

static int gen_DNSSL_specific_options(FILE *file, int ifn, int idx)
{
	char name[64];
	char buf[256];
	int rc;

	/* DNSSL AdvDNSSLLifetime option */
	snprintf(name, sizeof(name), "%s%d.%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_L, idx,
		EZCFG_RADVD_CONF_KEYWORD_ADV_DNSSL_LIFETIME);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_ADV_DNSSL_LIFETIME, buf);
	}

	/* DNSSL FlushDNSSL option */
	snprintf(name, sizeof(name), "%s%d.%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_L, idx,
		EZCFG_RADVD_CONF_KEYWORD_FLUSH_DNSSL);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "\t\t%s %s;\n", EZCFG_RADVD_CONF_KEYWORD_FLUSH_DNSSL, buf);
	}

	return (EXIT_SUCCESS);
}

static int gen_DNSSL_definitions(FILE *file, int ifn)
{
	char name[64];
	char buf[256];
	int rc;
	int i;
	int dnssl_number = 0;

	/* interface DNSSL numbers */
	snprintf(name, sizeof(name), "%s%d.%s",
		NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
		EZCFG_RADVD_DNSSL_NUMBER);
	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if (rc >= 0) {
		dnssl_number = atoi(buf);
	}

	for (i = 0; i < dnssl_number; i++) {
		/* DNSSL */
		snprintf(name, sizeof(name), "%s%d.%s%d.%s",
			NVRAM_SERVICE_OPTION(RADVD, IF), ifn,
			EZCFG_RADVD_L, i+1,
			EZCFG_RADVD_SUFFIXES);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc <= 0)
			continue;
		/* DNSSL begin */
		fprintf(file, "\t%s %s\n{\n", EZCFG_RADVD_CONF_KEYWORD_DNSSL, buf);

		gen_DNSSL_specific_options(file, ifn, i+1);

		/* DNSSL end */
		fprintf(file, "\t};\n");
	}

	return (EXIT_SUCCESS);
}

int pop_etc_radvd_conf(int flag)
{
        FILE *file = NULL;
	char name[64];
	char buf[256];
	int rc;
	int i;
	int interface_number = 0;

	/* generate /etc/radvd.conf */
	file = fopen("/etc/radvd.conf", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	switch (flag) {
	case RC_ACT_START :

		/* interface_number */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RADVD, INTERFACE_NUMBER),
			buf, sizeof(buf));
		if (rc >= 0) {
			interface_number = atoi(buf);
		}

		for (i = 0; i < interface_number; i++) {
			/* interface name */
			snprintf(name, sizeof(name), "%s%d.%s",
				NVRAM_SERVICE_OPTION(RADVD, IF), i+1,
				EZCFG_RADVD_NAME);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc <= 0)
				continue;
			/* interface begin */
			fprintf(file, "%s %s\n{\n", EZCFG_RADVD_CONF_KEYWORD_INTERFACE, buf);

			gen_interface_specific_options(file, i+1);
			gen_prefix_definitions(file, i+1);
			gen_clients(file, i+1);
			gen_route_definitions(file, i+1);
			gen_RDNSS_definitions(file, i+1);
			gen_DNSSL_definitions(file, i+1);

			/* interface end */
			fprintf(file, "};\n");
		}

		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
