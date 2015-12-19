/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2014-03-31   0.2       Prepare for new NVRAM model
 * ============================================================================
 */

#ifndef _EZCFG_H_
#define _EZCFG_H_

#include <syslog.h>
#include <sys/stat.h>

#include "ezcfg-ezbox_distro.h"
#include "ezcfg-nvram_defaults.h"
#include "ezcfg-errno.h"

#ifdef EZCFG_DEBUG
#include <assert.h>
#define ASSERT(exp) assert(exp)   
#define EZDBG(format, args...) \
  do { \
    FILE *ezdbg_fp = fopen("/tmp/ezdbg.log", "a");  \
    if (ezdbg_fp) {                                 \
      fprintf(ezdbg_fp, format, ## args);           \
      fclose(ezdbg_fp);                             \
    }                                               \
  } while(0)
#else
#define ASSERT(exp) do {} while(0) 
#define EZDBG(format, args...)
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define EZBOX_PROJECT_HOME_PAGE_URI	"https://github.com/zetalabs/ezbox/"

#define EZCFG_INVALID_SOCKET      -1
#define EZCFG_BUFFER_SIZE         8192
#define EZCFG_NAME_MAX            128
#define EZCFG_VALUE_MAX           128
#define EZCFG_PATH_MAX            128
#define EZCFG_LOCALE_MAX          32

/* ezcfg manipulate limits */
#define EZCFG_LOCK_RETRY_MAX            3
#define EZCFG_UNLOCK_RETRY_MAX          3
#define EZCFG_REF_COUNTER_RETRY_MAX     3

#define EZCFG_CONFIG_FILE_PATH	SYSCONFDIR "/ezcfg.conf"
#define EZCFG_NVRAM_CONFIG_FILE_PATH	SYSCONFDIR "/nvram.conf"
#define EZCFG_UPNP_TASK_FILE_PATH	SYSCONFDIR "/upnp_tasks"

/* function return value */
#define EZCFG_RET_OK            0
#define EZCFG_RET_FAIL         -1

/* minimum number of worker threads */
/* nvram socket */
#define EZCFG_THREAD_MIN_NUM	3

#define EZCFG_ROOT_PATH		SYSCONFDIR "/ezcfg"
#define EZCFG_VAR_PATH		"/var/ezcfg"
/* semaphore path */
#define EZCFG_SEM_ROOT_PATH	EZCFG_ROOT_PATH "/sem"
#define EZCFG_SEM_EZCFG_PATH	EZCFG_SEM_ROOT_PATH "/ezcfg"
#define EZCFG_SEM_PROJID_EZCFG	'e'

#define EZCFG_SEM_CTRL_INDEX            0
#define EZCFG_SEM_NVRAM_INDEX           1
#define EZCFG_SEM_RC_INDEX              2
#define EZCFG_SEM_UPNP_INDEX            3
#define EZCFG_SEM_EZCTP_INDEX           4
#define EZCFG_SEM_SHM_INDEX             5
#define EZCFG_SEM_SHM_QUEUE_INDEX       6
#define EZCFG_SEM_NUMBER                7

/* shared memory path */
#define EZCFG_SHM_ROOT_PATH	EZCFG_ROOT_PATH "/shm"
#define EZCFG_SHM_EZCFG_PATH	EZCFG_SHM_ROOT_PATH "/ezcfg"
#define EZCFG_SHM_EZCTP_PATH	EZCFG_SHM_ROOT_PATH "/ezctp"
#define EZCFG_SHM_PROJID_EZCFG	'f'
#define EZCFG_SHM_PROJID_EZCTP	't'
/* shared memory size */
#define EZCFG_SHM_SIZE_MAX	1073741824 /* 1024*1024*1024 */
/* shared memory nvram queue minimum length */
#define EZCFG_SHM_EZCFG_NVRAM_QUEUE_MIN        64
/* shared memory nvram queue maximum length */
#define EZCFG_SHM_EZCFG_NVRAM_QUEUE_MAX        1024
/* shared memory rc queue minimum length */
#define EZCFG_SHM_EZCFG_RC_QUEUE_MIN           64
/* shared memory rc queue maximum length */
#define EZCFG_SHM_EZCFG_RC_QUEUE_MAX           1024

/* socket path */
#define EZCFG_SOCK_ROOT_PATH    "@/etc/ezcfg/sock"
#define EZCFG_SOCK_CTRL_PATH    EZCFG_SOCK_ROOT_PATH "/ctrl"
#define EZCFG_SOCK_NVRAM_PATH   EZCFG_SOCK_ROOT_PATH "/nvram"
#define EZCFG_SOCK_UEVENT_PATH  EZCFG_SOCK_ROOT_PATH "/uevent"
#define EZCFG_SOCK_MASTER_PATH  EZCFG_SOCK_ROOT_PATH "/master"

/* Web GUI path */
//#define EZCFG_WEB_DOCUMENT_ROOT_PATH    EZCFG_VAR_PATH "/www"

/* file tail string */
#define EZCFG_FILE_LINE_TAIL_STRING     " \t\r\n"

/* file extension */
#define EZCFG_FILE_EXT_UNKNOWN          0
#define EZCFG_FILE_EXT_SHTM             1
#define EZCFG_FILE_EXT_SHTM_STRING      ".shtm"
#define EZCFG_FILE_EXT_JS               2
#define EZCFG_FILE_EXT_JS_STRING        ".js"
#define EZCFG_FILE_EXT_NVJS             3
#define EZCFG_FILE_EXT_NVJS_STRING      ".nvjs"


/* SSL */
#define EZCFG_SSL_CONF_ROOT_PATH	SYSCONFDIR "/ssl"
#define EZCFG_SSL_CERT_ROOT_PATH	EZCFG_SSL_CONF_ROOT_PATH "/certs"
#define EZCFG_SSL_PRIV_ROOT_PATH	EZCFG_SSL_CONF_ROOT_PATH "/private"


/* thread config definitions */
#define EZCFG_MASTER_SOCKET_QUEUE_LENGTH	20
#define EZCFG_MASTER_WAIT_TIME	5
#define EZCFG_WORKER_WAIT_TIME	1

/* ezcfg common definitions */
#define EZCFG_COMMON_LOG_LEVEL_ERR_STRING     "err"
#define EZCFG_COMMON_LOG_LEVEL_INFO_STRING    "info"
#define EZCFG_COMMON_LOG_LEVEL_DEBUG_STRING   "debug"
#define EZCFG_COMMON_DEFAULT_RULES_PATH       SYSCONFDIR "/ezcfg.rules"
#define EZCFG_COMMON_DEFAULT_LOCALE_STRING    "zh_CN.UTF-8"


/* ezcfg file trailing charlist */
#define EZCFG_EZCFG_TRAILING_CHARLIST       "\r\n\t "


/* ezcfg nvram definitions */
#define EZCFG_NVRAM_BUFFER_SIZE            0x10000 /* 64K Bytes */
#define EZCFG_NVRAM_BUFFER_SIZE_STRING     "65536" /* 64K Bytes */
#define EZCFG_NVRAM_BACKEND_NONE           0
#define EZCFG_NVRAM_BACKEND_NONE_STRING    "0"
#define EZCFG_NVRAM_BACKEND_FILE           1
#define EZCFG_NVRAM_BACKEND_FILE_STRING    "1"
#define EZCFG_NVRAM_BACKEND_FLASH          2
#define EZCFG_NVRAM_BACKEND_FLASH_STRING   "2"
#define EZCFG_NVRAM_BACKEND_HDD            3
#define EZCFG_NVRAM_BACKEND_HDD_STRING     "3"
#define EZCFG_NVRAM_CODING_NONE            0
#define EZCFG_NVRAM_CODING_NONE_STRING     "0"
#define EZCFG_NVRAM_CODING_GZIP            1
#define EZCFG_NVRAM_CODING_GZIP_STRING     "1"
//#define EZCFG_NVRAM_STORAGE_PATH           EZCFG_ROOT_PATH "/nvram.bin"
#define EZCFG_NVRAM_STORAGE_PATH           EZCFG_VAR_PATH "/nvram.bin"
//#define EZCFG_NVRAM_BACKUP_STORAGE_PATH    EZCFG_ROOT_PATH "/nvram_backup.bin"
#define EZCFG_NVRAM_BACKUP_STORAGE_PATH    EZCFG_VAR_PATH "/nvram_backup.bin"
#define EZCFG_NVRAM_STORAGE_NUM            2

/* ezcfg authentication type string */
#define EZCFG_AUTH_TYPE_HTTP_BASIC_STRING  "http-basic"
#define EZCFG_AUTH_TYPE_HTTP_DIGEST_STRING "http-digest"


/* ezcfg rc act string */
#define EZCFG_RC_ACT_START                 "start"
#define EZCFG_RC_ACT_STOP                  "stop"
#define EZCFG_RC_ACT_RESTART               "restart"
#define EZCFG_RC_ACT_RELOAD                "reload"

/* ezcfg rc service string */
#define EZCFG_RC_SERVICE_ACTION            "action"
#define EZCFG_RC_SERVICE_SYSTEM            "system"
#define EZCFG_RC_SERVICE_LOGIN             "login"
#define EZCFG_RC_SERVICE_EZCFG_HTTPD       "ezcfg_httpd"
#define EZCFG_RC_SERVICE_EZCFG_UPNPD       "ezcfg_upnpd"
#define EZCFG_RC_SERVICE_TELNETD           "telnetd"
#define EZCFG_RC_SERVICE_EMC2              "emc2"
#define EZCFG_RC_SERVICE_EMC2_LATENCY_TEST "emc2_latency_test"

/* ezcfg service binding type */
#define EZCFG_SERVICE_BINDING_UNKNOWN	0
#define EZCFG_SERVICE_BINDING_LAN	1
#define EZCFG_SERVICE_BINDING_WAN	2


/* ezcfg socket domain string */
#define EZCFG_SOCKET_DOMAIN_LOCAL_STRING   "local"
#define EZCFG_SOCKET_DOMAIN_INET_STRING    "inet"
#define EZCFG_SOCKET_DOMAIN_NETLINK_STRING "netlink"
#define EZCFG_SOCKET_DOMAIN_INET6_STRING   "inet6"

/* ezcfg socket type string */
#define EZCFG_SOCKET_TYPE_STREAM_STRING    "stream"
#define EZCFG_SOCKET_TYPE_DGRAM_STRING     "dgram"
#define EZCFG_SOCKET_TYPE_RAW_STRING       "raw"

/* ezcfg supported protocols */
#define EZCFG_PROTO_UNKNOWN             0
#define EZCFG_PROTO_CTRL                1
#define EZCFG_PROTO_HTTP                2
#define EZCFG_PROTO_HTTPS               3
#define EZCFG_PROTO_SOAP_HTTP           4
#define EZCFG_PROTO_IGRS                5
#define EZCFG_PROTO_IGRS_ISDP           6
#define EZCFG_PROTO_UEVENT              7
#define EZCFG_PROTO_UPNP_SSDP           8
#define EZCFG_PROTO_UPNP_HTTP           9
#define EZCFG_PROTO_UPNP_GENA           10
#define EZCFG_PROTO_JSON_HTTP           11
#define EZCFG_PROTO_NV_JSON_HTTP        12

//#define EZCFG_SOCKET_PROTO_UNKNOWN_STRING         "0"
#define EZCFG_SOCKET_PROTO_CTRL_STRING            "ctrl"
#define EZCFG_SOCKET_PROTO_HTTP_STRING            "http"
#define EZCFG_SOCKET_PROTO_HTTPS_STRING           "https"
#define EZCFG_SOCKET_PROTO_SOAP_HTTP_STRING       "soap-http"
#define EZCFG_SOCKET_PROTO_IGRS_STRING            "igrs"
#define EZCFG_SOCKET_PROTO_IGRS_ISDP_STRING       "igrs-isdp"
#define EZCFG_SOCKET_PROTO_UEVENT_STRING          "uevent"
#define EZCFG_SOCKET_PROTO_UPNP_SSDP_STRING       "upnp-ssdp"
#define EZCFG_SOCKET_PROTO_UPNP_HTTP_STRING       "upnp-http"
#define EZCFG_SOCKET_PROTO_UPNP_GENA_STRING       "upnp-gena"
#define EZCFG_SOCKET_PROTO_JSON_HTTP_STRING       "json-http"
#define EZCFG_SOCKET_PROTO_NV_JSON_HTTP_STRING    "nv-json-http"

#define EZCFG_PROTO_HTTP_PORT_NUMBER              80
#define EZCFG_PROTO_HTTP_PORT_NUMBER_STRING       "80"

#define EZCFG_PROTO_HTTPS_PORT_NUMBER             443
#define EZCFG_PROTO_HTTPS_PORT_NUMBER_STRING      "443"

//#define EZCFG_PROTO_IGRS_ISDP_IPADDR_STRING       "224.0.0.110"
#define EZCFG_PROTO_IGRS_ISDP_MCAST_IPADDR_STRING "239.255.255.250"
#define EZCFG_PROTO_IGRS_ISDP_MCAST_IPV6_LINK_LOCAL_ADDR_STRING "ff02::c"
#define EZCFG_PROTO_IGRS_ISDP_MCAST_IPV6_SITE_LOCAL_ADDR_STRING "ff05::c"
#define EZCFG_PROTO_IGRS_ISDP_MCAST_IPV6_GLOBAL_ADDR_STRING "ff0e::c"
#define EZCFG_PROTO_IGRS_ISDP_PORT_NUMBER         3880
#define EZCFG_PROTO_IGRS_ISDP_PORT_NUMBER_STRING  "3880"

#define EZCFG_PROTO_UPNP_SSDP_MCAST_IPADDR_STRING "239.255.255.250"
#define EZCFG_PROTO_UPNP_SSDP_MCAST_IPV6_LINK_LOCAL_ADDR_STRING "ff02::c"
#define EZCFG_PROTO_UPNP_SSDP_MCAST_IPV6_SITE_LOCAL_ADDR_STRING "ff05::c"
#define EZCFG_PROTO_UPNP_SSDP_MCAST_IPV6_GLOBAL_ADDR_STRING "ff0e::c"
#define EZCFG_PROTO_UPNP_SSDP_PORT_NUMBER         1900
#define EZCFG_PROTO_UPNP_SSDP_PORT_NUMBER_STRING  "1900"

#define EZCFG_PROTO_UPNP_HTTP_PORT_NUMBER         60080
#define EZCFG_PROTO_UPNP_HTTP_PORT_NUMBER_STRING  "60080"

#define EZCFG_PROTO_UPNP_GENA_PORT_NUMBER         61900
#define EZCFG_PROTO_UPNP_GENA_PORT_NUMBER_STRING  "61900"


/* ezcfg json definitions */
#define EZCFG_JSON_VALUE_TYPE_UNKNOWN   0
#define EZCFG_JSON_VALUE_TYPE_FALSE     1
#define EZCFG_JSON_VALUE_TYPE_NULL      2
#define EZCFG_JSON_VALUE_TYPE_TRUE      3
#define EZCFG_JSON_VALUE_TYPE_OBJECT    4
#define EZCFG_JSON_VALUE_TYPE_ARRAY     5
#define EZCFG_JSON_VALUE_TYPE_NUMBER    6
#define EZCFG_JSON_VALUE_TYPE_STRING    7
#define EZCFG_JSON_VALUE_TYPE_NVPAIR    8
#define EZCFG_JSON_VALUE_TYPE_EMPTYSET  9


/* ezcfg JSON over HTTP request/response mode */
#define EZCFG_JSON_HTTP_MODE_REQUEST        EZCFG_HTTP_MODE_REQUEST
#define EZCFG_JSON_HTTP_MODE_RESPONSE       EZCFG_HTTP_MODE_RESPONSE
#define EZCFG_JSON_HTTP_CHUNK_SIZE          20480
#define EZCFG_JSON_HTTP_MAX_CHUNK_NUM       32
/* ezcfg JSON over HTTP request size */
#define EZCFG_JSON_HTTP_MAX_REQUEST_SIZE    (EZCFG_JSON_HTTP_CHUNK_SIZE * EZCFG_JSON_HTTP_MAX_CHUNK_NUM)
/* ezcfg JSON over HTTP response size */
#define EZCFG_JSON_HTTP_MAX_RESPONSE_SIZE   EZCFG_JSON_HTTP_MAX_REQUEST_SIZE


/* ezcfg NVRAM on JSON over HTTP request/response mode */
#define EZCFG_JSON_NVRAM_HTTP_MODE_REQUEST        EZCFG_HTTP_MODE_REQUEST
#define EZCFG_JSON_NVRAM_HTTP_MODE_RESPONSE       EZCFG_HTTP_MODE_RESPONSE
#define EZCFG_JSON_NVRAM_HTTP_CHUNK_SIZE          20480
#define EZCFG_JSON_NVRAM_HTTP_MAX_CHUNK_NUM       32
/* ezcfg NVRAM on JSON over HTTP request size */
#define EZCFG_JSON_NVRAM_HTTP_MAX_REQUEST_SIZE    (EZCFG_JSON_NVRAM_HTTP_CHUNK_SIZE * EZCFG_JSON_NVRAM_HTTP_MAX_CHUNK_NUM)
/* ezcfg NVRAM on JSON over HTTP response size */
#define EZCFG_JSON_NVRAM_HTTP_MAX_RESPONSE_SIZE   EZCFG_JSON_NVRAM_HTTP_MAX_REQUEST_SIZE


/* ezcfg xml definitions */
#define EZCFG_XML_MAX_ELEMENTS	         128
#define EZCFG_XML_ENLARGE_SIZE           16
#define EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL 0
#define EZCFG_XML_ELEMENT_ATTRIBUTE_HEAD 1


/* ezcfg css definitions */
#define EZCFG_CSS_RULE_TYPE_RULE_SET        0
#define EZCFG_CSS_RULE_TYPE_AT_RULE         1


/* ezcfg soap element names */
#define EZCFG_SOAP_ENVELOPE_ELEMENT_NAME    "env:Envelope"
#define EZCFG_SOAP_HEADER_ELEMENT_NAME      "env:Header"
#define EZCFG_SOAP_BODY_ELEMENT_NAME        "env:Body"
#define EZCFG_SOAP_FAULT_ELEMENT_NAME       "env:Fault"
#define EZCFG_SOAP_CODE_ELEMENT_NAME        "env:Code"
#define EZCFG_SOAP_VALUE_ELEMENT_NAME       "env:Value"
#define EZCFG_SOAP_VALUE_ELEMENT_VALUE      "env:Sender"
#define EZCFG_SOAP_SUBCODE_ELEMENT_NAME     "env:Subcode"
#define EZCFG_SOAP_REASON_ELEMENT_NAME      "env:Reason"
#define EZCFG_SOAP_TEXT_ELEMENT_NAME        "env:Text"
#define EZCFG_SOAP_DETAIL_ELEMENT_NAME      "env:Detail"
#define EZCFG_SOAP_NODE_ELEMENT_NAME        "env:Node"
#define EZCFG_SOAP_ROLE_ELEMENT_NAME        "env:Role"
#define EZCFG_SOAP_ENVELOPE_ATTR_NS_NAME    "xmlns:env"
#define EZCFG_SOAP_ENVELOPE_ATTR_NS_VALUE   "http://www.w3.org/2003/05/soap-envelope"
#define EZCFG_SOAP_ENVELOPE_ATTR_ENC_NAME   "env:encodingStyle"
#define EZCFG_SOAP_ENVELOPE_ATTR_ENC_VALUE  "http://www.w3.org/2003/05/soap-encoding"

#define EZCFG_SOAP_NVRAM_GETNV_ELEMENT_NAME              "nvns:getNvram"
#define EZCFG_SOAP_NVRAM_GETNV_RESPONSE_ELEMENT_NAME     "nvns:getNvramResponse"
#define EZCFG_SOAP_NVRAM_SETNV_ELEMENT_NAME              "nvns:setNvram"
#define EZCFG_SOAP_NVRAM_SETNV_RESPONSE_ELEMENT_NAME     "nvns:setNvramResponse"
#define EZCFG_SOAP_NVRAM_UNSETNV_ELEMENT_NAME            "nvns:unsetNvram"
#define EZCFG_SOAP_NVRAM_UNSETNV_RESPONSE_ELEMENT_NAME   "nvns:unsetNvramResponse"
#define EZCFG_SOAP_NVRAM_SETMNV_ELEMENT_NAME             "nvns:setMultiNvram"
#define EZCFG_SOAP_NVRAM_SETMNV_RESPONSE_ELEMENT_NAME    "nvns:setMultiNvramResponse"
#define EZCFG_SOAP_NVRAM_LISTNV_ELEMENT_NAME             "nvns:listNvram"
#define EZCFG_SOAP_NVRAM_LISTNV_RESPONSE_ELEMENT_NAME    "nvns:listNvramResponse"
#define EZCFG_SOAP_NVRAM_COMMITNV_ELEMENT_NAME           "nvns:commitNvram"
#define EZCFG_SOAP_NVRAM_COMMITNV_RESPONSE_ELEMENT_NAME  "nvns:commitNvramResponse"
#define EZCFG_SOAP_NVRAM_INFONV_ELEMENT_NAME             "nvns:infoNvram"
#define EZCFG_SOAP_NVRAM_INFONV_RESPONSE_ELEMENT_NAME    "nvns:infoNvramResponse"
#define EZCFG_SOAP_NVRAM_INSERT_SOCKET_ELEMENT_NAME      "nvns:insertSocket"
#define EZCFG_SOAP_NVRAM_INSERT_SOCKET_RESPONSE_ELEMENT_NAME  \
	"nvns:insertSocketNvramResponse"
#define EZCFG_SOAP_NVRAM_REMOVE_SOCKET_ELEMENT_NAME      "nvns:removeSocket"
#define EZCFG_SOAP_NVRAM_REMOVE_SOCKET_RESPONSE_ELEMENT_NAME  \
	"nvns:removeSocketNvramResponse"
#define EZCFG_SOAP_NVRAM_INSERT_SSL_ELEMENT_NAME         "nvns:insertSsl"
#define EZCFG_SOAP_NVRAM_INSERT_SSL_RESPONSE_ELEMENT_NAME  \
	"nvns:insertSslNvramResponse"
#define EZCFG_SOAP_NVRAM_REMOVE_SSL_ELEMENT_NAME         "nvns:removeSsl"
#define EZCFG_SOAP_NVRAM_REMOVE_SSL_RESPONSE_ELEMENT_NAME  \
	"nvns:removeSslNvramResponse"
#define EZCFG_SOAP_NVRAM_NAME_ELEMENT_NAME               "nvns:name"
#define EZCFG_SOAP_NVRAM_VALUE_ELEMENT_NAME              "nvns:value"
#define EZCFG_SOAP_NVRAM_RESULT_ELEMENT_NAME             "nvns:result"
#define EZCFG_SOAP_NVRAM_NVRAM_ELEMENT_NAME              "nvns:nvram"
#define EZCFG_SOAP_NVRAM_ATTR_NS_NAME                    "xmlns:nvns"
#define EZCFG_SOAP_NVRAM_ATTR_NS_VALUE                   "http://www.ezidc.net/ezcfg/nvram/schemas"
#define EZCFG_SOAP_NVRAM_INVALID_NAME_FAULT_VALUE        "NVRAM Name is Invalid"
#define EZCFG_SOAP_NVRAM_INVALID_VALUE_FAULT_VALUE       "NVRAM Value is Invalid"
#define EZCFG_SOAP_NVRAM_OPERATION_FAIL_FAULT_VALUE      "NVRAM Operation Fail"
#define EZCFG_SOAP_NVRAM_RESULT_VALUE_OK                 "OK"
#define EZCFG_SOAP_NVRAM_RESULT_VALUE_ERROR              "ERROR"


/* ezcfg http definitions */
#define EZCFG_HTTP_MAX_HEADERS         64 /* must be less than 256 */
#define EZCFG_HTTP_MAX_BUFFER_SIZE     (8*1024*1024) /* 8M bytes*/
#define EZCFG_HTTP_CHUNK_SIZE          20480
#define EZCFG_HTTP_MAX_CHUNK_NUM       32
#define EZCFG_HTTP_MAX_REQUEST_SIZE    (EZCFG_HTTP_CHUNK_SIZE * EZCFG_HTTP_MAX_CHUNK_NUM)
#define EZCFG_HTTP_MAX_RESPONSE_SIZE   EZCFG_HTTP_MAX_REQUEST_SIZE
/* ezcfg http request/response mode */
#define EZCFG_HTTP_MODE_REQUEST        0
#define EZCFG_HTTP_MODE_RESPONSE       1


/* ezcfg HTTP html request/response mode */
#define EZCFG_HTTP_HTML_HOME_INDEX_URI      "/"
#define EZCFG_HTTP_HTML_LANG_DIR            DATADIR "/ezcfg/html/lang"
#define EZCFG_HTTP_HTML_INDEX_DOMAIN        "index"

/* ezcfg index page */
#define EZCFG_HTTP_HTML_INDEX_FILE_SHTM     "index" EZCFG_FILE_EXT_SHTM_STRING


/* ezcfg HTTP html admin request/response mode */
#define EZCFG_HTTP_HTML_ADMIN_PREFIX_URI    "/admin/"

/* ezcfg HTTP html admin authentication default string */
#define EZCFG_AUTH_USER_ADMIN_STRING       "root"
#define EZCFG_AUTH_REALM_ADMIN_STRING      "ezbox"
#define EZCFG_AUTH_DOMAIN_ADMIN_STRING     EZCFG_HTTP_HTML_ADMIN_PREFIX_URI
#define EZCFG_AUTH_SECRET_ADMIN_STRING     "admin"

/* ezcfg HTTP html nvram request/response mode */
#define EZCFG_HTTP_HTML_NVRAM_PREFIX_URI    EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "nvram/"

/* ezcfg HTTP html apply.cgi request/response mode */
#define EZCFG_HTTP_HTML_APPLY_PREFIX_URI    EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "apply/"

/* ezcfg uuid definitions */
#define EZCFG_UUID_BINARY_LEN	16 /* 128/8 */
#define EZCFG_UUID_STRING_LEN	36 /* 8+1+4+1+4+1+4+1+12 */
#define EZCFG_UUID_NIL_STRING	"00000000-0000-0000-0000-000000000000"


/* ezcfg control request/response mode */
#define EZCFG_CTRL_MAX_MESSAGE_SIZE         4096
#define EZCFG_CTRL_MAX_ARGS                 8


/* ezcfg nvram SOAP/HTTP binding */


/* ezcfg SOAP/HTTP request/response mode */
#define EZCFG_SOAP_HTTP_MODE_REQUEST        EZCFG_HTTP_MODE_REQUEST
#define EZCFG_SOAP_HTTP_MODE_RESPONSE       EZCFG_HTTP_MODE_RESPONSE
#define EZCFG_SOAP_HTTP_CHUNK_SIZE          20480
#define EZCFG_SOAP_HTTP_MAX_CHUNK_NUM       32
/* ezcfg SOAP/HTTP request size */
#define EZCFG_SOAP_HTTP_MAX_REQUEST_SIZE    (EZCFG_SOAP_HTTP_CHUNK_SIZE * EZCFG_SOAP_HTTP_MAX_CHUNK_NUM)
/* ezcfg SOAP/HTTP response size */
#define EZCFG_SOAP_HTTP_MAX_RESPONSE_SIZE   EZCFG_SOAP_HTTP_MAX_REQUEST_SIZE


/* ezcfg igrs request/response mode */
#define EZCFG_IGRS_HTTP_MODE_REQUEST        EZCFG_HTTP_MODE_REQUEST
#define EZCFG_IGRS_HTTP_MODE_RESPONSE       EZCFG_HTTP_MODE_RESPONSE
#define EZCFG_IGRS_HTTP_CHUNK_SIZE          20480
#define EZCFG_IGRS_HTTP_MAX_CHUNK_NUM       32
/* ezcfg igrs/HTTP request size */
#define EZCFG_IGRS_HTTP_MAX_REQUEST_SIZE    (EZCFG_IGRS_HTTP_CHUNK_SIZE * EZCFG_IGRS_HTTP_MAX_CHUNK_NUM)
/* ezcfg igrs/HTTP response size */
#define EZCFG_IGRS_HTTP_MAX_RESPONSE_SIZE   EZCFG_IGRS_HTTP_MAX_REQUEST_SIZE


/* ezcfg uevent request/response mode */
#define EZCFG_UEVENT_MAX_MESSAGE_SIZE       4096


/* ezcfg upnp ssdp request/response mode */
#define EZCFG_UPNP_SSDP_MAX_MESSAGE_SIZE    4096
/* UPnP SSDP advertisement interval 60 seconds */
#define EZCFG_UPNP_SSDP_ADVERTISE_EXPIRE_TIME   60

#endif /* _EZCFG_H_ */
