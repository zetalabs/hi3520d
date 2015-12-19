/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_worker.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-14   0.1       Split it from ezcfg-private.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_WORKER_H_
#define _EZCFG_PRIV_WORKER_H_

#include "ezcfg-types.h"

/* thread/worker.c */
/* worker inherits master's resource */
void ezcfg_worker_delete(struct ezcfg_worker *worker);
struct ezcfg_worker *ezcfg_worker_new(struct ezcfg_master *master);
pthread_t *ezcfg_worker_get_p_thread_id(struct ezcfg_worker *worker);
void ezcfg_worker_close_connection(struct ezcfg_worker *worker);
struct ezcfg_worker *ezcfg_worker_get_next(struct ezcfg_worker *worker);
bool ezcfg_worker_set_next(struct ezcfg_worker *worker, struct ezcfg_worker *next);
void ezcfg_worker_routine(struct ezcfg_worker *worker);
/* worker call this to notify master */
void ezcfg_master_stop_worker(struct ezcfg_master *master, struct ezcfg_worker *worker);
struct ezcfg *ezcfg_worker_get_ezcfg(struct ezcfg_worker *worker);
struct ezcfg_master *ezcfg_worker_get_master(struct ezcfg_worker *worker);
int ezcfg_worker_printf(struct ezcfg_worker *worker, const char *fmt, ...);
int ezcfg_worker_write(struct ezcfg_worker *worker, const char *buf, int len);
void *ezcfg_worker_get_proto_data(struct ezcfg_worker *worker);
struct ezcfg_socket *ezcfg_worker_get_socket(struct ezcfg_worker *worker);
bool ezcfg_worker_set_num_bytes_sent(struct ezcfg_worker *worker, int64_t num);
bool ezcfg_worker_set_birth_time(struct ezcfg_worker *worker, time_t time);

/* thread/worker_ctrl.c */
void ezcfg_worker_process_ctrl_new_connection(struct ezcfg_worker *worker);

/* thread/worker_http.c */
void ezcfg_worker_process_http_new_connection(struct ezcfg_worker *worker);

/* thread/worker_soap_http.c */
void ezcfg_worker_process_soap_http_new_connection(struct ezcfg_worker *worker);

/* thread/worker_igrs.c */
void ezcfg_worker_process_igrs_new_connection(struct ezcfg_worker *worker);

/* thread/worker_igrs_isdp.c */
void ezcfg_worker_process_igrs_isdp_new_connection(struct ezcfg_worker *worker);

/* thread/worker_uevent.c */
void ezcfg_worker_process_uevent_new_connection(struct ezcfg_worker *worker);

/* thread/worker_upnp_ssdp.c */
void ezcfg_worker_process_upnp_ssdp_new_connection(struct ezcfg_worker *worker);

/* thread/worker_upnp_http.c */
void ezcfg_worker_process_upnp_http_new_connection(struct ezcfg_worker *worker);

/* thread/worker_upnp_gena.c */
void ezcfg_worker_process_upnp_gena_new_connection(struct ezcfg_worker *worker);

#endif /* _EZCFG_PRIV_WORKER_H_ */
