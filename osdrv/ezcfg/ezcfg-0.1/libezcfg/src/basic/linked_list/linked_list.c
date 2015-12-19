/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : list/linked_list.c
 *
 * Description  : single linked list handler
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2014-03-18   0.1       Reimplement based on linked_list.c
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


struct linked_list_node {
  void *data;
  struct linked_list_node *next;
};

struct ezcfg_linked_list {
  struct ezcfg *ezcfg;
  int length;
  struct linked_list_node *head;
  struct linked_list_node *tail;
  int (*data_del_handler)(void *data);
  int (*data_cmp_handler)(const void *d1, const void *d2);
};

/*
 * private functions
 */
static void linked_list_node_del(struct ezcfg_linked_list *list, struct linked_list_node *p)
{
  if ((list->data_del_handler != NULL) &&
      (p->data != NULL)) {
    list->data_del_handler(p->data);
  }
  free(p);
}

/*
 * public functions
 */
struct ezcfg_linked_list *ezcfg_linked_list_new(struct ezcfg *ezcfg,
  int (*del_hdl)(void *),
  int (*cmp_hdl)(const void *, const void *))
{
  struct ezcfg_linked_list *list;

  ASSERT(ezcfg != NULL);
  ASSERT(del_hdl != NULL);
  ASSERT(cmp_hdl != NULL);

  /* increase ezcfg library context reference */
  if (ezcfg_inc_ref(ezcfg) != EZCFG_RET_OK) {
    EZDBG("%s(%d) <6>pid=[%d] ezcfg_inc_ref() failed\n", __func__, __LINE__, getpid());
    return NULL;
  }

  list = calloc(1, sizeof(struct ezcfg_linked_list));
  if (list != NULL) {
    list->ezcfg = ezcfg;
    list->data_del_handler = del_hdl;
    list->data_cmp_handler = cmp_hdl;
  }
  else {
    /* decrease ezcfg library context reference */
    if (ezcfg_dec_ref(ezcfg) != EZCFG_RET_OK) {
      EZDBG("%s(%d) <6>pid=[%d] ezcfg_dec_ref() failed\n", __func__, __LINE__, getpid());
    }
  }
  return list;
}

int ezcfg_linked_list_del(struct ezcfg_linked_list *list)
{
  struct ezcfg *ezcfg;

  ASSERT(list != NULL);
  ezcfg = list->ezcfg;

  while(list->head != NULL) {
    list->tail = list->head;
    list->head = (list->head)->next;
    linked_list_node_del(list, list->tail);
  }
  free(list);
  /* decrease ezcfg library context reference */
  if (ezcfg_dec_ref(ezcfg) != EZCFG_RET_OK) {
    EZDBG("ezcfg_dec_ref() failed\n");
  }
  return EZCFG_RET_OK;
}

/*
 * add link node to the head
 * replace the node if the name has been there.
 */
int ezcfg_linked_list_insert(struct ezcfg_linked_list *list, void *data)
{
  struct linked_list_node *np;

  ASSERT(list != NULL);
  ASSERT(data != NULL);

  if ((list->data_cmp_handler == NULL) ||
      (list->data_del_handler == NULL)) {
    return EZCFG_RET_FAIL;
  }

  np = list->head;

  /* check if the data has been there */
  while(np != NULL) {
    if (list->data_cmp_handler(np->data, data) == 0) {
      list->data_del_handler(np->data);
      np->data = data;
      return EZCFG_RET_OK;
    }
    np = np->next;
  }

  /* no such node in the list */
  np = calloc(1, sizeof(struct linked_list_node));
  if (np == NULL) {
    return EZCFG_RET_FAIL;
  }
  np->data = data;

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
int ezcfg_linked_list_append(struct ezcfg_linked_list *list, void *data)
{
  struct linked_list_node *np;

  ASSERT(list != NULL);
  ASSERT(data != NULL);

  if ((list->data_cmp_handler == NULL) ||
      (list->data_del_handler == NULL)) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    return EZCFG_RET_FAIL;
  }

  np = list->head;

  /* check if the name has been there */
  EZDBG("%s(%d)\n", __func__, __LINE__);
  while(np != NULL) {
    if (list->data_cmp_handler(np->data, data) == 0) {
      list->data_del_handler(np->data);
      np->data = data;
      EZDBG("%s(%d)\n", __func__, __LINE__);
      return EZCFG_RET_OK;
    }
    np = np->next;
  }

  /* no such node in the list */
  EZDBG("%s(%d)\n", __func__, __LINE__);
  np = calloc(1, sizeof(struct linked_list_node));
  if (np == NULL) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    return EZCFG_RET_FAIL;
  }
  np->data = data;

  /* append node */
  EZDBG("%s(%d)\n", __func__, __LINE__);
  if (list->tail == NULL) {
    list->head = np;
    list->tail = np;
  }
  else {
    (list->tail)->next = np;
    list->tail = np;
  }
  list->length += 1;
  EZDBG("%s(%d)\n", __func__, __LINE__);
  return EZCFG_RET_OK;
}

/*
 * take the data of head link node
 */
void *ezcfg_linked_list_take_data(struct ezcfg_linked_list *list)
{
  struct linked_list_node *np;
  void *data;

  ASSERT(list != NULL);

  np = list->head;

  if (np == NULL) {
    return NULL;
  }

  list->head = np->next;
  if (list->tail == np) {
    list->tail = NULL;
  }
  data = np->data;
  np->data = NULL;
  linked_list_node_del(list, np);
  list->length -= 1;

  return data;
}

/*
 * remove link node by name
 */
int ezcfg_linked_list_remove(struct ezcfg_linked_list *list, void *data)
{
  struct linked_list_node *np, *npp;

  ASSERT(list != NULL);
  ASSERT(data != NULL);

  if ((list->data_cmp_handler == NULL) ||
      (list->data_del_handler == NULL)) {
    return EZCFG_RET_FAIL;
  }

  np = list->head;

  /* remove the head node */
  if (list->data_cmp_handler(np->data, data) == 0) {
    list->head = np->next;
    if (np->next == NULL) {
      list->tail = NULL;
    }
    linked_list_node_del(list, np);
    list->length -= 1;
    return EZCFG_RET_OK;
  }

  npp = np;
  np = np->next;

  /* check if the data has been there */
  while(np != NULL) {
    if (list->data_cmp_handler(np->data, data) == 0) {
      npp->next = np->next;
      if (np->next == NULL) {
        list->tail = npp;
      }
      linked_list_node_del(list, np);
      list->length -= 1;
      return EZCFG_RET_OK;
    }
    npp = np;
    np = np->next;
  }

  /* no such node in the list */
  return EZCFG_RET_FAIL;
}

int ezcfg_linked_list_in(struct ezcfg_linked_list *list, void *data)
{
  struct linked_list_node *np;

  ASSERT(list != NULL);
  ASSERT(data != NULL);

  if (list->data_cmp_handler == NULL) {
    return false;
  }

  np = list->head;

  /* check if the name has been there */
  while(np != NULL) {
    if (list->data_cmp_handler(np->data, data) == 0) {
      return true;
    }
    np = np->next;
  }

  /* no such node in the list */
  return false;
}

int ezcfg_linked_list_get_length(struct ezcfg_linked_list *list)
{
  ASSERT(list != NULL);
  return list->length;
}

void *ezcfg_linked_list_get_node_data_by_index(struct ezcfg_linked_list *list, int i)
{
  struct linked_list_node *np;

  ASSERT(list != NULL);
  ASSERT((i > 0) && (i <= list->length));

  np = list->head;
  i--;

  while (i > 0) {
    np = np->next;
    i--;
  }

  return np->data;
}

/* It's user's duty to free the returned string */
char *ezcfg_linked_list_nv_pair_to_json_text(struct ezcfg_linked_list *list)
{
  struct linked_list_node *np;
  struct ezcfg_nv_pair *nvp;
  char *text_buf = NULL;
  int text_len = 0;
  char *p = NULL;

  ASSERT(list != NULL);

  np = list->head;
  while (np) {
    nvp = np->data;
    text_len += nvp->nlen;
    text_len += 3; /* two '"' and one ':' */
    text_len += nvp->vlen;
    text_len += 3; /* two '"' and one ',' */
    np = np->next;
  }
  text_len -= 1; /* remove last ',' */
  text_len += 2; /* add '{' and '}' */
  text_len += 1; /* add null-terminated */

  text_buf = malloc(text_len);
  if (text_buf) {
    p = text_buf;
    *p = '{';
    p++;
    np = list->head;
    while (np) {
      nvp = np->data;

      /* name */
      *p = '"';
      p++;
      memcpy(p, nvp->n, nvp->nlen);
      p += nvp->nlen;
      *p = '"';
      p++;
      *p = ':';
      p++;

      /* value */
      *p = '"';
      p++;
      memcpy(p, nvp->v, nvp->vlen);
      p += nvp->vlen;
      *p = '"';
      p++;
      *p = ',';
      p++;

      np = np->next;
    }
    p--; /* remove last ',' */
    *p = '}';
    p++;
    *p = '\0';
  }

  return text_buf;
}
