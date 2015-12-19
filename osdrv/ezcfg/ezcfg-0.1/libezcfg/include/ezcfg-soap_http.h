/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-soap_http.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-29   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_SOAP_HTTP_H_
#define _EZCFG_SOAP_HTTP_H_

#include "ezcfg.h"
#include "ezcfg-http.h"

struct ezcfg_soap_http {
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;
};

/* soap/soap_http_nvram.c */
int ezcfg_soap_http_handle_nvram_request(struct ezcfg_soap_http *sh, struct ezcfg_nvram *nvram);

/* ezcfg SOAP/HTTP http methods */
#define EZCFG_SOAP_HTTP_METHOD_GET          "GET"
#define EZCFG_SOAP_HTTP_METHOD_POST         "POST"
/* ezcfg SOAP/HTTP http headers */
#define EZCFG_SOAP_HTTP_HEADER_HOST                 "Host"
#define EZCFG_SOAP_HTTP_HEADER_CONTENT_TYPE         "Content-Type"
#define EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH       "Content-Length"
#define EZCFG_SOAP_HTTP_HEADER_ACCEPT               "Accept"
/* ezcfg SOAP/HTTP nvram handler */
#define EZCFG_SOAP_HTTP_NVRAM_GET_URI               "/ezcfg/nvram/soap-http/getNvram"
#define EZCFG_SOAP_HTTP_NVRAM_SET_URI               "/ezcfg/nvram/soap-http/setNvram"
#define EZCFG_SOAP_HTTP_NVRAM_UNSET_URI             "/ezcfg/nvram/soap-http/unsetNvram"
#define EZCFG_SOAP_HTTP_NVRAM_SET_MULTI_URI         "/ezcfg/nvram/soap-http/setMultiNvram"
#define EZCFG_SOAP_HTTP_NVRAM_LIST_URI              "/ezcfg/nvram/soap-http/listNvram"
#define EZCFG_SOAP_HTTP_NVRAM_INFO_URI              "/ezcfg/nvram/soap-http/infoNvram"
#define EZCFG_SOAP_HTTP_NVRAM_COMMIT_URI            "/ezcfg/nvram/soap-http/commitNvram"
#define EZCFG_SOAP_HTTP_NVRAM_INSERT_SOCKET_URI     "/ezcfg/nvram/soap-http/insertSocket"
#define EZCFG_SOAP_HTTP_NVRAM_REMOVE_SOCKET_URI     "/ezcfg/nvram/soap-http/removeSocket"
#define EZCFG_SOAP_HTTP_NVRAM_INSERT_SSL_URI        "/ezcfg/nvram/soap-http/insertSsl"
#define EZCFG_SOAP_HTTP_NVRAM_REMOVE_SSL_URI        "/ezcfg/nvram/soap-http/removeSsl"

/* ezcfg SOAP/HTTP socket handler */

#endif
