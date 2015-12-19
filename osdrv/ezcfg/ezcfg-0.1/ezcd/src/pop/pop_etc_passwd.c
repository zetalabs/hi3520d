/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_passwd.c
 *
 * Description  : ezbox /etc/passwd file generating program
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

static int generate_boot_passwd(FILE *file)
{
	fprintf(file, "%s\n", "root:!:0:0:root:/root:/bin/ash");
	fprintf(file, "%s\n", "daemon:*:1:1:daemon:/usr/sbin:/bin/false");
	fprintf(file, "%s\n", "bin:*:2:2:bin:/bin:/bin/false");
	fprintf(file, "%s\n", "sys:*:3:3:sys:/dev:/bin/false");
	fprintf(file, "%s\n", "sync:*:4:65534:sync:/bin:/bin/sync");
	fprintf(file, "%s\n", "games:*:5:60:games:/usr/games:/bin/false");
	fprintf(file, "%s\n", "man:*:6:12:man:/var/cache/man:/bin/false");
	fprintf(file, "%s\n", "lp:*:7:7:lp:/var/spool/lpd:/bin/false");
	fprintf(file, "%s\n", "mail:*:8:8:mail:/var/mail:/bin/false");
	fprintf(file, "%s\n", "news:*:9:9:news:/var/spool/news:/bin/false");
	fprintf(file, "%s\n", "uucp:*:10:10:uucp:/var/spool/uucp:/bin/false");
	fprintf(file, "%s\n", "proxy:*:13:13:proxy:/bin:/bin/false");
	fprintf(file, "%s\n", "www-data:*:33:33:www-data:/var/www:/bin/false");
	fprintf(file, "%s\n", "backup:*:34:34:backup:/var/backups:/bin/false");
	fprintf(file, "%s\n", "list:*:38:38:Mailing List Manager:/var/list:/bin/false");
	fprintf(file, "%s\n", "irc:*:39:39:ircd:/var/run/ircd:/bin/false");
	fprintf(file, "%s\n", "gnats:*:41:41:Gnats Bug-Reporting System (admin):/var/lib/gnats:/bin/false");
	fprintf(file, "%s\n", "nobody:*:65534:65534:nobody:/var:/bin/false");

	return (EXIT_SUCCESS);
}

int pop_etc_passwd(int flag)
{
        FILE *file = NULL;

	/* generate /etc/passwd */
	file = fopen("/etc/passwd", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	switch(flag) {
	case RC_ACT_BOOT :
	case RC_ACT_START :
		generate_boot_passwd(file);	
		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
