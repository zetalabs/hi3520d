/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_profile.c
 *
 * Description  : ezbox /etc/profile file generating program
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
#include "pop_func.h"

int pop_etc_profile(int flag)
{
        FILE *file = NULL;

	/* generate /etc/profile */
	file = fopen("/etc/profile", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	fprintf(file, "%s\n", "#!/bin/sh");
	fprintf(file, "%s\n", "[ -f /etc/banner ] && cat /etc/banner");
	fprintf(file, "%s\n", "export PATH=/bin:/sbin:/usr/bin:/usr/sbin");
	fprintf(file, "%s\n", "export HOME=$(grep -e \"^${USER:-root}:\" /etc/passwd | cut -d \":\" -f 6)");
	fprintf(file, "%s\n", "export HOME=${HOME:-/root}");
	fprintf(file, "%s\n", "export PS1='\\u@\\h:\\w\\$ '");
	fprintf(file, "%s\n", "[ -x /bin/more ] || alias more=less");
	fprintf(file, "%s\n", "[ -x /usr/bin/vim ] && alias vi=vim || alias vim=vi");
	fprintf(file, "%s\n", "[ -x /sbin/arp ] || arp() { cat /proc/net/arp; }");
	fprintf(file, "%s\n", "[ -z /bin/ldd ] || ldd() { LD_TRACE_LOADED_OBJECTS=1 $*; }");
	fprintf(file, "%s\n", "[ -d $HOME ] && cd $HOME");
		
	fclose(file);
	return (EXIT_SUCCESS);
}
