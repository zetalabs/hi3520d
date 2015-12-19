/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_pppoe_server.c
 *
 * Description  : ezbox run pppoe-server service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-09-13   0.1       Write it from scratch
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
	FILE *dbg_fp = fopen("/tmp/rc_pppoe_server.log", "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_pppoe_server(int argc, char **argv)
#endif
{
	int rc;
	char buf[128];
	char ifname[IFNAMSIZ];
	char lipaddr[16];
	char ripaddr[16];
	char session_num[8];
	struct stat stat_buf;
	int flag, ret;

	if (argc < 3) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "pppoe_server")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

#if (HAVE_EZBOX_LAN_NIC == 1)
	if (strcmp(argv[1], "lan") == 0 &&
            utils_service_binding_lan(NVRAM_SERVICE_OPTION(RC, PPPOE_SERVER_BINDING)) == true) {
		/* It's good */
	}
	else
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
	if (strcmp(argv[1], "wan") == 0 &&
            utils_service_binding_wan(NVRAM_SERVICE_OPTION(RC, PPPOE_SERVER_BINDING)) == true) {
		/* It's good */
	}
	else
#endif
#if ((HAVE_EZBOX_LAN_NIC == 1) || (HAVE_EZBOX_WAN_NIC == 1))
	{
		return (EXIT_FAILURE);
	}
#endif

	flag = utils_get_rc_act_type(argv[2]);

	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_STOP :
		utils_system("start-stop-daemon -K -n pppoe-server");
		if (flag == RC_ACT_STOP) {
			ret = EXIT_SUCCESS;
			break;
		}

		/* RC_ACT_RESTART fall through */
	case RC_ACT_START :
		rc = utils_nvram_cmp(NVRAM_SERVICE_OPTION(RC, PPPOE_SERVER_ENABLE), "1");
		if (rc < 0) {
			return (EXIT_FAILURE);
		}

		/* get interface name */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(PPPOE_SERVER, IFNAME), ifname, sizeof(ifname));
		if (rc < 0) {
			return (EXIT_FAILURE);
		}

		/* get local IP address */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(PPPOE_SERVER, LOCAL_IPADDR), lipaddr, sizeof(lipaddr));
		if (rc < 0) {
			return (EXIT_FAILURE);
		}

		/* get remote IP address */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(PPPOE_SERVER, REMOTE_IPADDR), ripaddr, sizeof(ripaddr));
		if (rc < 0) {
			return (EXIT_FAILURE);
		}

		/* get concurrent session number */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(PPPOE_SERVER, SESSION_NUMBER), session_num, sizeof(session_num));
		if (rc < 0) {
			return (EXIT_FAILURE);
		}

		/* mkdir for /etc/ppp */
		if ((stat("/etc/ppp", &stat_buf) != 0) ||
		    (S_ISDIR(stat_buf.st_mode) == 0)) {
			snprintf(buf, sizeof(buf), "%s -rf /etc/ppp", CMD_RM);
			utils_system(buf);
			mkdir("/etc/ppp", 0755);
		}

		pop_etc_ppp_pppoe_server_options(RC_ACT_START);

		snprintf(buf, sizeof(buf), "start-stop-daemon -S -n pppoe-server -a %s -- -I %s -L %s -R %s -N %s",
			CMD_PPPOE_SERVER,
			ifname, lipaddr, ripaddr, session_num);
		utils_system(buf);

		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}
	return (ret);
}
