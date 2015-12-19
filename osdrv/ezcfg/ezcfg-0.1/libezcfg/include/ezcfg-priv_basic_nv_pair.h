/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_basic_nv_pair.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2015-06-10   0.1       Split it from ezcfg-priv_common.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_BASIC_NV_PAIR_H_
#define _EZCFG_PRIV_BASIC_NV_PAIR_H_

/* basic/nv_pair/nv_pair.c */
struct ezcfg_nv_pair *ezcfg_nv_pair_new(char *name, char *value);
int ezcfg_nv_pair_del(struct ezcfg_nv_pair *nvp);
int ezcfg_nv_pair_del(struct ezcfg_nv_pair *nvp);
int ezcfg_nv_pair_cmp_n(struct ezcfg_nv_pair *nvp1, struct ezcfg_nv_pair *nvp2);
int ezcfg_nv_pair_cmp_v(struct ezcfg_nv_pair *nvp1, struct ezcfg_nv_pair *nvp2);

char *ezcfg_nv_pair_get_n(struct ezcfg_nv_pair *nvp);
char *ezcfg_nv_pair_get_v(struct ezcfg_nv_pair *nvp);

int ezcfg_nv_pair_get_nlen(struct ezcfg_nv_pair *nvp);
int ezcfg_nv_pair_get_vlen(struct ezcfg_nv_pair *nvp);

int ezcfg_nv_pair_del_handler(void *data);
int ezcfg_nv_pair_cmp_handler(const void *d1, const void *d2);

#endif /* _EZCFG_PRIV_BASIC_NV_PAIR_H_ */
