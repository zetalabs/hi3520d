/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_basic_binary_tree.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2014-03-18   0.1       Split it from ezcfg-priv_common.h
 * 2015-06-12   0.2       Rename it to ezcfg-priv_basic_binary_tree.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_BASIC_BINARY_TREE_H_
#define _EZCFG_PRIV_BASIC_BINARY_TREE_H_

#include "ezcfg-types.h"


/* basic/binary_tree/binary_tree.c */
struct ezcfg_binary_tree_node *ezcfg_binary_tree_node_new(struct ezcfg *ezcfg, void *data);
int ezcfg_binary_tree_node_del(struct ezcfg_binary_tree *tree, struct ezcfg_binary_tree_node *np);

void *ezcfg_binary_tree_node_get_data(struct ezcfg_binary_tree_node *np);
struct ezcfg_binary_tree_node *ezcfg_binary_tree_node_get_left(struct ezcfg_binary_tree_node *np);
struct ezcfg_binary_tree_node *ezcfg_binary_tree_node_get_right(struct ezcfg_binary_tree_node *np);
int ezcfg_binary_tree_node_append_left(struct ezcfg_binary_tree_node *np, struct ezcfg_binary_tree_node *np_left);
int ezcfg_binary_tree_node_append_right(struct ezcfg_binary_tree_node *np, struct ezcfg_binary_tree_node *np_right);

struct ezcfg_binary_tree *ezcfg_binary_tree_new(struct ezcfg *ezcfg);
int ezcfg_binary_tree_del(struct ezcfg_binary_tree *tree);

int ezcfg_binary_tree_reset(struct ezcfg_binary_tree *tree);
int ezcfg_binary_tree_set_data_del_handler(struct ezcfg_binary_tree *tree, int (*handler)(void *));
struct ezcfg_binary_tree_node *ezcfg_binary_tree_get_root(struct ezcfg_binary_tree *tree);
int ezcfg_binary_tree_set_root(struct ezcfg_binary_tree *tree, struct ezcfg_binary_tree_node *np);

#endif /* _EZCFG_PRIV_BINARY_TREE_H_ */
