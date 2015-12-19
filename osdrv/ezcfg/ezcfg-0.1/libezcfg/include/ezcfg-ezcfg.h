/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-ezcfg.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-03-03   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_EZCFG_H_
#define _EZCFG_EZCFG_H_

/* ezcfg config name prefix */
#define EZCFG_EZCFG_NVRAM_PREFIX            "ezcfg."

/* ezcfg file trailing charlist */
#define EZCFG_EZCFG_TRAILING_CHARLIST       "\r\n\t "

/* ezcfg config [sys] section */
#define EZCFG_EZCFG_SECTION_SYS             "sys"
#define EZCFG_EZCFG_KEYWORD_LANGUAGE        "language"
#define EZCFG_EZCFG_KEYWORD_TZ_AREA         "tz_area"
#define EZCFG_EZCFG_KEYWORD_TZ_LOCATION     "tz_location"

#define EZCFG_EZCFG_KEYWORD_RESTORE_DEFAULTS \
	"restore_defaults"

#define EZCFG_EZCFG_SYS_LANGUAGE            "sys.language"
#define EZCFG_EZCFG_SYS_TZ_AREA             "sys.tz_area"
#define EZCFG_EZCFG_SYS_TZ_LOCATION         "sys.tz_location"
#define EZCFG_EZCFG_SYS_RESTORE_DEFAULTS    "sys.restore_defaults"

/* ezcfg config [common] section */
#define EZCFG_EZCFG_SECTION_COMMON          "common"

#define EZCFG_EZCFG_KEYWORD_LOG_LEVEL       "log_level"
#define EZCFG_EZCFG_KEYWORD_RULES_PATH      "rules_path"
#define EZCFG_EZCFG_KEYWORD_ROOT_PATH       "root_path"
#define EZCFG_EZCFG_KEYWORD_SEM_EZCFG_PATH  "sem_ezcfg_path"
#define EZCFG_EZCFG_KEYWORD_SHM_EZCFG_PATH  "shm_ezcfg_path"
#define EZCFG_EZCFG_KEYWORD_SHM_EZCFG_SIZE  "shm_ezcfg_size"
#define EZCFG_EZCFG_KEYWORD_SHM_EZCFG_NVRAM_QUEUE_LENGTH \
	"shm_ezcfg_nvram_queue_length"
#define EZCFG_EZCFG_KEYWORD_SHM_EZCFG_RC_QUEUE_LENGTH \
	"shm_ezcfg_rc_queue_length"
#define EZCFG_EZCFG_KEYWORD_SHM_EZCTP_PATH  "shm_ezctp_path"
#define EZCFG_EZCFG_KEYWORD_SHM_EZCTP_SIZE  "shm_ezctp_size"
#define EZCFG_EZCFG_KEYWORD_SHM_EZCTP_CQ_UNIT_SIZE \
	"shm_ezctp_cq_unit_size"
#define EZCFG_EZCFG_KEYWORD_SHM_EZCTP_CQ_LENGTH \
	"shm_ezctp_cq_length"
#define EZCFG_EZCFG_KEYWORD_SOCK_CTRL_PATH  "sock_ctrl_path"
#define EZCFG_EZCFG_KEYWORD_SOCK_NVRAM_PATH "sock_nvram_path"
#define EZCFG_EZCFG_KEYWORD_SOCK_UEVENT_PATH \
	"sock_uevent_path"
#define EZCFG_EZCFG_KEYWORD_SOCK_MASTER_PATH \
	"sock_master_path"
#define EZCFG_EZCFG_KEYWORD_WEB_DOCUMENT_ROOT_PATH \
	"web_document_root_path"
#define EZCFG_EZCFG_KEYWORD_SOCKET_NUMBER   "socket_number"
#define EZCFG_EZCFG_KEYWORD_LOCALE          "locale"
#define EZCFG_EZCFG_KEYWORD_AUTH_NUMBER     "auth_number"
#define EZCFG_EZCFG_KEYWORD_SSL_NUMBER      "ssl_number"
#define EZCFG_EZCFG_KEYWORD_IGRS_NUMBER     "igrs_number"
#define EZCFG_EZCFG_KEYWORD_UPNP_NUMBER     "upnp_number"

#define EZCFG_EZCFG_COMMON_LOG_LEVEL        "common.log_level"
#define EZCFG_EZCFG_COMMON_RULES_PATH       "common.rules_path"
#define EZCFG_EZCFG_COMMON_SEM_EZCFG_PATH   "common.sem_ezcfg_path"
#define EZCFG_EZCFG_COMMON_SOCK_CTRL_PATH   "common.sock_ctrl_path"
#define EZCFG_EZCFG_COMMON_SOCK_NVRAM_PATH  "common.sock_nvram_path"
#define EZCFG_EZCFG_COMMON_SOCK_UEVENT_PATH "common.sock_uevent_path"
#define EZCFG_EZCFG_COMMON_SOCKET_NUMBER    "common.socket_number"
#define EZCFG_EZCFG_COMMON_LOCALE           "common.locale"
#define EZCFG_EZCFG_COMMON_AUTH_NUMBER      "common.auth_number"
#define EZCFG_EZCFG_COMMON_SSL_NUMBER       "common.ssl_number"
#define EZCFG_EZCFG_COMMON_IGRS_NUMBER      "common.igrs_number"
#define EZCFG_EZCFG_COMMON_UPNP_NUMBER      "common.upnp_number"

/* ezcfg config [nvram] section */
#define EZCFG_EZCFG_SECTION_NVRAM           "nvram"

#define EZCFG_EZCFG_KEYWORD_BUFFER_SIZE     "buffer_size"
#define EZCFG_EZCFG_KEYWORD_BACKEND_TYPE    "backend_type"
#define EZCFG_EZCFG_KEYWORD_CODING_TYPE     "coding_type"
#define EZCFG_EZCFG_KEYWORD_STORAGE_PATH    "storage_path"

#define EZCFG_EZCFG_NVRAM_0_BUFFER_SIZE     "nvram.0.buffer_size"
#define EZCFG_EZCFG_NVRAM_0_BACKEND_TYPE    "nvram.0.backend_type"
#define EZCFG_EZCFG_NVRAM_0_CODING_TYPE     "nvram.0.coding_type"
#define EZCFG_EZCFG_NVRAM_0_STORAGE_PATH    "nvram.0.storage_path"

#define EZCFG_EZCFG_NVRAM_1_BUFFER_SIZE     "nvram.1.buffer_size"
#define EZCFG_EZCFG_NVRAM_1_BACKEND_TYPE    "nvram.1.backend_type"
#define EZCFG_EZCFG_NVRAM_1_CODING_TYPE     "nvram.1.coding_type"
#define EZCFG_EZCFG_NVRAM_1_STORAGE_PATH    "nvram.1.storage_path"

/* ezcfg config [socket] section */
#define EZCFG_EZCFG_SECTION_SOCKET          "socket"

#define EZCFG_EZCFG_KEYWORD_DOMAIN          "domain"
#define EZCFG_EZCFG_KEYWORD_TYPE            "type"
#define EZCFG_EZCFG_KEYWORD_PROTOCOL        "protocol"
#define EZCFG_EZCFG_KEYWORD_ADDRESS         "address"

#define EZCFG_EZCFG_SOCKET_0_DOMAIN         "socket.0.domain"
#define EZCFG_EZCFG_SOCKET_0_TYPE           "socket.0.type"
#define EZCFG_EZCFG_SOCKET_0_PROTOCOL       "socket.0.protocol"
#define EZCFG_EZCFG_SOCKET_0_ADDRESS        "socket.0.address"

#define EZCFG_EZCFG_SOCKET_1_DOMAIN         "socket.1.domain"
#define EZCFG_EZCFG_SOCKET_1_TYPE           "socket.1.type"
#define EZCFG_EZCFG_SOCKET_1_PROTOCOL       "socket.1.protocol"
#define EZCFG_EZCFG_SOCKET_1_ADDRESS        "socket.1.address"

/* ezcfg config [auth] section */
#define EZCFG_EZCFG_SECTION_AUTH            "auth"

//#define EZCFG_EZCFG_KEYWORD_TYPE            "type"
#define EZCFG_EZCFG_KEYWORD_USER            "user"
#define EZCFG_EZCFG_KEYWORD_REALM           "realm"
//#define EZCFG_EZCFG_KEYWORD_DOMAIN          "domain"
#define EZCFG_EZCFG_KEYWORD_SECRET          "secret"

#define EZCFG_EZCFG_AUTH_0_TYPE             "auth.0.type"
#define EZCFG_EZCFG_AUTH_0_USER             "auth.0.user"
#define EZCFG_EZCFG_AUTH_0_REALM            "auth.0.realm"
#define EZCFG_EZCFG_AUTH_0_DOMAIN           "auth.0.domain"
#define EZCFG_EZCFG_AUTH_0_SECRET           "auth.0.secret"
#define EZCFG_EZCFG_AUTH_0_SECRET2          "auth.0.secret2"

/* ezcfg config [ssl] section */
#define EZCFG_EZCFG_SECTION_SSL             "ssl"

#define EZCFG_EZCFG_KEYWORD_ROLE            "role"
#define EZCFG_EZCFG_KEYWORD_METHOD          "method"
#define EZCFG_EZCFG_KEYWORD_CERTIFICATE_FILE            "certificate_file"
#define EZCFG_EZCFG_KEYWORD_CERTIFICATE_CHAIN_FILE      "certificate_chain_file"
#define EZCFG_EZCFG_KEYWORD_PRIVATE_KEY_FILE            "private_key_file"
#define EZCFG_EZCFG_KEYWORD_SOCKET_ENABLE   "socket_enable"
#define EZCFG_EZCFG_KEYWORD_SOCKET_DOMAIN   "socket_domain"
#define EZCFG_EZCFG_KEYWORD_SOCKET_TYPE     "socket_type"
#define EZCFG_EZCFG_KEYWORD_SOCKET_PROTOCOL "socket_protocol"
#define EZCFG_EZCFG_KEYWORD_SOCKET_ADDRESS  "socket_address"

#define EZCFG_EZCFG_SSL_0_ROLE              "ssl.0.role"
#define EZCFG_EZCFG_SSL_0_METHOD            "ssl.0.method"
#define EZCFG_EZCFG_SSL_0_SOCKET_ENABLE     "ssl.0.socket_enable"
#define EZCFG_EZCFG_SSL_0_SOCKET_DOMAIN     "ssl.0.socket_domain"
#define EZCFG_EZCFG_SSL_0_SOCKET_TYPE       "ssl.0.socket_type"
#define EZCFG_EZCFG_SSL_0_SOCKET_PROTOCOL   "ssl.0.socket_protocol"
#define EZCFG_EZCFG_SSL_0_SOCKET_ADDRESS    "ssl.0.socket_address"
#define EZCFG_EZCFG_SSL_0_CERTIFICATE_FILE  "ssl.0.certificate_file"
#define EZCFG_EZCFG_SSL_0_CERTIFICATE_CHAIN_FILE \
	"ssl.0.certificate_chain_file"
#define EZCFG_EZCFG_SSL_0_PRIVATE_KEY_FILE  "ssl.0.private_key_file"


/* ezcfg config [igrs] section */
#define EZCFG_EZCFG_SECTION_IGRS            "igrs"

/* ezcfg config [upnp] section */
#define EZCFG_EZCFG_SECTION_UPNP            "upnp"

//#define EZCFG_EZCFG_KEYWORD_ROLE            "role"
#define EZCFG_EZCFG_KEYWORD_DEVICE_TYPE     "device_type"
#define EZCFG_EZCFG_KEYWORD_DESCRIPTION_PATH    "description_path"
#define EZCFG_EZCFG_KEYWORD_INTERFACE       "interface"

#define EZCFG_EZCFG_UPNP_0_ROLE             "upnp.0.role"
#define EZCFG_EZCFG_UPNP_0_DEVICE_TYPE      "upnp.0.device_type"
#define EZCFG_EZCFG_UPNP_0_DESCRIPTION_PATH "upnp.0.description_path"
#define EZCFG_EZCFG_UPNP_0_INTERFACE        "upnp.0.interface"

#define EZCFG_EZCFG_UPNP_1_ROLE             "upnp.1.role"
#define EZCFG_EZCFG_UPNP_1_DEVICE_TYPE      "upnp.1.device_type"
#define EZCFG_EZCFG_UPNP_1_DESCRIPTION_PATH "upnp.1.description_path"
#define EZCFG_EZCFG_UPNP_1_INTERFACE        "upnp.1.interface"

#define EZCFG_EZCFG_HTTPD_ENABLE            "httpd.enable"
#define EZCFG_EZCFG_HTTPD_HTTP              "httpd.http"
#define EZCFG_EZCFG_HTTPD_HTTPS             "httpd.https"
#define EZCFG_EZCFG_HTTPD_BINDING           "httpd.binding"

#define EZCFG_EZCFG_UPNPD_ENABLE            "upnpd.enable"
#define EZCFG_EZCFG_UPNPD_BINDING           "upnpd.binding"

#define EZCFG_EZCFG_UPNPD_IGD1_ENABLE       "upnpd.igd1_enable"

#define EZCFG_EZCFG_IGRSD_ENABLE            "igrsd.enable"
#define EZCFG_EZCFG_IGRSD_BINDING           "igrsd.binding"

/* ezcfg config [ezctp] section */
#define EZCFG_EZCFG_SECTION_EZCTP           "ezctp"

#define EZCFG_EZCFG_KEYWORD_SHM_SIZE        "shm_size"
#define EZCFG_EZCFG_KEYWORD_CQ_UNIT_SIZE    "cq_unit_size"

#endif
