/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_find_pid_by_name.c
 *
 * Description  : ezcfg find PID by process name function
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
#include <dirent.h>

#include "ezcd.h"

#define BUF_SIZE 254

/*
 * Returns a list of all matching PIDs
 * It is the caller's duty to free the returned pidlist.
 */
proc_stat_t *utils_find_pid_by_name(char *pidName)
{
	DIR *dir;
	struct dirent *next;
	proc_stat_t *pidList = NULL, *pidTemp;
	int i = 0;

	dir = opendir("/proc");
	if (dir == NULL)
		return NULL;

	while ((next = readdir(dir)) != NULL) {
		FILE *fp;
		char line[BUF_SIZE];
		char name[BUF_SIZE];
		pid_t pid = 0;
		char state = '\0';
		int c = 0;
		char *tmp = NULL;

		/* must skip ".." since that is outside /proc */
		if (strcmp(next->d_name, "..") == 0)
			continue;

		/* if it isn't a number, we don't want it */
		if (!isdigit(*next->d_name))
			continue;

		snprintf(name, sizeof(name), "/proc/%s/stat", next->d_name);
		fp = fopen(name, "r");
		if (fp == NULL)
			continue;

		if (fgets(line, BUF_SIZE-1, fp) == NULL)
		{
			fclose(fp);
			continue;
		}
		fclose(fp);

		/* Buffer should contain a string like "1 (init) S 0 1 1 0 -1 4194560 159 310977 16 257 0 88 7630 1277 15 0 1 0 10 2191360 146 4294967295 134512640 135032784 3214904272 3214903892 4294960144 0 0 0 674311 3222390807 0 0 0 0 0 0 0" */
		tmp = line;
		c = 0;
		while (tmp && c < 3)
		{
			c++;
			tmp = strchr(tmp, ' ');
			if (tmp && c < 3) { tmp++; }
		}

		if (tmp) { *tmp = '\0'; }

		memset(name, '\0', sizeof(name));
		if (sscanf(line, "%d (%s %c", &pid, name, &state) != 3)
		{
			pid = 0;
			state = '\0';
		}

		/* remove ')' in the name, say "(init)" will be get name="init)" */
		tmp = strrchr(name, ')');
		if (tmp) { *tmp = '\0'; }

		if (strncmp(name, pidName, 15) == 0)
		{
			pidTemp = (proc_stat_t *)realloc(pidList, sizeof(proc_stat_t) * (i+2));
			if (pidTemp == NULL) {
				goto func_exit;
			}
			pidList = pidTemp;
			pidList[i].pid = pid;
			pidList[i].state = state;
			i++;
		}
	}

func_exit:
	closedir(dir);

	if (pidList)
	{
		pidList[i].pid = 0;
		pidList[i].state = '\0';
	}
	return (pidList);
}

/*
 * Returns true if has a process named as pidName
 */
bool utils_has_process_by_name(char *pidName)
{
	DIR *dir;
	struct dirent *next;

	dir = opendir("/proc");
	if (dir == NULL)
		return false;

	while ((next = readdir(dir)) != NULL) {
		FILE *fp;
		char line[BUF_SIZE];
		char name[BUF_SIZE];
		pid_t pid = 0;
		char state = '\0';
		int c = 0;
		char *tmp = NULL;

		/* must skip ".." since that is outside /proc */
		if (strcmp(next->d_name, "..") == 0)
			continue;

		/* if it isn't a number, we don't want it */
		if (!isdigit(*next->d_name))
			continue;

		snprintf(name, sizeof(name), "/proc/%s/stat", next->d_name);
		fp = fopen(name, "r");
		if (fp == NULL)
			continue;

		if (fgets(line, BUF_SIZE-1, fp) == NULL)
		{
			fclose(fp);
			continue;
		}
		fclose(fp);

		/* Buffer should contain a string like "1 (init) S 0 1 1 0 -1 4194560 159 310977 16 257 0 88 7630 1277 15 0 1 0 10 2191360 146 4294967295 134512640 135032784 3214904272 3214903892 4294960144 0 0 0 674311 3222390807 0 0 0 0 0 0 0" */
		tmp = line;
		c = 0;
		while (tmp && c < 3)
		{
			c++;
			tmp = strchr(tmp, ' ');
			if (tmp && c < 3) { tmp++; }
		}

		if (tmp) { *tmp = '\0'; }

		memset(name, '\0', sizeof(name));
		if (sscanf(line, "%d (%s %c", &pid, name, &state) != 3)
		{
			pid = 0;
			state = '\0';
		}

		/* remove ')' in the name, say "(init)" will be get name="init)" */
		tmp = strrchr(name, ')');
		if (tmp) { *tmp = '\0'; }

		if (strncmp(name, pidName, 15) == 0)
		{
			closedir(dir);
			return true;
		}
	}
	closedir(dir);
	return (false);
}
