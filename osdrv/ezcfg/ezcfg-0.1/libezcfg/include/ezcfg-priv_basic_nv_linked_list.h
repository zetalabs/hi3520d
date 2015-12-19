/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_basic_nv_linked_list.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2015-06-12   0.1       Split it ezcfg-priv_common.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_BASIC_NV_LINKED_LIST_H_
#define _EZCFG_PRIV_BASIC_NV_LINKED_LIST_H_

struct ezcfg_nv_linked_list;
struct ezcfg_nv_linked_list *ezcfg_nv_linked_list_new(struct ezcfg *ezcfg);
int ezcfg_nv_linked_list_del(struct ezcfg_nv_linked_list *list);

int ezcfg_nv_linked_list_insert(struct ezcfg_nv_linked_list *list, char *name, char *value);
int ezcfg_nv_linked_list_append(struct ezcfg_nv_linked_list *list, char *name, char *value);
int ezcfg_nv_linked_list_remove(struct ezcfg_nv_linked_list *list, char *name);
bool ezcfg_nv_linked_list_in(struct ezcfg_nv_linked_list *list, char *name);
int ezcfg_nv_linked_list_get_length(struct ezcfg_nv_linked_list *list);
char *ezcfg_nv_linked_list_get_node_name_by_index(struct ezcfg_nv_linked_list *list, int i);
char *ezcfg_nv_linked_list_get_node_value_by_index(struct ezcfg_nv_linked_list *list, int i);
char *ezcfg_nv_linked_list_get_node_value_by_name(struct ezcfg_nv_linked_list *list, char *name);

#endif /* _EZCFG_PRIV_BASIC_NV_LINKED_LIST_H_ */
