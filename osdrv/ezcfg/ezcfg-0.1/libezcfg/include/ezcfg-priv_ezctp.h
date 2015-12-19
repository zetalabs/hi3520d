/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_ezctp.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-03-13   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_EZCTP_H_
#define _EZCFG_PRIV_EZCTP_H_

#include "ezcfg-types.h"

/* ezctp/ezctp.c */
bool ezcfg_ezctp_delete(struct ezcfg_ezctp *ezctp);
struct ezcfg_ezctp *ezcfg_ezctp_new(struct ezcfg *ezcfg);
int ezcfg_ezctp_get_shm_id(struct ezcfg_ezctp *ezctp);
size_t ezcfg_ezctp_get_cq_unit_size(struct ezcfg_ezctp *ezctp);
bool ezcfg_ezctp_insert_data(struct ezcfg_ezctp *ezctp, void *data, size_t n, size_t size);

#endif /* _EZCFG_PRIV_EZCTP_H_ */
