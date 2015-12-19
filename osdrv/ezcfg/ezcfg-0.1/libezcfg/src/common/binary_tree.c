/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : common/binary_tree.c
 *
 * Description  : linked binary tree represence of tree
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2015-06-09   0.1       Copy from tree/binary_tree.c
 * ============================================================================
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#include "ezcfg_local.h"

struct binary_tree_node {
  void *data;
  struct binary_tree_node *up;
  struct binary_tree_node *left;
  struct binary_tree_node *right;
};

struct binary_tree {
  struct binary_tree_node *root;
  int (*data_delete_handler)(void *data);
  int (*data_compare_handler)(const void *d1, const void *d2);
};

/*
 * public functions
 */
struct binary_tree_node *local_binary_tree_node_new(void *data)
{
  struct binary_tree_node *np;

  np = calloc(1, sizeof(struct binary_tree_node));
  if (np == NULL) {
    return NULL;
  }
  np->data = data;
  return np;
}

int local_binary_tree_node_del(struct binary_tree *tree, struct binary_tree_node *np)
{
  ASSERT(tree != NULL);
  ASSERT(np != NULL);

  if (tree->data_delete_handler) {
    tree->data_delete_handler(np->data);
  }
  free(np);
  return EZCFG_RET_OK;
}

void *local_binary_tree_node_get_data(struct binary_tree_node *np)
{
  ASSERT(np != NULL);

  return np->data;
}

struct binary_tree_node *local_binary_tree_node_get_left(struct binary_tree_node *np)
{
  ASSERT(np != NULL);

  return np->left;
}

struct binary_tree_node *local_binary_tree_node_get_right(struct binary_tree_node *np)
{
  ASSERT(np != NULL);

  return np->right;
}

int local_binary_tree_node_append_left(struct binary_tree_node *np, struct binary_tree_node *np_left)
{
  struct binary_tree_node *tmp_np;

  ASSERT(np != NULL);
  ASSERT(np_left != NULL);

  if (np->left == NULL) {
    np->left = np_left;
    return EZCFG_RET_OK;
  }

  tmp_np = np->left;
  while (tmp_np->right != NULL) {
    tmp_np = tmp_np->right;
  }
  tmp_np->right = np_left;
  return EZCFG_RET_OK;
}

int local_binary_tree_node_append_right(struct binary_tree_node *np, struct binary_tree_node *np_right)
{
  struct binary_tree_node *tmp_np;

  ASSERT(np != NULL);
  ASSERT(np_right != NULL);

  tmp_np = np;
  while (tmp_np->right != NULL) {
    tmp_np = tmp_np->right;
  }
  tmp_np->right = np_right;
  return EZCFG_RET_OK;
}

struct binary_tree *local_binary_tree_new(void)
{
  struct binary_tree *tree;

  tree = calloc(1, sizeof(struct binary_tree));
  return tree;
}

int local_binary_tree_reset(struct binary_tree *tree)
{
  struct binary_tree_node *np, *prev_np;

  ASSERT(tree != NULL);

  if (tree->root == NULL) {
    goto func_out;
  }

  np = tree->root;
  tree->root = NULL;

  if ((np->left == NULL) &&
      (np->right == NULL)) {
    local_binary_tree_node_del(tree, np);
    goto func_out;
  }

  prev_np = np;
  while (prev_np != NULL) {
    if (prev_np->left != NULL) {
      /* handle left child */
      np = prev_np->left;
      while (np->left != NULL) {
        prev_np = np;
        np = np->left;
      }
      prev_np->left = np->right;
      local_binary_tree_node_del(tree, np);
    }
    else if (prev_np->right != NULL) {
      /* handle right child */
      np = prev_np;
      prev_np = np->right;
      local_binary_tree_node_del(tree, np);
    }
    else {
      np = prev_np;
      prev_np = np->up;
      local_binary_tree_node_del(tree, np);
    }
  }
func_out:
  return EZCFG_RET_OK;
}

int local_binary_tree_del(struct binary_tree *tree)
{
  int ret;

  ASSERT(tree != NULL);

  ret = local_binary_tree_reset(tree);
  if (ret == EZCFG_RET_OK) {
    free(tree);
  }
  return ret;
}

int local_binary_tree_set_data_delete_handler(struct binary_tree *tree, int (*handler)(void *))
{
  ASSERT(tree != NULL);
  tree->data_delete_handler = handler;
  return EZCFG_RET_OK;
}

struct binary_tree_node *local_binary_tree_get_root(struct binary_tree *tree)
{
  ASSERT(tree != NULL);
  return tree->root;
}

int local_binary_tree_set_root(struct binary_tree *tree, struct binary_tree_node *np)
{
  ASSERT(tree != NULL);
  tree->root = np;
  return EZCFG_RET_OK;
}
