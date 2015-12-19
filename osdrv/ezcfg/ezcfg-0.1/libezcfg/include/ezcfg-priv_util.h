/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_util.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-09   0.1       Split it from ezcfg-private.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_UTIL_H_
#define _EZCFG_PRIV_UTIL_H_

#include "ezcfg-types.h"
/* for INET_ADDRSTRLEN */
#include <netinet/in.h>
/* for IFNAMSIZ */
#include <net/if.h>

/* util/util.c */
#define UTIL_PATH_SIZE				1024
#define UTIL_NAME_SIZE				512
#define UTIL_LINE_SIZE				16384
#define EZCFG_ALLOWED_CHARS_INPUT		"/ $%?,"

int ezcfg_util_log_priority(const char *priority);
void ezcfg_util_remove_trailing_char(char *s, char c);
void ezcfg_util_remove_trailing_charlist(char *s, char *l);
char *ezcfg_util_skip_leading_char(char *s, char c);
char *ezcfg_util_skip_leading_charlist(char *s, char *l);

/* util/util_adler32.c */
uint32_t ezcfg_util_adler32(unsigned char *data, size_t len);

/* util/util_base64.c */
int ezcfg_util_base64_encode(unsigned char *src, unsigned char *dst, size_t src_len, size_t dst_len);
int ezcfg_util_base64_decode(unsigned char *src, unsigned char *dst, size_t src_len, size_t dst_len);

/* util/util_sha1.c */
int ezcfg_util_sha1(unsigned char *src, unsigned char *dst, size_t src_len, size_t dst_len);

/* util/util_crc32.c */
uint32_t ezcfg_util_crc32(unsigned char *data, size_t len);

/* util/util_conf.c */
char *ezcfg_util_get_conf_string(const char *path, const char *section, const int index, const char *keyword);

/* util/util_parse_args.c */
int ezcfg_util_parse_args(char *buf, size_t size, char **argv);

/* util/util_tzdata.c */
int ezcfg_util_tzdata_get_area_length(void);
char *ezcfg_util_tzdata_get_area_name_by_index(int i);
char *ezcfg_util_tzdata_get_area_desc_by_index(int i);
char *ezcfg_util_tzdata_get_area_desc_by_name(char *name);
int ezcfg_util_tzdata_get_location_length(char *area);
char *ezcfg_util_tzdata_get_location_name_by_index(char *area, int i);
char *ezcfg_util_tzdata_get_location_desc_by_index(char *area, int i);
char *ezcfg_util_tzdata_get_location_desc_by_name(char *area, char *name);
bool ezcfg_util_tzdata_check_area_location(char *area, char *location);

/* util/util_file_get_line.c */
bool ezcfg_util_file_get_line(FILE *fp, char *buf, size_t size, char *comment, char *rm);

/* util/util_file_extension.c */
int ezcfg_util_file_get_type_extension_index(const char *path);

/* util/util_javascript_var_escaped.c */
char *ezcfg_util_javascript_var_escaped(const char *src);

/* util/util_url.c */
size_t ezcfg_util_url_decode(const char *src, size_t src_len,
        char *dst, size_t dst_len, int is_form_url_encoded);
void ezcfg_util_url_remove_double_dots_and_double_slashes(char *s);

/* util/util_socket_protocol.c */
int ezcfg_util_socket_protocol_get_index(char *name);
bool ezcfg_util_socket_is_supported_protocol(const int proto);

/* util/util_socket_domain.c */
int ezcfg_util_socket_domain_get_index(char *name);
bool ezcfg_util_socket_is_supported_domain(const int domain);

/* util/util_socket_type.c */
int ezcfg_util_socket_type_get_index(char *name);
bool ezcfg_util_socket_is_supported_type(const int type);

/* util/util_socket_mcast.c */
bool ezcfg_util_socket_is_multicast_address(const int proto, const char *addr);

#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
/* util/util_upnp_role.c */
int ezcfg_util_upnp_role(char *name);

/* util/util_upnp_type.c */
int ezcfg_util_upnp_device_type(char *name);
char *ezcfg_util_upnp_get_device_type_description_path(int type);

/* util/util_upnp_monitor.c */
int ezcfg_util_upnp_monitor_set_task_file(const char *path);
int ezcfg_util_upnp_monitor_get_task_file(char *path, size_t len);
bool ezcfg_util_upnp_monitor_lock_task_file(char *sem_ezcfg_path);
bool ezcfg_util_upnp_monitor_unlock_task_file(char *sem_ezcfg_path);
#endif

/* util/util_language.c */
int ezcfg_util_lang_get_length(void);
char *ezcfg_util_lang_get_name_by_index(int i);
char *ezcfg_util_lang_get_desc_by_index(int i);
char *ezcfg_util_lang_get_desc_by_name(char *name);

/* util/util_wan.c */
size_t ezcfg_util_wan_get_type_length(void);
char *ezcfg_util_wan_get_type_name_by_index(int i);
char *ezcfg_util_wan_get_type_desc_by_index(int i);
char *ezcfg_util_wan_get_type_desc_by_name(char *name);

/* util/util_text.c */
char *ezcfg_util_text_get_mtu_enable(int i);
char *ezcfg_util_text_get_keep_enable(int i);
char *ezcfg_util_text_get_service_switch(bool v);

/* util/util_execute.c */
int ezcfg_util_execute(char *const argv[], char *in_path, char *out_path, int timeout, int *ppid);

/* util/util_rc.c */
bool ezcfg_util_rc(char *func, char *act, int s);
bool ezcfg_util_rc_list(struct ezcfg_rc_triple *list, char *func, char *act, int s);

/* util/util_service_binding.c */
int ezcfg_util_service_binding(char *interface);

/* util/util_if_ipaddr.c */
bool ezcfg_util_if_get_ipaddr(const char ifname[IFNAMSIZ], char ip[INET_ADDRSTRLEN]);

/* util/util_mkdir.c */
int ezcfg_util_mkdir(const char *path, mode_t mode, bool is_dir);

/* util/util_snprintf_ns_name.c */
int ezcfg_util_snprintf_ns_name(char *buf, size_t size, char *ns, char *name);

#endif /* _EZCFG_PRIV_UTIL_H_ */
