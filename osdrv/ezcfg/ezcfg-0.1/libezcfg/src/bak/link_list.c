/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : common/link_list.c
 *
 * Description  : single link list handler
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-23   0.1       Write it from scrach
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
  char *name;
  char *value;
  struct link_node *next;
};

struct ezcfg_link_list {
  struct ezcfg *ezcfg;
  int length;
  struct link_node *head;
  struct link_node *tail;
};

/*
 * private functions
 */
static void link_node_delete(struct link_node *p)
{
  free(p->name);
  free(p->value);
  free(p);
}

/*
 * public functions
 */
struct ezcfg_link_list *ezcfg_link_list_new(struct ezcfg *ezcfg)
{
  struct ezcfg_link_list *list;
  ASSERT(ezcfg != NULL);

  list = calloc(1, sizeof(struct ezcfg_link_list));
  if (list != NULL) {
    list->ezcfg = ezcfg;
  }
  return list;
}

void ezcfg_link_list_delete(struct ezcfg_link_list *list)
{
  ASSERT(list != NULL);

  while(list->head != NULL) {
    list->tail = list->head;
    list->head = (list->head)->next;
    link_node_delete(list->tail);
  }
  free(list);
}

/*
 * add link node to the head
 * replace the node if the name has been there.
 */
bool ezcfg_link_list_insert(struct ezcfg_link_list *list, char *name, char *value)
{
  struct link_node *np;
  char *p;

  ASSERT(list != NULL);
  ASSERT(name != NULL);
  ASSERT(value != NULL);

  np = list->head;

  /* check if the name has been there */
  while(np != NULL) {
    if (strcmp(np->name, name) == 0) {
      p = strdup(value);
      if (p == NULL)
	return false;
      free(np->value);
      np->value = p;
      return true;
    }
    np = np->next;
  }

  /* no such node in the list */
  np = malloc(sizeof(struct link_node));
  if (np == NULL) {
    return false;
  }
  memset(np, 0, sizeof(struct link_node));
  np->name = strdup(name);
  if (np->name == NULL) {
    link_node_delete(np);
    return false;
  }
  np->value = strdup(value);
  if (np->value == NULL) {
    link_node_delete(np);
    return false;
  }

  /* insert node */
  np->next = list->head;
  list->head = np;
  if (list->tail == NULL) {
    list->tail = np;
  }
  list->length += 1;
  return true;
}

/*
 * add link node to the tail
 * replace the node if the name has been there.
 */
bool ezcfg_link_list_append(struct ezcfg_link_list *list, char *name, char *value)
{
  struct link_node *np;
  char *p;

  ASSERT(list != NULL);
  ASSERT(name != NULL);
  ASSERT(value != NULL);

  np = list->head;

  /* check if the name has been there */
  while(np != NULL) {
    if (strcmp(np->name, name) == 0) {
      p = strdup(value);
      if (p == NULL)
	return false;
      free(np->value);
      np->value = p;
      return true;
    }
    np = np->next;
  }

  /* no such node in the list */
  np = malloc(sizeof(struct link_node));
  if (np == NULL) {
    return false;
  }
  memset(np, 0, sizeof(struct link_node));
  np->name = strdup(name);
  if (np->name == NULL) {
    link_node_delete(np);
    return false;
  }
  np->value = strdup(value);
  if (np->value == NULL) {
    link_node_delete(np);
    return false;
  }

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
  return true;
}

/*
 * remove link node by name
 */
bool ezcfg_link_list_remove(struct ezcfg_link_list *list, char *name)
{
  struct link_node *np, *npp;

  ASSERT(list != NULL);
  ASSERT(name != NULL);

  np = list->head;

  if (strcmp(np->name, name) == 0) {
    list->head = np->next;
    if (np->next == NULL) {
      list->tail = NULL;
    }
    link_node_delete(np);
    list->length -= 1;
    return true;
  }

  npp = np;
  np = np->next;

  /* check if the name has been there */
  while(np != NULL) {
    if (strcmp(np->name, name) == 0) {
      npp->next = np->next;
      if (np->next == NULL) {
	list->tail = npp;
      }
      link_node_delete(np);
      list->length -= 1;
      return true;
    }
    npp = np;
    np = np->next;
  }

  /* no such node in the list */
  return false;
}

bool ezcfg_link_list_in(struct ezcfg_link_list *list, char *name)
{
  struct link_node *np;

  ASSERT(list != NULL);
  ASSERT(name != NULL);

  np = list->head;

  /* check if the name has been there */
  while(np != NULL) {
    if (strcmp(np->name, name) == 0) {
      return true;
    }
    np = np->next;
  }

  /* no such node in the list */
  return false;
}

int ezcfg_link_list_get_length(struct ezcfg_link_list *list)
{
  ASSERT(list != NULL);
  return list->length;
}

char *ezcfg_link_list_get_node_name_by_index(struct ezcfg_link_list *list, int i)
{
  struct link_node *np;

  ASSERT(list != NULL);
  ASSERT((i > 0) && (i <= list->length));

  np = list->head;
  i--;

  while (i > 0) {
    np = np->next;
    i--;
  }

  return np->name;
}

char *ezcfg_link_list_get_node_value_by_index(struct ezcfg_link_list *list, int i)
{
  struct link_node *np;

  ASSERT(list != NULL);
  ASSERT((i > 0) && (i <= list->length));

  np = list->head;
  i--;

  while (i > 0) {
    np = np->next;
    i--;
  }

  return np->value;
}

char *ezcfg_link_list_get_node_value_by_name(struct ezcfg_link_list *list, char *name)
{
  struct link_node *np;

  ASSERT(list != NULL);
  ASSERT(name != NULL);

  np = list->head;

  /* check if the name has been there */
  while(np != NULL) {
    if (strcmp(np->name, name) == 0) {
      return np->value;
    }
    np = np->next;
  }

  /* no such node in the list */
  return NULL;
}
