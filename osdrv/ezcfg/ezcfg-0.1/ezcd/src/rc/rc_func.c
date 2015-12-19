/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_func.c
 *
 * Description  : ezbox rc service functions program
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

rc_func_t rc_functions[] = {
	{ "debug", RC_BOOT, rc_debug,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
	{ "base_files", RC_BOOT, rc_base_files,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
	{ "base_ldconfig", RC_BOOT, rc_ldconfig, 
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
	{ "ezcd", RC_BOOT, rc_ezcd,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
	{ EZCFG_RC_SERVICE_EZCFG_HTTPD, RC_BOOT, rc_ezcfg_httpd,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
	{ "nvram", RC_BOOT, rc_nvram,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
	{ "ezcm", RC_BOOT, rc_ezcm, 
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
#if 0
	{ "hotplug2", RC_BOOT, rc_hotplug2,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
#endif
	{ "mdev", RC_BOOT, rc_mdev,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
	{ "init", RC_BOOT, rc_init,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
	{ "load_modules", RC_BOOT, rc_load_modules,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
	{ "mount_system_fs", RC_BOOT, rc_mount_system_fs,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
#if (HAVE_EZBOX_SERVICE_SYSLOGD == 1)
	{ "syslogd", RC_BOOT, rc_syslogd,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
#endif
#if (HAVE_EZBOX_SERVICE_KLOGD == 1)
	{ "klogd", RC_BOOT, rc_klogd,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
#endif
	{ EZCFG_RC_SERVICE_LOGIN, RC_BOOT, rc_login,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
	{ "loopback", RC_BOOT, rc_loopback,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
	{ "netbase", RC_BOOT, rc_netbase,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
	{ EZCFG_RC_SERVICE_SYSTEM, RC_BOOT, rc_system,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
#if (HAVE_EZBOX_LAN_NIC == 1)
	{ "lan", RC_BOOT, rc_lan,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
	{ "lan_if", RC_BOOT, rc_lan_if,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
#endif
#if (HAVE_EZBOX_WLAN_NIC == 1)
	{ "wlan_if", RC_BOOT, rc_wlan_if,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
#if (HAVE_EZBOX_SERVICE_WPA_SUPPLICANT == 1)
	{ "wpa_supplicant", RC_BOOT, rc_wpa_supplicant,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
#endif
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
	{ "wan", RC_BOOT, rc_wan,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
	{ "wan_if", RC_BOOT, rc_wan_if,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
#endif
#if (HAVE_EZBOX_SERVICE_TELNETD == 1)
	{ EZCFG_RC_SERVICE_TELNETD, RC_BOOT, rc_telnetd,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
#endif
#if (HAVE_EZBOX_SERVICE_DNSMASQ == 1)
	{ "dnsmasq", RC_BOOT, rc_dnsmasq,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
#endif
#if (HAVE_EZBOX_SERVICE_FONTCONFIG == 1)
	{ "fontconfig", RC_BOOT, rc_fontconfig,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
#endif
#if (HAVE_EZBOX_SERVICE_NANO_X == 1)
	{ "nano_x", RC_BOOT, rc_nano_x,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
#elif (HAVE_EZBOX_SERVICE_KDRIVE == 1)
	{ "kdrive", RC_BOOT, rc_kdrive,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
#endif
#if (HAVE_EZBOX_SERVICE_DILLO == 1)
	{ "dillo", RC_BOOT, rc_dillo,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
#endif
#if (HAVE_EZBOX_SERVICE_EMC2 == 1)
	{ "realtime", RC_BOOT, rc_realtime,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
	{ EZCFG_RC_SERVICE_EMC2, RC_BOOT, rc_emc2,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
	{ EZCFG_RC_SERVICE_EMC2_LATENCY_TEST, RC_BOOT, rc_emc2_latency_test,
		RC_RUN_LEVEL(99,99,99),
		RC_RUN_LEVEL(1,1,1),
		NULL },
#endif
};

rc_func_t *utils_find_rc_func(char *name)
{
	int i;
	rc_func_t *f;

	for (i = 0; i < ARRAY_SIZE(rc_functions); i++) {
		f = &rc_functions[i];
		if (strcmp(f->name, name) == 0) return f;
	}

	return NULL;
}

int utils_get_rc_funcs_number(void)
{
	return (ARRAY_SIZE(rc_functions) - 1);
}

char *utils_get_rc_func_name(int i)
{
	if ((i < 1) || (i >= ARRAY_SIZE(rc_functions))) {
		return NULL;
	}
	else {
		return rc_functions[i].name;
	}
}
