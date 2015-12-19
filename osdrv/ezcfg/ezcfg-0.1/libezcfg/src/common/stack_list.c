/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : common/stack_list.c
 *
 * Description  : single linked list stack handler
 *
 * Copyright (C) 2008-2015 by ezbox-project
 *
 * History      Rev       Description
 * 2015-06-09   0.1       Copy from list/stack_list.c
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

struct stack_list_node {
  void *data;
  struct stack_list_node *next;
};

struct stack_list {
  int length;
  struct stack_list_node *head;
  int (*data_delete_handler)(void *data);
};

/*
 * private functions
 */
static void stack_list_node_delete(struct stack_list *list, struct stack_list_node *p)
{
  if ((list->data_delete_handler != NULL) &&
      (p->data != NULL)) {
    list->data_delete_handler(p->data);
  }
  free(p);
}

/*
 * public functions
 */
struct stack_list *local_stack_list_new(void)
{
  struct stack_list *list;

  list = calloc(1, sizeof(struct stack_list));
  return list;
}

void local_stack_list_del(struct stack_list *list)
{
  struct stack_list_node *np;

  ASSERT(list != NULL);

  np = list->head;
  while(np != NULL) {
    list->head = np->next;
    stack_list_node_delete(list, np);
    np = list->head;
  }
  free(list);
}

void local_stack_list_set_data_delete_handler(struct stack_list *list, int (*handler)(void *))
{
  ASSERT(list != NULL);
  list->data_delete_handler = handler;
}

/*
 * add stack list node to the head
 */
bool local_stack_list_push(struct stack_list *list, void *data)
{
  struct stack_list_node *np;

  ASSERT(list != NULL);
  ASSERT(data != NULL);

  np = calloc(1, sizeof(struct stack_list_node));
  if (np == NULL) {
    return false;
  }
  np->data = data;

  /* insert node */
  np->next = list->head;
  list->head = np;
  list->length += 1;
  return true;
}

/*
 * get stack list node from head
 */
void *local_stack_list_pop(struct stack_list *list)
{
  struct stack_list_node *np;
  void *data;

  ASSERT(list != NULL);

  if (list->head == NULL) {
    return NULL;
  }

  np = list->head;
  list->head = np->next;
  data = np->data;
  np->data = NULL;
  stack_list_node_delete(list, np);
  list->length -= 1;

  return data;
}

/*
 * get stack list length
 */
int local_stack_list_get_length(struct stack_list *list)
{
  ASSERT(list != NULL);
  return list->length;
}

/*
 * check if stack list is empty
 */
bool local_stack_list_is_empty(struct stack_list *list)
{
  ASSERT(list != NULL);
  if (list->length > 0)
    return false;
  else
    return true;
}

