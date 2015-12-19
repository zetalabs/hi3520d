/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_basic_stack_list.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2014-03-18   0.1       Split it from ezcfg-priv_common.h
 * 2015-06-12   0.2       Rename it to ezcfg-priv_basic_stack_list.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_BASIC_STACK_LIST_H_
#define _EZCFG_PRIV_BASIC_STACK_LIST_H_

#include "ezcfg-types.h"


/* basic/stack_list/stack_list.c */
struct ezcfg_stack_list;
struct ezcfg_stack_list *ezcfg_stack_list_new(struct ezcfg *ezcfg);
int ezcfg_stack_list_del(struct ezcfg_stack_list *list);

int ezcfg_stack_list_set_data_del_handler(struct ezcfg_stack_list *list, int (*handler)(void *));
bool ezcfg_stack_list_push(struct ezcfg_stack_list *list, void *data);
void *ezcfg_stack_list_pop(struct ezcfg_stack_list *list);
int ezcfg_stack_list_get_length(struct ezcfg_stack_list *list);
bool ezcfg_stack_list_is_empty(struct ezcfg_stack_list *list);

#endif /* _EZCFG_PRIV_BASIC_STACK_LIST_H_ */
