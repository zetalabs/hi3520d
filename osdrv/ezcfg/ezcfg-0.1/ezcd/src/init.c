/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : init.c
 *
 * Description  : ezbox initramfs init program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-10-29   0.1       Write it from scratch
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
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/reboot.h>

#include "ezcd.h"

#ifndef RB_HALT_SYSTEM
#  define RB_HALT_SYSTEM  0xcdef0123
#  define RB_POWER_OFF    0x4321fedc
#  define RB_AUTOBOOT     0x01234567
#endif

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

static void init_reap(int sig)
{
	pid_t pid;
	while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
		DBG("<6>init: reaped %d\n", pid);
	}
}

static void init_halt_reboot_poweroff(int sig)
{
	char *p;
	void *handle;
	union {
		rc_function_t func;
		void * obj;
	} alias;
	char *stop_argv[] = { "agent", "env", "stop", NULL };
	sigset_t set;
	pid_t pid;
	unsigned rb;

	/* reset signal handlers */
	signal(SIGUSR1, SIG_DFL);
	signal(SIGTERM, SIG_DFL);
	signal(SIGUSR2, SIG_DFL);
	sigfillset(&set);
	sigprocmask(SIG_UNBLOCK, &set, NULL);

	/* run agent environment stop processes */
	handle = dlopen("/lib/rcso/rc_agent.so", RTLD_NOW);
	if (handle == NULL) {
		DBG("<6>init: dlopen(%s) error %s\n", "/lib/rcso/rc_agent.so", dlerror());
		return;
	}

	/* clear any existing error */
	dlerror();

	alias.obj = dlsym(handle, "rc_agent");

	if ((p = dlerror()) != NULL)  {
		DBG("<6>init: dlsym error %s\n", p);
		dlclose(handle);
		return;
	}

	alias.func(ARRAY_SIZE(stop_argv) - 1, stop_argv);

	/* close loader handle */
	dlclose(handle);

        /* send signals to every process _except_ pid 1 */
	kill(-1, SIGTERM);
	sync();
	sleep(1);

	kill(-1, SIGKILL);
	sync();
	sleep(1);

	p = "halt";
	rb = RB_HALT_SYSTEM;
	if (sig == SIGTERM) {
		p = "reboot";
		rb = RB_AUTOBOOT;
	} else if (sig == SIGUSR2) {
		p = "poweroff";
		rb = RB_POWER_OFF;
	}
        DBG("<6> init: Requesting system %s", p);
	pid = vfork();
	if (pid == 0) { /* child */
		reboot(rb);
		_exit(EXIT_SUCCESS);
	}
	while (1)
		sleep(1);

	/* should never reach here */
}

int init_main(int argc, char **argv)
{
	char *p;
	void *handle;
	union {
		rc_function_t func;
		void * obj;
	} alias;
	char *start_argv[] = { "agent", "env", "start", NULL };
	sigset_t sigset;

	/* unset umask */
	umask(0);

	/* make the command line just say "init"  - thats all, nothing else */
	strncpy(argv[0], "init", strlen(argv[0]));
	/* wipe argv[1]-argv[N] so they don't clutter the ps listing */
	while (*++argv)
		memset(*argv, 0, strlen(*argv));

	signal(SIGCHLD, init_reap);
	signal(SIGUSR1, init_halt_reboot_poweroff);
	signal(SIGTERM, init_halt_reboot_poweroff);
	signal(SIGUSR2, init_halt_reboot_poweroff);

	sigemptyset(&sigset);

	if (utils_boot_partition_is_ready() == false) {
		DBG("<6>init: utils_boot_partition_is_ready() == false!\n");
		start_argv[2] = "bootstrap";
	}

	/* run agent environment start processes */
	handle = dlopen("/lib/rcso/rc_agent.so", RTLD_NOW);
	if (!handle) {
		DBG("<6>init: dlopen(%s) error %s\n", "/lib/rcso/rc_agent.so", dlerror());
		return (EXIT_FAILURE);
	}

	/* clear any existing error */
	dlerror();

	alias.obj = dlsym(handle, "rc_agent");

	if ((p = dlerror()) != NULL)  {
		DBG("<6>init: dlsym error %s\n", p);
		dlclose(handle);
		return (EXIT_FAILURE);
	}

	alias.func(ARRAY_SIZE(start_argv) - 1, start_argv);

	/* close loader handle */
	dlclose(handle);

	/* run main loop forever */
	while (1) {
		sigsuspend(&sigset);
	}

	/* should never run to this place!!! */
	return (EXIT_FAILURE);
}
