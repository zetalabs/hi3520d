/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_agent.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2013-07-29   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_AGENT_H_
#define _EZCFG_PRIV_AGENT_H_

/* bitmap for agent state */
#define AGENT_STATE_RUNNING	0
#define AGENT_STATE_STOPPED	1
#define AGENT_STATE_STOPPING	2

#define AGENT_SOCKET_QUEUE_LENGTH	20
#define AGENT_MASTER_WAIT_TIME		5

/* agent/agent.c */
struct ezcfg_agent *ezcfg_agent_start(struct ezcfg *ezcfg);
void ezcfg_agent_stop(struct ezcfg_agent *agent);
void ezcfg_agent_reload(struct ezcfg_agent *agent);
void ezcfg_agent_set_threads_max(struct ezcfg_agent *agent, int threads_max);

/* agent/agent_core.c */
void ezcfg_agent_core_delete(struct ezcfg_agent_core *core);
struct ezcfg_agent_core *ezcfg_agent_core_new(struct ezcfg *ezcfg);
struct ezcfg_nvram *ezcfg_agent_core_get_nvram(struct ezcfg_agent_core *agent);

/* agent/agent_master.c */
struct ezcfg_agent_master *ezcfg_agent_master_start(struct ezcfg *ezcfg, struct ezcfg_agent_core *core);
void ezcfg_agent_master_stop(struct ezcfg_agent_master *master);
void ezcfg_agent_master_reload(struct ezcfg_agent_master *master);
void ezcfg_agent_master_set_threads_max(struct ezcfg_agent_master *master, int threads_max);
pthread_t *ezcfg_agent_master_get_p_thread_id(struct ezcfg_agent_master *master);
int ezcfg_agent_master_set_receive_buffer_size(struct ezcfg_agent_master *master, int size);
void ezcfg_agent_master_routine(struct ezcfg_agent_master *master);
struct ezcfg *ezcfg_agent_master_get_ezcfg(struct ezcfg_agent_master *master);
struct ezcfg_nvram *ezcfg_agent_master_get_nvram(struct ezcfg_agent_master *master);
bool ezcfg_agent_master_is_stop(struct ezcfg_agent_master *master);
bool ezcfg_agent_master_get_socket(struct ezcfg_agent_master *master, struct ezcfg_socket *sp, int wait_time);
int ezcfg_agent_master_get_shm_id(struct ezcfg_agent_master *master);
int ezcfg_agent_master_get_sq_len(struct ezcfg_agent_master *master);
struct ezcfg_socket *ezcfg_agent_master_get_listening_sockets(struct ezcfg_agent_master *master);
struct ezcfg_socket **ezcfg_agent_master_get_p_listening_sockets(struct ezcfg_agent_master *master);

/* agent/agent_master_load_common_conf.c */
void ezcfg_agent_master_load_common_conf(struct ezcfg_agent_master *master);

/* agent/agent_master_load_socket_conf.c */
void ezcfg_agent_master_load_socket_conf(struct ezcfg_agent_master *master);

/* agent/agent_worker.c */
/* worker inherits master's resource */
void ezcfg_agent_worker_delete(struct ezcfg_agent_worker *worker);
struct ezcfg_agent_worker *ezcfg_agent_worker_new(struct ezcfg_agent_master *master);
pthread_t *ezcfg_agent_worker_get_p_thread_id(struct ezcfg_agent_worker *worker);
void ezcfg_agent_worker_close_connection(struct ezcfg_agent_worker *worker);
struct ezcfg_agent_worker *ezcfg_agent_worker_get_next(struct ezcfg_agent_worker *worker);
bool ezcfg_agent_worker_set_next(struct ezcfg_agent_worker *worker, struct ezcfg_agent_worker *next);
void ezcfg_agent_worker_routine(struct ezcfg_agent_worker *worker);
/* worker call this to notify master */
void ezcfg_agent_master_stop_worker(struct ezcfg_agent_master *master, struct ezcfg_agent_worker *worker);
struct ezcfg *ezcfg_agent_worker_get_ezcfg(struct ezcfg_agent_worker *worker);
struct ezcfg_agent_master *ezcfg_agent_worker_get_master(struct ezcfg_agent_worker *worker);
int ezcfg_agent_worker_printf(struct ezcfg_agent_worker *worker, const char *fmt, ...);
int ezcfg_agent_worker_write(struct ezcfg_agent_worker *worker, const char *buf, int len);
void *ezcfg_agent_worker_get_proto_data(struct ezcfg_agent_worker *worker);
struct ezcfg_socket *ezcfg_agent_worker_get_socket(struct ezcfg_agent_worker *worker);
bool ezcfg_agent_worker_set_num_bytes_sent(struct ezcfg_agent_worker *worker, int64_t num);
bool ezcfg_agent_worker_set_birth_time(struct ezcfg_agent_worker *worker, time_t time);

/* agent/agent_worker_ctrl.c */
void ezcfg_agent_worker_process_ctrl_new_connection(struct ezcfg_agent_worker *worker);

/* agent/agent_worker_soap_http.c */
void ezcfg_agent_worker_process_soap_http_new_connection(struct ezcfg_agent_worker *worker);

/* agent/agent_worker_json_http.c */
void ezcfg_agent_worker_process_json_http_new_connection(struct ezcfg_agent_worker *worker);

/* agent/agent_worker_nv_json_http.c */
void ezcfg_agent_worker_process_nv_json_http_new_connection(struct ezcfg_agent_worker *worker);

#endif /* _EZCFG_PRIV_AGENT_H_ */
