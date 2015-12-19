/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : preinit.c
 *
 * Description  : ezbox initramfs preinit program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-06-13   0.1       Write it from scratch
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
#include <dlfcn.h>

#include "ezcd.h"

#if 0
#define DBG(format, args...) do {\
	FILE *dbg_fp = fopen("/dev/kmsg", "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

int preinit_main(int argc, char **argv)
{
	char *p;
	void *handle;
	union {
		rc_function_t func;
		void * obj;
	} alias;
	char *boot_argv[] = { "agent", "env", "boot", NULL };

	/* unset umask */
	umask(0);

	/* run agent environment boot processes */
	handle = dlopen("/lib/rcso/rc_agent.so", RTLD_NOW);
	if (handle == NULL) {
		DBG("<6>preinit: dlopen(%s) error %s\n", "/lib/rcso/rc_agent.so", dlerror());
		return (EXIT_FAILURE);
	}

	/* clear any existing error */
	dlerror();

	alias.obj = dlsym(handle, "rc_agent");

	if ((p = dlerror()) != NULL)  {
		DBG("<6>preinit: dlsym error %s\n", p);
		dlclose(handle);
		return (EXIT_FAILURE);
	}

	alias.func(ARRAY_SIZE(boot_argv) - 1, boot_argv);

	/* close loader handle */
	dlclose(handle);

	/* run init */
	/* if cmdline has root= switch_root to new root device */
	init_main(argc, argv);

	return (EXIT_SUCCESS);
}
