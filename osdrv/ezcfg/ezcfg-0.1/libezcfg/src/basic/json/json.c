/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : basic/json/json.c
 *
 * Description  : simple implementation of RFC7159
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2014-03-03   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#include "json_local.h"

/*
object
    {}
    { members } 
members
    pair
    pair , members
pair
    string : value
array
    []
    [ elements ]
elements
    value
    value , elements
value
    string
    number
    object
    array
    true
    false
    null
*/

#if 0
struct ezcfg_json_value {
  int type; /* false / null / true / object / array / number / string */
  char *element;
  int visited; /* used for stack operation */
};

struct ezcfg_json {
  struct ezcfg *ezcfg;
  struct ezcfg_binary_tree *text; /* JSON-text = object / array */
  struct ezcfg_stack_list *stack;
  int msg_len;
};
#endif

/* Private functions */
/*
RFC7159
      ws = *(
                %x20 /              ; Space
                %x09 /              ; Horizontal tab
                %x0A /              ; Line feed or New line
                %x0D                ; Carriage return
            )
*/
static char *skip_ws(char *buf)
{
  char *p = buf;
  while ((*p == 0x20) || (*p == 0x09) ||
	 (*p == 0x0a) || (*p == 0x0d))
    p++;
  return p;
}

static bool is_token_delim(char c)
{
  if ((c == 0x20) || (c == 0x09) ||
      (c == 0x0a) || (c == 0x0d) ||
      (c == ',')  || (c == '}')  ||
      (c == ']'))
    return true;
  else
    return false;
}

static char * get_number(char *buf)
{
  char *p = buf;
  if (*p == '-') p++;
  if ((*p < '0') || (*p > '9'))
    return NULL;

  /* int */
  if (*p == '0') {
    p++;
    if (*p == '\0')
      return p;
    if (*p == '.')
      goto frac;
    else if ((*p == 'e') || (*p == 'E'))
      goto exp;
    else
      return p;
  }
  else {
    while ((*p >= '0') && (*p <= '9')) p++;
    if (*p == '\0')
      return p;
    if (*p == '.')
      goto frac;
    else if ((*p == 'e') || (*p == 'E'))
      goto exp;
    else
      return p;
  }

 frac:
  /* frac */
  p++;
  if ((*p < '0') || (*p > '9'))
    return (p-1);
  while ((*p >= '0') && (*p <= '9')) p++;
  if (*p == '\0')
    return p;

  if ((*p != 'e') && (*p != 'E'))
    return p;

 exp:
  /* exp */
  p++;
  if ((*p != '+') && (*p != '-') && ((*p < '0') || (*p > '9')))
    return (p-1);
  if ((*p == '+') || (*p == '-')) p++;
  while ((*p >= '0') && (*p <= '9')) p++;

  return p;
}

static bool stack_clean_up(struct ezcfg_stack_list *sp)
{
  while (ezcfg_stack_list_is_empty(sp) == false) {
    ezcfg_stack_list_pop(sp);
  }
  return true;
}

static bool tree_clean_up(struct ezcfg_binary_tree *tp)
{
  if (ezcfg_binary_tree_reset(tp) == EZCFG_RET_OK) {
    return true;
  }
  else {
    return false;
  }
}

static struct ezcfg_json_value *json_value_new(int type)
{
  struct ezcfg_json_value *vp;

  vp = calloc(1, sizeof(struct ezcfg_json_value));
  if (vp) {
    vp->type = type;
  }
  return vp;
}

/* false / null / true / object / array / number / string */
static bool json_value_is_native_value(struct ezcfg_json_value *vp)
{
  if ((vp->type == EZCFG_JSON_VALUE_TYPE_FALSE)  ||
      (vp->type == EZCFG_JSON_VALUE_TYPE_NULL)   ||
      (vp->type == EZCFG_JSON_VALUE_TYPE_TRUE)   ||
      (vp->type == EZCFG_JSON_VALUE_TYPE_OBJECT) ||
      (vp->type == EZCFG_JSON_VALUE_TYPE_ARRAY)  ||
      (vp->type == EZCFG_JSON_VALUE_TYPE_NUMBER) ||
      (vp->type == EZCFG_JSON_VALUE_TYPE_STRING)) {
    return true;
  }
  else {
    return false;
  }
}

static bool json_value_set_element(struct ezcfg_json_value *vp, char *buf, int len)
{
  vp->element = malloc(len+1);
  if (vp->element == NULL) {
    return false;
  }
  strncpy(vp->element, buf, len);
  vp->element[len] = '\0';
  return true;
}

static bool json_value_set_nvpair_string(struct ezcfg_json_value *vp, char *buf, int len)
{
  return json_value_set_element(vp, buf, len);
}

static bool json_value_set_string_element(struct ezcfg_json_value *vp, char *buf, int len)
{
  return json_value_set_element(vp, buf, len);
}

static bool json_value_set_number_element(struct ezcfg_json_value *vp, char *buf, int len)
{
  return json_value_set_element(vp, buf, len);
}

/* Public functions */
struct ezcfg_json_value *ezcfg_json_value_create(char *buf)
{
  if (buf == NULL) {
    return NULL;
  }

  if (*buf == '"') {
    return json_value_new(EZCFG_JSON_VALUE_TYPE_STRING);
  }
  else if (*buf == '{') {
    return json_value_new(EZCFG_JSON_VALUE_TYPE_OBJECT);
  }
  else if (*buf == '[') {
    return json_value_new(EZCFG_JSON_VALUE_TYPE_ARRAY);
  }
  else if ((strncmp(buf, "false", 5) == 0) &&
	   (is_token_delim(buf[5]) == true)) {
    return json_value_new(EZCFG_JSON_VALUE_TYPE_FALSE);
  }
  else if ((strncmp(buf, "null", 4) == 0) &&
	   (is_token_delim(buf[4]) == true)) {
    return json_value_new(EZCFG_JSON_VALUE_TYPE_NULL);
  }
  else if ((strncmp(buf, "true", 4) == 0) &&
	   (is_token_delim(buf[4]) == true)) {
    return json_value_new(EZCFG_JSON_VALUE_TYPE_TRUE);
  }
  else if ((*buf == '-' ) ||
	   ((*buf >= '0') &&
	    (*buf <= '9'))) {
    return json_value_new(EZCFG_JSON_VALUE_TYPE_NUMBER);
  }
  else {
    return NULL;
  }
}

int ezcfg_json_value_del(struct ezcfg_json_value *vp)
{
  ASSERT(vp != NULL);

  if (vp->element != NULL) {
    free(vp->element);
  }
  free(vp);
  return EZCFG_RET_OK;
}

int ezcfg_json_value_del_wrap(void *data)
{
  return ezcfg_json_value_del((struct ezcfg_json_value *)data);
}

int ezcfg_json_reset(struct ezcfg_json *json)
{
  ASSERT(json != NULL);

  /* first clean up stack stuff */
  if (json->stack) {
    stack_clean_up(json->stack);
  }

  /* then clean up text stuff */
  if (json->text) {
    tree_clean_up(json->text);
  }

  /* reset msg_len */
  json->msg_len = 0;

  return EZCFG_RET_OK;
}

int ezcfg_json_del(struct ezcfg_json *json)
{
  ASSERT(json != NULL);

  ezcfg_json_reset(json);

  if (json->stack) {
    ezcfg_stack_list_del(json->stack);
  }

  if (json->text) {
    ezcfg_binary_tree_del(json->text);
  }

  free(json);
  return EZCFG_RET_OK;
}

/**
 * ezcfg_json_new:
 * Create ezcfg json parser data structure
 * Returns: a new ezcfg json parser data structure
 **/
struct ezcfg_json *ezcfg_json_new(struct ezcfg *ezcfg)
{
  struct ezcfg_json *json = NULL;

  ASSERT(ezcfg != NULL);

  /* initialize json parser data structure */
  json = calloc(1, sizeof(struct ezcfg_json));
  if (json == NULL) {
    err(ezcfg, "initialize json parser error.\n");
    return NULL;
  }

  json->text = ezcfg_binary_tree_new(ezcfg);
  if (json->text == NULL) {
    err(ezcfg, "initialize json text error.\n");
    ezcfg_json_del(json);
    return NULL;
  }

  json->stack = ezcfg_stack_list_new(ezcfg);
  if (json->stack == NULL) {
    err(ezcfg, "initialize json stack error.\n");
    ezcfg_json_del(json);
    return NULL;
  }

  json->ezcfg = ezcfg;

  return json;
}

int ezcfg_json_parse_text(struct ezcfg_json *json, char *text, int len)
{
  struct ezcfg *ezcfg;
  char *cur, *p;
  struct ezcfg_json_value *vp;
  struct ezcfg_binary_tree_node *np, *np_left, *np_right;
  int ret = EZCFG_RET_FAIL;
  int buf_len;

  ASSERT(json != NULL);
  ASSERT(text != NULL);
  ASSERT(len > 0);

  ezcfg = json->ezcfg;

  cur = skip_ws(text);
  if ((*cur != '{') &&
      (*cur != '[')) {
    return EZCFG_RET_FAIL;
  }

  vp = ezcfg_json_value_create(cur);
  if (vp == NULL) {
    return EZCFG_RET_FAIL;
  }
  np = ezcfg_binary_tree_node_new(ezcfg, vp);
  if (np == NULL) {
    ezcfg_json_value_del(vp);
    return EZCFG_RET_FAIL;
  }

  /* add root node for binary tree */
  ezcfg_binary_tree_set_data_del_handler(json->text, ezcfg_json_value_del_wrap);
  ezcfg_binary_tree_set_root(json->text, np);
  if (ezcfg_stack_list_push(json->stack, np) == false) {
    goto func_out;
  }

  while ((*cur != '\0') &&
	 (ezcfg_stack_list_is_empty(json->stack) == false)) {
    cur = skip_ws(cur);
    np = ezcfg_stack_list_pop(json->stack);
    if (np == NULL) {
      goto func_out;
    }
    vp = ezcfg_binary_tree_node_get_data(np);
    switch (vp->type) {
    case EZCFG_JSON_VALUE_TYPE_OBJECT:
      if (ezcfg_binary_tree_node_get_left(np) == NULL) {
	if (*cur != '{') {
	  goto func_out;
	}
	cur++;
	json->msg_len++; /* increase for '{' */
	cur = skip_ws(cur);
	if (*cur == '}') {
	  /* it's an empty set object */
	  vp = json_value_new(EZCFG_JSON_VALUE_TYPE_EMPTYSET);
	  if (vp == NULL) {
	    goto func_out;
	  }
	  np_left = ezcfg_binary_tree_node_new(ezcfg, vp);
	  if (np_left == NULL) {
	    ezcfg_json_value_del(vp);
	    goto func_out;
	  }
	  ezcfg_binary_tree_node_append_left(np, np_left);
	  cur++;
	  json->msg_len++; /* increase for '}' */
	  continue;
	}
	if (*cur != '"') {
	  goto func_out;
	}
	/* It maybe a name string start tag of nvpair */
	vp = json_value_new(EZCFG_JSON_VALUE_TYPE_NVPAIR);
	if (vp == NULL) {
	  goto func_out;
	}
	np_left = ezcfg_binary_tree_node_new(ezcfg, vp);
	if (np_left == NULL) {
	  ezcfg_json_value_del(vp);
	  goto func_out;
	}
	ezcfg_binary_tree_node_append_left(np, np_left);
	/* push back to the stack */
	if (ezcfg_stack_list_push(json->stack, np) == false) {
	  goto func_out;
	}
	if (ezcfg_stack_list_push(json->stack, np_left) == false) {
	  goto func_out;
	}
      }
      else if (ezcfg_binary_tree_node_get_right(np) == NULL) {
	if (*cur != '}') {
	  goto func_out;
	}
	/* It's object end tag */
	cur++;
	json->msg_len++; /* increase for '}' */
      }
      else {
	goto func_out;
      }
      break;
    case EZCFG_JSON_VALUE_TYPE_NVPAIR:
      if (ezcfg_binary_tree_node_get_left(np) == NULL) {
	if (*cur != '"') {
	  goto func_out;
	}
	cur++;
	json->msg_len++; /* increase for string start '"' */
	p = strchr(cur, '"');
	if (p == NULL) {
	  goto func_out;
	}
	buf_len = p - cur;

	if (json_value_set_nvpair_string(vp, cur, buf_len) == false) {
	  goto func_out;
	}
	json->msg_len += buf_len; /* increase for string content */
	json->msg_len++; /* increase for string end '"' */

	/* cur move to ": value" part */
	cur = skip_ws(p+1);

	/* value part of name-value pair not ready! */
	if (*cur != ':') {
	  goto func_out;
	}
	cur++;
	json->msg_len++; /* increase for ':' */
	cur = skip_ws(cur);

	/* It maybe a value */
	vp = ezcfg_json_value_create(cur);
	if (vp == NULL) {
	  goto func_out;
	}
	np_left = ezcfg_binary_tree_node_new(ezcfg, vp);
	if (np_left == NULL) {
	  ezcfg_json_value_del(vp);
	  goto func_out;
	}
	ezcfg_binary_tree_node_append_left(np, np_left);
	/* push back to the stack */
	if (ezcfg_stack_list_push(json->stack, np) == false) {
	  goto func_out;
	}
	if (ezcfg_stack_list_push(json->stack, np_left) == false) {
	  goto func_out;
	}
      }
      else if (ezcfg_binary_tree_node_get_right(np) == NULL) {
	if (*cur == '}') {
	  /* It's object end tag */
	  continue;
	}
	else if (*cur != ',') {
	  goto func_out;
	}
	/* move to next name-value pair */
	cur++;
	json->msg_len++; /* increase for ',' */
	/* It maybe a name string start tag of nvpair */
	vp = json_value_new(EZCFG_JSON_VALUE_TYPE_NVPAIR);
	if (vp == NULL) {
	  goto func_out;
	}
	np_right = ezcfg_binary_tree_node_new(ezcfg, vp);
	if (np_right == NULL) {
	  ezcfg_json_value_del(vp);
	  goto func_out;
	}
	ezcfg_binary_tree_node_append_right(np, np_right);

	/* push back to the stack */
	if (ezcfg_stack_list_push(json->stack, np_right) == false) {
	  goto func_out;
	}
      }
      else {
	goto func_out;
      }
      break;
    case EZCFG_JSON_VALUE_TYPE_ARRAY:
      if (ezcfg_binary_tree_node_get_left(np) == NULL) {
	if (*cur != '[') {
	  goto func_out;
	}
	cur++;
	json->msg_len++; /* increase for '[' */
	cur = skip_ws(cur);
	if (*cur == ']') {
	  /* it's an empty set object */
	  vp = json_value_new(EZCFG_JSON_VALUE_TYPE_EMPTYSET);
	  if (vp == NULL) {
	    goto func_out;
	  }
	  np_left = ezcfg_binary_tree_node_new(ezcfg, vp);
	  if (np_left == NULL) {
	    ezcfg_json_value_del(vp);
	    goto func_out;
	  }
	  ezcfg_binary_tree_node_append_left(np, np_left);
	  cur++;
	  json->msg_len++; /* increase for ']' */
	  continue;
	}
	/* It maybe a value */
	vp = ezcfg_json_value_create(cur);
	if (vp == NULL) {
	  goto func_out;
	}
	np_left = ezcfg_binary_tree_node_new(ezcfg, vp);
	if (np_left == NULL) {
	  ezcfg_json_value_del(vp);
	  goto func_out;
	}
	ezcfg_binary_tree_node_append_left(np, np_left);
	/* push back to the stack */
	if (ezcfg_stack_list_push(json->stack, np) == false) {
	  goto func_out;
	}
	if (ezcfg_stack_list_push(json->stack, np_left) == false) {
	  goto func_out;
	}
      }
      else if (ezcfg_binary_tree_node_get_right(np) == NULL) {
	if (*cur == ']') {
	  /* It's array end tag */
	  cur++;
	  json->msg_len++; /* increase for ']' */
	  continue;
	}
	if (*cur != ',') {
	  goto func_out;
	}
	cur++;
	json->msg_len++; /* increase for ',' */
	cur = skip_ws(cur);
	/* It maybe a value */
	vp = ezcfg_json_value_create(cur);
	if (vp == NULL) {
	  goto func_out;
	}
	np_left = ezcfg_binary_tree_node_new(ezcfg, vp);
	if (np_left == NULL) {
	  ezcfg_json_value_del(vp);
	  goto func_out;
	}

	/* check if vp->value is json native value */
	if (json_value_is_native_value(vp) == false) {
	  goto func_out;
	}

	/* find the last element of the array */
	ezcfg_binary_tree_node_append_left(np, np_left);

	/* push back to the stack */
	if (ezcfg_stack_list_push(json->stack, np) == false) {
	  goto func_out;
	}
	if (ezcfg_stack_list_push(json->stack, np_left) == false) {
	  goto func_out;
	}
      }
      else {
	goto func_out;
      }
      break;
    case EZCFG_JSON_VALUE_TYPE_STRING:
      if (*cur != '"') {
	goto func_out;
      }
      /* It's name string start tag */
      cur++;
      json->msg_len++; /* increase for string start '"' */
      p = strchr(cur, '"');
      if (p == NULL) {
	goto func_out;
      }
      buf_len = p - cur;
      if (json_value_set_string_element(vp, cur, buf_len) == false) {
	goto func_out;
      }
      json->msg_len += buf_len; /* increase for string content */
      json->msg_len++; /* increase for string end '"' */
      cur = p+1;
      break;
    case EZCFG_JSON_VALUE_TYPE_FALSE:
      /* skip "false" */
      cur += 5;
      json->msg_len += 5; /* increase for false */
      break;
    case EZCFG_JSON_VALUE_TYPE_NULL:
      /* skip "null" */
      cur += 4;
      json->msg_len += 4; /* increase for null */
      break;
    case EZCFG_JSON_VALUE_TYPE_TRUE:
      /* skip "true" */
      cur += 4;
      json->msg_len += 4; /* increase for true */
      break;
    case EZCFG_JSON_VALUE_TYPE_NUMBER:
      p = get_number(cur);
      if (p == NULL) {
	goto func_out;
      }
      buf_len = p - cur;
      if (json_value_set_number_element(vp, cur, buf_len) == false) {
	goto func_out;
      }
      json->msg_len += buf_len; /* increase for number */
      cur = p; /* p point to the next char of number */
      break;
    default:
      goto func_out;
      break;
    }
  }

  cur = skip_ws(cur);
  if ((*cur == '\0') && (ezcfg_stack_list_is_empty(json->stack) == true)) {
    ret = EZCFG_RET_OK;
  }

 func_out:
  if (ret == EZCFG_RET_FAIL) {
    ezcfg_json_reset(json);
  }
  return ret;
}

int ezcfg_json_get_msg_len(struct ezcfg_json *json)
{
  return json->msg_len;
}

int ezcfg_json_write_message(struct ezcfg_json *json, char *buf, int len)
{
  //struct ezcfg *ezcfg;
  int ret = -1;
  char *cur, *p;
  struct ezcfg_json_value *vp;
  struct ezcfg_binary_tree_node *np, *np_left, *np_right;
  int cur_len = 0;

  ASSERT(json != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  /* check if the buffer length is enough */
  if (len <= json->msg_len) {
    return ret;
  }

  /* check if json has valid text and stack */
  if ((json->text == NULL) ||
      (json->stack == NULL)) {
    return ret;
  }

  /* check if the stack is empty */
  if (ezcfg_stack_list_is_empty(json->stack) == false) {
    return ret;
  }

  /* check if the value type is valid */
  np = ezcfg_binary_tree_get_root(json->text);
  if (np == NULL) {
    return ret;
  }
  vp = ezcfg_binary_tree_node_get_data(np);
  if (vp == NULL) {
    return ret;
  }

  if ((vp->type != EZCFG_JSON_VALUE_TYPE_OBJECT) &&
      (vp->type != EZCFG_JSON_VALUE_TYPE_ARRAY)) {
    return ret;
  }

  cur = buf;
  cur_len = 0;
  ezcfg_stack_list_push(json->stack, np);

  while ((cur_len < json->msg_len) &&
	 (ezcfg_stack_list_is_empty(json->stack) == false)) {
    np = ezcfg_stack_list_pop(json->stack);
    if (np == NULL) {
      goto func_out;
    }
    vp = ezcfg_binary_tree_node_get_data(np);
    switch (vp->type) {
    case EZCFG_JSON_VALUE_TYPE_OBJECT:
      if (vp->visited == 0) {
	*cur = '{';
	cur++;
	cur_len++;
	vp->visited = 1;

	np_left = ezcfg_binary_tree_node_get_left(np);
	if (ezcfg_stack_list_push(json->stack, np) == false) {
	  goto func_out;
	}
	if (np_left) {
	  if (ezcfg_stack_list_push(json->stack, np_left) == false) {
	    goto func_out;
	  }
	}
      }
      else if (vp->visited == 1) {
	*cur = '}';
	cur++;
	cur_len++;
	vp->visited = 0;

	np_right = ezcfg_binary_tree_node_get_right(np);
	if (np_right) {
	  *cur = ',';
	  cur++;
	  cur_len++;
	  if (ezcfg_stack_list_push(json->stack, np_right) == false) {
	    goto func_out;
	  }
	}
      }
      else {
	goto func_out;
      }
      break;
    case EZCFG_JSON_VALUE_TYPE_NVPAIR:
      if (vp->visited == 0) {
	np_left = ezcfg_binary_tree_node_get_left(np);
	if (np_left == NULL) {
	  goto func_out;
	}

	*cur = '"';
	cur++;
	cur_len++;

	p = vp->element;
	while(*p != '\0') {
	  *cur++ = *p++;
	  cur_len++;
	}

	*cur = '"';
	cur++;
	cur_len++;
	*cur = ':';
	cur++;
	cur_len++;
	vp->visited = 1;

	if (ezcfg_stack_list_push(json->stack, np) == false) {
	  goto func_out;
	}
	if (ezcfg_stack_list_push(json->stack, np_left) == false) {
	  goto func_out;
	}
      }
      else if (vp->visited == 1) {
	vp->visited = 0;
	np_right = ezcfg_binary_tree_node_get_right(np);
	if (np_right) {
	  *cur = ',';
	  cur++;
	  cur_len++;
	  if (ezcfg_stack_list_push(json->stack, np_right) == false) {
	    goto func_out;
	  }
	}
      }
      else {
	goto func_out;
      }
      break;
    case EZCFG_JSON_VALUE_TYPE_ARRAY:
      if (vp->visited == 0) {
	*cur = '[';
	cur++;
	cur_len++;
	vp->visited = 1;

	np_left = ezcfg_binary_tree_node_get_left(np);
	if (ezcfg_stack_list_push(json->stack, np) == false) {
	  goto func_out;
	}
	if (np_left) {
	  if (ezcfg_stack_list_push(json->stack, np_left) == false) {
	    goto func_out;
	  }
	}
      }
      else if (vp->visited == 1) {
	*cur = ']';
	cur++;
	cur_len++;
	vp->visited = 0;

	np_right = ezcfg_binary_tree_node_get_right(np);
	if (np_right) {
	  *cur = ',';
	  cur++;
	  cur_len++;
	  if (ezcfg_stack_list_push(json->stack, np_right) == false) {
	    goto func_out;
	  }
	}
      }
      else {
	goto func_out;
      }
      break;
    case EZCFG_JSON_VALUE_TYPE_EMPTYSET:
      np_left = ezcfg_binary_tree_node_get_left(np);
      np_right = ezcfg_binary_tree_node_get_right(np);
      if (np_left) {
	goto func_out;
      }

      if (np_right) {
	goto func_out;
      }

      if (vp->element) {
	goto func_out;
      }
      break;
    case EZCFG_JSON_VALUE_TYPE_STRING:
      np_left = ezcfg_binary_tree_node_get_left(np);
      if (np_left) {
	goto func_out;
      }

      *cur = '"';
      cur++;
      cur_len++;

      p = vp->element;
      while(*p != '\0') {
	*cur++ = *p++;
	cur_len++;
      }

      *cur = '"';
      cur++;
      cur_len++;

      np_right = ezcfg_binary_tree_node_get_right(np);
      if (np_right) {
	*cur = ',';
	cur++;
	cur_len++;
	if (ezcfg_stack_list_push(json->stack, np_right) == false) {
	  goto func_out;
	}
      }
      break;
    case EZCFG_JSON_VALUE_TYPE_FALSE:
      np_left = ezcfg_binary_tree_node_get_left(np);
      np_right = ezcfg_binary_tree_node_get_right(np);
      if (np_left) {
	goto func_out;
      }

      strcpy(cur, "false");
      cur += 5;
      cur_len += 5;

      if (np_right) {
	*cur = ',';
	cur++;
	cur_len++;
	if (ezcfg_stack_list_push(json->stack, np_right) == false) {
	  goto func_out;
	}
      }
      break;
    case EZCFG_JSON_VALUE_TYPE_NULL:
      np_left = ezcfg_binary_tree_node_get_left(np);
      np_right = ezcfg_binary_tree_node_get_right(np);
      if (np_left) {
	goto func_out;
      }

      strcpy(cur, "null");
      cur += 4;
      cur_len += 4;

      if (np_right) {
	*cur = ',';
	cur++;
	cur_len++;
	if (ezcfg_stack_list_push(json->stack, np_right) == false) {
	  goto func_out;
	}
      }
      break;
    case EZCFG_JSON_VALUE_TYPE_TRUE:
      np_left = ezcfg_binary_tree_node_get_left(np);
      np_right = ezcfg_binary_tree_node_get_right(np);
      if (np_left) {
	goto func_out;
      }

      strcpy(cur, "true");
      cur += 4;
      cur_len += 4;

      if (np_right) {
	*cur = ',';
	cur++;
	cur_len++;
	if (ezcfg_stack_list_push(json->stack, np_right) == false) {
	  goto func_out;
	}
      }
      break;
    case EZCFG_JSON_VALUE_TYPE_NUMBER:
      np_left = ezcfg_binary_tree_node_get_left(np);
      np_right = ezcfg_binary_tree_node_get_right(np);
      if (np_left) {
	goto func_out;
      }

      p = vp->element;
      while(*p != '\0') {
	*cur++ = *p++;
	cur_len++;
      }

      if (np_right) {
	*cur = ',';
	cur++;
	cur_len++;
	if (ezcfg_stack_list_push(json->stack, np_right) == false) {
	  goto func_out;
	}
      }
      break;
    default:
      goto func_out;
      break;
    }
  }

  if ((cur_len == json->msg_len) &&
      (ezcfg_stack_list_is_empty(json->stack) == true)) {
    ret = cur_len;
  }

 func_out:
  return ret;
}

struct ezcfg_linked_list *ezcfg_json_build_nvram_node_list(struct ezcfg_json *json)
{
  struct ezcfg *ezcfg;
  struct ezcfg_nv_pair *data = NULL;
  struct ezcfg_linked_list *list = NULL;
  struct ezcfg_binary_tree_node *np, *np_left, *np_right;
  struct ezcfg_json_value *vp, *vp_left;

  ASSERT(json != NULL);

  ezcfg = json->ezcfg;
  list = ezcfg_linked_list_new(ezcfg,
    ezcfg_nv_pair_del_handler,
    ezcfg_nv_pair_cmp_handler);
  if (list == NULL) {
    return NULL;
  }

  np = ezcfg_binary_tree_get_root(json->text);
  if (np == NULL) {
    goto exit_fail;
  }
  vp = ezcfg_binary_tree_node_get_data(np);
  if (vp == NULL) {
    goto exit_fail;
  }

  if (vp->type != EZCFG_JSON_VALUE_TYPE_OBJECT) {
    goto exit_fail;
  }

  np_left = ezcfg_binary_tree_node_get_left(np);
  np = np_left;

  while(np) {
    vp = ezcfg_binary_tree_node_get_data(np);
    if (vp == NULL) {
      goto exit_fail;
    }
    if (vp->type != EZCFG_JSON_VALUE_TYPE_NVPAIR) {
      goto exit_fail;
    }
    np_left = ezcfg_binary_tree_node_get_left(np);
    if (np_left == NULL) {
      goto exit_fail;
    }
    vp_left = ezcfg_binary_tree_node_get_data(np_left);
    if (vp_left == NULL) {
      goto exit_fail;
    }
    if (vp_left->type != EZCFG_JSON_VALUE_TYPE_STRING) {
      goto exit_fail;
    }

    /* build nv_pair */
    data = ezcfg_nv_pair_new(vp->element, vp_left->element);
    if (data == NULL) {
      goto exit_fail;
    }
    if (ezcfg_linked_list_append(list, data) != EZCFG_RET_OK) {
      goto exit_fail;
    }
    data = NULL;

    /* move to next nv-pair */
    np_right = ezcfg_binary_tree_node_get_right(np);
    np = np_right;
  }

  return list;

 exit_fail:
  if (data) {
    ezcfg_nv_pair_del(data);
  }
  if (list) {
    ezcfg_linked_list_del(list);
  }
  return NULL;
}
