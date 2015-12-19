/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_igrs.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-14   0.1       Split it from ezcfg-private.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_IGRS_H_
#define _EZCFG_PRIV_IGRS_H_

#include "ezcfg-types.h"

/* igrs/igrs.c */
void ezcfg_igrs_delete(struct ezcfg_igrs *igrs);
struct ezcfg_igrs *ezcfg_igrs_new(struct ezcfg *ezcfg);
bool ezcfg_igrs_set_version_major(struct ezcfg_igrs *igrs, unsigned short major);
bool ezcfg_igrs_set_version_minor(struct ezcfg_igrs *igrs, unsigned short minor);
int ezcfg_igrs_get_message_length(struct ezcfg_igrs *igrs);
unsigned short ezcfg_igrs_set_message_type(struct ezcfg_igrs *igrs, const char *type);
bool ezcfg_igrs_set_source_device_id(struct ezcfg_igrs *igrs, const char *uuid_str);
char *ezcfg_igrs_get_source_device_id(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_target_device_id(struct ezcfg_igrs *igrs, const char *uuid_str);
char *ezcfg_igrs_get_target_device_id(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_sequence_id(struct ezcfg_igrs *igrs, unsigned int seq_id);
unsigned int ezcfg_igrs_get_sequence_id(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_source_user_id(struct ezcfg_igrs *igrs, const char *user_id);
char *ezcfg_igrs_get_source_user_id(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_service_security_id(struct ezcfg_igrs *igrs, const char *security_id);
char *ezcfg_igrs_get_service_security_id(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_source_client_id(struct ezcfg_igrs *igrs, unsigned int client_id);
unsigned int ezcfg_igrs_get_source_client_id(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_source_service_id(struct ezcfg_igrs *igrs, unsigned int service_id);
unsigned int ezcfg_igrs_get_source_service_id(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_target_service_id(struct ezcfg_igrs *igrs, unsigned int service_id);
unsigned int ezcfg_igrs_get_target_service_id(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_invoke_args(struct ezcfg_igrs *igrs, const char *invoke_args);
char *ezcfg_igrs_get_invoke_args(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_build_message(struct ezcfg_igrs *igrs);
int ezcfg_igrs_write_message(struct ezcfg_igrs *igrs, char *buf, int len);
void ezcfg_igrs_dump(struct ezcfg_igrs *igrs);
struct ezcfg_soap *ezcfg_igrs_get_soap(struct ezcfg_igrs *igrs);
struct ezcfg_http *ezcfg_igrs_get_http(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_message_type_ops(struct ezcfg_igrs *igrs, const struct ezcfg_igrs_msg_op *message_type_ops, unsigned short num_message_types);
unsigned short ezcfg_igrs_get_version_major(struct ezcfg_igrs *igrs);
unsigned short ezcfg_igrs_get_version_minor(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_target_client_id(struct ezcfg_igrs *igrs, unsigned int client_id);
unsigned int ezcfg_igrs_get_target_client_id(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_acknowledge_id(struct ezcfg_igrs *igrs, unsigned int ack_id);
unsigned int ezcfg_igrs_get_acknowledge_id(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_return_code(struct ezcfg_igrs *igrs, unsigned short code);
unsigned short ezcfg_igrs_get_return_code(struct ezcfg_igrs *igrs);
int ezcfg_igrs_handle_message(struct ezcfg_igrs *igrs);
char *ezcfg_igrs_get_http_header_value(struct ezcfg_igrs *igrs, char *name);
void ezcfg_igrs_reset_attributes(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_parse_header(struct ezcfg_igrs *igrs, char *buf, int len);
bool ezcfg_igrs_parse_message_body(struct ezcfg_igrs *igrs);
char *ezcfg_igrs_set_message_body(struct ezcfg_igrs *igrs, const char *body, int len);
int ezcfg_igrs_http_get_message_length(struct ezcfg_igrs *igrs);
int ezcfg_igrs_http_write_message(struct ezcfg_igrs *igrs, char *buf, int len);
void ezcfg_igrs_list_delete(struct ezcfg_igrs **list);

#endif /* _EZCFG_PRIV_IGRS_H_ */
