/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : igrs/igrs_invoke_service.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-26   0.1       Write it from scratch
 * 2010-11-29   0.1.1     Split it from igrs.c
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-igrs.h"

bool ezcfg_igrs_build_invoke_service_request(struct ezcfg_igrs *igrs)
{
  struct ezcfg *ezcfg;
  struct ezcfg_http *http;
  struct ezcfg_soap *soap;
  char buf[1024];
  char *msg;
  size_t msg_len;
  int n;
  int body_index, session_index, child_index;

  ASSERT(igrs != NULL);
  ASSERT(igrs->http != NULL);
  ASSERT(igrs->soap != NULL);

  ezcfg = igrs->ezcfg;
  http = igrs->http;
  soap = igrs->soap;

  /* clean SOAP structure info */
  ezcfg_soap_reset_attributes(soap);

  /* build SOAP */
  ezcfg_soap_set_version_major(soap, 1);
  ezcfg_soap_set_version_minor(soap, 2);

  /* SOAP Envelope */
  ezcfg_soap_set_envelope(soap, EZCFG_IGRS_SOAP_ENVELOPE_ELEMENT_NAME);
  ezcfg_soap_add_envelope_attribute(soap, EZCFG_IGRS_SOAP_ENVELOPE_ATTR_NS_NAME, EZCFG_IGRS_SOAP_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
  ezcfg_soap_add_envelope_attribute(soap, EZCFG_IGRS_SOAP_ENVELOPE_ATTR_ENC_NAME, EZCFG_IGRS_SOAP_ENVELOPE_ATTR_ENC_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

  /* SOAP Body */
  body_index = ezcfg_soap_set_body(soap, EZCFG_IGRS_SOAP_BODY_ELEMENT_NAME);

  /* Body child Session part */
  session_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_IGRS_SOAP_SESSION_ELEMENT_NAME, NULL);
  if (session_index < 0) {
    err(ezcfg, "ezcfg_soap_add_body_child(%d)\n", __LINE__);
    return false;
  }
  ezcfg_soap_add_body_child_attribute(soap, session_index, EZCFG_IGRS_SOAP_SESSION_ATTR_NS_NAME, EZCFG_IGRS_SOAP_SESSION_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

  /* SourceCleintId part */
  child_index = -1;
  snprintf(buf, sizeof(buf), "%u", igrs->source_client_id);
  child_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_SOAP_SOURCE_CLIENT_ID_ELEMENT_NAME, buf);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_soap_add_body_child(%d)\n", __LINE__);
    return false;
  }

  /* TargetServiceId part */
  snprintf(buf, sizeof(buf), "%u", igrs->target_service_id);
  child_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_SOAP_TARGET_SERVICE_ID_ELEMENT_NAME, buf);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_soap_add_body_child(%d)\n", __LINE__);
    return false;
  }

  /* SequenceId part */
  snprintf(buf, sizeof(buf), "%u", igrs->sequence_id);
  child_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_SOAP_SEQUENCE_ID_ELEMENT_NAME, buf);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_soap_add_body_child(%d)\n", __LINE__);
    return false;
  }

  /* The specific invocation acknowledgement message */
  if (igrs->invoke_args != NULL) {
    child_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_SOAP_INVOKE_ARGS_ELEMENT_NAME, igrs->invoke_args);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child(%d)\n", __LINE__);
      return false;
    }
  }

  n = ezcfg_soap_get_message_length(soap);
  if (n < 0) {
    err(ezcfg, "ezcfg_soap_get_message_length\n");
    return false;
  }
  msg_len = n+1; /* one more for \0 */
  if (msg_len <= sizeof(buf)) {
    msg = buf;
  }
  else {
    msg = (char *)malloc(msg_len);
    if (msg == NULL) {
      err(ezcfg, "msg malloc\n");
      return false;
    }
  }

  memset(msg, '\0', msg_len);
  n = ezcfg_soap_write_message(soap, msg, msg_len);
  if (n < 0) {
    err(ezcfg, "ezcfg_soap_write_message\n");
    if (msg != buf) { free(msg); }
    return false;
  }

  /* FIXME: name point to http->request_uri !!!
   * never reset http before using name */
  /* clean http structure info */
  ezcfg_http_reset_attributes(http);

  ezcfg_http_set_message_body(http, msg, n);
  if (msg != buf) { free(msg); }

  /* build HTTP request line */
  ezcfg_http_set_request_method(http, EZCFG_IGRS_HTTP_METHOD_POST_EXT);
  ezcfg_http_set_request_uri(http, "/IGRS");
  ezcfg_http_set_version_major(http, 1);
  ezcfg_http_set_version_minor(http, 1);

  /* build HTTP headers */
  snprintf(buf, sizeof(buf), "%s", "192.168.1.1:3880");
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_HOST , buf);

  snprintf(buf, sizeof(buf), "\"%s\";ns=01", EZCFG_IGRS_SOAP_SESSION_ATTR_NS_VALUE);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_MAN , buf);

  snprintf(buf, sizeof(buf), "IGRS/%d.%d", igrs->version_major, igrs->version_minor);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_01_IGRS_VERSION , buf);

  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_01_IGRS_MESSAGE_TYPE , EZCFG_IGRS_MSG_INVOKE_SERVICE_REQUEST);

  snprintf(buf, sizeof(buf), "urn:IGRS:Device:DeviceId:%s", igrs->source_device_id);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_01_SOURCE_DEVICE_ID , buf);

  snprintf(buf, sizeof(buf), "urn:IGRS:Device:DeviceId:%s", igrs->target_device_id);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_01_TARGET_DEVICE_ID , buf);

  snprintf(buf, sizeof(buf), "%u", igrs->sequence_id);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_01_SEQUENCE_ID , buf);

  snprintf(buf, sizeof(buf), "%s; %s=%s", EZCFG_HTTP_MIME_TEXT_XML, EZCFG_HTTP_CHARSET_NAME, EZCFG_HTTP_CHARSET_UTF8);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_CONTENT_TYPE , buf);

  snprintf(buf, sizeof(buf), "%u", ezcfg_http_get_message_body_len(http));
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_CONTENT_LENGTH , buf);

  snprintf(buf, sizeof(buf), "\"%s\";ns=02", EZCFG_IGRS_SOAP_ENVELOPE_ATTR_NS_VALUE);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_MAN , buf);

  snprintf(buf, sizeof(buf), "\"%s\"", EZCFG_IGRS_SOAP_ACTION_INVOKE_SERVICE_REQUEST);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_02_SOAP_ACTION , buf);

  return true;
}

int ezcfg_igrs_write_invoke_service_request(struct ezcfg_igrs *igrs, char *buf, int len)
{
  struct ezcfg *ezcfg;
  struct ezcfg_http *http;
  //struct ezcfg_soap *soap;

  char *p;
  int n;

  ASSERT(igrs != NULL);
  ASSERT(igrs->http != NULL);
  ASSERT(igrs->soap != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  ezcfg = igrs->ezcfg;
  http = igrs->http;
  //soap = igrs->soap;

  p = buf;
  n = ezcfg_http_write_request_line(http, p, len);
  if (n < 0) {
    err(ezcfg, "ezcfg_http_write_request_line\n");
    return n;
  }
  p += n;
  len -= n;

  n = ezcfg_http_write_headers(http, p, len);
  if (n < 0) {
    err(ezcfg, "ezcfg_http_write_headers\n");
    return n;
  }
  p += n;
  len -= n;

  n = ezcfg_http_write_crlf(http, p, len);
  if (n < 0) {
    err(ezcfg, "ezcfg_http_write_crlf\n");
    return n;
  }
  p += n;
  len -= n;

  if (ezcfg_http_get_message_body(http) != NULL) {
    n = ezcfg_http_write_message_body(http, p, len);
    if (n < 0) {
      err(ezcfg, "ezcfg_http_write_message_body\n");
      return n;
    }
    p += n;
  }

  return (p-buf);
}

int ezcfg_igrs_handle_invoke_service_request(struct ezcfg_igrs *igrs)
{
  struct ezcfg *ezcfg;
  //struct ezcfg_http *http;
  struct ezcfg_soap *soap;
  char tmp_device_id[EZCFG_UUID_STRING_LEN+1]; /* +1 for \0-terminated */
  int body_index, session_index, child_index;
  char *p;

  ASSERT(igrs != NULL);
  ASSERT(igrs->http != NULL);
  ASSERT(igrs->soap != NULL);

  ezcfg = igrs->ezcfg;
  //http = igrs->http;
  soap = igrs->soap;

  /* handle request */
  /* SOAP Body */
  body_index = ezcfg_soap_get_body_index(soap);
  if (body_index < 1) {
    err(ezcfg, "ezcfg_soap_get_body_index\n");
    return body_index;
  }

  /* Body child Session part */
  session_index = ezcfg_soap_get_element_index(soap, body_index, -1, EZCFG_IGRS_SOAP_SESSION_ELEMENT_NAME);
  if (session_index < 0) {
    err(ezcfg, "ezcfg_soap_get_element_index(%s)\n", EZCFG_IGRS_SOAP_SESSION_ELEMENT_NAME);
    return session_index;
  }

  /* SourceCleintId part */
  child_index = ezcfg_soap_get_element_index(soap, session_index, -1, EZCFG_IGRS_SOAP_SOURCE_CLIENT_ID_ELEMENT_NAME);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_soap_get_element_index(%s)\n", EZCFG_IGRS_SOAP_SOURCE_CLIENT_ID_ELEMENT_NAME);
    return child_index;
  }
  p = ezcfg_soap_get_element_content_by_index(soap, child_index);
  if (p == NULL) {
    err(ezcfg, " ezcfg_soap_get_element_content_by_index(%s)\n", EZCFG_IGRS_SOAP_SOURCE_CLIENT_ID_ELEMENT_NAME);
    return -1;
  }
  ezcfg_igrs_set_source_client_id(igrs, strtoul(p, NULL, 10));

  /* TargetServiceId part */
  child_index = ezcfg_soap_get_element_index(soap, session_index, -1, EZCFG_IGRS_SOAP_TARGET_SERVICE_ID_ELEMENT_NAME);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_soap_get_element_index(%s)\n", EZCFG_IGRS_SOAP_TARGET_SERVICE_ID_ELEMENT_NAME);
    return child_index;
  }
  p = ezcfg_soap_get_element_content_by_index(soap, child_index);
  if (p == NULL) {
    err(ezcfg, " ezcfg_soap_get_element_content_by_index(%s)\n", EZCFG_IGRS_SOAP_TARGET_SERVICE_ID_ELEMENT_NAME);
    return -1;
  }
  ezcfg_igrs_set_target_service_id(igrs, strtoul(p, NULL, 10));

  /* SequenceId part */
  child_index = ezcfg_soap_get_element_index(soap, session_index, -1, EZCFG_IGRS_SOAP_SEQUENCE_ID_ELEMENT_NAME);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_soap_get_element_index(%s)\n", EZCFG_IGRS_SOAP_SEQUENCE_ID_ELEMENT_NAME);
    return child_index;
  }
  p = ezcfg_soap_get_element_content_by_index(soap, child_index);
  if (p == NULL) {
    err(ezcfg, " ezcfg_soap_get_element_content_by_index(%s)\n", EZCFG_IGRS_SOAP_SEQUENCE_ID_ELEMENT_NAME);
    return -1;
  }
  ezcfg_igrs_set_sequence_id(igrs, strtoul(p, NULL, 10));

  /* The specific invocation acknowledgement message */
  child_index = ezcfg_soap_get_element_index(soap, session_index, -1, EZCFG_IGRS_SOAP_INVOKE_ARGS_ELEMENT_NAME);
  if (child_index > 0) {
    p = ezcfg_soap_get_element_content_by_index(soap, child_index);
    ezcfg_igrs_set_invoke_args(igrs, p);
  }

  /* build response SOAP info */
  ezcfg_igrs_set_source_service_id(igrs, ezcfg_igrs_get_target_service_id(igrs));
  ezcfg_igrs_set_target_client_id(igrs, ezcfg_igrs_get_source_client_id(igrs));
  ezcfg_igrs_set_acknowledge_id(igrs, ezcfg_igrs_get_sequence_id(igrs));
  ezcfg_igrs_set_return_code(igrs, 200);

  /* exchange source/target device name */
  strcpy(tmp_device_id, ezcfg_igrs_get_source_device_id(igrs));
  ezcfg_igrs_set_source_device_id(igrs, ezcfg_igrs_get_target_device_id(igrs));
  ezcfg_igrs_set_target_device_id(igrs, tmp_device_id);

  /* set IGRS message type */
  ezcfg_igrs_set_message_type(igrs, EZCFG_IGRS_MSG_INVOKE_SERVICE_RESPONSE);
  return 0;
}

bool ezcfg_igrs_build_invoke_service_response(struct ezcfg_igrs *igrs)
{
  struct ezcfg *ezcfg;
  struct ezcfg_http *http;
  struct ezcfg_soap *soap;
  char buf[1024];
  char *msg;
  size_t msg_len;
  int n;
  int body_index, session_index, child_index;

  ASSERT(igrs != NULL);
  ASSERT(igrs->http != NULL);
  ASSERT(igrs->soap != NULL);

  ezcfg = igrs->ezcfg;
  http = igrs->http;
  soap = igrs->soap;

  /* clean SOAP structure info */
  ezcfg_soap_reset_attributes(soap);

  /* build SOAP */
  ezcfg_soap_set_version_major(soap, 1);
  ezcfg_soap_set_version_minor(soap, 2);

  /* SOAP Envelope */
  ezcfg_soap_set_envelope(soap, EZCFG_IGRS_SOAP_ENVELOPE_ELEMENT_NAME);
  ezcfg_soap_add_envelope_attribute(soap, EZCFG_IGRS_SOAP_ENVELOPE_ATTR_NS_NAME, EZCFG_IGRS_SOAP_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
  ezcfg_soap_add_envelope_attribute(soap, EZCFG_IGRS_SOAP_ENVELOPE_ATTR_ENC_NAME, EZCFG_IGRS_SOAP_ENVELOPE_ATTR_ENC_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

  /* SOAP Body */
  body_index = ezcfg_soap_set_body(soap, EZCFG_IGRS_SOAP_BODY_ELEMENT_NAME);

  /* Body child Session part */
  session_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_IGRS_SOAP_SESSION_ELEMENT_NAME, NULL);
  if (session_index < 0) {
    err(ezcfg, "ezcfg_soap_add_body_child(%d)\n", __LINE__);
    return false;
  }
  ezcfg_soap_add_body_child_attribute(soap, session_index, EZCFG_IGRS_SOAP_SESSION_ATTR_NS_NAME, EZCFG_IGRS_SOAP_SESSION_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

  /* SourceServiceId part */
  child_index = -1;
  snprintf(buf, sizeof(buf), "%u", igrs->source_service_id);
  child_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_SOAP_SOURCE_SERVICE_ID_ELEMENT_NAME, buf);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_soap_add_body_child(%d)\n", __LINE__);
    return false;
  }

  /* TargetClientId part */
  snprintf(buf, sizeof(buf), "%u", igrs->target_client_id);
  child_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_SOAP_TARGET_CLIENT_ID_ELEMENT_NAME, buf);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_soap_add_body_child(%d)\n", __LINE__);
    return false;
  }

  /* AcknowledgeId part */
  snprintf(buf, sizeof(buf), "%u", igrs->acknowledge_id);
  child_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_SOAP_ACKNOWLEDGE_ID_ELEMENT_NAME, buf);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_soap_add_body_child(%d)\n", __LINE__);
    return false;
  }

  /* ReturnCode part */
  snprintf(buf, sizeof(buf), "%u", igrs->return_code);
  child_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_SOAP_RETURN_CODE_ELEMENT_NAME, buf);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_soap_add_body_child(%d)\n", __LINE__);
    return false;
  }

  /* The specific invocation acknowledgement message */
  if (igrs->invoke_args != NULL) {
    child_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_SOAP_INVOKE_ARGS_ELEMENT_NAME, igrs->invoke_args);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_soap_add_body_child(%d)\n", __LINE__);
      return false;
    }
  }

  n = ezcfg_soap_get_message_length(soap);
  if (n < 0) {
    err(ezcfg, "ezcfg_soap_get_message_length\n");
    return false;
  }
  msg_len = n+1; /* one more for \0 */
  if (msg_len <= sizeof(buf)) {
    msg = buf;
  }
  else {
    msg = (char *)malloc(msg_len);
    if (msg == NULL) {
      err(ezcfg, "msg malloc\n");
      return false;
    }
  }

  memset(msg, '\0', msg_len);
  n = ezcfg_soap_write_message(soap, msg, msg_len);
  if (n < 0) {
    err(ezcfg, "ezcfg_soap_write_message\n");
    if (msg != buf) { free(msg); }
    return false;
  }

  /* FIXME: name point to http->request_uri !!!
   * never reset http before using name */
  /* clean http structure info */
  ezcfg_http_reset_attributes(http);

  ezcfg_http_set_message_body(http, msg, n);
  if (msg != buf) { free(msg); }

  /* build HTTP status line */
  ezcfg_http_set_state_response(http);
  ezcfg_http_set_version_major(http, 1);
  ezcfg_http_set_version_minor(http, 1);
  ezcfg_http_set_status_code(http, 200);

  /* build HTTP headers */
  snprintf(buf, sizeof(buf), "%s", "");
  ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_EXT , buf);

  snprintf(buf, sizeof(buf), "no-cache=\"%s\"", EZCFG_HTTP_HEADER_EXT);
  ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CACHE_CONTROL , buf);

  snprintf(buf, sizeof(buf), "\"%s\";ns=01", EZCFG_IGRS_SOAP_SESSION_ATTR_NS_VALUE);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_MAN , buf);

  snprintf(buf, sizeof(buf), "IGRS/%d.%d", igrs->version_major, igrs->version_minor);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_01_IGRS_VERSION , buf);

  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_01_IGRS_MESSAGE_TYPE , EZCFG_IGRS_MSG_INVOKE_SERVICE_RESPONSE);

  snprintf(buf, sizeof(buf), "urn:IGRS:Device:DeviceId:%s", igrs->target_device_id);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_01_TARGET_DEVICE_ID , buf);

  snprintf(buf, sizeof(buf), "urn:IGRS:Device:DeviceId:%s", igrs->source_device_id);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_01_SOURCE_DEVICE_ID , buf);

  snprintf(buf, sizeof(buf), "%s; %s=%s", EZCFG_HTTP_MIME_TEXT_XML, EZCFG_HTTP_CHARSET_NAME, EZCFG_HTTP_CHARSET_UTF8);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_CONTENT_TYPE , buf);

  snprintf(buf, sizeof(buf), "%u", ezcfg_http_get_message_body_len(http));
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_CONTENT_LENGTH , buf);

  snprintf(buf, sizeof(buf), "\"%s\";ns=02", EZCFG_IGRS_SOAP_ENVELOPE_ATTR_NS_VALUE);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_MAN , buf);

  snprintf(buf, sizeof(buf), "\"%s\"", EZCFG_IGRS_SOAP_ACTION_INVOKE_SERVICE_RESPONSE);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_02_SOAP_ACTION , buf);

  return true;
}

int ezcfg_igrs_write_invoke_service_response(struct ezcfg_igrs *igrs, char *buf, int len)
{
  struct ezcfg *ezcfg;
  struct ezcfg_http *http;
  //struct ezcfg_soap *soap;

  char *p;
  int n;

  ASSERT(igrs != NULL);
  ASSERT(igrs->http != NULL);
  ASSERT(igrs->soap != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  ezcfg = igrs->ezcfg;
  http = igrs->http;
  //soap = igrs->soap;

  p = buf;
  n = ezcfg_http_write_start_line(http, p, len);
  if (n < 0) {
    err(ezcfg, "ezcfg_http_write_start_line\n");
    return n;
  }
  p += n;
  len -= n;

  n = ezcfg_http_write_headers(http, p, len);
  if (n < 0) {
    err(ezcfg, "ezcfg_http_write_headers\n");
    return n;
  }
  p += n;
  len -= n;

  n = ezcfg_http_write_crlf(http, p, len);
  if (n < 0) {
    err(ezcfg, "ezcfg_http_write_crlf\n");
    return n;
  }
  p += n;
  len -= n;

  if (ezcfg_http_get_message_body(http) != NULL) {
    n = ezcfg_http_write_message_body(http, p, len);
    if (n < 0) {
      err(ezcfg, "ezcfg_http_write_message_body\n");
      return n;
    }
    p += n;
  }

  return (p-buf);
}

int ezcfg_igrs_handle_invoke_service_response(struct ezcfg_igrs *igrs)
{
  //struct ezcfg *ezcfg;
  //struct ezcfg_http *http;
  //struct ezcfg_soap *soap;

  ASSERT(igrs != NULL);
  ASSERT(igrs->http != NULL);
  ASSERT(igrs->soap != NULL);

  //ezcfg = igrs->ezcfg;
  //http = igrs->http;
  //soap = igrs->soap;

  /* handle response */

  return 0;
}
