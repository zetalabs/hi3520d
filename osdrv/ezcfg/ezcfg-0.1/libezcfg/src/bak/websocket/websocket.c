/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : websocket/websocket.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2013-04-14   0.1       Write it from scratch
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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <linux/netlink.h>

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-websocket.h"

#if 0
#define DBG(format, args...) do { \
	char path[256]; \
	FILE *dbg_fp; \
	snprintf(path, 256, "/tmp/%d-debug.txt", getpid()); \
	dbg_fp = fopen(path, "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, "tid=[%d] ", (int)gettid()); \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

/*
 * websocket depends on stream socket
 */
struct ezcfg_websocket {
	struct ezcfg *ezcfg;
	struct ezcfg_socket *sp;        /* stream socket pointer */
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	struct ezcfg_ssl *ssl;
#endif
};


/**
 * private functions
 */


/**
 * public functions
 */

/**
 * ezcfg_websocket_delete:
 * Delete ezcfg websocket.
 * Returns:
 **/
void ezcfg_websocket_delete(struct ezcfg_websocket *wsp)
{
	//struct ezcfg *ezcfg;
	struct ezcfg_socket *sp;
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	struct ezcfg_ssl *sslp;
#endif
	ASSERT(wsp != NULL);

	//ezcfg = wsp->ezcfg;
	sp = wsp->sp;
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	sslp = wsp->ssl;
#endif

	/* FIXME: must close socket first */
	if (sp != NULL)
		ezcfg_socket_delete(sp);
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	if (sslp != NULL)
		ezcfg_ssl_delete(sslp);
#endif

	free(wsp);
}

/**
 * ezcfg_websocket_new:
 * Create ezcfg websocket.
 * Returns: a new ezcfg websocket
 **/
struct ezcfg_websocket *ezcfg_websocket_new(struct ezcfg *ezcfg)
{
	struct ezcfg_websocket *wsp;

	wsp = malloc(sizeof(struct ezcfg_websocket));
	if (wsp == NULL)
		return NULL;
	memset(wsp, 0, sizeof(struct ezcfg_websocket));

	return wsp;
}

struct ezcfg_socket *ezcfg_websocket_get_socket(struct ezcfg_websocket *wsp)
{
	//struct ezcfg *ezcfg;

	ASSERT(wsp != NULL);

	//ezcfg = wsp->ezcfg;

	return wsp->sp;
}

bool ezcfg_websocket_set_socket(struct ezcfg_websocket *wsp, struct ezcfg_socket *sp)
{
	//struct ezcfg *ezcfg;

	ASSERT(wsp != NULL);

	//ezcfg = wsp->ezcfg;
	if (wsp->sp != NULL) {
		ezcfg_socket_delete(sp);
	}
	wsp->sp = sp;

	return true;
}

#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
struct ezcfg_ssl *ezcfg_websocket_get_ssl(struct ezcfg_websocket *wsp)
{
	//struct ezcfg *ezcfg;

	ASSERT(wsp != NULL);

	//ezcfg = wsp->ezcfg;

	return wsp->ssl;
}

bool ezcfg_websocket_set_ssl(struct ezcfg_websocket *wsp, struct ezcfg_ssl *sslp)
{
	//struct ezcfg *ezcfg;

	ASSERT(wsp != NULL);

	//ezcfg = wsp->ezcfg;
	if (wsp->ssl != NULL) {
		ezcfg_ssl_delete(wsp->ssl);
	}
	wsp->ssl = sslp;

	return true;
}
#endif
