/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_basic_auth.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-02-08   0.1       Split it from ezcfg-private.h
 * 2015-06-10   0.2       Rename it to ezcfg-priv_basic_auth.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_BASIC_AUTH_H_
#define _EZCFG_PRIV_BASIC_AUTH_H_

#include "ezcfg-types.h"

/* basic/auth/auth.c */
struct ezcfg_auth * ezcfg_auth_new(struct ezcfg *ezcfg);
int ezcfg_auth_delete(struct ezcfg_auth *auth);
bool ezcfg_auth_set_type(struct ezcfg_auth *auth, char *type);
bool ezcfg_auth_set_user(struct ezcfg_auth *auth, char *user);
bool ezcfg_auth_set_realm(struct ezcfg_auth *auth, char *realm);
bool ezcfg_auth_set_domain(struct ezcfg_auth *auth, char *domain);
bool ezcfg_auth_set_secret(struct ezcfg_auth *auth, char *secret);
bool ezcfg_auth_is_valid(struct ezcfg_auth *auth);
bool ezcfg_auth_list_in(struct ezcfg_auth **list, struct ezcfg_auth *auth);
bool ezcfg_auth_list_insert(struct ezcfg_auth **list, struct ezcfg_auth *auth);
void ezcfg_auth_list_delete(struct ezcfg_auth **list);
bool ezcfg_auth_check_authorized(struct ezcfg_auth **list, struct ezcfg_auth *auth);

#endif /* _EZCFG_PRIV_BASIC_AUTH_H_ */
