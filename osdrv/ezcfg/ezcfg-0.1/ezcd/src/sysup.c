/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : main.c
 *
 * Description  : sysup main program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2013-05-02   0.1       Write it from scratch
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
#include <termios.h>
#include <sys/ioctl.h>

#define ROOT_HOME_PATH          "/root"

#ifndef RB_HALT_SYSTEM
#  define RB_HALT_SYSTEM  0xcdef0123
#  define RB_POWER_OFF    0x4321fedc
#  define RB_AUTOBOOT     0x01234567
#endif

#define CMD_SH                  "/bin/sh"

#define RC_MAX_ARGS     16
#define RC_COMMAND_LINE_SIZE    256
#define IS_BLANK(c)     ((c) == ' ' || (c) == '\t')
#define IS_COLON(c)     ((c) == ':')

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

/* try to open up the specified device */
int utils_device_open(const char *device, int mode)
{
	int m, f, fd;

	m = mode | O_NONBLOCK;

	/* retry up to 5 times */
	for (f = 0; f < 5; f++) {
		fd = open(device, m, 0600);
		if (fd >= 0)
			break;
	}
	if (fd < 0)
		return fd;
	/* reset original flags. */
	if (m != mode)
		fcntl(fd, F_SETFL, mode);
	return fd;
}

/* return parsed args number */
int utils_parse_args(char *buf, size_t size, char **argv)
{
	char *p;
	//int i;
	int argc;

	argc = 0;
	//i = 0;
	p = buf;
	while((argc < RC_MAX_ARGS - 1) && (*p != '\0')) {
		/* skip blank char */
		while(IS_BLANK(*p)) {
			*p = '\0';
			p++;
		}
		if (*p == '\0')
			break;

		/* find argv[] start */
		argv[argc] = p;
		argc++;
		p++;
		while(!IS_BLANK(*p) && (*p != '\0')) p++;
	}

	/* check if all string has been parsed */
	if (*p != '\0') {
		return -1;
	}

	/* final argv[] must be NULL */
	argv[argc] = NULL;
	return (argc);
}

ssize_t utils_safe_read(int fd, void *buf, size_t count)
{
	ssize_t n;

	do {
		n = read(fd, buf, count);
	} while (n < 0 && errno == EINTR);

	return n;
}

ssize_t utils_safe_write(int fd, const void *buf, size_t count)
{
	ssize_t n;

	do {
		n = write(fd, buf, count);
	} while (n < 0 && errno == EINTR);

	return n;
}

ssize_t utils_full_write(int fd, const void *buf, size_t len)
{
	ssize_t cc;
	ssize_t total;

	total = 0;

	while (len) {
		cc = utils_safe_write(fd, buf, len);
		if (cc < 0) {
			if (total) {
				return total;
			}
			return cc;
		}

		total += cc;
		buf = ((const char *)buf) + cc;
		len -= cc;
	}

	return total;
}

static struct action_s {
	pid_t pid;
	char *terminal;
	char type;
	char *command;
} shell_actions[] = {
	{ 0, "/dev/tts/0", 'a', CMD_SH },
	{ 0, "/dev/ttyS0", 'a', CMD_SH },
	{ 0, "/dev/ttyS1", 'a', CMD_SH },
	{ 0, "/dev/tty1", 'a', CMD_SH },
	{ 0, "/dev/tty2", 'a', CMD_SH },
	{ 0, "/dev/tty3", 'a', CMD_SH },
	{ 0, "/dev/tty4", 'a', CMD_SH },
	{ 0, "/dev/tty5", 'a', CMD_SH },
	{ 0, "/dev/tty6", 'a', CMD_SH },
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
	sigset_t set;
	pid_t pid;
	unsigned rb;

	/* reset signal handlers */
	signal(SIGUSR1, SIG_DFL);
	signal(SIGTERM, SIG_DFL);
	signal(SIGUSR2, SIG_DFL);
	sigfillset(&set);
	sigprocmask(SIG_UNBLOCK, &set, NULL);

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

	/* run main loop forever */
	while (1) {
		sigsuspend(&sigset);
	}

	/* should never run to this place!!! */
	return (EXIT_FAILURE);
}

int utils_make_preboot_dirs(void)
{
	char buf[32];

	/* /proc */
	mkdir("/proc", 0555);
	mount("proc", "/proc", "proc", MS_MGC_VAL, NULL);

	/* sysfs -> /sys */
	mkdir("/sys", 0755);
	mount("sysfs", "/sys", "sysfs", MS_MGC_VAL, NULL);

	/* /dev */
	mkdir("/dev", 0755);
	mount("devfs", "/dev", "tmpfs", MS_MGC_VAL, NULL);

	/* init shms */
	mkdir("/dev/shm", 0777);

	/* mount /dev/pts */
	mkdir("/dev/pts", 0777);
	mount("devpts", "/dev/pts", "devpts", MS_MGC_VAL, NULL);

	mknod("/dev/console", S_IRUSR|S_IWUSR|S_IFCHR, makedev(5, 1));
	mknod("/dev/null", S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH|S_IFCHR, makedev(1, 3));
	mknod("/dev/kmsg", S_IRUSR|S_IWUSR|S_IFCHR, makedev(1, 11));

	mknod("/dev/tty0", S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH|S_IFCHR, makedev(4, 0));
	mknod("/dev/tty1", S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH|S_IFCHR, makedev(4, 1));
	mknod("/dev/tty2", S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH|S_IFCHR, makedev(4, 2));
	mknod("/dev/tty3", S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH|S_IFCHR, makedev(4, 3));
	mknod("/dev/tty4", S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH|S_IFCHR, makedev(4, 4));
	mknod("/dev/tty5", S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH|S_IFCHR, makedev(4, 5));
	mknod("/dev/tty6", S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH|S_IFCHR, makedev(4, 6));
	mknod("/dev/ttyS0", S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH|S_IFCHR, makedev(4, 64));
	mknod("/dev/ttyS1", S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH|S_IFCHR, makedev(4, 65));

	/* /etc */
	mkdir("/etc", 0755);
	mount("tmpfs", "/etc", "tmpfs", MS_MGC_VAL, NULL);

	/* /boot */
	mkdir("/boot", 0777);

	/* /data */
	mkdir("/data", 0777);

	/* /var */
	mkdir("/var", 0777);
	mkdir("/var/lock", 0777);
	mkdir("/var/log", 0777);
	mkdir("/var/run", 0777);
	mkdir("/var/tmp", 0777);

	/* useful /var directories */
	mkdir("/var/lib", 0777);
	mkdir("/var/lib/misc", 0777);

	/* /tmp */
	//snprintf(buf, sizeof(buf), "%s -rf /tmp", CMD_RM);
	//utils_system(buf);
	//if (symlink("/var/tmp", "/tmp") == -1) {
	//	DBG("%s-%s(%d)\n", __FILE__, __func__, __LINE__);
	//}

	/* user's home directory */
	mkdir("/home", 0755);

	/* root's home directory */
	mkdir(ROOT_HOME_PATH, 0755);

	return (EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
	utils_make_preboot_dirs();
	DBG("%s(%d) pid=[%d] hello world!\n", __func__, __LINE__, getpid());

	/* prepare shell */
	if (shelld_main(argc, argv) == EXIT_FAILURE) {
		DBG("%s(%d) shelld error!\n", __func__, __LINE__);
		return EXIT_FAILURE;
	}
	DBG("%s(%d) pid=[%d] finish shelld!\n", __func__, __LINE__, getpid());

	/* init */
	init_main(argc, argv);
	DBG("%s(%d) pid=[%d] cruel world!\n", __func__, __LINE__, getpid());
	return EXIT_FAILURE;
}
