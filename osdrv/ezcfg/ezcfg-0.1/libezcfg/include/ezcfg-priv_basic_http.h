/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_basic_http.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-28   0.1       Split it from ezcfg-private.h
 * 2015-06-10   0.2       Rename it to ezcfg-priv_basic_http.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_BASIC_HTTP_H_
#define _EZCFG_PRIV_BASIC_HTTP_H_

#include "ezcfg-types.h"

/* basic/http/http.c */
int ezcfg_http_del(struct ezcfg_http *http);
struct ezcfg_http *ezcfg_http_new(struct ezcfg *ezcfg);
int ezcfg_http_set_state_request(struct ezcfg_http *http);
int ezcfg_http_set_state_response(struct ezcfg_http *http);
bool ezcfg_http_is_state_request(struct ezcfg_http *http);
bool ezcfg_http_is_state_response(struct ezcfg_http *http);
int ezcfg_http_delete_remote_user(struct ezcfg_http *http);
int ezcfg_http_reset_attributes(struct ezcfg_http *http);
bool ezcfg_http_parse_header(struct ezcfg_http *http, char *buf, int len);
struct ezcfg *ezcfg_http_get_ezcfg(struct ezcfg_http *http);
unsigned short ezcfg_http_get_version_major(struct ezcfg_http *http);
unsigned short ezcfg_http_get_version_minor(struct ezcfg_http *http);
bool ezcfg_http_set_version_major(struct ezcfg_http *http, unsigned short major);
bool ezcfg_http_set_version_minor(struct ezcfg_http *http, unsigned short minor);
bool ezcfg_http_set_method_strings(struct ezcfg_http *http, const char **method_strings, unsigned char num_methods);
bool ezcfg_http_set_known_header_strings(struct ezcfg_http *http, const char **header_strings, unsigned char num_headers);
bool ezcfg_http_set_status_code_maps(struct ezcfg_http *http, const void *maps, unsigned short num_status_codes);
unsigned char ezcfg_http_set_request_method(struct ezcfg_http *http, const char *method);
int ezcfg_http_request_method_cmp(struct ezcfg_http *http, const char *method);
char *ezcfg_http_get_request_uri(struct ezcfg_http *http);
bool ezcfg_http_set_request_uri(struct ezcfg_http *http, const char *uri);
bool ezcfg_http_get_is_ssl(struct ezcfg_http *http);
bool ezcfg_http_set_is_ssl(struct ezcfg_http *http, const bool is_ssl);
unsigned short ezcfg_http_set_status_code(struct ezcfg_http *http, unsigned short status_code);
char *ezcfg_http_set_message_body(struct ezcfg_http *http, const char *body, int len);
char *ezcfg_http_get_message_body(struct ezcfg_http *http);
int ezcfg_http_get_message_body_len(struct ezcfg_http *http);
char *ezcfg_http_get_header_value(struct ezcfg_http *http, char *name);
int ezcfg_http_dump(struct ezcfg_http *http);
int ezcfg_http_get_request_line_length(struct ezcfg_http *http);
int ezcfg_http_write_request_line(struct ezcfg_http *http, char *buf, int len);
int ezcfg_http_get_status_line_length(struct ezcfg_http *http);
int ezcfg_http_write_status_line(struct ezcfg_http *http, char *buf, int len);
int ezcfg_http_get_start_line_length(struct ezcfg_http *http);
int ezcfg_http_write_start_line(struct ezcfg_http *http, char *buf, int len);
int ezcfg_http_get_crlf_length(struct ezcfg_http *http);
int ezcfg_http_write_crlf(struct ezcfg_http *http, char *buf, int len);
int ezcfg_http_get_headers_length(struct ezcfg_http *http);
int ezcfg_http_write_headers(struct ezcfg_http *http, char *buf, int len);
bool ezcfg_http_add_header(struct ezcfg_http *http, char *name, char *value);
int ezcfg_http_write_message_body(struct ezcfg_http *http, char *buf, int len);
int ezcfg_http_get_message_length(struct ezcfg_http *http);
int ezcfg_http_write_message(struct ezcfg_http *http, char *buf, int len);
bool ezcfg_http_parse_auth(struct ezcfg_http *http, struct ezcfg_auth *auth);
bool ezcfg_http_parse_post_data(struct ezcfg_http *http, struct ezcfg_nv_linked_list *list);

#endif /* _EZCFG_PRIV_BASIC_HTTP_H_ */
