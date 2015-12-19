/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_avl_tree.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2014-03-18   0.1       Split it from ezcfg-priv_common.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_AVL_TREE_H_
#define _EZCFG_PRIV_AVL_TREE_H_

#include "ezcfg-types.h"


/* tree/avl_tree.c */
struct ezcfg_avl_tree;
struct ezcfg_avl_tree *ezcfg_avl_tree_new(struct ezcfg *ezcfg);
void ezcfg_avl_tree_delete(struct ezcfg_avl_tree *tree);
void ezcfg_avl_tree_set_data_delete_handler(struct ezcfg_avl_tree *tree, int (*handler)(void *));

#endif /* _EZCFG_PRIV_AVL_TREE_H_ */
