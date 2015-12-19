/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : basic/nv_linked_list/nv_linked_list.c
 *
 * Description  : single link list handler
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-23   0.1       Write it from scrach
 * 2015-06-12   0.2       Reimplement it as a basic object
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

struct link_node {
  char *n;
  char *v;
  int nlen;
  int vlen;
  struct link_node *next;
};

struct ezcfg_nv_linked_list {
  struct ezcfg *ezcfg;
  int length;
  struct link_node *head;
  struct link_node *tail;
};

/*
 * private functions
 */
static void link_node_del(struct link_node *p)
{
  free(p->n);
  free(p->v);
  free(p);
}

/*
 * public functions
 */
struct ezcfg_nv_linked_list *ezcfg_nv_linked_list_new(struct ezcfg *ezcfg)
{
  struct ezcfg_nv_linked_list *list = NULL;
  ASSERT(ezcfg != NULL);

  list = calloc(1, sizeof(struct ezcfg_nv_linked_list));
  if (list != NULL) {
    list->ezcfg = ezcfg;
  }
  return list;
}

int ezcfg_nv_linked_list_del(struct ezcfg_nv_linked_list *list)
{
  ASSERT(list != NULL);

  while(list->head != NULL) {
    list->tail = list->head;
    list->head = (list->head)->next;
    link_node_del(list->tail);
  }
  free(list);
  return EZCFG_RET_OK;
}

/*
 * add link node to the head
 * replace the node if the name has been there.
 */
int ezcfg_nv_linked_list_insert(struct ezcfg_nv_linked_list *list, char *name, char *value)
{
  struct link_node *np = NULL;
  char *p = NULL;

  ASSERT(list != NULL);
  ASSERT(name != NULL);
  ASSERT(value != NULL);

  np = list->head;

  /* check if the name has been there */
  while(np != NULL) {
    if (strcmp(np->n, name) == 0) {
      p = strdup(value);
      if (p == NULL)
	return EZCFG_RET_FAIL;
      free(np->v);
      np->v = p;
      np->vlen = strlen(np->v);
      return EZCFG_RET_OK;
    }
    np = np->next;
  }

  /* no such node in the list */
  np = malloc(sizeof(struct link_node));
  if (np == NULL) {
    return EZCFG_RET_FAIL;
  }
  memset(np, 0, sizeof(struct link_node));
  np->n = strdup(name);
  if (np->n == NULL) {
    link_node_del(np);
    return EZCFG_RET_FAIL;
  }
  np->v = strdup(value);
  if (np->v == NULL) {
    link_node_del(np);
    return EZCFG_RET_FAIL;
  }
  np->nlen = strlen(np->n);
  np->vlen = strlen(np->v);

  /* insert node */
  np->next = list->head;
  list->head = np;
  if (list->tail == NULL) {
    list->tail = np;
  }
  list->length += 1;
  return EZCFG_RET_OK;
}

/*
 * add link node to the tail
 * replace the node if the name has been there.
 */
int ezcfg_nv_linked_list_append(struct ezcfg_nv_linked_list *list, char *name, char *value)
{
  struct link_node *np;
  char *p;

  ASSERT(list != NULL);
  ASSERT(name != NULL);
  ASSERT(value != NULL);

  np = list->head;

  /* check if the name has been there */
  while(np != NULL) {
    if (strcmp(np->n, name) == 0) {
      p = strdup(value);
      if (p == NULL)
	return EZCFG_RET_FAIL;
      free(np->v);
      np->v = p;
      np->vlen = strlen(np->v);
      return EZCFG_RET_OK;
    }
    np = np->next;
  }

  /* no such node in the list */
  np = malloc(sizeof(struct link_node));
  if (np == NULL) {
    return EZCFG_RET_FAIL;
  }
  memset(np, 0, sizeof(struct link_node));
  np->n = strdup(name);
  if (np->n == NULL) {
    link_node_del(np);
    return EZCFG_RET_FAIL;
  }
  np->v = strdup(value);
  if (np->v == NULL) {
    link_node_del(np);
    return EZCFG_RET_FAIL;
  }
  np->nlen = strlen(np->n);
  np->vlen = strlen(np->v);

  np->next = NULL;
  /* append node */
  if (list->tail == NULL) {
    list->head = np;
    list->tail = np;
  }
  else {
    (list->tail)->next = np;
    list->tail = np;
  }
  list->length += 1;
  return EZCFG_RET_OK;
}

/*
 * remove link node by name
 */
int ezcfg_nv_linked_list_remove(struct ezcfg_nv_linked_list *list, char *name)
{
  struct link_node *np, *npp;

  ASSERT(list != NULL);
  ASSERT(name != NULL);

  np = list->head;

  if (strcmp(np->n, name) == 0) {
    list->head = np->next;
    if (np->next == NULL) {
      list->tail = NULL;
    }
    link_node_del(np);
    list->length -= 1;
    return EZCFG_RET_OK;
  }

  npp = np;
  np = np->next;

  /* check if the name has been there */
  while(np != NULL) {
    if (strcmp(np->n, name) == 0) {
      npp->next = np->next;
      if (np->next == NULL) {
	list->tail = npp;
      }
      link_node_del(np);
      list->length -= 1;
      return EZCFG_RET_OK;
    }
    npp = np;
    np = np->next;
  }

  /* no such node in the list */
  return EZCFG_RET_FAIL;
}

bool ezcfg_nv_linked_list_in(struct ezcfg_nv_linked_list *list, char *name)
{
  struct link_node *np;

  ASSERT(list != NULL);
  ASSERT(name != NULL);

  np = list->head;

  /* check if the name has been there */
  while(np != NULL) {
    if (strcmp(np->n, name) == 0) {
      return true;
    }
    np = np->next;
  }

  /* no such node in the list */
  return false;
}

int ezcfg_nv_linked_list_get_length(struct ezcfg_nv_linked_list *list)
{
  ASSERT(list != NULL);
  return list->length;
}

char *ezcfg_nv_linked_list_get_node_name_by_index(struct ezcfg_nv_linked_list *list, int i)
{
  struct link_node *np;

  ASSERT(list != NULL);
  ASSERT(i > 0);

  if (i > list->length) {
    return NULL;
  }

  np = list->head;
  i--;

  while (i > 0) {
    np = np->next;
    i--;
  }

  return np->n;
}

char *ezcfg_nv_linked_list_get_node_value_by_index(struct ezcfg_nv_linked_list *list, int i)
{
  struct link_node *np;

  ASSERT(list != NULL);
  ASSERT(i > 0);

  if (i > list->length) {
    return NULL;
  }

  np = list->head;
  i--;

  while (i > 0) {
    np = np->next;
    i--;
  }

  return np->v;
}

char *ezcfg_nv_linked_list_get_node_value_by_name(struct ezcfg_nv_linked_list *list, char *name)
{
  struct link_node *np;

  ASSERT(list != NULL);
  ASSERT(name != NULL);

  np = list->head;

  /* check if the name has been there */
  while(np != NULL) {
    if (strcmp(np->n, name) == 0) {
      return np->v;
    }
    np = np->next;
  }

  /* no such node in the list */
  return NULL;
}
