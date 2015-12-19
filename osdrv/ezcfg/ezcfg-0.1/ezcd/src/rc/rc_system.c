/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_system.c
 *
 * Description  : ezbox run system services
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

#include "ezcd.h"
#include "rc_func.h"
#include "utils.h"

#if 0
#define DBG(format, args...) do {\
	FILE *fp = fopen("/dev/kmsg", "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

#define DBG2() do {\
	pid_t pid = getpid(); \
	FILE *fp = fopen("/dev/kmsg", "a"); \
	if (fp) { \
		char buf[32]; \
		FILE *fp2; \
		int i; \
		for(i=pid; i<pid+30; i++) { \
			snprintf(buf, sizeof(buf), "/proc/%d/stat", i); \
			fp2 = fopen(buf, "r"); \
			if (fp2) { \
				if (fgets(buf, sizeof(buf)-1, fp2) != NULL) { \
					fprintf(fp, "pid=[%d] buf=%s\n", i, buf); \
				} \
				fclose(fp2); \
			} \
		} \
		fclose(fp); \
	} \
} while(0)

int rc_system(int flag)
{
	if(geteuid() != 0)
	{
		DBG("run rc_system euid is not 0!\n");
		return (EXIT_FAILURE);
	}

	switch (flag) {
	case RC_BOOT :
		/* prepare boot device and basic infrastructure for the system */
		rc_preboot(RC_BOOT);

		/* load preinit kernel modules */
		rc_load_modules(RC_BOOT);

		/* preinit mount system supported fs */
		rc_mount_system_fs(RC_BOOT);

		/* prepare dynamic data storage path */
		rc_data_storage(RC_BOOT);

		/* start ezcfg daemon */
		rc_ezcd(RC_BOOT);

		/* setup nvram config file */
		rc_nvram(RC_BOOT);

		/* setup ezcm config file */
		rc_ezcm(RC_BOOT);

		/* setup network base files */
		rc_netbase(RC_BOOT);

		/* setup iptables */
#if (HAVE_EZBOX_SERVICE_IPTABLES == 1)
		rc_iptables(RC_BOOT);
#endif

		break;

	case RC_RESTART :
	case RC_STOP :
		/* run in root HOME path */
		setenv("HOME", ROOT_HOME_PATH, 1);
		chdir(ROOT_HOME_PATH);

#if (HAVE_EZBOX_SERVICE_DILLO == 1)
		rc_dillo(RC_STOP);
#endif

#if (HAVE_EZBOX_SERVICE_NANO_X == 1)
		rc_nano_x(RC_STOP);
#elif (HAVE_EZBOX_SERVICE_KDRIVE == 1)
		rc_kdrive(RC_STOP);
#endif

		/* FIXME: We do it in wan interface startup script */
#if 0
		/* stop WAN interface binding services */
		rc_wan_services(RC_STOP);
#endif

		/* bring down WAN interface */
#if (HAVE_EZBOX_WAN_NIC == 1)
		rc_wan(RC_STOP);
#endif

		/* stop LAN interface binding services */
#if (HAVE_EZBOX_LAN_NIC == 1)
		rc_lan_services(RC_STOP);
#endif

		/* bring down LAN interface */
#if (HAVE_EZBOX_LAN_NIC == 1)
		rc_lan(RC_STOP);
#endif

		/* stop klog service */
#if (HAVE_EZBOX_SERVICE_KLOGD == 1)
		rc_klogd(RC_STOP);
#endif

		/* stop syslog service */
#if (HAVE_EZBOX_SERVICE_SYSLOGD == 1)
		rc_syslogd(RC_STOP);
#endif

		/* setup ezcm config file */
		rc_ezcm(RC_STOP);

		/* setup nvram config file */
		rc_nvram(RC_STOP);

		/* stop ezcfg daemon */
		rc_ezcd(RC_STOP);

		break;

	case RC_START :
		/* run in root HOME path */
		setenv("HOME", ROOT_HOME_PATH, 1);
		chdir(ROOT_HOME_PATH);

		/* restart ezcfg daemon */
		rc_ezcd(RC_RELOAD);

		/* re-generate nvram config file */
		rc_nvram(RC_RELOAD);

		/* re-generate ezcm config file */
		rc_ezcm(RC_RELOAD);

		/* start syslog service */
#if (HAVE_EZBOX_SERVICE_SYSLOGD == 1)
		rc_syslogd(RC_START);
#endif

		/* start klog service */
#if (HAVE_EZBOX_SERVICE_KLOGD == 1)
		rc_klogd(RC_START);
#endif

		/* re-generate /dev/ node */
		rc_mdev(RC_RESTART);

		/* re-generate network base files */
		rc_netbase(RC_RESTART);

		/* load kernel modules */
		rc_load_modules(RC_START);

		/* mount system supported fs */
		rc_mount_system_fs(RC_START);

		/* load iptables/netfileter kernel modules */
#if (HAVE_EZBOX_SERVICE_IPTABLES == 1)
		rc_iptables(RC_START);
#endif

		/* misc files for the base system */
		rc_base_files(RC_START);

		/* build /etc/passwd & /etc/group, 
		 * also setup root password
		 */
		rc_login(RC_START);

		/* bring up loopback interface */
		rc_loopback(RC_START);

		/* bring up LAN interface link up but not configurate it */
#if (HAVE_EZBOX_LAN_NIC == 1)
		rc_lan_if(RC_START);
#endif

		/* bring up WLAN interface link up but not configurate it */
#if (HAVE_EZBOX_WLAN_NIC == 1)
		rc_wlan_if(RC_START);
#endif

		/* bring up WAN interface link up but not configurate it */
		/* LAN interface alias need it */
#if (HAVE_EZBOX_WAN_NIC == 1)
		rc_wan_if(RC_START);
#endif

		/* bring up LAN interface link up and configurate it */
#if (HAVE_EZBOX_LAN_NIC == 1)
		rc_lan(RC_START);
#endif

		/* start LAN interface binding services */
#if (HAVE_EZBOX_LAN_NIC == 1)
		rc_lan_services(RC_START);
#endif

		/* bring up WAN interface link up and configurate it */
#if (HAVE_EZBOX_WAN_NIC == 1)
		rc_wan(RC_START);
#endif

		/* FIXME: We do it in wan interface startup script */
#if 0
		if (utils_wan_interface_is_up() == true) {
			/* start WAN interface binding services */
			rc_wan_services(RC_START);
		}
#endif

#if (HAVE_EZBOX_SERVICE_FONTCONFIG == 1)
		rc_fontconfig(RC_START);
#endif

#if (HAVE_EZBOX_SERVICE_NANO_X == 1)
		rc_nano_x(RC_START);
#elif (HAVE_EZBOX_SERVICE_KDRIVE == 1)
		rc_kdrive(RC_START);
#endif

#if (HAVE_EZBOX_SERVICE_DILLO == 1)
		rc_dillo(RC_START);
#endif

#if (HAVE_EZBOX_SERVICE_EMC2 == 1)
		rc_emc2(RC_START);
#endif

		break;
	}

	return (EXIT_SUCCESS);
}
