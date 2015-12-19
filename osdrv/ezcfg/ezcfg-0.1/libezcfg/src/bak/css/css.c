/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : css/css.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-13   0.1       Write it from scratch
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

struct at_rule {
  char *identifier;
  char *content;
};

struct declaration {
  char *name;
  char *value;
  struct declaration *next;
};

struct rule_set {
  char *selector;
  struct declaration *declarations;
  struct declaration *tail;
};

struct css_rule {
  int type;
  union {
    struct at_rule a;
    struct rule_set s;
  } u;
  struct css_rule *next;
};

struct ezcfg_css {
  struct ezcfg *ezcfg;

  /* CSS version info */
  unsigned short version_major;
  unsigned short version_minor;

  /* CSS rules */
  struct css_rule *rules;
  struct css_rule *rule_tail;
};

/**
 * Private functions
 **/
static void css_delete_at_rule(struct at_rule a)
{
  if (a.identifier != NULL)
    free(a.identifier);

  if (a.content != NULL)
    free(a.content);
}

static void css_delete_rule_set(struct rule_set s)
{
  struct declaration *dec;

  while (s.declarations != NULL) {
    dec = s.declarations;
    s.declarations = dec->next;
    if (dec->name != NULL)
      free(dec->name);
    if (dec->value != NULL)
      free(dec->value);
    free(dec);
  }

  if (s.selector != NULL)
    free(s.selector);
}

static void css_delete_rules(struct css_rule **rules)
{
  struct css_rule *rule;
  ASSERT(rules != NULL);

  while(*rules != NULL) {
    rule = *rules;
    *rules = rule->next;
    if (rule->type == EZCFG_CSS_RULE_TYPE_AT_RULE) {
      css_delete_at_rule((rule->u).a);
    }
    else if (rule->type == EZCFG_CSS_RULE_TYPE_RULE_SET) {
      css_delete_rule_set((rule->u).s);
    }
    free(rule);
  }
}

static int css_write_at_rule(struct css_rule *rule, char *buf, int len)
{
  int ret;
  ret = (strlen((rule->u).a.identifier) + strlen((rule->u).a.content) + 3);
  if (snprintf(buf, len, "@%s %s\n", (rule->u).a.identifier, (rule->u).a.content) == ret) {
    return ret;
  }
  else {
    return -1;
  }
}

static int css_write_rule_set_declaration(struct declaration *dec, char *buf, int len)
{
  int ret;
  ret = (strlen(dec->name) + strlen(dec->value) + 4);
  if (snprintf(buf, len, "%s: %s;\n", dec->name, dec->value) == ret) {
    return ret;
  }
  else {
    return -1;
  }
}

static int css_write_rule_set(struct css_rule *rule, char *buf, int len)
{
  struct declaration *dec;
  char *p;
  int l;
  int ret;

  p = buf;
  l = len;

  /* first write selector and left curly brace */
  ret = strlen((rule->u).s.selector) + 3;
  if (snprintf(p, l, "%s {\n", (rule->u).s.selector) != ret) {
    return -1;
  }
  p += ret;
  l -= ret;

  /* then write blocks */
  dec = (rule->u).s.declarations;
  while(dec != NULL) {
    ret = css_write_rule_set_declaration(dec, p, l);
    if (ret < 0) {
      return ret;
    }
    p += ret;
    l -= ret;
    dec = dec->next;
  }

  /* last write right curly brace */
  ret = 2;
  if (snprintf(p, l, "}\n") != ret) {
    return -1;
  }
  l -= ret;
  return (len - l);
}

static int css_get_at_rule_length(struct css_rule *rule)
{
  /* snprintf(buf, len, "@%s %s\n", (rule->u).a.identifier, (rule->u).a.content) */
  return (strlen((rule->u).a.identifier) + strlen((rule->u).a.content) + 3);
}

static int css_get_rule_set_declaration_length(struct declaration *dec)
{
  /* snprintf(buf, len, "%s: %s;\n", dec->name, dec->value) */
  return (strlen(dec->name) + strlen(dec->value) + 4);
}

static int css_get_rule_set_length(struct css_rule *rule)
{
  struct declaration *dec;
  int ret = 0;

  /* first write selector and left curly brace */
  /* (snprintf(p, l, "%s {\n", (rule->u).s.selector) */
  ret += strlen((rule->u).s.selector) + 3;

  /* then write blocks */
  dec = (rule->u).s.declarations;
  while(dec != NULL) {
    ret += css_get_rule_set_declaration_length(dec);
    dec = dec->next;
  }

  /* last write right curly brace */
  /* snprintf(p, l, "}\n") */
  ret += 2;
  return ret;
}

static int css_add_rule_set_property(struct css_rule *rule, char *name, char *value)
{
  struct declaration *dec;
  char *p;

  dec = (rule->u).s.declarations;

  while(dec != NULL) {
    if (strcmp(dec->name, name) == 0) {
      if (strcmp(dec->value, value) == 0) {
	return 0;
      }
      else {
	p = strdup(value);
	if (p == NULL) {
	  return -1;
	}
	free(dec->value);
	dec->value = p;
	return 0;
      }
    }
    dec = dec->next;
  }

  /* no same property name */
  dec = calloc(1, sizeof(struct declaration));
  if (dec == NULL) {
    return -1;
  }
  memset(dec, 0, sizeof(struct declaration));
  dec->name = strdup(name);
  if (dec->name == NULL) {
    free(dec);
    return -1;
  }
  dec->value = strdup(value);
  if (dec->value == NULL) {
    free(dec->name);
    free(dec);
    return -1;
  }

  /* set declaration head */
  if ((rule->u).s.declarations == NULL) {
    (rule->u).s.declarations = dec;
  }

  /* set declaration tail */
  dec->next = NULL;
  if ((rule->u).s.tail != NULL) {
    ((rule->u).s.tail)->next = dec;
  }
  (rule->u).s.tail = dec;
  return 0;
}

/**
 * Public functions
 **/
void ezcfg_css_delete(struct ezcfg_css *css)
{
  //struct ezcfg *ezcfg;

  ASSERT(css != NULL);

  //ezcfg = css->ezcfg;

  if (css->rules != NULL)
    css_delete_rules(&(css->rules));

  free(css);
}

/**
 * ezcfg_css_new:
 * Create ezcfg css builder data structure
 * Returns: a new ezcfg css builder data structure
 **/
struct ezcfg_css *ezcfg_css_new(struct ezcfg *ezcfg)
{
  struct ezcfg_css *css;

  ASSERT(ezcfg != NULL);

  /* initialize html info builder data structure */
  css = calloc(1, sizeof(struct ezcfg_css));
  if (css == NULL) {
    err(ezcfg, "initialize css builder error.\n");
    return NULL;
  }

  memset(css, 0, sizeof(struct ezcfg_css));

  css->ezcfg = ezcfg;

  return css;
}

unsigned short ezcfg_css_get_version_major(struct ezcfg_css *css)
{
  ASSERT(css != NULL);

  return css->version_major ;
}

unsigned short ezcfg_css_get_version_minor(struct ezcfg_css *css)
{
  ASSERT(css != NULL);

  return css->version_minor ;
}

bool ezcfg_css_set_version_major(struct ezcfg_css *css, unsigned short major)
{
  //struct ezcfg *ezcfg;

  ASSERT(css != NULL);

  //ezcfg = css->ezcfg;

  css->version_major = major;

  return true;
}

bool ezcfg_css_set_version_minor(struct ezcfg_css *css, unsigned short minor)
{
  //struct ezcfg *ezcfg;

  ASSERT(css != NULL);

  //ezcfg = css->ezcfg;

  css->version_minor = minor;

  return true;
}

int ezcfg_css_add_rule_set(struct ezcfg_css *css, char *selector, char *name, char *value)
{
  //struct ezcfg *ezcfg;
  struct css_rule *rule;

  ASSERT(css != NULL);

  //ezcfg = css->ezcfg;

  rule = css->rules;

  /* check selector */
  while (rule != NULL) {
    if (rule->type == EZCFG_CSS_RULE_TYPE_RULE_SET) {
      if (strcmp((rule->u).s.selector, selector) == 0) {
	/* find selector */
	return css_add_rule_set_property(rule, name, value);
      }
    }
    rule = rule->next;
  }

  /* no selector, add a new rule */
  rule = calloc(1, sizeof(struct css_rule));
  if (rule == NULL) {
    return -1;
  }

  memset(rule, 0, sizeof(struct css_rule));
  rule->type = EZCFG_CSS_RULE_TYPE_RULE_SET;
  (rule->u).s.selector = strdup(selector);
  if ((rule->u).s.selector == NULL) {
    free(rule);
    return -1;
  }

  if (css_add_rule_set_property(rule, name, value) < 0) {
    free(rule);
    return -1;
  }

  /* set rule head, say rules */
  if (css->rules == NULL) {
    css->rules = rule;
  }

  /* set rule_tail */
  rule->next = NULL;
  if (css->rule_tail != NULL) {
    (css->rule_tail)->next = rule;
  }
  css->rule_tail = rule;
  return 0;
}

int ezcfg_css_get_message_length(struct ezcfg_css *css)
{
  //struct ezcfg *ezcfg;
  struct css_rule *rule;
  int ret, count;

  ASSERT(css != NULL);

  //ezcfg = css->ezcfg;

  rule = css->rules;
  count = 0;
  while(rule != NULL) {
    if (rule->type == EZCFG_CSS_RULE_TYPE_AT_RULE) {
      ret = css_get_at_rule_length(rule);
    }
    else {
      ret = css_get_rule_set_length(rule);
    }
    if (ret < 0) {
      return ret;
    }
    count += ret;
    rule = rule->next;
  }
  return count;
}

int ezcfg_css_write_message(struct ezcfg_css *css, char *buf, int len)
{
  //struct ezcfg *ezcfg;
  struct css_rule *rule;
  char *p;
  int l;
  int ret;

  ASSERT(css != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  //ezcfg = css->ezcfg;

  rule = css->rules;
  p = buf;
  l = len;
  while(rule != NULL) {
    if (rule->type == EZCFG_CSS_RULE_TYPE_AT_RULE) {
      ret = css_write_at_rule(rule, p, l);
    }
    else if (rule->type == EZCFG_CSS_RULE_TYPE_RULE_SET) {
      ret = css_write_rule_set(rule, p, l);
    }
    else {
      ret = -1;
    }
    if (ret < 0) {
      return ret;
    }
    p += ret;
    l -= ret;
    rule = rule->next;
  }
  return (len - l);
}
