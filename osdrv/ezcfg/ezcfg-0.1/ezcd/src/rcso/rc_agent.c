/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_agent.c
 *
 * Description  : implement running action command rcso
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-12-21   0.1       Write it from scratch
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

static int call_action_function(int argc, char **argv, char *prefix, char *agent)
{
	char path[128];
	char name[64];
	void *handle;
	union {
		action_func_t func;
		void * obj;
	} alias;
	char *error;
	int ret = EXIT_FAILURE;

	/* process rc function */
	snprintf(name, sizeof(name), "%s_action_%s", agent, argv[0]);
	snprintf(path, sizeof(path), "%s/%s/rcso/%s.so", prefix, agent, name);
	handle = dlopen(path, RTLD_NOW);
	if (handle == NULL) {
		goto func_exit;
	}

	/* clear any existing error */
	dlerror();

	alias.obj = dlsym(handle, name);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto func_exit;
	}

	/* handle rc operations */
	ret = alias.func(argc, argv);

func_exit:
	/* close loader handle */
	if (handle != NULL) {
		dlclose(handle);
	}
	return (ret);
}

/*
 * usage: rc agent [agent name] [action name]
 */
#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_agent(int argc, char **argv)
#endif
{
	char buf[RC_COMMAND_LINE_SIZE];
	char *prefix;
	char path[128];
	int fargc;
	char *fargv[RC_MAX_ARGS];
	FILE *fp = NULL;
	int ret = EXIT_FAILURE;

	if (argc < 3) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "agent")) {
		return (EXIT_FAILURE);
	}

	if (!isupper(argv[1][0]) && !islower(argv[1][0])) {
		return (EXIT_FAILURE);
	}

	if (!isupper(argv[2][0]) && !islower(argv[2][0])) {
		return (EXIT_FAILURE);
	}

	/* check first place */
	prefix = AGENTS_PATH_PREFIX;
	snprintf(path, sizeof(path), "%s/%s/action/%s", prefix, argv[1], argv[2]);
	fp = fopen(path, "r");
	if (fp == NULL) {
		/* check second place */
		prefix = AGENTS_PATH_PREFIX2;
		snprintf(path, sizeof(path), "%s/%s/action/%s", prefix, argv[1], argv[2]);
		if (fp == NULL) {
			return (EXIT_FAILURE);
		}
	}

	DBG("<6> agent: action file=[%s]\n", path);
	/* read action file */
	while (utils_file_get_line(fp, buf, sizeof(buf), "#", LINE_TAIL_STRING) == true) {
		fargc = utils_parse_args(buf, strlen(buf) + 1, fargv);
		if (fargc > 0) {
			DBG("<6> action: %s (%s) start!\n", fargv[0], fargv[1]);
			ret = call_action_function(fargc, fargv, prefix, argv[1]);
			if (ret == EXIT_FAILURE) {
				DBG("<6> action: %s error!\n", fargv[0]);
			}
		}
	}

	fclose(fp);
	return (ret);
}
