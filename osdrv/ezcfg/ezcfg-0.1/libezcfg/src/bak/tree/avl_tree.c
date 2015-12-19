/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : tree/avl_tree.c
 *
 * Description  : AVL tree handler
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2014-03-18   0.1       Write it from scrach
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

struct avl_tree_node {
	void *data;
	struct avl_tree_node *left;
	struct avl_tree_node *right;
	int height;
};

struct ezcfg_avl_tree {
	struct ezcfg *ezcfg;
	int height;
	struct avl_tree_node *root;
	struct ezcfg_stack_list *stack;
	int (*data_delete_handler)(void *data);
	int (*data_compare_handler)(const void *d1, const void *d2);
};

/*
 * private functions
 */
static void avl_tree_node_delete(struct ezcfg_avl_tree *tree, struct avl_tree_node *p)
{
	if (tree->data_delete_handler) {
		tree->data_delete_handler(p->data);
	}
	free(p);
}

/*
 * public functions
 */
struct ezcfg_avl_tree *ezcfg_avl_tree_new(struct ezcfg *ezcfg)
{
	struct ezcfg_avl_tree *tree;
	ASSERT(ezcfg != NULL);

	tree = calloc(1, sizeof(struct ezcfg_avl_tree));
	if (tree == NULL) {
		return NULL;
	}
	tree->stack = ezcfg_stack_list_new(ezcfg);
	if (tree->stack == NULL) {
		ezcfg_avl_tree_delete(tree);
		return NULL;
	}
	tree->ezcfg = ezcfg;
	return tree;
}

void ezcfg_avl_tree_delete(struct ezcfg_avl_tree *tree)
{
	struct avl_tree_node *np;

	ASSERT(tree != NULL);

	if (tree->stack != NULL) {
		/* clean up stack */
		while (ezcfg_stack_list_is_empty(tree->stack) == false) {
			ezcfg_stack_list_pop(tree->stack);
		}

		/* use stack to clean up tree node */
		np = tree->root;
		if (np != NULL) {
			if (ezcfg_stack_list_push(tree->stack, np) == false) {
				ezcfg_stack_list_delete(tree->stack);
				goto func_out;
			}
		}
		while (ezcfg_stack_list_is_empty(tree->stack) == false) {
			np = ezcfg_stack_list_pop(tree->stack);
			if (np->right != NULL) {
				if (ezcfg_stack_list_push(tree->stack, np->right) == false) {
					ezcfg_stack_list_delete(tree->stack);
					goto func_out;
				}
			}
			if (np->left != NULL) {
				if (ezcfg_stack_list_push(tree->stack, np->left) == false) {
					ezcfg_stack_list_delete(tree->stack);
					goto func_out;
				}
			}
			avl_tree_node_delete(tree, np);
		}
	}
func_out:
	free(tree);
}

void ezcfg_avl_tree_set_data_delete_handler(struct ezcfg_avl_tree *tree, int (*handler)(void *))
{
	ASSERT(tree != NULL);
	tree->data_delete_handler = handler;
}
