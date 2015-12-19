/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : ezcd.c
 *
 * Description  : ezbox config daemon main program
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
#include <sys/poll.h>

#include "ezcd.h"

#define EZCD_PRIORITY                  -4
#define EZCI_PRIORITY                  -2

#define handle_error_en(en, msg) \
	do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

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

#define INFO(format, args...) do {\
	FILE *info_fp = fopen("/dev/kmsg", "a"); \
	if (info_fp) { \
		fprintf(info_fp, format, ## args); \
		fclose(info_fp); \
	} \
} while(0)

static bool debug = false;
static int rc = EXIT_FAILURE;
static pthread_t root_thread;
static struct ezcfg_master *master = NULL;

static void ezcd_show_usage(void)
{
	printf("Usage: ezcd [-d] [-D] [-t max_worker_threads]\n");
	printf("\n");
	printf("  -d\tdaemonize\n");
	printf("  -D\tdebug mode\n");
	printf("  -t\tmax worker threads\n");
	printf("  -h\thelp\n");
	printf("\n");
}

#if 0
static int mem_size_mb(void)
{
	FILE *fp;
	char buf[1024];
	long int memsize = -1;

	fp = fopen("/proc/meminfo", "r");
	if (fp == NULL)
		return -1;

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		long int value;

		if (sscanf(buf, "MemTotal: %ld kB", &value) == 1) {
			memsize = value / 1024;
			break;
		}
	}

	fclose(fp);
	return memsize;
}
#endif

static void *sig_thread_routine(void *arg)
{
	sigset_t *set = (sigset_t *) arg;
	int s, sig;

	for (;;) {
		s = sigwait(set, &sig);
		if (s != 0) {
			DBG("<6>ezcd: sigwait errno = [%d]\n", s);
			continue;
		}
		DBG("<6>ezcd: Signal handling thread got signal %d\n", sig);
		switch(sig) {
		case SIGTERM :
			ezcfg_api_master_stop(master);
			rc = EXIT_SUCCESS;
			return NULL;
		case SIGUSR1 :
			ezcfg_api_master_reload(master);
			break;
		case SIGCHLD :
			/* do nothing for child exit */
			break;
		default :
			DBG("<6>ezcd: unknown signal [%d]\n", sig);
			break;
		}
	}

	return NULL;
}

int ezcd_main(int argc, char **argv)
{
	struct ezcfg *ezcfg = NULL;
	char *p = NULL;
	bool daemonize = false;
	int fd = -1;
	int threads_max = 0;
	int s;
	pthread_t sig_thread;
	sigset_t sigset;

	for (;;) {
		int c;
		c = getopt( argc, argv, "t:dDh");
		if (c == EOF) break;
		switch (c) {
			case 't':
				threads_max = atoi(optarg);
				break;
			case 'd':
				daemonize = true;
				break;
			case 'D':
				debug = true;
				break;
			case 'h':
			default:
				ezcd_show_usage();
				return (EXIT_FAILURE);
		}
        }

	if (getuid() != 0) {
		fprintf(stderr, "root privileges required\n");
		exit(EXIT_FAILURE);
	}

	/* set umask before creating any file/directory */
	s = chdir("/");
	umask(0022);

	ezcfg = ezcfg_api_common_new(EZCD_CONFIG_FILE_PATH);
	if (ezcfg == NULL) {
		fprintf(stderr, "%s format error.\n", EZCD_CONFIG_FILE_PATH);
		exit(EXIT_FAILURE);
	}

	p = ezcfg_api_common_get_root_path(ezcfg);
	if ((p != NULL) && (*p == '/')) {
		utils_mkdir(p, 0777, true);
	}

	/* setup semaphore path */
	p = ezcfg_api_common_get_sem_ezcfg_path(ezcfg);
	if ((p != NULL) && (*p == '/')) {
		utils_mkdir(p, 0777, false);
		fd = open(p, O_CREAT|O_RDWR, S_IRWXU);
		if (fd < 0) {
			fprintf(stderr, "cannot open %s\n", p);
			ezcfg_api_common_delete(ezcfg);
			exit(EXIT_FAILURE);
		}
		close(fd);
	}
	else {
		fprintf(stderr, "the path=[%s] of %s is incorrect\n", p, "semaphore for ezcfg");
		ezcfg_api_common_delete(ezcfg);
		exit(EXIT_FAILURE);
	}

	/* setup shared memory path */
	p = ezcfg_api_common_get_shm_ezcfg_path(ezcfg);
	if ((p != NULL) && (*p == '/')) {
		utils_mkdir(p, 0777, false);
		fd = open(p, O_CREAT|O_RDWR, S_IRWXU);
		if (fd < 0) {
			fprintf(stderr, "cannot open %s\n", p);
			ezcfg_api_common_delete(ezcfg);
			exit(EXIT_FAILURE);
		}
		close(fd);
	}
	else {
		fprintf(stderr, "the path=[%s] of %s is incorrect\n", p, "shared memory for ezcfg");
		ezcfg_api_common_delete(ezcfg);
		exit(EXIT_FAILURE);
	}

#if (HAVE_EZBOX_SERVICE_EZCTP == 1)
	p = ezcfg_api_common_get_shm_ezctp_path(ezcfg);
	if ((p != NULL) && (*p == '/')) {
		utils_mkdir(p, 0777, false);
		fd = open(p, O_CREAT|O_RDWR, S_IRWXU);
		if (fd < 0) {
			fprintf(stderr, "cannot open %s\n", p);
			ezcfg_api_common_delete(ezcfg);
			exit(EXIT_FAILURE);
		}
		close(fd);
	}
	else {
		fprintf(stderr, "the path=[%s] of %s is incorrect\n", p, "shared memory for ezctp");
		ezcfg_api_common_delete(ezcfg);
		exit(EXIT_FAILURE);
	}
#endif

	ezcfg_api_common_delete(ezcfg);
	ezcfg = NULL;

	/* before opening new files, make sure std{in,out,err} fds are in a sane state */
	fd = open("/dev/null", O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "cannot open /dev/null\n");
		exit(EXIT_FAILURE);
	}
	if (write(STDOUT_FILENO, 0, 0) < 0)
		dup2(fd, STDOUT_FILENO);
	if (write(STDERR_FILENO, 0, 0) < 0)
		dup2(fd, STDERR_FILENO);

	if (!debug) {
		dup2(fd, STDIN_FILENO);
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);
	}
	if (fd > STDERR_FILENO)
		close(fd);

	if (daemonize)
	{
		pid_t pid = fork();
		switch (pid) {
		case 0:
			/* child process */
			break;

		case -1:
			/* error */
			return (EXIT_FAILURE);

		default:
			/* parant process */
			return (EXIT_SUCCESS);
		}
	}

        /* set scheduling priority for the main daemon process */
	setpriority(PRIO_PROCESS, 0, EZCD_PRIORITY);

	setsid();

	/* main process */
	INFO("<6>ezcd: booting...\n");
	/* prepare signal handling thread */
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGCHLD);
	sigaddset(&sigset, SIGUSR1);
	sigaddset(&sigset, SIGTERM);
	s = pthread_sigmask(SIG_BLOCK, &sigset, NULL);
	if (s != 0) {
		DBG("<6>ezcd: pthread_sigmask\n");
		handle_error_en(s, "pthread_sigmask");
	}

	/* get root thread id */
	root_thread = pthread_self();

	s = pthread_create(&sig_thread, NULL, &sig_thread_routine, (void *) &sigset);
	if (s != 0) {
		DBG("<6>ezcd: pthread_create\n");
		handle_error_en(s, "pthread_create");
	}

	if (threads_max < EZCFG_THREAD_MIN_NUM) {
		int memsize = utils_get_mem_size_mb();

		/* set value depending on the amount of RAM */
		if (memsize > 0)
			threads_max = EZCFG_THREAD_MIN_NUM + (memsize / 8);
		else
			threads_max = EZCFG_THREAD_MIN_NUM;
	}

	/* prepare master thread */
	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		DBG("<6>ezcd: init ezcfg_api\n");
		return (EXIT_FAILURE);
	};

	master = ezcfg_api_master_start("ezcd", threads_max);
	if (master == NULL) {
		DBG("<6>ezcd: Cannot initialize ezcd master\n");
		return (EXIT_FAILURE);
	}

	INFO("<6>ezcd: starting version " VERSION "\n");

	/* wait for exit signal */
	s = pthread_join(sig_thread, NULL);
	if (s != 0) {
		ezcfg_api_master_stop(master);
		DBG("<6>ezcd: pthread_join\n");
		handle_error_en(s, "pthread_join");
	}

	return (rc);
}
