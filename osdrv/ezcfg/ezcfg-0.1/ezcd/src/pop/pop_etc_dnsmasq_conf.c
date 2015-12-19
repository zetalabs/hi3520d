/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_dnsmasq_conf.c
 *
 * Description  : ezbox /etc/dnsmasq.conf file generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-02   0.1       Write it from scratch
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

#define DNSMASQ_LEASE_FILE_DIR	"/var/lib/misc"

int pop_etc_dnsmasq_conf(int flag)
{
        FILE *file = NULL;
	char name[32];
	char buf[64];
	char ifname[IFNAMSIZ];
	int sip[4];
	int eip[4];
	int nmsk[4];
	int lease;
	int i;
	int rc;
	struct stat stat_buf;

	/* generate /etc/hosts */
	file = fopen("/etc/dnsmasq.conf", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	switch (flag) {
	case RC_ACT_START :
		if (stat(DNSMASQ_LEASE_FILE_DIR, &stat_buf) == 0 &&
		    S_ISDIR(stat_buf.st_mode)) {
			/* it's good. */
		}
		else {
			utils_system(CMD_RM " -rf " DNSMASQ_LEASE_FILE_DIR);
			utils_system(CMD_MKDIR " -p " DNSMASQ_LEASE_FILE_DIR);
		}

		/* Never forward plain names (without a dot or domain part) */
		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DNSMASQ, DOMAIN_NEEDED), "1") == 0) {
			fprintf(file, "%s\n", SERVICE_OPTION(DNSMASQ, DOMAIN_NEEDED));
		}

		/* get upstream servers from somewhere other that /etc/resolv.conf */
		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_WAN_DNS_ENABLE), "0") == 0) {
			FILE *fp = NULL;
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DNSMASQ, RESOLV_FILE), "/etc/resolv.conf.dnsmasq");
			fp = fopen("/etc/resolv.conf.dnsmasq", "w");
			if (fp != NULL) {
				for (i = 1; i <= 3; i++) {
					snprintf(name, sizeof(name), "%s%d",
						NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_DNS), i);
					rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
					if (rc >= 0) {
						fprintf(file, "nameserver %s\n", buf);
					}
				}
				fclose(fp);
			}
		}

		/* --user */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DNSMASQ, USER));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DNSMASQ, USER), buf);
		}
		/* --bogus-priv */
		if (utils_service_enable(NVRAM_SERVICE_OPTION(DNSMASQ, BOGUS_PRIV)) == true) {
			fprintf(file, "%s\n", SERVICE_OPTION(DNSMASQ, BOGUS_PRIV));
		}
		/* --bind-interfaces */
		if (utils_service_enable(NVRAM_SERVICE_OPTION(DNSMASQ, BIND_INTERFACES)) == true) {
			fprintf(file, "%s\n", SERVICE_OPTION(DNSMASQ, BIND_INTERFACES));
		}
		/* --min-port */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DNSMASQ, MIN_PORT));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DNSMASQ, MIN_PORT), buf);
		}
		/* don't listenning on lo interface */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(LOOPBACK, IFNAME));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DNSMASQ, EXCEPT_INTERFACE), buf);
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DNSMASQ, NO_DHCP_INTERFACE), buf);
		}
		/* listen for DHCP and DNS requests only on specified interfaces */
		if (utils_service_binding_lan(NVRAM_SERVICE_OPTION(RC, DNSMASQ_BINDING)) == true) {
			/* get interface */
			snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(LAN, IFNAME));
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc < 1) {
				break;
			}
			snprintf(ifname, sizeof(ifname), "%s", buf);

			/* get DHCP server start IP address */
			snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_START_IPADDR));
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc < 0 || sscanf(buf, "%d.%d.%d.%d",
				&sip[0], &sip[1], &sip[2], &sip[3]) != 4) {
				break;
			}


			/* get DHCP server end IP address */
			snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_END_IPADDR));
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc < 0 || sscanf(buf, "%d.%d.%d.%d",
				&eip[0], &eip[1], &eip[2], &eip[3]) != 4) {
				break;
			}

			/* get DHCP server IP netmask */
			snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_NETMASK));
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc < 0 || sscanf(buf, "%d.%d.%d.%d",
				&nmsk[0], &nmsk[1], &nmsk[2], &nmsk[3]) != 4) {
				break;
			}

			/* get DHCP server lease time */
			snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_LEASE));
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc < 0) {
				break;
			}
			lease = atoi(buf);

			fprintf(file, "%s=%s\n", SERVICE_OPTION(DNSMASQ, INTERFACE), ifname);
			fprintf(file, "%s=%s:%s,%d.%d.%d.%d,%d.%d.%d.%d,%d.%d.%d.%d,%d\n", SERVICE_OPTION(DNSMASQ, DHCP_RANGE),
				SERVICE_OPTION(DNSMASQ, INTERFACE), ifname,
				sip[0], sip[1], sip[2], sip[3],
				eip[0], eip[1], eip[2], eip[3],
				nmsk[0], nmsk[1], nmsk[2], nmsk[3],
				lease);
		}
		else {
			/* "interface=" to disable all interfaces except loop */
			fprintf(file, "%s=\n", SERVICE_OPTION(DNSMASQ, INTERFACE));
		}
		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
