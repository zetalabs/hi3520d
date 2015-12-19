/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_group.c
 *
 * Description  : ezbox /etc/group file generating program
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

static int generate_boot_group(FILE *file)
{
	fprintf(file, "%s\n", "root:x:0:");
	fprintf(file, "%s\n", "daemon:x:1:");
	fprintf(file, "%s\n", "bin:x:2:");
	fprintf(file, "%s\n", "sys:x:3:");
	fprintf(file, "%s\n", "adm:x:4:");
	fprintf(file, "%s\n", "tty:x:5:");
	fprintf(file, "%s\n", "disk:x:6:");
	fprintf(file, "%s\n", "lp:x:7:");
	fprintf(file, "%s\n", "mail:x:8:");
	fprintf(file, "%s\n", "news:x:9:");
	fprintf(file, "%s\n", "uucp:x:10:");
	fprintf(file, "%s\n", "man:x:12:");
	fprintf(file, "%s\n", "proxy:x:13:");
	fprintf(file, "%s\n", "kmem:x:15:");
	fprintf(file, "%s\n", "dialout:x:20:");
	fprintf(file, "%s\n", "fax:x:21:");
	fprintf(file, "%s\n", "voice:x:22:");
	fprintf(file, "%s\n", "cdrom:x:24:");
	fprintf(file, "%s\n", "floppy:x:25:");
	fprintf(file, "%s\n", "tape:x:26:");
	fprintf(file, "%s\n", "sudo:x:27:");
	fprintf(file, "%s\n", "audio:x:29:");
	fprintf(file, "%s\n", "dip:x:30:");
	fprintf(file, "%s\n", "www-data:x:33:");
	fprintf(file, "%s\n", "backup:x:34:");
	fprintf(file, "%s\n", "operator:x:37:");
	fprintf(file, "%s\n", "list:x:38:");
	fprintf(file, "%s\n", "irc:x:39:");
	fprintf(file, "%s\n", "src:x:40:");
	fprintf(file, "%s\n", "gnats:x:41:");
	fprintf(file, "%s\n", "shadow:x:42:");
	fprintf(file, "%s\n", "utmp:x:43:");
	fprintf(file, "%s\n", "video:x:44:");
	fprintf(file, "%s\n", "sasl:x:45:");
	fprintf(file, "%s\n", "plugdev:x:46:");
	fprintf(file, "%s\n", "staff:x:50:");
	fprintf(file, "%s\n", "games:x:60:");
	fprintf(file, "%s\n", "users:x:100:");
	fprintf(file, "%s\n", "nogroup:x:65534:");

	return (EXIT_SUCCESS);
}

int pop_etc_group(int flag)
{
        FILE *file = NULL;

	/* generate /etc/group */
	file = fopen("/etc/group", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	switch(flag) {
	case RC_ACT_BOOT :
	case RC_ACT_START :
		generate_boot_group(file);
		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
