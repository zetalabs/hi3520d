/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_cgconfig_conf.c
 *
 * Description  : ezbox /etc/cgconfig.conf file generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2013-03-19   0.1       Write it from scratch
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

static int generate_cgroup_mount_points(FILE *file)
{
#if 0
mount {
        cpu = /sys/fs/cgroup/cpu;
        cpuacct = /sys/fs/cgroup/cpuacct;
        cpuset = /sys/fs/cgroup/cpuset;
        devices = /sys/fs/cgroup/devices;
        memory = /sys/fs/cgroup/memory;
        freezer = /sys/fs/cgroup/freezer;
}
#endif
	fprintf(file, "%s\n", "mount {");

	/* cpu = /sys/fs/cgroup/cpu; */
	fprintf(file, "\t%s = %s;\n",
		"cpu",
		"/sys/fs/cgroup/cpu");

	/* cpuacct = /sys/fs/cgroup/cpuacct; */
	fprintf(file, "\t%s = %s;\n",
		"cpuacct",
		"/sys/fs/cgroup/cpuacct");

	/* cpuset = /sys/fs/cgroup/cpuset; */
	fprintf(file, "\t%s = %s;\n",
		"cpuset",
		"/sys/fs/cgroup/cpuset");

	/* blkio = /sys/fs/cgroup/blkio; */
	fprintf(file, "\t%s = %s;\n",
		"blkio",
		"/sys/fs/cgroup/blkio");

	/* debug = /sys/fs/cgroup/debug; */
	fprintf(file, "\t%s = %s;\n",
		"debug",
		"/sys/fs/cgroup/debug");

	/* devices = /sys/fs/cgroup/devices; */
	fprintf(file, "\t%s = %s;\n",
		"devices",
		"/sys/fs/cgroup/devices");

	/* freezer = /sys/fs/cgroup/freezer; */
	fprintf(file, "\t%s = %s;\n",
		"freezer",
		"/sys/fs/cgroup/freezer");

	/* net_cls = /sys/fs/cgroup/net_cls; */
	fprintf(file, "\t%s = %s;\n",
		"net_cls",
		"/sys/fs/cgroup/net_cls");

	/* net_prio = /sys/fs/cgroup/net_prio; */
	fprintf(file, "\t%s = %s;\n",
		"net_prio",
		"/sys/fs/cgroup/net_prio");

	/* perf_event = /sys/fs/cgroup/perf_event; */
	fprintf(file, "\t%s = %s;\n",
		"perf_event",
		"/sys/fs/cgroup/perf_event");

	fprintf(file, "%s\n", "}");

	return (EXIT_SUCCESS);
}

int pop_etc_cgconfig_conf(int flag)
{
        FILE *file = NULL;

	/* generate /etc/cgconfig.conf */
	file = fopen("/etc/cgconfig.conf", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	generate_cgroup_mount_points(file);

	switch (flag) {
	case RC_ACT_BOOT :
		break;

	case RC_ACT_RESTART :
	case RC_ACT_START :
		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
