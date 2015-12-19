/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : soap/soap_http_nvram.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-09-07   0.1       Write it from scratch
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
#include "ezcfg-soap_http.h"

#if 0
#define DBG(format, args...) do { \
    char path[256];		  \
    FILE *fp;						    \
    snprintf(path, 256, "/tmp/%d-debug.txt", getpid());	    \
    fp = fopen(path, "a");				    \
    if (fp) {						    \
      fprintf(fp, format, ## args);			    \
      fclose(fp);					    \
    }							    \
  } while(0)
#else
#define DBG(format, args...)
#endif

/**
 * Private functions
 **/

static int build_nvram_get_response(struct ezcfg_soap_http *sh, struct ezcfg_nvram *nvram, const char *name)
{
  struct ezcfg *ezcfg;
  struct ezcfg_http *http;
  struct ezcfg_soap *soap;
  char *value = NULL;
  int body_index, child_index;
  int n;
  bool ret;
  char *msg = NULL;
  int msg_len;
  int rc = 0;
	
  ASSERT(sh != NULL);
  ASSERT(sh->http != NULL);
  ASSERT(sh->soap != NULL);
  ASSERT(nvram != NULL);
  ASSERT(name != NULL);

  ezcfg = sh->ezcfg;
  http = sh->http;
  soap = sh->soap;

  /* get nvram entry value, must free it!!! */
  ret = ezcfg_nvram_get_entry_value(nvram, name, &value);

  if (value != NULL) {
    int getnv_index;

    /* clean SOAP structure info */
    ezcfg_soap_reset_attributes(soap);

    /* build SOAP */
    ezcfg_soap_set_version_major(soap, 1);
    ezcfg_soap_set_version_minor(soap, 2);

    /* SOAP Envelope */
    ezcfg_soap_set_envelope(soap, EZCFG_SOAP_ENVELOPE_ELEMENT_NAME);
    ezcfg_soap_add_envelope_attribute(soap, EZCFG_SOAP_ENVELOPE_ATTR_NS_NAME, EZCFG_SOAP_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

    /* SOAP Body */
    body_index = ezcfg_soap_set_body(soap, EZCFG_SOAP_BODY_ELEMENT_NAME);

    /* Body child getNvram part */
    getnv_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_SOAP_NVRAM_GETNV_RESPONSE_ELEMENT_NAME, NULL);
    if (getnv_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }
    ezcfg_soap_add_body_child_attribute(soap, getnv_index, EZCFG_SOAP_NVRAM_ATTR_NS_NAME, EZCFG_SOAP_NVRAM_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

    /* nvram name part */
    child_index = ezcfg_soap_add_body_child(soap, getnv_index, -1, EZCFG_SOAP_NVRAM_NAME_ELEMENT_NAME, name);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* nvram value part */
    child_index = ezcfg_soap_add_body_child(soap, getnv_index, -1, EZCFG_SOAP_NVRAM_VALUE_ELEMENT_NAME, value);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    msg_len = ezcfg_soap_get_message_length(soap);
    if (msg_len < 0) {
      err(ezcfg, "ezcfg_soap_get_message_length\n");
      rc = -1;
      goto exit;
    }
    msg_len += strlen("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    msg_len++; /* one more for '\n' */
    msg_len++; /* one more for '\0' */
    msg = (char *)malloc(msg_len);
    if (msg == NULL) {
      err(ezcfg, "malloc error.\n");
      rc = -1;
      goto exit;
    }

    memset(msg, 0, msg_len);
    snprintf(msg, msg_len, "%s\n", "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    n = strlen(msg);
    n += ezcfg_soap_write_message(soap, msg + n, msg_len - n);

    /* FIXME: name point to http->request_uri !!!
     * never reset http before using name */
    /* clean http structure info */
    ezcfg_http_reset_attributes(http);
    ezcfg_http_set_status_code(http, 200);
    ezcfg_http_set_state_response(http);

    if (ezcfg_http_set_message_body(http, msg, n) == NULL) {
      err(ezcfg, "ezcfg_http_set_message_body\n");
      rc = -1;
      goto exit;
    }

    snprintf(msg, msg_len, "%s; %s=%s", EZCFG_HTTP_MIME_APPLICATION_SOAP_XML , EZCFG_HTTP_CHARSET_NAME , EZCFG_HTTP_CHARSET_UTF8);
    ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_TYPE , msg);

    snprintf(msg, msg_len, "%u", ezcfg_http_get_message_body_len(http));
    ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH , msg);

    /* set return value */
    rc = 0;
  }
  else {
    int fault_index;
    int code_index;
    int reason_index;

    if (ret == true) {
      err(ezcfg, "no nvram [%s]\n", name);
    }
    else {
      err(ezcfg, "get nvram [%s] error\n", name);
    }

    /* clean SOAP structure info */
    ezcfg_soap_reset_attributes(soap);

    /* build SOAP */
    ezcfg_soap_set_version_major(soap, 1);
    ezcfg_soap_set_version_minor(soap, 2);

    /* SOAP Envelope */
    ezcfg_soap_set_envelope(soap, EZCFG_SOAP_ENVELOPE_ELEMENT_NAME);
    ezcfg_soap_add_envelope_attribute(soap, EZCFG_SOAP_ENVELOPE_ATTR_NS_NAME, EZCFG_SOAP_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

    /* SOAP Body */
    body_index = ezcfg_soap_set_body(soap, EZCFG_SOAP_BODY_ELEMENT_NAME);

    /* SOAP Fault part */
    fault_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_SOAP_FAULT_ELEMENT_NAME, NULL);
    if (fault_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* SOAP Fault Code part */
    code_index = ezcfg_soap_add_body_child(soap, fault_index, -1, EZCFG_SOAP_CODE_ELEMENT_NAME, NULL);
    if (code_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* SOAP Fault Code value part */
    child_index = ezcfg_soap_add_body_child(soap, code_index, -1, EZCFG_SOAP_VALUE_ELEMENT_NAME, EZCFG_SOAP_VALUE_ELEMENT_VALUE);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* SOAP Fault Reason part */
    reason_index = ezcfg_soap_add_body_child(soap, fault_index, -1, EZCFG_SOAP_REASON_ELEMENT_NAME, NULL);
    if (reason_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* SOAP Fault Reason Text part */
    child_index = ezcfg_soap_add_body_child(soap, reason_index, -1, EZCFG_SOAP_TEXT_ELEMENT_NAME, EZCFG_SOAP_NVRAM_INVALID_NAME_FAULT_VALUE);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    msg_len = ezcfg_soap_get_message_length(soap);
    if (msg_len < 0) {
      err(ezcfg, "ezcfg_soap_get_message_length\n");
      rc = -1;
      goto exit;
    }
    msg_len += strlen("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    msg_len++; /* one more for '\n' */
    msg_len++; /* one more for '\0' */
    msg = (char *)malloc(msg_len);
    if (msg == NULL) {
      err(ezcfg, "malloc error.\n");
      rc = -1;
      goto exit;
    }

    memset(msg, 0, msg_len);
    snprintf(msg, msg_len, "%s\n", "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    n = strlen(msg);
    n += ezcfg_soap_write_message(soap, msg + n, msg_len - n);

    /* FIXME: name point to http->request_uri !!!
     * never reset http before using name */
    /* clean http structure info */
    ezcfg_http_reset_attributes(http);
    ezcfg_http_set_status_code(http, 200);
    ezcfg_http_set_state_response(http);

    if (ezcfg_http_set_message_body(http, msg, n) == NULL) {
      err(ezcfg, "ezcfg_http_set_message_body\n");
      rc = -1;
      goto exit;
    }

    snprintf(msg, msg_len, "%s; %s=%s", EZCFG_HTTP_MIME_APPLICATION_SOAP_XML , EZCFG_HTTP_CHARSET_NAME , EZCFG_HTTP_CHARSET_UTF8);
    ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_TYPE , msg);

    snprintf(msg, msg_len, "%u", ezcfg_http_get_message_body_len(http));
    ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH , msg);

    /* set return value */
    rc = 0;
  }
 exit:
  if (value != NULL)
    free(value);

  if (msg != NULL)
    free(msg);

  return rc;
}

static int build_nvram_set_response(struct ezcfg_soap_http *sh, struct ezcfg_nvram *nvram, const char *name, const char *value)
{
  struct ezcfg *ezcfg;
  struct ezcfg_http *http;
  struct ezcfg_soap *soap;
  char *result;
  int body_index, child_index;
  int n;
  char *msg = NULL;
  int msg_len;
  int rc = 0;
	
  ASSERT(sh != NULL);
  ASSERT(sh->http != NULL);
  ASSERT(sh->soap != NULL);
  ASSERT(nvram != NULL);

  ezcfg = sh->ezcfg;
  http = sh->http;
  soap = sh->soap;
  result = NULL;

  if (name != NULL && value != NULL) {
    if (ezcfg_nvram_set_entry(nvram, name, value) == true) {
      result = EZCFG_SOAP_NVRAM_RESULT_VALUE_OK;
    }
    else {
      result = EZCFG_SOAP_NVRAM_RESULT_VALUE_ERROR;
    }
  }

  if (result != NULL) {
    int setnv_index;

    /* clean SOAP structure info */
    ezcfg_soap_reset_attributes(soap);

    /* build SOAP */
    ezcfg_soap_set_version_major(soap, 1);
    ezcfg_soap_set_version_minor(soap, 2);

    /* SOAP Envelope */
    ezcfg_soap_set_envelope(soap, EZCFG_SOAP_ENVELOPE_ELEMENT_NAME);
    ezcfg_soap_add_envelope_attribute(soap, EZCFG_SOAP_ENVELOPE_ATTR_NS_NAME, EZCFG_SOAP_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

    /* SOAP Body */
    body_index = ezcfg_soap_set_body(soap, EZCFG_SOAP_BODY_ELEMENT_NAME);

    /* Body child setNvram part */
    setnv_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_SOAP_NVRAM_SETNV_RESPONSE_ELEMENT_NAME, NULL);
    if (setnv_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }
    ezcfg_soap_add_body_child_attribute(soap, setnv_index, EZCFG_SOAP_NVRAM_ATTR_NS_NAME, EZCFG_SOAP_NVRAM_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

    /* nvram result part */
    child_index = ezcfg_soap_add_body_child(soap, setnv_index, -1, EZCFG_SOAP_NVRAM_RESULT_ELEMENT_NAME, result);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    msg_len = ezcfg_soap_get_message_length(soap);
    if (msg_len < 0) {
      err(ezcfg, "ezcfg_soap_get_message_length\n");
      rc = -1;
      goto exit;
    }
    msg_len += strlen("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    msg_len++; /* one more for '\n' */
    msg_len++; /* one more for '\0' */
    msg = (char *)malloc(msg_len);
    if (msg == NULL) {
      err(ezcfg, "malloc error.\n");
      rc = -1;
      goto exit;
    }

    memset(msg, 0, msg_len);
    snprintf(msg, msg_len, "%s\n", "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    n = strlen(msg);
    n += ezcfg_soap_write_message(soap, msg + n, msg_len - n);

    /* FIXME: name point to http->request_uri !!!
     * never reset http before using name */
    /* clean http structure info */
    ezcfg_http_reset_attributes(http);
    ezcfg_http_set_status_code(http, 200);
    ezcfg_http_set_state_response(http);

    ezcfg_http_set_message_body(http, msg, n);

    snprintf(msg, msg_len, "%s; %s=%s", EZCFG_HTTP_MIME_APPLICATION_SOAP_XML , EZCFG_HTTP_CHARSET_NAME , EZCFG_HTTP_CHARSET_UTF8);
    ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_TYPE , msg);

    snprintf(msg, msg_len, "%u", ezcfg_http_get_message_body_len(http));
    ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH , msg);

    /* set return value */
    rc = 0;
  }
  else {
    int fault_index;
    int code_index;
    int reason_index;

    err(ezcfg, "can't set nvram [%s]\n", name);

    /* clean SOAP structure info */
    ezcfg_soap_reset_attributes(soap);

    /* build SOAP */
    ezcfg_soap_set_version_major(soap, 1);
    ezcfg_soap_set_version_minor(soap, 2);

    /* SOAP Envelope */
    ezcfg_soap_set_envelope(soap, EZCFG_SOAP_ENVELOPE_ELEMENT_NAME);
    ezcfg_soap_add_envelope_attribute(soap, EZCFG_SOAP_ENVELOPE_ATTR_NS_NAME, EZCFG_SOAP_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

    /* SOAP Body */
    body_index = ezcfg_soap_set_body(soap, EZCFG_SOAP_BODY_ELEMENT_NAME);

    /* SOAP Fault part */
    fault_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_SOAP_FAULT_ELEMENT_NAME, NULL);
    if (fault_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* SOAP Fault Code part */
    code_index = ezcfg_soap_add_body_child(soap, fault_index, -1, EZCFG_SOAP_CODE_ELEMENT_NAME, NULL);
    if (code_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* SOAP Fault Code value part */
    child_index = ezcfg_soap_add_body_child(soap, code_index, -1, EZCFG_SOAP_VALUE_ELEMENT_NAME, EZCFG_SOAP_VALUE_ELEMENT_VALUE);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* SOAP Fault Reason part */
    reason_index = ezcfg_soap_add_body_child(soap, fault_index, -1, EZCFG_SOAP_REASON_ELEMENT_NAME, NULL);
    if (reason_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* SOAP Fault Reason Text part */
    child_index = ezcfg_soap_add_body_child(soap, reason_index, -1, EZCFG_SOAP_TEXT_ELEMENT_NAME, EZCFG_SOAP_NVRAM_INVALID_VALUE_FAULT_VALUE);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    msg_len = ezcfg_soap_get_message_length(soap);
    if (msg_len < 0) {
      err(ezcfg, "ezcfg_soap_get_message_length\n");
      rc = -1;
      goto exit;
    }
    msg_len += strlen("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    msg_len++; /* one more for '\n' */
    msg_len++; /* one more for '\0' */
    msg = (char *)malloc(msg_len);
    if (msg == NULL) {
      err(ezcfg, "malloc error.\n");
      rc = -1;
      goto exit;
    }

    memset(msg, 0, msg_len);
    snprintf(msg, msg_len, "%s\n", "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    n = strlen(msg);
    n += ezcfg_soap_write_message(soap, msg + n, msg_len - n);

    /* FIXME: name point to http->request_uri !!!
     * never reset http before using name */
    /* clean http structure info */
    ezcfg_http_reset_attributes(http);
    ezcfg_http_set_status_code(http, 200);
    ezcfg_http_set_state_response(http);

    ezcfg_http_set_message_body(http, msg, n);

    snprintf(msg, msg_len, "%s; %s=%s", EZCFG_HTTP_MIME_APPLICATION_SOAP_XML , EZCFG_HTTP_CHARSET_NAME , EZCFG_HTTP_CHARSET_UTF8);
    ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_TYPE , msg);

    snprintf(msg, msg_len, "%u", ezcfg_http_get_message_body_len(http));
    ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH , msg);

    /* set return value */
    rc = 0;
  }
 exit:
  if (msg != NULL)
    free(msg);

  return rc;
}

static int build_nvram_unset_response(struct ezcfg_soap_http *sh, struct ezcfg_nvram *nvram, const char *name)
{
  struct ezcfg *ezcfg;
  struct ezcfg_http *http;
  struct ezcfg_soap *soap;
  char *value = NULL, *result;
  int body_index, child_index;
  int n;
  bool ret;
  char *msg = NULL;
  int msg_len;
  int rc = 0;
	
  ASSERT(sh != NULL);
  ASSERT(sh->http != NULL);
  ASSERT(sh->soap != NULL);
  ASSERT(nvram != NULL);
  ASSERT(name != NULL);

  ezcfg = sh->ezcfg;
  http = sh->http;
  soap = sh->soap;
  result = NULL;

  /* get nvram entry value, must free it!!! */
  ret = ezcfg_nvram_get_entry_value(nvram, name, &value);

  if (value != NULL) {
    ezcfg_nvram_unset_entry(nvram, name);
    result = EZCFG_SOAP_NVRAM_RESULT_VALUE_OK;
    free(value);
  }

  if (result != NULL) {
    int unsetnv_index;

    /* clean SOAP structure info */
    ezcfg_soap_reset_attributes(soap);

    /* build SOAP */
    ezcfg_soap_set_version_major(soap, 1);
    ezcfg_soap_set_version_minor(soap, 2);

    /* SOAP Envelope */
    ezcfg_soap_set_envelope(soap, EZCFG_SOAP_ENVELOPE_ELEMENT_NAME);
    ezcfg_soap_add_envelope_attribute(soap, EZCFG_SOAP_ENVELOPE_ATTR_NS_NAME, EZCFG_SOAP_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

    /* SOAP Body */
    body_index = ezcfg_soap_set_body(soap, EZCFG_SOAP_BODY_ELEMENT_NAME);

    /* Body child unsetNvram part */
    unsetnv_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_SOAP_NVRAM_UNSETNV_RESPONSE_ELEMENT_NAME, NULL);
    if (unsetnv_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }
    ezcfg_soap_add_body_child_attribute(soap, unsetnv_index, EZCFG_SOAP_NVRAM_ATTR_NS_NAME, EZCFG_SOAP_NVRAM_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

    /* nvram result part */
    child_index = ezcfg_soap_add_body_child(soap, unsetnv_index, -1, EZCFG_SOAP_NVRAM_RESULT_ELEMENT_NAME, result);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    msg_len = ezcfg_soap_get_message_length(soap);
    if (msg_len < 0) {
      err(ezcfg, "ezcfg_soap_get_message_length\n");
      rc = -1;
      goto exit;
    }
    msg_len += strlen("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    msg_len++; /* one more for '\n' */
    msg_len++; /* one more for '\0' */
    msg = (char *)malloc(msg_len);
    if (msg == NULL) {
      err(ezcfg, "malloc error.\n");
      rc = -1;
      goto exit;
    }

    memset(msg, 0, msg_len);
    snprintf(msg, msg_len, "%s\n", "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    n = strlen(msg);
    n += ezcfg_soap_write_message(soap, msg + n, msg_len - n);

    /* FIXME: name point to http->request_uri !!!
     * never reset http before using name */
    /* clean http structure info */
    ezcfg_http_reset_attributes(http);
    ezcfg_http_set_status_code(http, 200);
    ezcfg_http_set_state_response(http);

    ezcfg_http_set_message_body(http, msg, n);

    snprintf(msg, msg_len, "%s; %s=%s", EZCFG_HTTP_MIME_APPLICATION_SOAP_XML , EZCFG_HTTP_CHARSET_NAME , EZCFG_HTTP_CHARSET_UTF8);
    ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_TYPE , msg);

    snprintf(msg, msg_len, "%u", ezcfg_http_get_message_body_len(http));
    ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH , msg);

    /* set return value */
    rc = 0;
  }
  else {
    int fault_index;
    int code_index;
    int reason_index;

    if (ret == true) {
      err(ezcfg, "no nvram [%s]\n", name);
    }
    else {
      err(ezcfg, "get nvram [%s] error\n", name);
    }

    /* clean SOAP structure info */
    ezcfg_soap_reset_attributes(soap);

    /* build SOAP */
    ezcfg_soap_set_version_major(soap, 1);
    ezcfg_soap_set_version_minor(soap, 2);

    /* SOAP Envelope */
    ezcfg_soap_set_envelope(soap, EZCFG_SOAP_ENVELOPE_ELEMENT_NAME);
    ezcfg_soap_add_envelope_attribute(soap, EZCFG_SOAP_ENVELOPE_ATTR_NS_NAME, EZCFG_SOAP_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

    /* SOAP Body */
    body_index = ezcfg_soap_set_body(soap, EZCFG_SOAP_BODY_ELEMENT_NAME);

    /* SOAP Fault part */
    fault_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_SOAP_FAULT_ELEMENT_NAME, NULL);
    if (fault_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* SOAP Fault Code part */
    code_index = ezcfg_soap_add_body_child(soap, fault_index, -1, EZCFG_SOAP_CODE_ELEMENT_NAME, NULL);
    if (code_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* SOAP Fault Code value part */
    child_index = ezcfg_soap_add_body_child(soap, code_index, -1, EZCFG_SOAP_VALUE_ELEMENT_NAME, EZCFG_SOAP_VALUE_ELEMENT_VALUE);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* SOAP Fault Reason part */
    reason_index = ezcfg_soap_add_body_child(soap, fault_index, -1, EZCFG_SOAP_REASON_ELEMENT_NAME, NULL);
    if (reason_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* SOAP Fault Reason Text part */
    child_index = ezcfg_soap_add_body_child(soap, reason_index, -1, EZCFG_SOAP_TEXT_ELEMENT_NAME, EZCFG_SOAP_NVRAM_INVALID_NAME_FAULT_VALUE);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    msg_len = ezcfg_soap_get_message_length(soap);
    if (msg_len < 0) {
      err(ezcfg, "ezcfg_soap_get_message_length\n");
      rc = -1;
      goto exit;
    }
    msg_len += strlen("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    msg_len++; /* one more for '\n' */
    msg_len++; /* one more for '\0' */
    msg = (char *)malloc(msg_len);
    if (msg == NULL) {
      err(ezcfg, "malloc error.\n");
      rc = -1;
      goto exit;
    }

    memset(msg, 0, msg_len);
    snprintf(msg, msg_len, "%s\n", "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    n = strlen(msg);
    n += ezcfg_soap_write_message(soap, msg + n, msg_len - n);

    /* FIXME: name point to http->request_uri !!!
     * never reset http before using name */
    /* clean http structure info */
    ezcfg_http_reset_attributes(http);
    ezcfg_http_set_status_code(http, 200);
    ezcfg_http_set_state_response(http);

    ezcfg_http_set_message_body(http, msg, n);

    snprintf(msg, msg_len, "%s; %s=%s", EZCFG_HTTP_MIME_APPLICATION_SOAP_XML , EZCFG_HTTP_CHARSET_NAME , EZCFG_HTTP_CHARSET_UTF8);
    ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_TYPE , msg);

    snprintf(msg, msg_len, "%u", ezcfg_http_get_message_body_len(http));
    ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH , msg);

    /* set return value */
    rc = 0;
  }
 exit:
  if (msg != NULL)
    free(msg);

  return rc;
}

static int build_nvram_set_multi_response(struct ezcfg_soap_http *sh, struct ezcfg_nvram *nvram, struct ezcfg_link_list *list)
{
  struct ezcfg *ezcfg;
  struct ezcfg_http *http;
  struct ezcfg_soap *soap;
  char *result;
  int body_index, child_index;
  int n;
  char *msg = NULL;
  int msg_len;
  int rc = 0;
	
  ASSERT(sh != NULL);
  ASSERT(sh->http != NULL);
  ASSERT(sh->soap != NULL);
  ASSERT(nvram != NULL);

  ezcfg = sh->ezcfg;
  http = sh->http;
  soap = sh->soap;
  result = NULL;

  if (list != NULL) {
    if (ezcfg_nvram_set_multi_entries(nvram, list) == true) {
      result = EZCFG_SOAP_NVRAM_RESULT_VALUE_OK;
    }
    else {
      result = EZCFG_SOAP_NVRAM_RESULT_VALUE_ERROR;
    }
  }

  if (result != NULL) {
    int setmnv_index;

    /* clean SOAP structure info */
    ezcfg_soap_reset_attributes(soap);

    /* build SOAP */
    ezcfg_soap_set_version_major(soap, 1);
    ezcfg_soap_set_version_minor(soap, 2);

    /* SOAP Envelope */
    ezcfg_soap_set_envelope(soap, EZCFG_SOAP_ENVELOPE_ELEMENT_NAME);
    ezcfg_soap_add_envelope_attribute(soap, EZCFG_SOAP_ENVELOPE_ATTR_NS_NAME, EZCFG_SOAP_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

    /* SOAP Body */
    body_index = ezcfg_soap_set_body(soap, EZCFG_SOAP_BODY_ELEMENT_NAME);

    /* Body child setMultiNvram part */
    setmnv_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_SOAP_NVRAM_SETMNV_RESPONSE_ELEMENT_NAME, NULL);
    if (setmnv_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }
    ezcfg_soap_add_body_child_attribute(soap, setmnv_index, EZCFG_SOAP_NVRAM_ATTR_NS_NAME, EZCFG_SOAP_NVRAM_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

    /* nvram result part */
    child_index = ezcfg_soap_add_body_child(soap, setmnv_index, -1, EZCFG_SOAP_NVRAM_RESULT_ELEMENT_NAME, result);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    msg_len = ezcfg_soap_get_message_length(soap);
    if (msg_len < 0) {
      err(ezcfg, "ezcfg_soap_get_message_length\n");
      rc = -1;
      goto exit;
    }
    msg_len += strlen("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    msg_len++; /* one more for '\n' */
    msg_len++; /* one more for '\0' */
    msg = (char *)malloc(msg_len);
    if (msg == NULL) {
      err(ezcfg, "malloc error.\n");
      rc = -1;
      goto exit;
    }

    memset(msg, 0, msg_len);
    snprintf(msg, msg_len, "%s\n", "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    n = strlen(msg);
    n += ezcfg_soap_write_message(soap, msg + n, msg_len - n);

    /* FIXME: name point to http->request_uri !!!
     * never reset http before using name */
    /* clean http structure info */
    ezcfg_http_reset_attributes(http);
    ezcfg_http_set_status_code(http, 200);
    ezcfg_http_set_state_response(http);

    ezcfg_http_set_message_body(http, msg, n);

    snprintf(msg, msg_len, "%s; %s=%s", EZCFG_HTTP_MIME_APPLICATION_SOAP_XML , EZCFG_HTTP_CHARSET_NAME , EZCFG_HTTP_CHARSET_UTF8);
    ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_TYPE , msg);

    snprintf(msg, msg_len, "%u", ezcfg_http_get_message_body_len(http));
    ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH , msg);

    /* set return value */
    rc = 0;
  }
  else {
    int fault_index;
    int code_index;
    int reason_index;

    err(ezcfg, "can't set multi nvram\n");

    /* clean SOAP structure info */
    ezcfg_soap_reset_attributes(soap);

    /* build SOAP */
    ezcfg_soap_set_version_major(soap, 1);
    ezcfg_soap_set_version_minor(soap, 2);

    /* SOAP Envelope */
    ezcfg_soap_set_envelope(soap, EZCFG_SOAP_ENVELOPE_ELEMENT_NAME);
    ezcfg_soap_add_envelope_attribute(soap, EZCFG_SOAP_ENVELOPE_ATTR_NS_NAME, EZCFG_SOAP_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

    /* SOAP Body */
    body_index = ezcfg_soap_set_body(soap, EZCFG_SOAP_BODY_ELEMENT_NAME);

    /* SOAP Fault part */
    fault_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_SOAP_FAULT_ELEMENT_NAME, NULL);
    if (fault_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* SOAP Fault Code part */
    code_index = ezcfg_soap_add_body_child(soap, fault_index, -1, EZCFG_SOAP_CODE_ELEMENT_NAME, NULL);
    if (code_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* SOAP Fault Code value part */
    child_index = ezcfg_soap_add_body_child(soap, code_index, -1, EZCFG_SOAP_VALUE_ELEMENT_NAME, EZCFG_SOAP_VALUE_ELEMENT_VALUE);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* SOAP Fault Reason part */
    reason_index = ezcfg_soap_add_body_child(soap, fault_index, -1, EZCFG_SOAP_REASON_ELEMENT_NAME, NULL);
    if (reason_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* SOAP Fault Reason Text part */
    child_index = ezcfg_soap_add_body_child(soap, reason_index, -1, EZCFG_SOAP_TEXT_ELEMENT_NAME, EZCFG_SOAP_NVRAM_INVALID_VALUE_FAULT_VALUE);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    msg_len = ezcfg_soap_get_message_length(soap);
    if (msg_len < 0) {
      err(ezcfg, "ezcfg_soap_get_message_length\n");
      rc = -1;
      goto exit;
    }
    msg_len += strlen("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    msg_len++; /* one more for '\n' */
    msg_len++; /* one more for '\0' */
    msg = (char *)malloc(msg_len);
    if (msg == NULL) {
      err(ezcfg, "malloc error.\n");
      rc = -1;
      goto exit;
    }

    memset(msg, 0, msg_len);
    snprintf(msg, msg_len, "%s\n", "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    n = strlen(msg);
    n += ezcfg_soap_write_message(soap, msg + n, msg_len - n);

    /* FIXME: name point to http->request_uri !!!
     * never reset http before using name */
    /* clean http structure info */
    ezcfg_http_reset_attributes(http);
    ezcfg_http_set_status_code(http, 200);
    ezcfg_http_set_state_response(http);

    ezcfg_http_set_message_body(http, msg, n);

    snprintf(msg, msg_len, "%s; %s=%s", EZCFG_HTTP_MIME_APPLICATION_SOAP_XML , EZCFG_HTTP_CHARSET_NAME , EZCFG_HTTP_CHARSET_UTF8);
    ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_TYPE , msg);

    snprintf(msg, msg_len, "%u", ezcfg_http_get_message_body_len(http));
    ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH , msg);

    /* set return value */
    rc = 0;
  }
 exit:
  if (msg != NULL)
    free(msg);

  return rc;
}

static int build_nvram_commit_response(struct ezcfg_soap_http *sh, struct ezcfg_nvram *nvram)
{
  struct ezcfg *ezcfg;
  struct ezcfg_http *http;
  struct ezcfg_soap *soap;
  int body_index, child_index;
  int n;
  char *msg = NULL;
  int msg_len;
  int rc = 0;
	
  ASSERT(sh != NULL);
  ASSERT(sh->http != NULL);
  ASSERT(sh->soap != NULL);
  ASSERT(nvram != NULL);

  ezcfg = sh->ezcfg;
  http = sh->http;
  soap = sh->soap;

  if (ezcfg_nvram_commit(nvram) == true) {
    int commitnv_index;

    /* clean SOAP structure info */
    ezcfg_soap_reset_attributes(soap);

    /* build SOAP */
    ezcfg_soap_set_version_major(soap, 1);
    ezcfg_soap_set_version_minor(soap, 2);

    /* SOAP Envelope */
    ezcfg_soap_set_envelope(soap, EZCFG_SOAP_ENVELOPE_ELEMENT_NAME);
    ezcfg_soap_add_envelope_attribute(soap, EZCFG_SOAP_ENVELOPE_ATTR_NS_NAME, EZCFG_SOAP_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

    /* SOAP Body */
    body_index = ezcfg_soap_set_body(soap, EZCFG_SOAP_BODY_ELEMENT_NAME);

    /* Body child commitNvram part */
    commitnv_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_SOAP_NVRAM_COMMITNV_RESPONSE_ELEMENT_NAME, NULL);
    if (commitnv_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }
    ezcfg_soap_add_body_child_attribute(soap, commitnv_index, EZCFG_SOAP_NVRAM_ATTR_NS_NAME, EZCFG_SOAP_NVRAM_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

    /* nvram result part */
    child_index = ezcfg_soap_add_body_child(soap, commitnv_index, -1, EZCFG_SOAP_NVRAM_RESULT_ELEMENT_NAME, EZCFG_SOAP_NVRAM_RESULT_VALUE_OK);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    msg_len = ezcfg_soap_get_message_length(soap);
    if (msg_len < 0) {
      err(ezcfg, "ezcfg_soap_get_message_length\n");
      rc = -1;
      goto exit;
    }
    msg_len += strlen("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    msg_len++; /* one more for '\n' */
    msg_len++; /* one more for '\0' */
    msg = (char *)malloc(msg_len);
    if (msg == NULL) {
      err(ezcfg, "malloc error.\n");
      rc = -1;
      goto exit;
    }

    memset(msg, 0, msg_len);
    snprintf(msg, msg_len, "%s\n", "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    n = strlen(msg);
    n += ezcfg_soap_write_message(soap, msg + n, msg_len - n);

    /* FIXME: name point to http->request_uri !!!
     * never reset http before using name */
    /* clean http structure info */
    ezcfg_http_reset_attributes(http);
    ezcfg_http_set_status_code(http, 200);
    ezcfg_http_set_state_response(http);

    ezcfg_http_set_message_body(http, msg, n);

    snprintf(msg, msg_len, "%s; %s=%s", EZCFG_HTTP_MIME_APPLICATION_SOAP_XML , EZCFG_HTTP_CHARSET_NAME , EZCFG_HTTP_CHARSET_UTF8);
    ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_TYPE , msg);

    snprintf(msg, msg_len, "%u", ezcfg_http_get_message_body_len(http));
    ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH , msg);

    /* set return value */
    rc = 0;
  }
  else {
    int fault_index;
    int code_index;
    int reason_index;

    err(ezcfg, "nvram commit fail\n");

    /* clean SOAP structure info */
    ezcfg_soap_reset_attributes(soap);

    /* build SOAP */
    ezcfg_soap_set_version_major(soap, 1);
    ezcfg_soap_set_version_minor(soap, 2);

    /* SOAP Envelope */
    ezcfg_soap_set_envelope(soap, EZCFG_SOAP_ENVELOPE_ELEMENT_NAME);
    ezcfg_soap_add_envelope_attribute(soap, EZCFG_SOAP_ENVELOPE_ATTR_NS_NAME, EZCFG_SOAP_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

    /* SOAP Body */
    body_index = ezcfg_soap_set_body(soap, EZCFG_SOAP_BODY_ELEMENT_NAME);

    /* SOAP Fault part */
    fault_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_SOAP_FAULT_ELEMENT_NAME, NULL);
    if (fault_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* SOAP Fault Code part */
    code_index = ezcfg_soap_add_body_child(soap, fault_index, -1, EZCFG_SOAP_CODE_ELEMENT_NAME, NULL);
    if (code_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* SOAP Fault Code value part */
    child_index = ezcfg_soap_add_body_child(soap, code_index, -1, EZCFG_SOAP_VALUE_ELEMENT_NAME, EZCFG_SOAP_VALUE_ELEMENT_VALUE);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* SOAP Fault Reason part */
    reason_index = ezcfg_soap_add_body_child(soap, fault_index, -1, EZCFG_SOAP_REASON_ELEMENT_NAME, NULL);
    if (reason_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    /* SOAP Fault Reason Text part */
    child_index = ezcfg_soap_add_body_child(soap, reason_index, -1, EZCFG_SOAP_TEXT_ELEMENT_NAME, EZCFG_SOAP_NVRAM_OPERATION_FAIL_FAULT_VALUE);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child\n");
      rc = -1;
      goto exit;
    }

    msg_len = ezcfg_soap_get_message_length(soap);
    if (msg_len < 0) {
      err(ezcfg, "ezcfg_soap_get_message_length\n");
      rc = -1;
      goto exit;
    }
    msg_len += strlen("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    msg_len++; /* one more for '\n' */
    msg_len++; /* one more for '\0' */
    msg = (char *)malloc(msg_len);
    if (msg == NULL) {
      err(ezcfg, "malloc error.\n");
      rc = -1;
      goto exit;
    }

    memset(msg, 0, msg_len);
    snprintf(msg, msg_len, "%s\n", "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    n = strlen(msg);
    n += ezcfg_soap_write_message(soap, msg + n, msg_len - n);

    /* FIXME: name point to http->request_uri !!!
     * never reset http before using name */
    /* clean http structure info */
    ezcfg_http_reset_attributes(http);
    ezcfg_http_set_status_code(http, 200);
    ezcfg_http_set_state_response(http);

    ezcfg_http_set_message_body(http, msg, n);

    snprintf(msg, msg_len, "%s; %s=%s", EZCFG_HTTP_MIME_APPLICATION_SOAP_XML , EZCFG_HTTP_CHARSET_NAME , EZCFG_HTTP_CHARSET_UTF8);
    ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_TYPE , msg);

    snprintf(msg, msg_len, "%u", ezcfg_http_get_message_body_len(http));
    ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH , msg);

    /* set return value */
    rc = 0;
  }
 exit:
  if (msg != NULL)
    free(msg);

  return rc;
}

static struct ezcfg_link_list *build_nvram_node_list(struct ezcfg_soap_http *sh, char *root)
{
  struct ezcfg *ezcfg;
  //struct ezcfg_http *http;
  struct ezcfg_soap *soap;
  char *name = NULL, *value = NULL;
  int body_index, child_index;
  int root_index, nvnode_index;
  struct ezcfg_link_list *list;

  ezcfg = sh->ezcfg;
  //http = sh->http;
  soap = sh->soap;

  list = ezcfg_link_list_new(ezcfg);
  if (list == NULL) {
    return NULL;
  }

  /* get root part */
  body_index = ezcfg_soap_get_body_index(soap);
  root_index = ezcfg_soap_get_element_index(soap, body_index, -1, root);

  /* get nvram node index */
  nvnode_index = ezcfg_soap_get_element_index(soap, root_index, -1, EZCFG_SOAP_NVRAM_NVRAM_ELEMENT_NAME);
  while(nvnode_index > 0) {
    /* get nvram node name */
    child_index = ezcfg_soap_get_element_index(soap, nvnode_index, -1, EZCFG_SOAP_NVRAM_NAME_ELEMENT_NAME);
    if (child_index < 2) {
      ezcfg_link_list_delete(list);
      return NULL;
    }
    name = ezcfg_soap_get_element_content_by_index(soap, child_index);
		
    /* get nvram node value */
    child_index = ezcfg_soap_get_element_index(soap, nvnode_index, child_index, EZCFG_SOAP_NVRAM_VALUE_ELEMENT_NAME);
    if (child_index < 2) {
      ezcfg_link_list_delete(list);
      return NULL;
    }
    value = ezcfg_soap_get_element_content_by_index(soap, child_index);

    /* add nvram to list */
    if (ezcfg_link_list_insert(list, name, value) == false) {
      ezcfg_link_list_delete(list);
      return NULL;
    }

    /* get next nvram node index */
    nvnode_index = ezcfg_soap_get_element_index(soap, root_index, nvnode_index, EZCFG_SOAP_NVRAM_NVRAM_ELEMENT_NAME);
  }

  return list;
}


/**
 * Public functions
 **/

int ezcfg_soap_http_handle_nvram_request(struct ezcfg_soap_http *sh, struct ezcfg_nvram *nvram)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_http *http;
  struct ezcfg_soap *soap;
  char *request_uri;
  char *name = NULL, *value = NULL;
  //char *result = NULL;
  int body_index, child_index;
  int ret = -1;

  ASSERT(sh != NULL);
  ASSERT(sh->http != NULL);
  ASSERT(sh->soap != NULL);
  ASSERT(nvram != NULL);

  //ezcfg = sh->ezcfg;
  http = sh->http;
  soap = sh->soap;
  //result = NULL;

  request_uri = ezcfg_http_get_request_uri(http);

  if (strncmp(request_uri, EZCFG_SOAP_HTTP_NVRAM_GET_URI, strlen(EZCFG_SOAP_HTTP_NVRAM_GET_URI)) == 0) {
    /* nvram get uri=[/ezcfg/nvram/soap-http/getNvram?name=xxx] */
    name = request_uri + strlen(EZCFG_SOAP_HTTP_NVRAM_GET_URI);
    if (strncmp(name, "?name=", 6) == 0) {
      name += 6; /* skip "?name=" */
      ret = build_nvram_get_response(sh, nvram, name);
    }
  }
  else if (strcmp(request_uri, EZCFG_SOAP_HTTP_NVRAM_SET_URI) == 0) {
    /* nvram get uri=[/ezcfg/nvram/soap-http/setNvram] */
    int setnv_index;

    /* get setNvram part */
    body_index = ezcfg_soap_get_body_index(soap);
    setnv_index = ezcfg_soap_get_element_index(soap, body_index, -1, EZCFG_SOAP_NVRAM_SETNV_ELEMENT_NAME);

    /* get nvram node name */
    child_index = ezcfg_soap_get_element_index(soap, setnv_index, -1, EZCFG_SOAP_NVRAM_NAME_ELEMENT_NAME);
    if (child_index < 2) {
      goto set_out;
    }
    name = ezcfg_soap_get_element_content_by_index(soap, child_index);
		
    /* get nvram node value */
    child_index = ezcfg_soap_get_element_index(soap, setnv_index, child_index, EZCFG_SOAP_NVRAM_VALUE_ELEMENT_NAME);
    if (child_index < 2) {
      goto set_out;
    }
    value = ezcfg_soap_get_element_content_by_index(soap, child_index);
  set_out:
    ret = build_nvram_set_response(sh, nvram, name, value);
  }
  else if (strncmp(request_uri, EZCFG_SOAP_HTTP_NVRAM_UNSET_URI, strlen(EZCFG_SOAP_HTTP_NVRAM_UNSET_URI)) == 0) {
    /* nvram get uri=[/ezcfg/nvram/soap-http/unsetNvram?name=xxx] */
    name = request_uri + strlen(EZCFG_SOAP_HTTP_NVRAM_UNSET_URI) + 6;
    ret = build_nvram_unset_response(sh, nvram, name);
  }
  else if (strcmp(request_uri, EZCFG_SOAP_HTTP_NVRAM_SET_MULTI_URI) == 0) {
    /* nvram get uri=[/ezcfg/nvram/soap-http/setMultiNvram] */
    struct ezcfg_link_list *list;
    list = build_nvram_node_list(sh, EZCFG_SOAP_NVRAM_SETMNV_ELEMENT_NAME);
    ret = build_nvram_set_multi_response(sh, nvram, list);
    if (list != NULL) {
      ezcfg_link_list_delete(list);
    }
  }
  else if (strcmp(request_uri, EZCFG_SOAP_HTTP_NVRAM_COMMIT_URI) == 0) {
    /* nvram get uri=[/ezcfg/nvram/soap-http/commitNvram] */
    ret = build_nvram_commit_response(sh, nvram);
  }
  return ret;
}
