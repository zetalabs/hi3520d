/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-ssl.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-01-31   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_SSL_H_
#define _EZCFG_SSL_H_

/*----------------------------*/
/* ezcfg nvram name prefix */
#define EZCFG_SSL_NVRAM_PREFIX            "ssl."

/* ezcfg SSL role string */
#define EZCFG_SSL_ROLE_UNKNOWN          0
#define EZCFG_SSL_ROLE_SERVER           1
#define EZCFG_SSL_ROLE_CLIENT           2

#define EZCFG_SSL_ROLE_SERVER_STRING    "server"
#define EZCFG_SSL_ROLE_CLIENT_STRING    "client"

/* ezcfg SSL method string */
#define EZCFG_SSL_METHOD_UNKNOWN        0
#define EZCFG_SSL_METHOD_SSLV2          1
#define EZCFG_SSL_METHOD_SSLV3          2
#define EZCFG_SSL_METHOD_TLSV1          3
#define EZCFG_SSL_METHOD_SSLV23         4

#define EZCFG_SSL_METHOD_SSLV2_STRING   "SSLv2"
#define EZCFG_SSL_METHOD_SSLV3_STRING   "SSLv3"
#define EZCFG_SSL_METHOD_TLSV1_STRING   "TLSv1"
#define EZCFG_SSL_METHOD_SSLV23_STRING  "SSLv23"

#endif /* _EZCFG_SSL_H_ */
