/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_iptables_firewall.c
 *
 * Description  : ezbox /etc/iptables/firewall generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-01-08   0.1       Write it from scratch
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

#if 0
#define DBG(format, args...) do {\
	char dbg_path[256]; \
	FILE *dbg_fp; \
	snprintf(dbg_path, 256, "/tmp/%d-debug.txt", getpid()); \
	dbg_fp = fopen(dbg_path, "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif


/********************
 * firewall data structure
 ********************/
typedef struct pending_rule_s {
	char *rule;
	struct pending_rule_s *next;
} pending_rule_t;

typedef struct fw_param_s {
	bool fw_enable;
	bool nat_enable;
	bool dmz_enable;
	/* need to free part */
	char *loopback_ifname;
	char *lan_ifname;
	char *wan_ifname;
	char *lan_ipaddr;
	char *lan_netmask;
	char *wan_ipaddr;
	char *wan_netmask;
	char *dmz_dst_ipaddr;
	pending_rule_t *filter_forward;
} fw_param_t;

static void delete_pending_rule(pending_rule_t *p) {
	if (p != NULL) {
		if (p->rule != NULL) {
			free(p->rule);
		}

		free(p);
	}
}

static bool add_pending_rule(pending_rule_t **l, const char *rule)
{
	pending_rule_t *p, *q;

	p = malloc(sizeof(pending_rule_t));
	if (p == NULL)
		return false;

	memset(p, 0, sizeof(pending_rule_t));
	p->rule = strdup(rule);
	if (p->rule == NULL) {
		delete_pending_rule(p);
		return false;
	}
	p->next = NULL;

	/* find tail node of rule list */
	q = *l;
	if (q == NULL) {
		/* the rule list is empty,
		 * set p as the head of the rule list
		 */
		*l = p;
	}
	else {
		/* the rule list is not empty,
		 * set p as the tail of the rule list
		 */
		while (q->next != NULL) {
			q = q->next;
		}
		q->next = p;
	}

	return true;
}

static void delete_pending_rules(pending_rule_t **l)
{
	pending_rule_t *p;

	p = *l;
	while (p != NULL) {
		*l = p->next;
		delete_pending_rule(p);
		p = *l;
	}
}

static void delete_fw_param(fw_param_t *fwp)
{
	if (fwp->loopback_ifname != NULL)
		free(fwp->loopback_ifname);

	if (fwp->lan_ifname != NULL)
		free(fwp->lan_ifname);

	if (fwp->wan_ifname != NULL)
		free(fwp->wan_ifname);

	if (fwp->lan_ipaddr != NULL)
		free(fwp->lan_ipaddr);

	if (fwp->lan_netmask != NULL)
		free(fwp->lan_netmask);

	if (fwp->wan_ipaddr != NULL)
		free(fwp->wan_ipaddr);

	if (fwp->wan_netmask != NULL)
		free(fwp->wan_netmask);

	if (fwp->dmz_dst_ipaddr != NULL)
		free(fwp->dmz_dst_ipaddr);

	if (fwp->filter_forward != NULL)
		delete_pending_rules(&(fwp->filter_forward));

	free(fwp);
}

static bool init_fw_param(fw_param_t *fwp)
{
	char buf[256];

	/* firewall enable */
	fwp->fw_enable = utils_service_enable(NVRAM_SERVICE_OPTION(IPTABLES, FIREWALL_ENABLE));
	/* NAT enable */
	fwp->nat_enable = utils_service_enable(NVRAM_SERVICE_OPTION(IPTABLES, NAT_ENABLE));
	/* DMZ enable */
	fwp->dmz_enable = utils_service_enable(NVRAM_SERVICE_OPTION(IPTABLES, DMZ_ENABLE));

	/* loopback interface */
	if (ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(LOOPBACK, IFNAME), buf, sizeof(buf)) > 0) {
		fwp->loopback_ifname = strdup(buf);
		if (fwp->loopback_ifname == NULL)
			return false;
	}

	/* LAN interface */
	if (ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(LAN, IFNAME), buf, sizeof(buf)) > 0) {
		fwp->lan_ifname = strdup(buf);
		if (fwp->lan_ifname == NULL)
			return false;
	}

	/* WAN interface */
	if (ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, IFNAME), buf, sizeof(buf)) > 0) {
		fwp->wan_ifname = strdup(buf);
		if (fwp->wan_ifname == NULL)
			return false;
	}

	/* LAN IP address */
	if (ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(LAN, IPADDR), buf, sizeof(buf)) > 0) {
		fwp->lan_ipaddr = strdup(buf);
		if (fwp->lan_ipaddr == NULL)
			return false;
	}

	/* LAN netmask */
	if (ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(LAN, NETMASK), buf, sizeof(buf)) > 0) {
		fwp->lan_netmask = strdup(buf);
		if (fwp->lan_netmask == NULL)
			return false;
	}

	/* WAN IP address */
	if (ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, IPADDR), buf, sizeof(buf)) > 0) {
		fwp->wan_ipaddr = strdup(buf);
		if (fwp->wan_ipaddr == NULL)
			return false;
	}

	/* WAN netmask */
	if (ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, NETMASK), buf, sizeof(buf)) > 0) {
		fwp->wan_netmask = strdup(buf);
		if (fwp->wan_netmask == NULL)
			return false;
	}

	/* DMZ destination IP address */
	if (ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(IPTABLES, DMZ_DST_IPADDR), buf, sizeof(buf)) > 0) {
		fwp->dmz_dst_ipaddr = strdup(buf);
		if (fwp->dmz_dst_ipaddr == NULL)
			return false;
	}

	/* pending filter forward rules list */
	fwp->filter_forward = NULL;

	return true;
}

/********************
 * mangle table
 ********************/
static bool build_mangle_table(FILE *fp, fw_param_t *fwp)
{
	char buf[1024];

	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		"*mangle");
	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		":PREROUTING ACCEPT [0:0]");
	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		":OUTPUT ACCEPT [0:0]");

	/* block WAN to LAN IP address packets */
	if (fwp->nat_enable == true) {
		if ((fwp->wan_ifname != NULL) &&
		    (fwp->lan_ipaddr != NULL) &&
		    (fwp->lan_netmask != NULL)) {
			utils_file_print_line(fp, buf, sizeof(buf), "-A PREROUTING -i %s -d %s/%s -j DROP\n",
				fwp->wan_ifname,
				fwp->lan_ipaddr,
				fwp->lan_netmask);
		}
	}

	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		"COMMIT");

	return true;
}

/********************
 * nat table
 ********************/
static bool build_nat_prerouting(FILE *fp, char *buf, size_t size, fw_param_t *fwp)
{
	/* ICMP packets should always be redirected to INPUT chains */
	if ((fwp->wan_ipaddr != NULL) &&
	    (fwp->lan_ipaddr != NULL)) {
		utils_file_print_line(fp, buf, size, "-A PREROUTING -p icmp -d %s -j DNAT --to-destination %s\n",
			fwp->wan_ipaddr,
			fwp->lan_ipaddr);
	}

	/* setup DMZ rule */
	if (fwp->dmz_enable == true) {
		if ((fwp->wan_ipaddr != NULL) &&
		    (fwp->dmz_dst_ipaddr != NULL)) {
			utils_file_print_line(fp, buf, size, "-A PREROUTING -d %s -j DNAT --to-destination %s\n",
				fwp->wan_ipaddr,
				fwp->dmz_dst_ipaddr);
		}

		/* add pending FORWARD rules for DMZ, which will be added in filter table */
		if ((fwp->lan_ifname != NULL) &&
		    (fwp->dmz_dst_ipaddr != NULL)) {
			snprintf(buf, size, "-A FORWARD -o %s -d %s -j ACCEPT",
				fwp->lan_ifname,
				fwp->dmz_dst_ipaddr);
			add_pending_rule(&(fwp->filter_forward), buf);
		}
	}

	return true;
}

static bool build_nat_postrouting(FILE *fp, char *buf, size_t size, fw_param_t *fwp)
{
	/* NAT enable */
	if (fwp->nat_enable == true) {
		if (fwp->wan_ifname != NULL) {
			utils_file_print_line(fp, buf, size, "-A POSTROUTING -o %s -j MASQUERADE\n",
				fwp->wan_ifname);
		}
	}

	return true;
}

static bool build_nat_table(FILE *fp, fw_param_t *fwp)
{
	char buf[1024];

	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		"*nat");
	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		":PREROUTING ACCEPT [0:0]");
	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		":POSTROUTING ACCEPT [0:0]");
	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		":OUTPUT ACCEPT [0:0]");

	build_nat_prerouting(fp, buf, sizeof(buf), fwp);
	build_nat_postrouting(fp, buf, sizeof(buf), fwp);

	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		"COMMIT");

	return true;
}

/********************
 * filter table
 ********************/
static bool build_filter_input(FILE *fp, char *buf, size_t size, fw_param_t *fwp)
{
	/* filtering known SPI states */
	if (fwp->lan_ifname != NULL) {
		utils_file_print_line(fp, buf, size, "-A INPUT -m state --state INVALID ! -i %s -j DROP\n",
			fwp->lan_ifname);
		utils_file_print_line(fp, buf, size, "-A INPUT -i %s -m state --state NEW -j ACCEPT\n",
			fwp->lan_ifname);
	}
	if (fwp->loopback_ifname != NULL) {
		utils_file_print_line(fp, buf, size, "-A INPUT -i %s -m state --state NEW -j ACCEPT\n",
			fwp->loopback_ifname);
	}
	utils_file_print_line(fp, buf, size, "-A INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT\n");

	/* ICMP request from WAN interface */
	utils_file_print_line(fp, buf, size, "-A INPUT -p icmp -j ACCEPT\n");

	/* drop other packets not recognized */
	if (fwp->lan_ifname != NULL) {
		utils_file_print_line(fp, buf, size, "-A INPUT ! -i %s -j DROP\n",
			fwp->lan_ifname);
	}

	return true;
}

static bool build_filter_output(FILE *fp, char *buf, size_t size, fw_param_t *fwp)
{
	return true;
}

static bool build_filter_forward(FILE *fp, char *buf, size_t size, fw_param_t *fwp)
{
	pending_rule_t *p;

	/* if SPI firewall enabled, check LAN size packets first */
	if(fwp->fw_enable == true) {
		if (fwp->lan_ifname != NULL) {
			utils_file_print_line(fp, buf, size, "-A FORWARD -i %s -o %s -j ACCEPT\n",
				fwp->lan_ifname, fwp->lan_ifname);
		}
		/* drop the invalid state packets */
		utils_file_print_line(fp, buf, size, "-A FORWARD -m state --state INVALID -j DROP\n");
        }

	/* rendering pending forward rules */
	p = fwp->filter_forward;
	while(p != NULL) {
		if (p->rule != NULL) {
			utils_file_print_line(fp, buf, size, "%s\n", p->rule);
		}
		fwp->filter_forward = p->next;
		delete_pending_rule(p);
		p = fwp->filter_forward;
	}

	/* if SPI firewall enabled, accept valid connections' packets,
	 * drop the others
	 */
	if(fwp->fw_enable == true) {
		/* accept valid packets */
		utils_file_print_line(fp, buf, size, "-A FORWARD -m state --state RELATED,ESTABLISHED -j ACCEPT\n");

		/* accept new connection packet from LAN */
		if (fwp->lan_ifname != NULL) {
			utils_file_print_line(fp, buf, size, "-A FORWARD -i %s -m state --state NEW -j ACCEPT\n",
				fwp->lan_ifname);
		}

		/* accept new connection packet from WAN if NAT is enabled */
		if ((fwp->nat_enable == false) &&
		    (fwp->wan_ifname != NULL)) {
			utils_file_print_line(fp, buf, size, "-A FORWARD -i %s -m state --state NEW -j ACCEPT\n",
				fwp->wan_ifname);
		}

		/* default rule is DROP */
		utils_file_print_line(fp, buf, size, "-A FORWARD -j DROP\n");
        }

	return true;
}

static bool build_filter_table(FILE *fp, fw_param_t *fwp)
{
	char buf[1024];

	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		"*filter");
	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		":INPUT ACCEPT [0:0]");
	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		":FORWARD ACCEPT [0:0]");
	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		":OUTPUT ACCEPT [0:0]");

	build_filter_input(fp, buf, sizeof(buf), fwp);
	build_filter_output(fp, buf, sizeof(buf), fwp);
	build_filter_forward(fp, buf, sizeof(buf), fwp);

	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		"COMMIT");

	return true;
}

int pop_etc_iptables_firewall(int flag)
{
	FILE *file = NULL;
	int ret = EXIT_FAILURE;
	fw_param_t *fwp = NULL;

	file = fopen("/etc/iptables/firewall", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_STOP :
		if (flag == RC_ACT_STOP) {
			ret = EXIT_SUCCESS;
			break;
		}

		/* RC_ACT_RESTART fall through */
	case RC_ACT_START :
		fwp = malloc(sizeof(fw_param_t));
		if (fwp == NULL) {
			ret = EXIT_FAILURE;
			break;
		}
		/* clean fwp */
		memset(fwp, 0, sizeof(fw_param_t));

		/* initialize fw_param */
		if (init_fw_param(fwp) == false) {
			ret = EXIT_FAILURE;
			break;
		}

		/* generate MANGLE table */
		if (build_mangle_table(file, fwp) == false) {
			ret = EXIT_FAILURE;
			break;
		}

		/* generate NAT table */
		if (build_nat_table(file, fwp) == false) {
			ret = EXIT_FAILURE;
			break;
		}

		/* generate FILTER table */
		if (build_filter_table(file, fwp) == false) {
			ret = EXIT_FAILURE;
			break;
		}

		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}

	if (file != NULL) {
		fclose(file);
	}
	
	if (fwp != NULL) {
		delete_fw_param(fwp);
	}

	if (ret == EXIT_FAILURE) {
		unlink("/etc/iptables/firewall");
	}

	return (ret);
}
