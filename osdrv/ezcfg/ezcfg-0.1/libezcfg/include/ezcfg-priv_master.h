/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_master.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-06   0.1       Split it from ezcfg-private.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_MASTER_H_
#define _EZCFG_PRIV_MASTER_H_

/* thread/master.c */
struct ezcfg_master *ezcfg_master_start(struct ezcfg *ezcfg);
void ezcfg_master_stop(struct ezcfg_master *master);
void ezcfg_master_reload(struct ezcfg_master *master);
void ezcfg_master_set_threads_max(struct ezcfg_master *master, int threads_max);
pthread_t *ezcfg_master_get_p_thread_id(struct ezcfg_master *master);
int ezcfg_master_set_receive_buffer_size(struct ezcfg_master *master, int size);
void ezcfg_master_routine(struct ezcfg_master *master);
struct ezcfg *ezcfg_master_get_ezcfg(struct ezcfg_master *master);
bool ezcfg_master_is_stop(struct ezcfg_master *master);
bool ezcfg_master_get_socket(struct ezcfg_master *master, struct ezcfg_socket *sp, int wait_time);
int ezcfg_master_get_shm_id(struct ezcfg_master *master);
struct ezcfg_nvram *ezcfg_master_get_nvram(struct ezcfg_master *master);
struct ezcfg_agent *ezcfg_master_get_agent(struct ezcfg_master *master);
void ezcfg_master_set_agent(struct ezcfg_master *master, struct ezcfg_agent *agent);
int ezcfg_master_get_sq_len(struct ezcfg_master *master);
struct ezcfg_socket *ezcfg_master_get_listening_sockets(struct ezcfg_master *master);
struct ezcfg_socket **ezcfg_master_get_p_listening_sockets(struct ezcfg_master *master);
struct ezcfg_auth *ezcfg_master_get_auths(struct ezcfg_master *master);
struct ezcfg_auth **ezcfg_master_get_p_auths(struct ezcfg_master *master);
int ezcfg_master_auth_mutex_lock(struct ezcfg_master *master);
int ezcfg_master_auth_mutex_unlock(struct ezcfg_master *master);
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
struct ezcfg_ssl *ezcfg_master_get_ssl(struct ezcfg_master *master);
struct ezcfg_ssl **ezcfg_master_get_p_ssl(struct ezcfg_master *master);
int ezcfg_master_ssl_mutex_lock(struct ezcfg_master *master);
int ezcfg_master_ssl_mutex_unlock(struct ezcfg_master *master);
#endif
#if (HAVE_EZBOX_SERVICE_EZCFG_IGRSD == 1)
struct ezcfg_igrs *ezcfg_master_get_igrs(struct ezcfg_master *master);
struct ezcfg_igrs **ezcfg_master_get_p_igrs(struct ezcfg_master *master);
int ezcfg_master_igrs_mutex_lock(struct ezcfg_master *master);
int ezcfg_master_igrs_mutex_unlock(struct ezcfg_master *master);
#endif
#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
struct ezcfg_upnp *ezcfg_master_get_upnp(struct ezcfg_master *master);
struct ezcfg_upnp **ezcfg_master_get_p_upnp(struct ezcfg_master *master);
int ezcfg_master_upnp_mutex_lock(struct ezcfg_master *master);
int ezcfg_master_upnp_mutex_unlock(struct ezcfg_master *master);
#endif

/* thread/master_uevent.c */
bool ezcfg_master_handle_uevent_socket(struct ezcfg_master *master,
	struct ezcfg_socket *listener,
	struct ezcfg_socket *accepted);

/* thread/master_upnp_ssdp.c */
bool ezcfg_master_handle_upnp_ssdp_socket(struct ezcfg_master *master,
	struct ezcfg_socket *listener,
	struct ezcfg_socket *accepted);

/* thread/master_load_common_conf.c */
void ezcfg_master_load_common_conf(struct ezcfg_master *master);

/* thread/master_load_socket_conf.c */
void ezcfg_master_load_socket_conf(struct ezcfg_master *master);

/* thread/master_load_auth_conf.c */
void ezcfg_master_load_auth_conf(struct ezcfg_master *master);

/* thread/master_load_ssl_conf.c */
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
void ezcfg_master_load_ssl_conf(struct ezcfg_master *master);
#endif

/* thread/master_load_igrs_conf.c */
#if (HAVE_EZBOX_SERVICE_EZCFG_IGRSD == 1)
void ezcfg_master_load_igrs_conf(struct ezcfg_master *master);
#endif

/* thread/master_load_upnp_conf.c */
#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
void ezcfg_master_load_upnp_conf(struct ezcfg_master *master);
#endif

#endif /* _EZCFG_PRIV_MASTER_H_ */
