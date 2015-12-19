/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ssl/ssl.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-12   0.1       Write it from scratch
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

#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/opensslv.h>
#endif

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-ssl.h"

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
 * SSL depends on stream socket
 */
struct ezcfg_ssl {
	struct ezcfg *ezcfg;
	struct ezcfg_socket *sp;        /* stream socket pointer */
	int role;                       /* combine, server, client */
	int method;                     /* SSL2, SSL3, TLS1, peer determed */
	SSL_CTX *ctx;                   /* SSL context */
	SSL *session;			/* SSL session structure */
	char *certificate_file;
	char *certificate_chain_file;
	char *private_key_file;

	struct ezcfg_ssl *next; /* link pointer */
};


/**
 * private functions
 */
static bool ssl_is_same(struct ezcfg_ssl *s1, struct ezcfg_ssl *s2)
{
	return false;
}


/**
 * public functions
 */

/**
 * ezcfg_ssl_delete:
 * Delete ezcfg SSL socket.
 * Returns:
 **/
void ezcfg_ssl_delete(struct ezcfg_ssl *sslp)
{
	//struct ezcfg *ezcfg;
	ASSERT(sslp != NULL);

	//ezcfg = sslp->ezcfg;

	/* FIXME: must close SSL session first */
	ezcfg_ssl_close_session(sslp);

	if (sslp->sp != NULL)
		ezcfg_socket_delete(sslp->sp);

	if (sslp->ctx != NULL)
		SSL_CTX_free(sslp->ctx);

	if (sslp->certificate_file != NULL)
		free(sslp->certificate_file);

	if (sslp->certificate_chain_file != NULL)
		free(sslp->certificate_chain_file);

	if (sslp->private_key_file != NULL)
		free(sslp->private_key_file);

	free(sslp);
}

/**
 * ezcfg_ssl_new:
 * Create ezcfg SSL socket.
 * Returns: a new ezcfg SSL socket
 **/
struct ezcfg_ssl *ezcfg_ssl_new(struct ezcfg *ezcfg, const int role, const int method)
{
	struct ezcfg_ssl *sslp;
#if (OPENSSL_VERSION_NUMBER >= 0x10000000L)
	const SSL_METHOD *meth;
#else
	SSL_METHOD *meth;
#endif

	sslp = malloc(sizeof(struct ezcfg_ssl));
	if (sslp == NULL)
		return NULL;
	memset(sslp, 0, sizeof(struct ezcfg_ssl));

	SSL_library_init(); /* load encryption & hash algorithms for SSL */
	SSL_load_error_strings(); /* load the error strings for good error reporting */

	switch (role) {
	case EZCFG_SSL_ROLE_UNKNOWN :
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
#ifndef OPENSSL_NO_SSL2
		if (method == EZCFG_SSL_METHOD_SSLV2) {
			meth = SSLv2_method();
		}
		else
#endif
		if (method == EZCFG_SSL_METHOD_SSLV3) {
			meth = SSLv3_method();
		}
		else if (method == EZCFG_SSL_METHOD_TLSV1) {
			meth = TLSv1_method();
		}
		else if (method == EZCFG_SSL_METHOD_SSLV23) {
			meth = SSLv23_method();
		}
		else
#endif
		{
			goto fail_exit;
		}
		break;

	case EZCFG_SSL_ROLE_SERVER :
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
#ifndef OPENSSL_NO_SSL2
		if (method == EZCFG_SSL_METHOD_SSLV2) {
			meth = SSLv2_server_method();
		}
		else
#endif
		if (method == EZCFG_SSL_METHOD_SSLV3) {
			meth = SSLv3_server_method();
		}
		else if (method == EZCFG_SSL_METHOD_TLSV1) {
			meth = TLSv1_server_method();
		}
		else if (method == EZCFG_SSL_METHOD_SSLV23) {
			meth = SSLv23_server_method();
		}
		else
#endif
		{
			goto fail_exit;
		}
		break;

	case EZCFG_SSL_ROLE_CLIENT :
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
#ifndef OPENSSL_NO_SSL2
		if (method == EZCFG_SSL_METHOD_SSLV2) {
			meth = SSLv2_client_method();
		}
		else
#endif
		if (method == EZCFG_SSL_METHOD_SSLV3) {
			meth = SSLv3_client_method();
		}
		else if (method == EZCFG_SSL_METHOD_TLSV1) {
			meth = TLSv1_client_method();
		}
		else if (method == EZCFG_SSL_METHOD_SSLV23) {
			meth = SSLv23_client_method();
		}
		else
#endif
		{
			goto fail_exit;
		}
		break;

	default :
		goto fail_exit;
	}

	sslp->ctx = SSL_CTX_new(meth);

	return sslp;

fail_exit:
	ezcfg_ssl_delete(sslp);
	return NULL;
}

struct ezcfg_socket *ezcfg_ssl_get_socket(struct ezcfg_ssl *sslp)
{
	//struct ezcfg *ezcfg;

	ASSERT(sslp != NULL);

	//ezcfg = sslp->ezcfg;

	return sslp->sp;
}

bool ezcfg_ssl_set_socket(struct ezcfg_ssl *sslp, struct ezcfg_socket *sp)
{
	//struct ezcfg *ezcfg;

	ASSERT(sslp != NULL);

	//ezcfg = sslp->ezcfg;
	if (sslp->sp != NULL) {
		ezcfg_socket_delete(sp);
	}
	sslp->sp = sp;

	return true;
}

bool ezcfg_ssl_open_session(struct ezcfg_ssl *sslp, const int sock)
{
	//struct ezcfg *ezcfg;
	SSL *s;

	ASSERT(sslp != NULL);
	ASSERT(sock >= 0);

	//ezcfg = sslp->ezcfg;

	if (sslp->session != NULL) {
		return false;
	}

	if (sslp->sp == NULL) {
		return false;
	}

	s = SSL_new(sslp->ctx);
	if (s == NULL) {
		return false;
	}

	if (SSL_set_fd(s, sock) == 0) {
		SSL_free(s);
		return false;
	}

	if ((sslp->role == EZCFG_SSL_ROLE_SERVER) &&
	    (SSL_accept(s) == 1)) {
		sslp->session = s;
		/* setup SSL session associated sock */
		ezcfg_socket_set_sock(sslp->sp, sock);
		return true;
	}
	else if ((sslp->role == EZCFG_SSL_ROLE_SERVER) &&
	         (SSL_connect(s) == 1)) {
		sslp->session = s;
		/* setup SSL session associated sock */
		ezcfg_socket_set_sock(sslp->sp, sock);
		return true;
	}
	else {
		SSL_free(s);
		return false;
	}
}

bool ezcfg_ssl_close_session(struct ezcfg_ssl *sslp)
{
	//struct ezcfg *ezcfg;

	ASSERT(sslp != NULL);

	//ezcfg = sslp->ezcfg;
	if (sslp->session != NULL) {
		SSL_free(sslp->session);
		sslp->session = NULL;
	}
	if (sslp->sp != NULL) {
		/* clear SSL session associated sock */
		ezcfg_socket_set_sock(sslp->sp, -1);
	}

	return true;
}

const char *ezcfg_ssl_get_certificate_file(struct ezcfg_ssl *sslp)
{
	//struct ezcfg *ezcfg;

	ASSERT(sslp != NULL);

	//ezcfg = sslp->ezcfg;

	return sslp->certificate_file;
}

bool ezcfg_ssl_set_certificate_file(struct ezcfg_ssl *sslp, const char *file)
{
	//struct ezcfg *ezcfg;
	char *p;

	ASSERT(sslp != NULL);
	ASSERT(file != NULL);

	//ezcfg = sslp->ezcfg;

	p = strdup(file);
	if (p == NULL) {
		return false;
	}

	if (sslp->certificate_file != NULL) {
		free(sslp->certificate_file);
	}
	sslp->certificate_file = p;

	return true;
}

const char *ezcfg_ssl_get_certificate_chain_file(struct ezcfg_ssl *sslp)
{
	//struct ezcfg *ezcfg;

	ASSERT(sslp != NULL);

	//ezcfg = sslp->ezcfg;

	return sslp->certificate_chain_file;
}

bool ezcfg_ssl_set_certificate_chain_file(struct ezcfg_ssl *sslp, const char *file)
{
	//struct ezcfg *ezcfg;
	char *p;

	ASSERT(sslp != NULL);
	ASSERT(file != NULL);

	//ezcfg = sslp->ezcfg;

	p = strdup(file);
	if (p == NULL) {
		return false;
	}

	if (sslp->certificate_chain_file != NULL) {
		free(sslp->certificate_chain_file);
	}
	sslp->certificate_chain_file = p;

	return true;
}

const char *ezcfg_ssl_get_private_key_file(struct ezcfg_ssl *sslp)
{
	//struct ezcfg *ezcfg;

	ASSERT(sslp != NULL);

	//ezcfg = sslp->ezcfg;

	return sslp->private_key_file;
}

bool ezcfg_ssl_set_private_key_file(struct ezcfg_ssl *sslp, const char *file)
{
	//struct ezcfg *ezcfg;
	char *p;

	ASSERT(sslp != NULL);
	ASSERT(file != NULL);

	//ezcfg = sslp->ezcfg;

	p = strdup(file);
	if (p == NULL) {
		return false;
	}

	if (sslp->private_key_file != NULL) {
		free(sslp->private_key_file);
	}
	sslp->private_key_file = p;

	return true;
}

bool ezcfg_ssl_is_valid(struct ezcfg_ssl *sslp)
{
	//struct ezcfg *ezcfg;

	ASSERT(sslp != NULL);

	//ezcfg = sslp->ezcfg;

	return true;
}

int ezcfg_ssl_read(struct ezcfg_ssl *sslp, void *buf, int len, int flags)
{
	struct ezcfg *ezcfg;
	SSL *s;
	char * p;
	int status, n;

	ASSERT(sslp != NULL);
	ASSERT(buf != NULL);
	ASSERT(len >= 0);

	ezcfg = sslp->ezcfg;
	s = sslp->session;
	p = buf;
	status = 0;
        memset(buf, '\0', len);

	while (status == 0) {
		n = SSL_read(s, p + status, len - status);
		if (n < 0) {
			if (errno == EPIPE) {
				info(ezcfg, "pipe error: %m\n");
				return -EPIPE;
			}
			else if (errno == EINTR || errno == EAGAIN) {
				info(ezcfg, "interrupted: %m\n");
				continue;
			}
			else {
				err(ezcfg, "read fail: %m\n");
				return -errno;
			}
		}

		if (n == 0) {
			info(ezcfg, "remote end closed connection: %m\n");
			p = buf;
			p[len-1] = '\0';
			break;
		}
		status += n;
	}

	return status;
}

int ezcfg_ssl_write(struct ezcfg_ssl *sslp, const void *buf, int len, int flags)
{
	struct ezcfg *ezcfg;
	SSL *s;
	const char *p;
	int status, n;

	ASSERT(sslp != NULL);
	ASSERT(buf != NULL);
	ASSERT(len >= 0);

	ezcfg = sslp->ezcfg;
	p = buf;
	status = 0;
	s = sslp->session;

	while (status != len) {
		n = SSL_write(s, p + status, len -status);
		if (n < 0) {
			if (errno == EPIPE) {
				info(ezcfg, "remote end closed connection: %m\n");
				return -EPIPE;
			}
			else if (errno == EINTR || errno == EAGAIN) {
				info(ezcfg, "interrupted: %m\n");
				continue;
			}
			else {
				err(ezcfg, "write fail: %m\n");
				return -errno;
			}
		}
		status += n;
	}

	return status;
}

bool ezcfg_ssl_list_in(struct ezcfg_ssl **list, struct ezcfg_ssl *sslp)
{
	//struct ezcfg *ezcfg;
	struct ezcfg_ssl *sslp2;

	ASSERT(list != NULL);
	ASSERT(sslp != NULL);

	//ezcfg = sslp->ezcfg;

	sslp2 = *list;
	while (sslp2 != NULL) {
		if (ssl_is_same(sslp2, sslp) == true) {
			return true;
		}
		sslp2 = sslp2->next;
	}
	return false;
}

bool ezcfg_ssl_list_insert(struct ezcfg_ssl **list, struct ezcfg_ssl *sslp)
{
	ASSERT(list != NULL);
	ASSERT(sslp != NULL);

	sslp->next = *list;
	*list = sslp;
	return true;
}

void ezcfg_ssl_list_delete(struct ezcfg_ssl **list)
{
	struct ezcfg_ssl *sslp;

	ASSERT(list != NULL);

	sslp = *list;
	while (sslp != NULL) {
		*list = sslp->next;
		ezcfg_ssl_delete(sslp);
		sslp = *list;
	}
}

struct ezcfg_ssl *ezcfg_ssl_list_find_by_socket(struct ezcfg_ssl **list, struct ezcfg_socket *sp)
{
	struct ezcfg_ssl *sslp;
	struct ezcfg_socket *sp2;

	ASSERT(list != NULL);
	ASSERT(sp != NULL);

	sslp = *list;
	while (sslp != NULL) {
		sp2 = sslp->sp;
		if (ezcfg_socket_compare(sp2, sp) == true) {
			return sslp;
		}
		sslp = sslp->next;
	}
	return NULL;
}
