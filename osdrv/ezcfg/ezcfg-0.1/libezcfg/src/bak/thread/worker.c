/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/worker.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2013 by ezbox-project
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
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-soap_http.h"

#if 1
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
 * ezcfg_worker:
 * Opaque object handling one event source.
 * Multi-threads model - worker part.
 */
struct ezcfg_worker {
	pthread_t thread_id;
	struct ezcfg *ezcfg;
	struct ezcfg_worker *next; /* Linkage */
	struct ezcfg_master *master;
	struct ezcfg_socket *sp;
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	struct ezcfg_ssl *sslp;
#endif
	unsigned char proto;
	void *proto_data;
	time_t birth_time;
	int64_t num_bytes_sent;
};

#define handle_error_en(en, msg) \
	do { errno = en; perror(msg); } while (0)

static void reset_connection_attributes(struct ezcfg_worker *worker) {
	struct ezcfg *ezcfg;

	ASSERT(worker != NULL);

	ezcfg = worker->ezcfg;

	switch(worker->proto) {
	case EZCFG_PROTO_CTRL :
		ezcfg_ctrl_reset_attributes(worker->proto_data);
		break;
#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD == 1)
	case EZCFG_PROTO_HTTP :
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	case EZCFG_PROTO_HTTPS :
#endif
		ezcfg_http_reset_attributes(worker->proto_data);
		break;
#endif
	case EZCFG_PROTO_SOAP_HTTP :
		ezcfg_soap_http_reset_attributes(worker->proto_data);
		break;
#if (HAVE_EZBOX_SERVICE_EZCFG_IGRSD == 1)
	case EZCFG_PROTO_IGRS :
		ezcfg_igrs_reset_attributes(worker->proto_data);
		break;
	case EZCFG_PROTO_IGRS_ISDP :
		info(ezcfg, "IGRS ISDP reset attributes\n");
		break;
#endif
	case EZCFG_PROTO_UEVENT :
		info(ezcfg, "UEVENT reset attributes\n");
		break;
#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
	case EZCFG_PROTO_UPNP_SSDP :
		ezcfg_upnp_ssdp_reset_attributes(worker->proto_data);
		break;
	case EZCFG_PROTO_UPNP_HTTP :
		ezcfg_http_reset_attributes(worker->proto_data);
		break;
	case EZCFG_PROTO_UPNP_GENA :
		ezcfg_upnp_gena_reset_attributes(worker->proto_data);
		break;
#endif
	default :
		err(ezcfg, "unknown protocol\n");
	}

	worker->num_bytes_sent = 0;
}

static void close_connection(struct ezcfg_worker *worker)
{
	ASSERT(worker != NULL);

#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	if (worker->sslp != NULL) {
		ezcfg_ssl_close_session(worker->sslp);
		worker->sslp = NULL;
	}
#endif

	ezcfg_socket_close_sock(worker->sp);
}

static void init_protocol_data(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;

	ASSERT(worker != NULL);
	/* proto_data should be empty before init */
	ASSERT(worker->proto_data == NULL);

	ezcfg = worker->ezcfg;

	/* set communication protocol */
	worker->proto = ezcfg_socket_get_proto(worker->sp);

	/* initialize protocol data structure */
	switch(worker->proto) {
	case EZCFG_PROTO_CTRL :
		worker->proto_data = ezcfg_ctrl_new(ezcfg);
		break;
#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD == 1)
	case EZCFG_PROTO_HTTP :
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	case EZCFG_PROTO_HTTPS :
#endif
		worker->proto_data = ezcfg_http_new(ezcfg);
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
		/* for HTTPS we set SSL tag */
		if (worker->proto == EZCFG_PROTO_HTTPS) {
			/* find socket associated SSL entry */
			worker->sslp = ezcfg_ssl_list_find_by_socket(
				ezcfg_master_get_p_ssl(worker->master),
				worker->sp);

			if ((worker->proto_data != NULL) &&
			    (worker->sslp != NULL)) {
				ezcfg_http_set_is_ssl(worker->proto_data, true);
			}
			else {
				if (worker->proto_data != NULL) {
					ezcfg_http_delete(worker->proto_data);
					worker->proto_data = NULL;
				}
				/* worker->sslp will be clean in close_connection() */
			}
		}
#endif
		break;
#endif
	case EZCFG_PROTO_SOAP_HTTP :
		worker->proto_data = ezcfg_soap_http_new(ezcfg);
		break;
#if (HAVE_EZBOX_SERVICE_EZCFG_IGRSD == 1)
	case EZCFG_PROTO_IGRS :
		worker->proto_data = ezcfg_igrs_new(ezcfg);
		break;
	case EZCFG_PROTO_IGRS_ISDP :
		worker->proto_data = ezcfg_igrs_isdp_new(ezcfg);
		break;
#endif
	case EZCFG_PROTO_UEVENT :
		worker->proto_data = ezcfg_uevent_new(ezcfg);
		break;
#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
	case EZCFG_PROTO_UPNP_SSDP :
		worker->proto_data = ezcfg_upnp_ssdp_new(ezcfg);
		break;
	case EZCFG_PROTO_UPNP_HTTP :
		worker->proto_data = ezcfg_http_new(ezcfg);
		break;
	case EZCFG_PROTO_UPNP_GENA :
		worker->proto_data = ezcfg_upnp_gena_new(ezcfg);
		break;
#endif
	default :
		info(ezcfg, "unknown protocol\n");
	}
}

static void process_new_connection(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;

	ASSERT(worker != NULL);

	ezcfg = worker->ezcfg;

	reset_connection_attributes(worker);

	/* dispatch protocol handler */
	switch(worker->proto) {
#if 0
	case EZCFG_PROTO_CTRL :
		ezcfg_worker_process_ctrl_new_connection(worker);
		break;
#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD == 1)
	case EZCFG_PROTO_HTTP :
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	case EZCFG_PROTO_HTTPS :
#endif
		ezcfg_worker_process_http_new_connection(worker);
		break;
#endif
	case EZCFG_PROTO_SOAP_HTTP :
		ezcfg_worker_process_soap_http_new_connection(worker);
		break;
#if (HAVE_EZBOX_SERVICE_EZCFG_IGRSD == 1)
	case EZCFG_PROTO_IGRS :
		ezcfg_worker_process_igrs_new_connection(worker);
		break;
	case EZCFG_PROTO_IGRS_ISDP :
		ezcfg_worker_process_igrs_isdp_new_connection(worker);
		break;
#endif
	case EZCFG_PROTO_UEVENT :
		ezcfg_worker_process_uevent_new_connection(worker);
		break;
#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
	case EZCFG_PROTO_UPNP_SSDP :
		ezcfg_worker_process_upnp_ssdp_new_connection(worker);
		break;
	case EZCFG_PROTO_UPNP_HTTP :
		ezcfg_worker_process_upnp_http_new_connection(worker);
		break;
	case EZCFG_PROTO_UPNP_GENA :
		ezcfg_worker_process_upnp_gena_new_connection(worker);
		break;
#endif
#endif
	default :
		err(ezcfg, "unknown protocol\n");
	}
}
 
static void release_protocol_data(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;

	ASSERT(worker != NULL);

	ezcfg = worker->ezcfg;

	/* release protocol data */
	switch(worker->proto) {
	case EZCFG_PROTO_CTRL :
		ezcfg_ctrl_delete(worker->proto_data);
		worker->proto_data = NULL;
		break;
#if (HAVE_EZBOX_SERVICE_EZCFG_HTTPD == 1)
	case EZCFG_PROTO_HTTP :
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	case EZCFG_PROTO_HTTPS :
#endif
		ezcfg_http_delete(worker->proto_data);
		worker->proto_data = NULL;
		break;
#endif
	case EZCFG_PROTO_SOAP_HTTP :
		ezcfg_soap_http_delete(worker->proto_data);
		worker->proto_data = NULL;
		break;
#if (HAVE_EZBOX_SERVICE_EZCFG_IGRSD == 1)
	case EZCFG_PROTO_IGRS :
		ezcfg_igrs_delete(worker->proto_data);
		worker->proto_data = NULL;
		break;
	case EZCFG_PROTO_IGRS_ISDP :
		ezcfg_igrs_isdp_delete(worker->proto_data);
		worker->proto_data = NULL;
		break;
#endif
	case EZCFG_PROTO_UEVENT :
		ezcfg_uevent_delete(worker->proto_data);
		worker->proto_data = NULL;
		break;
#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
	case EZCFG_PROTO_UPNP_SSDP :
		ezcfg_upnp_ssdp_delete(worker->proto_data);
		worker->proto_data = NULL;
		break;
	case EZCFG_PROTO_UPNP_HTTP :
		ezcfg_http_delete(worker->proto_data);
		worker->proto_data = NULL;
		break;
	case EZCFG_PROTO_UPNP_GENA :
		ezcfg_upnp_gena_delete(worker->proto_data);
		worker->proto_data = NULL;
		break;
#endif
	default :
		err(ezcfg, "unknown protocol\n");
	}
}

/**
 * Public functions
 **/
void ezcfg_worker_delete(struct ezcfg_worker *worker)
{
	ASSERT(worker != NULL);

	if (worker->sp != NULL) {
		ezcfg_socket_delete(worker->sp);
	}

	free(worker);
}

struct ezcfg_worker *ezcfg_worker_new(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;
	struct ezcfg_worker *worker = NULL;
	struct ezcfg_socket *sp = NULL;

	ASSERT(master != NULL);
	worker = calloc(1, sizeof(struct ezcfg_worker));
	if (worker == NULL)
		return NULL;

	ezcfg = ezcfg_master_get_ezcfg(master);
	sp = ezcfg_socket_calloc(ezcfg, 1);
	if (sp == NULL) {
		free(worker);
		return NULL;
	}

	memset(worker, 0, sizeof(struct ezcfg_worker));

	worker->ezcfg = ezcfg;
	worker->master = master;
	worker->sp = sp;
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	worker->sslp = NULL;
#endif
	worker->proto = EZCFG_PROTO_UNKNOWN;
	worker->proto_data = NULL;

	return worker;
}

pthread_t *ezcfg_worker_get_p_thread_id(struct ezcfg_worker *worker)
{
	return &(worker->thread_id);
}

struct ezcfg_worker *ezcfg_worker_get_next(struct ezcfg_worker *worker)
{
	ASSERT(worker != NULL);
	return worker->next;
}

bool ezcfg_worker_set_next(struct ezcfg_worker *worker, struct ezcfg_worker *next)
{
	ASSERT(worker != NULL);
	worker->next = next;
	return true;
}

void ezcfg_worker_close_connection(struct ezcfg_worker *worker)
{
	close_connection(worker);
}

void ezcfg_worker_routine(struct ezcfg_worker *worker) 
{
	struct ezcfg_master *master;
	//sigset_t sigset;
	//int s;

	ASSERT(worker != NULL);

	master = worker->master;

	/* Block signal HUP, USR1 */
	/* do it in root thread */
#if 0
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGHUP);
	sigaddset(&sigset, SIGUSR1);
	s = pthread_sigmask(SIG_BLOCK, &sigset, NULL);
	if (s != 0) {
		handle_error_en(s, "pthread_sigmask");
		goto worker_exit;
	}
#endif

	while ((ezcfg_master_is_stop(worker->master) == false) &&
	       (ezcfg_master_get_socket(worker->master, worker->sp, EZCFG_WORKER_WAIT_TIME) == true)) {

		/* record start working time */
		worker->birth_time = time(NULL);

		/* initialize protocol data */
		init_protocol_data(worker);

		/* process the connection */
		if (worker->proto_data != NULL) {
			process_new_connection(worker);
		}

		/* close connection */
		close_connection(worker);

		/* release protocol data */
		if (worker->proto_data != NULL) {
			release_protocol_data(worker);
		}
	}

	/* clean worker resource */
	/* do it in ezcfg_master_stop_worker */
	//ezcfg_worker_delete(worker);

	/* Signal master that we're done with connection and exiting */
	ezcfg_master_stop_worker(master, worker);
}

struct ezcfg *ezcfg_worker_get_ezcfg(struct ezcfg_worker *worker)
{
	return worker->ezcfg;
}

struct ezcfg_master *ezcfg_worker_get_master(struct ezcfg_worker *worker)
{
	return worker->master;
}

int ezcfg_worker_printf(struct ezcfg_worker *worker, const char *fmt, ...)
{
	char *buf;
	int buf_len;
	int len;
	int ret;
	va_list ap;

	buf_len = EZCFG_BUFFER_SIZE ;
	buf = (char *)malloc(buf_len);
	if (buf == NULL) {
		return -1;
	}

	va_start(ap, fmt);
	len = vsnprintf(buf, buf_len, fmt, ap);
	va_end(ap);

#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	if (worker->sslp != NULL) {
		ret = ezcfg_ssl_write(worker->sslp, buf, len, 0);
	}
	else
#endif
	{
		ret = ezcfg_socket_write(worker->sp, buf, len, 0);
	}
	free(buf);
	return ret;
}

int ezcfg_worker_write(struct ezcfg_worker *worker, const char *buf, int len)
{
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	if (worker->sslp != NULL) {
		return ezcfg_ssl_write(worker->sslp, buf, len, 0);
	}
	else
#endif
	{
		return ezcfg_socket_write(worker->sp, buf, len, 0);
	}
}

void *ezcfg_worker_get_proto_data(struct ezcfg_worker *worker)
{
	return worker->proto_data;
}

struct ezcfg_socket *ezcfg_worker_get_socket(struct ezcfg_worker *worker)
{
	return worker->sp;
}

bool ezcfg_worker_set_num_bytes_sent(struct ezcfg_worker *worker, int64_t num)
{
	worker->num_bytes_sent = num;
	return true;
}

bool ezcfg_worker_set_birth_time(struct ezcfg_worker *worker, time_t t)
{
	worker->birth_time = t;
	return true;
}
