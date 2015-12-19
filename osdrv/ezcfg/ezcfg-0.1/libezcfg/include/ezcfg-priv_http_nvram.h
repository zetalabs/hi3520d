/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_http_nvram.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-12   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_HTTP_NVRAM_H_
#define _EZCFG_PRIV_HTTP_NVRAM_H_

#include "ezcfg-types.h"

/* http/http_nvram.c */
struct ezcfg_http_nvram;
void ezcfg_http_nvram_delete(struct ezcfg_http_nvram *hn);
struct ezcfg_http_nvram *ezcfg_http_nvram_new(struct ezcfg *ezcfg);
bool ezcfg_http_nvram_set_http(struct ezcfg_http_nvram *hn, struct ezcfg_http *http);
bool ezcfg_http_nvram_set_nvram(struct ezcfg_http_nvram *hn, struct ezcfg_nvram *nvram);
bool ezcfg_http_nvram_set_content_type(struct ezcfg_http_nvram *hn, const int type);
int ezcfg_http_nvram_get_content_type(struct ezcfg_http_nvram *hn);
bool ezcfg_http_nvram_set_root(struct ezcfg_http_nvram *hn, const char *root);
bool ezcfg_http_nvram_set_path(struct ezcfg_http_nvram *hn, const char *path);
int ezcfg_http_handle_nvram_request(struct ezcfg_http_nvram *hn);

#endif /* _EZCFG_PRIV_HTTP_NVRAM_H_ */
