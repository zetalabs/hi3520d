/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_network_interfaces.c
 *
 * Description  : ezbox /etc/network/interfaces generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-03   0.1       Write it from scratch
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

static int set_loopback_interface(FILE *file)
{
	fprintf(file, "iface lo inet loopback\n");
	fprintf(file, "\n");
	return (EXIT_SUCCESS);
}

#if (HAVE_EZBOX_LAN_NIC == 1)
static int set_lan_interface(FILE *file)
{
	char lan_ifname[IFNAMSIZ];
	int lan_ipaddr[4];
	int lan_netmask[4];
	int lan_gateway[4];
	char buf[128];
	int rc;

	/* setup lan interface */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(LAN, IFNAME), lan_ifname, sizeof(lan_ifname));
	if (rc < 0)
		return (EXIT_FAILURE);

	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(LAN, IPADDR), buf, sizeof(buf));
	if (rc < 0)
		return (EXIT_FAILURE);

	if (sscanf(buf, "%d.%d.%d.%d",
	       &lan_ipaddr[0],
	       &lan_ipaddr[1],
	       &lan_ipaddr[2],
	       &lan_ipaddr[3]) != 4)
		return (EXIT_FAILURE);

	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(LAN, NETMASK), buf, sizeof(buf));
	if (rc < 0)
		return (EXIT_FAILURE);

	if (sscanf(buf, "%d.%d.%d.%d",
	       &lan_netmask[0],
	       &lan_netmask[1],
	       &lan_netmask[2],
	       &lan_netmask[3]) != 4)
		return (EXIT_FAILURE);

	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(LAN, GATEWAY), buf, sizeof(buf));
	if (rc < 0)
		return (EXIT_FAILURE);

	if (sscanf(buf, "%d.%d.%d.%d",
	       &lan_gateway[0],
	       &lan_gateway[1],
	       &lan_gateway[2],
	       &lan_gateway[3]) != 4)
		return (EXIT_FAILURE);

	fprintf(file, "iface %s inet static\n", lan_ifname);
	fprintf(file, "\taddress %d.%d.%d.%d\n",
		lan_ipaddr[0],
		lan_ipaddr[1],
		lan_ipaddr[2],
		lan_ipaddr[3]);
	fprintf(file, "\tnetmask %d.%d.%d.%d\n",
		lan_netmask[0],
		lan_netmask[1],
		lan_netmask[2],
		lan_netmask[3]);
	fprintf(file, "\tnetwork %d.%d.%d.%d\n",
		lan_ipaddr[0] & lan_netmask[0],
		lan_ipaddr[1] & lan_netmask[1],
		lan_ipaddr[2] & lan_netmask[2],
		lan_ipaddr[3] & lan_netmask[3]);
	if ((lan_gateway[0] != 0) &&
	    (lan_gateway[3] != 0)) {
		fprintf(file, "\tgateway %d.%d.%d.%d\n",
			lan_gateway[0],
			lan_gateway[1],
			lan_gateway[2],
			lan_gateway[3]);
	}
	fprintf(file, "\n");
	return (EXIT_SUCCESS);
}
#endif

#if (HAVE_EZBOX_WAN_NIC == 1)
static int set_wan_interface(FILE *file)
{
	char wan_ifname[IFNAMSIZ];
	int wan_ipaddr[4];
	int wan_netmask[4];
	int wan_gateway[4];
	int wan_type;
	char buf[128];
	int rc;
	struct stat stat_buf;

	/* setup wan interface */
	wan_type = utils_get_wan_type();
	if (wan_type == WAN_TYPE_UNKNOWN)
		return (EXIT_FAILURE);

	switch(wan_type) {
	case WAN_TYPE_DHCP :
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, DHCP_IFNAME), wan_ifname, sizeof(wan_ifname));
		if (rc < 0)
			return (EXIT_FAILURE);

		fprintf(file, "iface %s inet dhcp\n", wan_ifname);
		fprintf(file, "\n");

		/* also make udhcpc script symbol link */
		if (stat(UDHCPC_SCRIPT_FILE_DIR, &stat_buf) == 0 &&
		    S_ISDIR(stat_buf.st_mode)) {
			/* it's good. */
                }
		else {
		        utils_system(CMD_RM " -rf " UDHCPC_SCRIPT_FILE_DIR);
		        utils_system(CMD_MKDIR " -p " UDHCPC_SCRIPT_FILE_DIR);
		}
		utils_system(CMD_RM " -rf " UDHCPC_SCRIPT_FILE_PATH);
		rc = symlink(CMD_UDHCPC_SCRIPT, UDHCPC_SCRIPT_FILE_PATH);
		if (rc < 0)
			return (EXIT_FAILURE);
		break;

	case WAN_TYPE_STATIC :
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, STATIC_IPADDR), buf, sizeof(buf));
		if (rc < 0)
			return (EXIT_FAILURE);

		if (sscanf(buf, "%d.%d.%d.%d",
		       &wan_ipaddr[0],
		       &wan_ipaddr[1],
		       &wan_ipaddr[2],
		       &wan_ipaddr[3]) != 4)
			return (EXIT_FAILURE);

		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, STATIC_NETMASK), buf, sizeof(buf));
		if (rc < 0)
			return (EXIT_FAILURE);

		if (sscanf(buf, "%d.%d.%d.%d",
		       &wan_netmask[0],
		       &wan_netmask[1],
		       &wan_netmask[2],
		       &wan_netmask[3]) != 4)
			return (EXIT_FAILURE);

		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, STATIC_GATEWAY), buf, sizeof(buf));
		if (rc < 0)
			return (EXIT_FAILURE);

		if (sscanf(buf, "%d.%d.%d.%d",
		       &wan_gateway[0],
		       &wan_gateway[1],
		       &wan_gateway[2],
		       &wan_gateway[3]) != 4)
			return (EXIT_FAILURE);

		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, STATIC_IFNAME), wan_ifname, sizeof(wan_ifname));
		if (rc < 0)
			return (EXIT_FAILURE);

		fprintf(file, "iface %s inet static\n", wan_ifname);
		fprintf(file, "\taddress %d.%d.%d.%d\n",
			wan_ipaddr[0],
			wan_ipaddr[1],
			wan_ipaddr[2],
			wan_ipaddr[3]);
		fprintf(file, "\tnetmask %d.%d.%d.%d\n",
			wan_netmask[0],
			wan_netmask[1],
			wan_netmask[2],
			wan_netmask[3]);
		fprintf(file, "\tnetwork %d.%d.%d.%d\n",
			wan_ipaddr[0] & wan_netmask[0],
			wan_ipaddr[1] & wan_netmask[1],
			wan_ipaddr[2] & wan_netmask[2],
			wan_ipaddr[3] & wan_netmask[3]);
		fprintf(file, "\tgateway %d.%d.%d.%d\n",
			wan_gateway[0],
			wan_gateway[1],
			wan_gateway[2],
			wan_gateway[3]);
		fprintf(file, "\n");
		break;

	case WAN_TYPE_PPPOE :
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, PPPOE_DEVICE_NAME), wan_ifname, sizeof(wan_ifname));
		if (rc < 0)
			return (EXIT_FAILURE);

		fprintf(file, "iface %s inet manual\n", wan_ifname);
		fprintf(file, "\n");
		break;
	}
	return (EXIT_SUCCESS);
}
#endif

int pop_etc_network_interfaces(int flag)
{
	FILE *file;
	int ret;

	file = fopen("/etc/network/interfaces", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	switch (flag) {
	case RC_ACT_BOOT :
	case RC_ACT_START :
		/* set loopback */
		set_loopback_interface(file);

		/* set lan interface */
#if (HAVE_EZBOX_LAN_NIC == 1)
		set_lan_interface(file);
#endif

		/* set wan interface */
#if (HAVE_EZBOX_WAN_NIC == 1)
		set_wan_interface(file);
#endif
		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}

	fclose(file);
	return (ret);
}
