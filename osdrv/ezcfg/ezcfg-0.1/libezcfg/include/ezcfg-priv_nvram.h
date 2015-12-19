/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_nvram.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-02-10   0.1       Split it from ezcfg-private.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_NVRAM_H_
#define _EZCFG_PRIV_NVRAM_H_

#include "ezcfg-types.h"

#if 0
/* nvram/nvram.c */
int ezcfg_nvram_delete(struct ezcfg_nvram *nvram);
struct ezcfg_nvram *ezcfg_nvram_new(struct ezcfg *ezcfg);
int ezcfg_nvram_set_backend_type(struct ezcfg_nvram *nvram, const int index, const int type);
int ezcfg_nvram_set_coding_type(struct ezcfg_nvram *nvram, const int index, const int type);
int ezcfg_nvram_set_total_space(struct ezcfg_nvram *nvram, const int total_space);
int ezcfg_nvram_get_total_space(struct ezcfg_nvram *nvram);
int ezcfg_nvram_get_free_space(struct ezcfg_nvram *nvram);
int ezcfg_nvram_get_used_space(struct ezcfg_nvram *nvram);
int ezcfg_nvram_get_version_string(struct ezcfg_nvram *nvram, char *buf, size_t len);
int ezcfg_nvram_set_entry(struct ezcfg_nvram *nvram, const char *name, const char *value);
int ezcfg_nvram_get_entry_value(struct ezcfg_nvram *nvram, const char *name, char **value);
int ezcfg_nvram_unset_entry(struct ezcfg_nvram *nvram, const char *name);
int ezcfg_nvram_commit(struct ezcfg_nvram *nvram);
int ezcfg_nvram_initialize(struct ezcfg_nvram *nvram);
int ezcfg_nvram_match_entry(struct ezcfg_nvram *nvram, char *name1, char *name2);
int ezcfg_nvram_match_entry_value(struct ezcfg_nvram *nvram, char *name, char *value);
int ezcfg_nvram_is_valid_entry_value(struct ezcfg_nvram *nvram, char *name, char *value);
int ezcfg_nvram_get_entries(struct ezcfg_nvram *nvram, struct ezcfg_linked_list *list);
int ezcfg_nvram_set_entries(struct ezcfg_nvram *nvram, struct ezcfg_linked_list *list);
int ezcfg_nvram_set_multi_entries(struct ezcfg_nvram *nvram, struct ezcfg_link_list *list);
int ezcfg_nvram_unset_multi_entries(struct ezcfg_nvram *nvram, struct ezcfg_link_list *list);
#endif

#endif /* _EZCFG_PRIV_NVRAM_H_ */
