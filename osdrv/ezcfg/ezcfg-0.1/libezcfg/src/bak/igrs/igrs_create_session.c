/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : igrs/igrs_create_session.c
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

bool ezcfg_igrs_build_create_session_request(struct ezcfg_igrs *igrs)
{
  struct ezcfg *ezcfg;
  struct ezcfg_http *http;
  struct ezcfg_soap *soap;
  char buf[1024];
  char *msg;
  size_t msg_len;
  int n;
  int body_index, session_index, userinfo_index, child_index;

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

  child_index = -1;
  /* SourceCleintId part */
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

  /* UserInfo part */
  userinfo_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_SOAP_USER_INFO_ELEMENT_NAME, NULL);
  if (userinfo_index < 0) {
    err(ezcfg, "ezcfg_soap_add_body_child(%d)\n", __LINE__);
    return false;
  }

  snprintf(buf, sizeof(buf), "%s", igrs->source_user_id ? igrs->source_user_id : "NULL");
  child_index = ezcfg_soap_add_body_child(soap, userinfo_index, -1, EZCFG_IGRS_SOAP_SOURCE_USER_ID_ELEMENT_NAME, buf);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_soap_add_body_child(%d)\n", __LINE__);
    return false;
  }

  snprintf(buf, sizeof(buf), "urn:IGRS:ServiceSecurity:%s", igrs->service_security_id ? igrs->service_security_id : "NULL");
  child_index = ezcfg_soap_add_body_child(soap, userinfo_index, child_index, EZCFG_IGRS_SOAP_SERVICE_SECURITY_ID_ELEMENT_NAME, buf);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_soap_add_body_child(%d)\n", __LINE__);
    return false;
  }

  child_index = ezcfg_soap_add_body_child(soap, userinfo_index, child_index, EZCFG_IGRS_SOAP_TOKEN_ELEMENT_NAME, "");
  if (child_index < 0) {
    err(ezcfg, "ezcfg_soap_add_body_child(%d)\n", __LINE__);
    return false;
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

  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_01_IGRS_MESSAGE_TYPE , EZCFG_IGRS_MSG_CREATE_SESSION_REQUEST);

  snprintf(buf, sizeof(buf), "urn:IGRS:Device:DeviceId:%s", igrs->target_device_id);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_01_TARGET_DEVICE_ID , buf);

  snprintf(buf, sizeof(buf), "urn:IGRS:Device:DeviceId:%s", igrs->source_device_id);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_01_SOURCE_DEVICE_ID , buf);

  snprintf(buf, sizeof(buf), "%u", igrs->sequence_id);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_01_SEQUENCE_ID , buf);

  snprintf(buf, sizeof(buf), "%s; %s=%s", EZCFG_HTTP_MIME_TEXT_XML, EZCFG_HTTP_CHARSET_NAME, EZCFG_HTTP_CHARSET_UTF8);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_CONTENT_TYPE , buf);

  snprintf(buf, sizeof(buf), "%u", ezcfg_http_get_message_body_len(http));
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_CONTENT_LENGTH , buf);

  snprintf(buf, sizeof(buf), "\"%s\";ns=02", EZCFG_IGRS_SOAP_ENVELOPE_ATTR_NS_VALUE);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_MAN , buf);

  snprintf(buf, sizeof(buf), "\"%s\"", EZCFG_IGRS_SOAP_ACTION_CREATE_SESSION_REQUEST);
  ezcfg_http_add_header(http, EZCFG_IGRS_HTTP_HEADER_02_SOAP_ACTION , buf);

  return true;
}

int ezcfg_igrs_write_create_session_request(struct ezcfg_igrs *igrs, char *buf, int len)
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

int ezcfg_igrs_handle_create_session_request(struct ezcfg_igrs *igrs)
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

  return 0;
}
