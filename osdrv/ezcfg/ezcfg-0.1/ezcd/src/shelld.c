/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : shelld.c
 *
 * Description  : ezbox console shell daemon program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-04   0.1       Write it from scratch
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

static struct action_s {
	pid_t pid;
	char *terminal;
	char type;
	char *command;
} shell_actions[] = {
#if 0
	{ 0, "/dev/tts/0", 'a', CMD_SH },
	{ 0, "/dev/ttyS0", 'a', CMD_SH },
	{ 0, "/dev/ttyS1", 'a', CMD_SH },
	{ 0, "/dev/tty1", 'a', CMD_SH },
	{ 0, "/dev/tty2", 'a', CMD_SH },
	{ 0, "/dev/tty3", 'a', CMD_SH },
	{ 0, "/dev/tty4", 'a', CMD_SH },
	{ 0, "/dev/tty5", 'a', CMD_SH },
	{ 0, "/dev/tty6", 'a', CMD_SH },
#else
	{ 0, "/dev/tts/0", 'a', CMD_LOGIN },
	{ 0, "/dev/ttyS0", 'a', CMD_LOGIN },
	{ 0, "/dev/ttyS1", 'a', CMD_LOGIN },
	{ 0, "/dev/tty1", 'a', CMD_LOGIN },
	{ 0, "/dev/tty2", 'a', CMD_LOGIN },
	{ 0, "/dev/tty3", 'a', CMD_LOGIN },
	{ 0, "/dev/tty4", 'a', CMD_LOGIN },
	{ 0, "/dev/tty5", 'a', CMD_LOGIN },
	{ 0, "/dev/tty6", 'a', CMD_LOGIN },
#endif
	{ 0, NULL, 0, NULL }
};

/* set terminal settings to reasonable defaults */
static void set_term(int fd)
{
	struct termios tty;

	tcgetattr(fd, &tty);

	/* set control chars */
	tty.c_cc[VINTR]  = 3;   /* C-c */
	tty.c_cc[VQUIT]  = 28;  /* C-\ */
	tty.c_cc[VERASE] = 127; /* C-? */
	tty.c_cc[VKILL]  = 21;  /* C-u */
	tty.c_cc[VEOF]   = 4;   /* C-d */
	tty.c_cc[VSTART] = 17;  /* C-q */
	tty.c_cc[VSTOP]  = 19;  /* C-s */
	tty.c_cc[VSUSP]  = 26;  /* C-z */

	/* use line dicipline 0 */
	tty.c_line = 0;

	/* make it be sane */
	tty.c_cflag &= CBAUD|CBAUDEX|CSIZE|CSTOPB|PARENB|PARODD;
	tty.c_cflag |= CREAD|HUPCL|CLOCAL;

	/* input modes */
	tty.c_iflag = ICRNL | IXON | IXOFF;

	/* output modes */
	tty.c_oflag = OPOST | ONLCR;

	/* local modes */
	tty.c_lflag =
		ISIG | ICANON | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE | IEXTEN;

	tcsetattr(fd, TCSANOW, &tty);
}

/* open the new terminal device */
static int open_stdio_to_tty(const char* tty_name)
{
	/* empty tty_name means "use init's tty", else... */
	if (tty_name[0]) {
		int fd;

		close(STDIN_FILENO);
		/* fd can be only < 0 or 0: */
		fd = utils_device_open(tty_name, O_RDWR);
		if (fd) {
			DBG("<6>shelld: can't open %s: %s",
				tty_name, strerror(errno));
			return 0; /* failure */
		}
		dup2(STDIN_FILENO, STDOUT_FILENO);
		dup2(STDIN_FILENO, STDERR_FILENO);
	}
	set_term(STDIN_FILENO);
	return 1; /* success */
}

static pid_t do_command(struct action_s *a)
{
	pid_t pid;
	char buf[RC_COMMAND_LINE_SIZE];
	char *argv[RC_MAX_ARGS];
	sigset_t set;

	sigfillset(&set);
	sigprocmask(SIG_BLOCK, &set, NULL);

	pid = fork();
	if (pid < 0) {
		DBG("<6>shelld do_command: can't fork");
	}
	if (pid) {
		sigfillset(&set);
		sigprocmask(SIG_UNBLOCK, &set, NULL);
		return pid; /* parent or error */
	}

	/* child */
	/* reset signal handlers that were set by the parent process */
	signal(SIGCHLD, SIG_DFL);
	signal(SIGALRM, SIG_DFL);
	signal(SIGUSR1, SIG_DFL);
	sigfillset(&set);
	sigprocmask(SIG_UNBLOCK, &set, NULL);

	/* create a new session and make ourself the process group leader */
	setsid();

	/* open the new terminal device */
	if (!open_stdio_to_tty(a->terminal)) {
		_exit(EXIT_FAILURE);
	}

	/* askfirst type */
	if (a->type == 'a') {
		static const char press_enter[] =
			"\nPlease press Enter to activate this console. \n";
		char c;
		DBG("waiting for enter to start '%s'"
			"(pid %d, tty '%s')\n",
			a->command, getpid(), a->terminal);
		utils_full_write(STDOUT_FILENO, press_enter, sizeof(press_enter) - 1);
		while (utils_safe_read(STDIN_FILENO, &c, 1) == 1 && c != '\n')
			continue;
	}

	snprintf(buf, sizeof(buf), "%s", a->command);
	if (utils_parse_args(buf, strlen(buf) + 1, argv) > 0) {
		execvp(argv[0], argv);
	}
	_exit(-1);
}

static void run_shell(struct action_s *actions)
{
	struct action_s *a;
	for (a = actions; a->command != NULL; a++) {
		DBG("<6>shelld: pid[%d], term[%s], cmd[%s]\n",
			a->pid, a->terminal, a->command);
		if (a->pid == 0 || (a->pid > 0 && kill(a->pid, 0) != 0)) {
			a->pid = do_command(a);
		}
	}
}

static int check_first_run(struct action_s *actions)
{
	struct action_s *a;

	for (a = actions; a->command != NULL; a++) {
		if (a->pid > 0 && kill(a->pid, 0) != 0) {
			a->pid = -1;
		}
		if (a->pid == 0)
			return 0;
	}
	return 1;
}

static void shelld_reap(int sig)
{
	pid_t pid;

	while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
		DBG("<6>shelld: reaped %d\n", pid);
	}
}

static void shelld_alarm(int sig)
{
	DBG("<6>shelld: alarmed\n");
}

static void shelld_cleanup(int sig)
{
	struct action_s *a;

	DBG("<6>shelld: cleanup\n");
	for (a = shell_actions; a->command != NULL; a++) {
		if (a->pid < 0)
			a->pid = 0;
	}
}

int shelld_main(int argc, char **argv)
{
	pid_t pid;
	sigset_t sigset;
	int first_run_ok;

	pid = fork();
	if (pid < 0) {
		DBG("<6>shelld: can't fork");
		return (EXIT_FAILURE);
	}
	if (pid > 0) {
		return (EXIT_SUCCESS); /* parent */
	}

	/* child */
	/* unset umask */
	umask(0);

	signal(SIGCHLD, shelld_reap);
	signal(SIGALRM, shelld_alarm);
	signal(SIGUSR1, shelld_cleanup);

	sigemptyset(&sigset);

	/* run main loop forever */
	while (1) {
		first_run_ok = 0;
		/* run shell actions */
		run_shell(shell_actions);
		while(first_run_ok == 0) {
			alarm(1);
			sigsuspend(&sigset);
			first_run_ok = check_first_run(shell_actions);
		}
		sigsuspend(&sigset);
		sleep(1);
	}

	/* should never run to this place!!! */
	return (EXIT_FAILURE);
}
