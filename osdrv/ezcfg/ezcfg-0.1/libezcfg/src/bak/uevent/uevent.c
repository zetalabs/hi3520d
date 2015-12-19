/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : uevent/uevent.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-08-10   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-uevent.h"

/**
 * private functions
 **/

/**
 * Public functions
 **/
void ezcfg_uevent_delete(struct ezcfg_uevent *uevent)
{
	//struct ezcfg *ezcfg;

	ASSERT(uevent != NULL);

	//ezcfg = uevent->ezcfg;

	free(uevent);
}

/**
 * ezcfg_uevent_new:
 * Create ezcfg uevent protocol data structure
 * Returns: a new ezcfg uevent protocol data structure
 **/
struct ezcfg_uevent *ezcfg_uevent_new(struct ezcfg *ezcfg)
{
	struct ezcfg_uevent *uevent;

	ASSERT(ezcfg != NULL);

	/* initialize uevent protocol data structure */
	uevent = calloc(1, sizeof(struct ezcfg_uevent));
	if (uevent == NULL) {
		return NULL;
	}

	memset(uevent, 0, sizeof(struct ezcfg_uevent));

	return uevent;
}

