/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/http_html_index.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-03-25   0.1       Write it from scratch
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

#if (HAVE_EZBOX_EZCFG_NLS == 1)
#include <locale.h>
#include <libintl.h>
#endif

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-http.h"
#include "ezcfg-html.h"

/**
 * Private functions
 **/

static int build_home_index_response(struct ezcfg_http *http, struct ezcfg_nvram *nvram)
{
  struct ezcfg *ezcfg;
  struct ezcfg_html *html = NULL;
  struct ezcfg_locale *locale = NULL;
  int head_index, body_index, p_index, child_index;
  char *msg = NULL;
  int msg_len;
  char buf[1024];
  int n;
  int rc = 0;
	
  ASSERT(http != NULL);
  ASSERT(nvram != NULL);

  ezcfg = ezcfg_http_get_ezcfg(http);

  /* set locale info */
  locale = ezcfg_locale_new(ezcfg);
  if (locale != NULL) {
    ezcfg_locale_set_domain(locale, EZCFG_HTTP_HTML_INDEX_DOMAIN);
    ezcfg_locale_set_dir(locale, EZCFG_HTTP_HTML_LANG_DIR);
  }

  html = ezcfg_html_new(ezcfg);

  if (html == NULL) {
    err(ezcfg, "can not alloc html.\n");
    goto func_exit;
  }

  /* clean HTML structure info */
  ezcfg_html_reset_attributes(html);

  /* build HTML */
  ezcfg_html_set_version_major(html, 4);
  ezcfg_html_set_version_minor(html, 1);

  /* HTML root */
  ezcfg_html_set_root(html, EZCFG_HTML_HTML_ELEMENT_NAME);

  /* HTML Head */
  head_index = ezcfg_html_set_head(html, EZCFG_HTML_HEAD_ELEMENT_NAME);
  if (head_index < 0) {
    err(ezcfg, "ezcfg_html_set_head error.\n");
    goto func_exit;
  }

  /* HTML Meta charset */
  child_index = ezcfg_html_add_head_child(html, head_index, -1, EZCFG_HTML_META_ELEMENT_NAME, NULL);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_html_add_head_child error.\n");
    goto func_exit;
  }

  /* HTML http-equiv content-type */
  ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_HTTP_EQUIV_ATTRIBUTE_NAME, EZCFG_HTTP_HEADER_CONTENT_TYPE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
  snprintf(buf, sizeof(buf), "%s; %s=%s", EZCFG_HTTP_MIME_TEXT_HTML, EZCFG_HTTP_CHARSET_NAME, EZCFG_HTTP_CHARSET_UTF8);
  ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_CONTENT_ATTRIBUTE_NAME, buf, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

  /* HTML Meta Cache-control */
  child_index = ezcfg_html_add_head_child(html, head_index, child_index, EZCFG_HTML_META_ELEMENT_NAME, NULL);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_html_add_head_child error.\n");
    goto func_exit;
  }

  /* HTML http-equiv cache-control */
  ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_HTTP_EQUIV_ATTRIBUTE_NAME, EZCFG_HTTP_HEADER_CACHE_CONTROL, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
  ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_CONTENT_ATTRIBUTE_NAME, EZCFG_HTTP_CACHE_REQUEST_NO_CACHE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

  /* HTML Meta expires */
  child_index = ezcfg_html_add_head_child(html, head_index, child_index, EZCFG_HTML_META_ELEMENT_NAME, NULL);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_html_add_head_child error.\n");
    goto func_exit;
  }

  /* HTML http-equiv expires */
  ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_HTTP_EQUIV_ATTRIBUTE_NAME, EZCFG_HTTP_HEADER_EXPIRES, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
  ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_CONTENT_ATTRIBUTE_NAME, "0", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

  /* HTML Meta pragma */
  child_index = ezcfg_html_add_head_child(html, head_index, child_index, EZCFG_HTML_META_ELEMENT_NAME, NULL);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_html_add_head_child error.\n");
    goto func_exit;
  }

  /* HTML http-equiv pragma */
  ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_HTTP_EQUIV_ATTRIBUTE_NAME, EZCFG_HTTP_HEADER_PRAGMA, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
  ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_CONTENT_ATTRIBUTE_NAME, EZCFG_HTTP_PRAGMA_NO_CACHE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);


  /* HTML Title */
  child_index = ezcfg_html_add_head_child(html, head_index, child_index, EZCFG_HTML_TITLE_ELEMENT_NAME, ezcfg_locale_text(locale, "Welcome to ezbox"));
  if (child_index < 0) {
    err(ezcfg, "ezcfg_html_add_head_child error.\n");
    goto func_exit;
  }


  /* HTML Body */
  body_index = ezcfg_html_set_body(html, EZCFG_HTML_BODY_ELEMENT_NAME);
  if (body_index < 0) {
    err(ezcfg, "ezcfg_html_set_body error.\n");
    goto func_exit;
  }


  child_index = -1;
  /* HTML P */
  child_index = ezcfg_html_add_body_child(html, body_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, NULL);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_html_add_body_child error.\n");
    goto func_exit;
  }

  /* save <p> index */
  p_index = child_index;
  child_index = -1;

  child_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_A_ELEMENT_NAME, ezcfg_locale_text(locale, "Administration Page"));
  if (child_index < 0) {
    err(ezcfg, "ezcfg_html_add_body_child error.\n");
    goto func_exit;
  }

  ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_HREF_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
  /* restore <p> index */
  child_index = p_index;

  /* HTML P */
  child_index = ezcfg_html_add_body_child(html, body_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, NULL);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_html_add_body_child error.\n");
    goto func_exit;
  }

  /* save <p> index */
  p_index = child_index;
  child_index = -1;

  child_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_A_ELEMENT_NAME, ezcfg_locale_text(locale, "ezbox Project Home Page"));
  if (child_index < 0) {
    err(ezcfg, "ezcfg_html_add_body_child error.\n");
    goto func_exit;
  }

  /* <a href="http://code.google.com/p/ezbox/">ezbox Project Home Page</a> */
  ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_HREF_ATTRIBUTE_NAME, EZBOX_PROJECT_HOME_PAGE_URI, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
  /* restore <p> index */
  child_index = p_index;

  /* HTML P */
  child_index = ezcfg_html_add_body_child(html, body_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, NULL);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_html_add_body_child error.\n");
    goto func_exit;
  }

  /* save <p> index */
  p_index = child_index;
  child_index = -1;

  child_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_A_ELEMENT_NAME, ezcfg_locale_text(locale, "User Forum"));
  if (child_index < 0) {
    err(ezcfg, "ezcfg_html_add_body_child error.\n");
    goto func_exit;
  }

  /* <a href="http://code.google.com/p/ezbox/">User Forum</a> */
  ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_HREF_ATTRIBUTE_NAME, EZBOX_PROJECT_HOME_PAGE_URI, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
  /* restore <p> index */
  //child_index = p_index;

  msg_len = ezcfg_html_get_message_length(html);
  if (msg_len < 0) {
    err(ezcfg, "ezcfg_html_get_message_length\n");
    rc = -1;
    goto func_exit;
  }
  msg_len++; /* one more for '\0' */
  msg = (char *)malloc(msg_len);
  if (msg == NULL) {
    err(ezcfg, "malloc error.\n");
    rc = -1;
    goto func_exit;
  }

  memset(msg, 0, msg_len);
  n = ezcfg_html_write_message(html, msg, msg_len);
  if (n < 0) {
    err(ezcfg, "ezcfg_html_write_message\n");
    rc = -1;
    goto func_exit;
  }

  /* FIXME: name point to http->request_uri !!!
   * never reset http before using name */
  /* clean http structure info */
  ezcfg_http_reset_attributes(http);
  ezcfg_http_set_status_code(http, 200);
  ezcfg_http_set_state_response(http);

  if (ezcfg_http_set_message_body(http, msg, n) == NULL) {
    err(ezcfg, "ezcfg_http_set_message_body\n");
    rc = -1;
    goto func_exit;
  }

  /* HTML http-equiv content-type */
  snprintf(buf, sizeof(buf), "%s; %s=%s", EZCFG_HTTP_MIME_TEXT_HTML, EZCFG_HTTP_CHARSET_NAME, EZCFG_HTTP_CHARSET_UTF8);
  ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CONTENT_TYPE, buf);

  /* HTML http-equiv cache-control */
  ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CACHE_CONTROL, EZCFG_HTTP_CACHE_REQUEST_NO_CACHE);

  /* HTML http-equiv expires */
  ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_EXPIRES, "0");

  /* HTML http-equiv pragma */
  ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_PRAGMA, EZCFG_HTTP_PRAGMA_NO_CACHE);

  snprintf(buf, sizeof(buf), "%u", ezcfg_http_get_message_body_len(http));
  ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CONTENT_LENGTH , buf);

  /* set return value */
  rc = 0;
 func_exit:
  if (locale != NULL)
    ezcfg_locale_delete(locale);

  if (html != NULL)
    ezcfg_html_delete(html);

  if (msg != NULL)
    free(msg);

  return rc;
}

static int build_redirect_to_home_response(struct ezcfg_http *http, struct ezcfg_nvram *nvram)
{
  struct ezcfg *ezcfg;
  struct ezcfg_html *html = NULL;
  int head_index, body_index;
  char *msg = NULL;
  int msg_len;
  int n;
  int rc = 0;
	
  ASSERT(http != NULL);
  ASSERT(nvram != NULL);

  ezcfg = ezcfg_http_get_ezcfg(http);

  html = ezcfg_html_new(ezcfg);

  if (html == NULL) {
    err(ezcfg, "can not alloc html.\n");
    goto func_exit;
  }

  /* clean HTML structure info */
  ezcfg_html_reset_attributes(html);

  /* build HTML */
  ezcfg_html_set_version_major(html, 4);
  ezcfg_html_set_version_minor(html, 1);

  /* HTML root */
  ezcfg_html_set_root(html, EZCFG_HTML_HTML_ELEMENT_NAME);

  /* HTML Head */
  head_index = ezcfg_html_set_head(html, EZCFG_HTML_HEAD_ELEMENT_NAME);
  if (head_index < 0) {
    err(ezcfg, "ezcfg_html_set_head()\n");
    goto func_exit;
  }

  /* HTML Body */
  body_index = ezcfg_html_set_body(html, EZCFG_HTML_BODY_ELEMENT_NAME);
  if (body_index < 0) {
    err(ezcfg, "ezcfg_html_set_body()\n");
    goto func_exit;
  }

  msg_len = ezcfg_html_get_message_length(html);
  if (msg_len < 0) {
    err(ezcfg, "ezcfg_html_get_message_length\n");
    rc = -1;
    goto func_exit;
  }
  msg_len++; /* one more for '\0' */
  msg = (char *)malloc(msg_len);
  if (msg == NULL) {
    err(ezcfg, "malloc error.\n");
    rc = -1;
    goto func_exit;
  }

  memset(msg, 0, msg_len);
  n = ezcfg_html_write_message(html, msg, msg_len);
  if (n < 0) {
    err(ezcfg, "ezcfg_html_write_message\n");
    rc = -1;
    goto func_exit;
  }

  /* FIXME: name point to http->request_uri !!!
   * never reset http before using name */
  /* clean http structure info */
  ezcfg_http_reset_attributes(http);
  ezcfg_http_set_status_code(http, 200);
  ezcfg_http_set_state_response(http);

  if (ezcfg_http_set_message_body(http, msg, n) == NULL) {
    err(ezcfg, "ezcfg_http_set_message_body\n");
    rc = -1;
    goto func_exit;
  }

  /* set return value */
  rc = 0;

 func_exit:
  if (html != NULL)
    ezcfg_html_delete(html);

  if (msg != NULL)
    free(msg);

  return rc;
}

/**
 * Public functions
 **/

int ezcfg_http_handle_index_request(struct ezcfg_http *http, struct ezcfg_nvram *nvram)
{
  //struct ezcfg *ezcfg;
  char *request_uri;
  int ret = -1;

  ASSERT(http != NULL);
  ASSERT(nvram != NULL);

  //ezcfg = ezcfg_http_get_ezcfg(http);

  request_uri = ezcfg_http_get_request_uri(http);

  if (request_uri != NULL &&
      strcmp(request_uri, EZCFG_HTTP_HTML_HOME_INDEX_URI) == 0) {
    /* home index uri=[/] */
    ret = build_home_index_response(http, nvram);
  }
  else {
    /* redirect to home index */
    ret = build_redirect_to_home_response(http, nvram);
  }
  return ret;
}
