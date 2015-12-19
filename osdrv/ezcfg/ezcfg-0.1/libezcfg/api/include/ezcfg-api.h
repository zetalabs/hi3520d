#ifndef _EZCFG_API_H_
#define _EZCFG_API_H_

/* common function interface */
struct ezcfg;
bool ezcfg_api_common_initialized(void);
char *ezcfg_api_common_get_config_file(void);
int ezcfg_api_common_set_config_file(const char *path);
char *ezcfg_api_common_get_config_file_content(void);
struct ezcfg *ezcfg_api_common_new(char *path);
void ezcfg_api_common_del(struct ezcfg *ezcfg);
char *ezcfg_api_common_get_root_path(struct ezcfg *ezcfg);
char *ezcfg_api_common_get_sem_ezcfg_path(struct ezcfg *ezcfg);
char *ezcfg_api_common_get_shm_ezcfg_path(struct ezcfg *ezcfg);
int ezcfg_api_common_get_shm_ezcfg_size(struct ezcfg *ezcfg, size_t *psize);
int ezcfg_api_common_increase_shm_ezcfg_nvram_queue_num(struct ezcfg *ezcfg);
int ezcfg_api_common_decrease_shm_ezcfg_nvram_queue_num(struct ezcfg *ezcfg);
int ezcfg_api_common_increase_shm_ezcfg_rc_queue_num(struct ezcfg *ezcfg);
int ezcfg_api_common_decrease_shm_ezcfg_rc_queue_num(struct ezcfg *ezcfg);
#if (HAVE_EZBOX_SERVICE_EZCTP == 1)
char *ezcfg_api_common_get_shm_ezctp_path(struct ezcfg *ezcfg);
int ezcfg_api_common_get_shm_ezctp_size(struct ezcfg *ezcfg, size_t *psize);
#endif

/* common utilities */
int ezcfg_api_util_get_conf_string(const char *path,
        const char *section, const int idx, const char *keyword,
        char *buf, size_t len);

/* Agent interface */
int ezcfg_api_agent_start(char *init_conf);
int ezcfg_api_agent_stop(char *init_conf);
int ezcfg_api_agent_reload(char *init_conf);
int ezcfg_api_agent_set_debug(char *init_conf, bool flag);

#if 0
/* Master thread interface */
struct ezcfg_master;
struct ezcfg_master *ezcfg_api_master_start(const char *name, int threads_max);
int ezcfg_api_master_stop(struct ezcfg_master *master);
int ezcfg_api_master_reload(struct ezcfg_master *master);

/* CTRL interface */
int ezcfg_api_ctrl_exec(char *const argv[], char *output, size_t len);

/* IPC interface */
int ezcfg_api_ipc_get_ezcfg_shm_id(int *shm_id);

/* function argument interface */
struct ezcfg_arg_nvram_socket;
struct ezcfg_arg_nvram_socket *ezcfg_api_arg_nvram_socket_new(void);
int ezcfg_api_arg_nvram_socket_del(struct ezcfg_arg_nvram_socket *ap);
int ezcfg_api_arg_nvram_socket_get_domain(struct ezcfg_arg_nvram_socket *ap, char **pp);
int ezcfg_api_arg_nvram_socket_set_domain(struct ezcfg_arg_nvram_socket *ap, const char *domain);
int ezcfg_api_arg_nvram_socket_get_type(struct ezcfg_arg_nvram_socket *ap, char **pp);
int ezcfg_api_arg_nvram_socket_set_type(struct ezcfg_arg_nvram_socket *ap, const char *type);
int ezcfg_api_arg_nvram_socket_get_protocol(struct ezcfg_arg_nvram_socket *ap, char **pp);
int ezcfg_api_arg_nvram_socket_set_protocol(struct ezcfg_arg_nvram_socket *ap, const char *protocol);
int ezcfg_api_arg_nvram_socket_get_address(struct ezcfg_arg_nvram_socket *ap, char **pp);
int ezcfg_api_arg_nvram_socket_set_address(struct ezcfg_arg_nvram_socket *ap, const char *address);

struct ezcfg_arg_nvram_ssl;
struct ezcfg_arg_nvram_ssl *ezcfg_api_arg_nvram_ssl_new(void);
int ezcfg_api_arg_nvram_ssl_del(struct ezcfg_arg_nvram_ssl *ap);
int ezcfg_api_arg_nvram_ssl_get_role(struct ezcfg_arg_nvram_ssl *ap, char **pp);
int ezcfg_api_arg_nvram_ssl_set_role(struct ezcfg_arg_nvram_ssl *ap, const char *role);
int ezcfg_api_arg_nvram_ssl_get_method(struct ezcfg_arg_nvram_ssl *ap, char **pp);
int ezcfg_api_arg_nvram_ssl_set_method(struct ezcfg_arg_nvram_ssl *ap, const char *method);
int ezcfg_api_arg_nvram_ssl_get_socket_enable(struct ezcfg_arg_nvram_ssl *ap, char **pp);
int ezcfg_api_arg_nvram_ssl_set_socket_enable(struct ezcfg_arg_nvram_ssl *ap, const char *socket_enable);
int ezcfg_api_arg_nvram_ssl_get_socket_domain(struct ezcfg_arg_nvram_ssl *ap, char **pp);
int ezcfg_api_arg_nvram_ssl_set_socket_domain(struct ezcfg_arg_nvram_ssl *ap, const char *socket_domain);
int ezcfg_api_arg_nvram_ssl_get_socket_type(struct ezcfg_arg_nvram_ssl *ap, char **pp);
int ezcfg_api_arg_nvram_ssl_set_socket_type(struct ezcfg_arg_nvram_ssl *ap, const char *socket_type);
int ezcfg_api_arg_nvram_ssl_get_socket_protocol(struct ezcfg_arg_nvram_ssl *ap, char **pp);
int ezcfg_api_arg_nvram_ssl_set_socket_protocol(struct ezcfg_arg_nvram_ssl *ap, const char *socket_protocol);
int ezcfg_api_arg_nvram_ssl_get_socket_address(struct ezcfg_arg_nvram_ssl *ap, char **pp);
int ezcfg_api_arg_nvram_ssl_set_socket_address(struct ezcfg_arg_nvram_ssl *ap, const char *socket_address);
int ezcfg_api_arg_nvram_ssl_get_certificate_file(struct ezcfg_arg_nvram_ssl *ap, char **pp);
int ezcfg_api_arg_nvram_ssl_set_certificate_file(struct ezcfg_arg_nvram_ssl *ap, const char *file);
int ezcfg_api_arg_nvram_ssl_get_certificate_chain_file(struct ezcfg_arg_nvram_ssl *ap, char **pp);
int ezcfg_api_arg_nvram_ssl_set_certificate_chain_file(struct ezcfg_arg_nvram_ssl *ap, const char *file);
int ezcfg_api_arg_nvram_ssl_get_private_key_file(struct ezcfg_arg_nvram_ssl *ap, char **pp);
int ezcfg_api_arg_nvram_ssl_set_private_key_file(struct ezcfg_arg_nvram_ssl *ap, const char *file);

/* NVRAM interface */
int ezcfg_api_nvram_get(const char *name, char *value, size_t len);
int ezcfg_api_nvram_set(const char *name, const char *value);
int ezcfg_api_nvram_unset(const char *name);
int ezcfg_api_nvram_set_multi(char *list, const int num);
int ezcfg_api_nvram_list(char *list, size_t len);
int ezcfg_api_nvram_info(char *list, size_t len);
int ezcfg_api_nvram_commit(void);
void ezcfg_api_nvram_set_debug(bool enable_debug);
int ezcfg_api_nvram_insert_socket(struct ezcfg_arg_nvram_socket *ap);
int ezcfg_api_nvram_remove_socket(struct ezcfg_arg_nvram_socket *ap);
int ezcfg_api_nvram_insert_ssl(struct ezcfg_arg_nvram_ssl *ap);
int ezcfg_api_nvram_remove_ssl(struct ezcfg_arg_nvram_ssl *ap);
#endif

int ezcfg_api_nvram_change(char *init_conf, char *ns, char *nv_json, char **presult);

#if 0
/* ezctp interface */
#if (HAVE_EZBOX_SERVICE_EZCTP == 1)
bool ezcfg_api_ezctp_require_semaphore(char *sem_ezcfg_path);
bool ezcfg_api_ezctp_release_semaphore(char *sem_ezcfg_path);
bool ezcfg_api_ezctp_insert_market_data(void *shm_ezcfg_addr, const void *data, size_t n, size_t size);
bool ezcfg_api_ezctp_remove_market_data(void *shm_ezcfg_addr, void **pdata, size_t *n, size_t *psize);
bool ezcfg_api_ezctp_save_market_data(void *shm_ezcfg_addr, FILE *fp, size_t size, int flag);
#endif

/* rc interface */
int ezcfg_api_rc_require_semaphore(char *sem_ezcfg_path);
int ezcfg_api_rc_release_semaphore(char *sem_ezcfg_path);

/* UUID interface */
int ezcfg_api_uuid1_string(char *str, int len);
int ezcfg_api_uuid3_string(char *str, int len);
int ezcfg_api_uuid4_string(char *str, int len);
int ezcfg_api_uuid5_string(char *str, int len);

/* UPnP interface */
int ezcfg_api_upnp_set_task_file(const char *path);
int ezcfg_api_upnp_get_task_file(char *path, int len);
bool ezcfg_api_upnp_lock_task_file(char *sem_ezcfg_path);
bool ezcfg_api_upnp_unlock_task_file(char *sem_ezcfg_path);

/* u-boot-env interface */
int ezcfg_api_ubootenv_get(char *name, char *value, size_t len);
int ezcfg_api_ubootenv_set(char *name, char *value);
int ezcfg_api_ubootenv_list(char *list, size_t len);
int ezcfg_api_ubootenv_check(void);
size_t ezcfg_api_ubootenv_size(void);

/* firmware interface */
int ezcfg_api_firmware_upgrade(char *name, char *model);
#endif

#endif
