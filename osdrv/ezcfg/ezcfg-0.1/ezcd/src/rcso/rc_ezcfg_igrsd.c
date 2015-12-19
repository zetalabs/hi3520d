/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_ezcfg_igrsd.c
 *
 * Description  : ezbox run ezcfg igrsd service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-30   0.1       Write it from scratch
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

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_ezcfg_igrsd(int argc, char **argv)
#endif
{
	int rc = -1;
	int ip[4];
	char buf[256];
	char isdp_addr[64];
	int flag, ret = EXIT_FAILURE;
	struct ezcfg_arg_nvram_socket *ap1 = NULL;

	if (argc < 3) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "ezcfg_igrsd")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		goto func_exit;
	}

	buf[0] = '\0';
#if (HAVE_EZBOX_LAN_NIC == 1)
	if (strcmp(argv[1], "lan") == 0 &&
	    utils_service_binding_lan(NVRAM_SERVICE_OPTION(EZCFG, IGRSD_BINDING)) == true) {
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(LAN, IPADDR), buf, sizeof(buf));
		if (rc < 0) {
			return (EXIT_FAILURE);
		}
	} else
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
	if (strcmp(argv[1], "wan") == 0 &&
	    utils_service_binding_wan(NVRAM_SERVICE_OPTION(EZCFG, IGRSD_BINDING)) == true) {
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, IPADDR), buf, sizeof(buf));
		if (rc < 0) {
			return (EXIT_FAILURE);
		}
	} else
#endif
#if ((HAVE_EZBOX_LAN_NIC == 1) || (HAVE_EZBOX_WAN_NIC == 1))
	{
		return (EXIT_FAILURE);
	}
#endif

	rc = sscanf(buf, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
	if (rc != 4) {
		return (EXIT_FAILURE);
	}

	snprintf(isdp_addr, sizeof(isdp_addr), "%s:%d@%d.%d.%d.%d",
		EZCFG_PROTO_IGRS_ISDP_MCAST_IPADDR_STRING,
		EZCFG_PROTO_IGRS_ISDP_PORT_NUMBER,
		ip[0], ip[1], ip[2], ip[3]);

	ap1 = ezcfg_api_arg_nvram_socket_new();
	if (ap1 == NULL) {
		goto func_exit;
	}

	flag = utils_get_rc_act_type(argv[2]);

	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_STOP :
		/* delete ezcfg igrsd listening sockets */
		rc = ezcfg_api_arg_nvram_socket_set_domain(ap1, EZCFG_SOCKET_DOMAIN_INET_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_socket_set_type(ap1, EZCFG_SOCKET_TYPE_DGRAM_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_socket_set_protocol(ap1, EZCFG_SOCKET_PROTO_IGRS_ISDP_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_socket_set_protocol(ap1, isdp_addr);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_nvram_remove_socket(ap1);

		/* restart ezcfg daemon */
		/* FIXME: do it in action config file */
#if 0
		if (rc >= 0) {
			rc_ezcd(RC_ACT_RELOAD);
		}
#endif
		if (flag == RC_ACT_STOP) {
			ret = EXIT_SUCCESS;
			break;
		}

		/* RC_ACT_RESTART fall through */

	case RC_ACT_START :
		rc = utils_nvram_cmp(NVRAM_SERVICE_OPTION(EZCFG, IGRSD_ENABLE), "1");
		if (rc < 0) {
			goto func_exit;
		}

		/* prepare IGRS xml files */
		pop_etc_ezcfg_igrsd(flag);

		/* add ezcfg igrsd listening sockets */
		rc = ezcfg_api_arg_nvram_socket_set_domain(ap1, EZCFG_SOCKET_DOMAIN_INET_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_socket_set_type(ap1, EZCFG_SOCKET_TYPE_DGRAM_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_socket_set_protocol(ap1, EZCFG_SOCKET_PROTO_IGRS_ISDP_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_socket_set_protocol(ap1, isdp_addr);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_nvram_insert_socket(ap1);

		/* restart ezcfg daemon */
		/* FIXME: do it in config file */
#if 0
		if (rc >= 0) {
			rc_ezcd(RC_ACT_RELOAD);
		}
#endif
		ret = EXIT_SUCCESS;
		break;

	default:
		ret = EXIT_FAILURE;
		break;
	}
func_exit:
	if (ap1 != NULL) {
		ezcfg_api_arg_nvram_socket_delete(ap1);
	}

	return (ret);
}
