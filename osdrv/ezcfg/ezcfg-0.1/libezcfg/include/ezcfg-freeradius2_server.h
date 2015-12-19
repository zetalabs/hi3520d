/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-freeradius2_server.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-10-11   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_FREERADIUS2_SERVER_H_
#define _EZCFG_FREERADIUS2_SERVER_H_

/* ezcfg nvram name prefix */
#define EZCFG_FREERADIUS2_SERVER_NVRAM_PREFIX   "freeradius2_server."

/* ezcfg freeradius2 genenal options names */
#define EZCFG_FREERADIUS2_OPT_KEYWORD_NAME      "name"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_MAX_REQUEST_TIME \
	"max_request_time"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_CLEANUP_DELAY \
	"cleanup_delay"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_MAX_REQUESTS \
	"max_requests"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_TYPE      "type"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_IPADDR    "ipaddr"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_IPV6ADDR  "ipv6addr"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_PORT      "port"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_INTERFACE "interface"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_CLIENTS   "clients"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_HOSTNAME_LOOKUPS \
	"hostname_lookups"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_ALLOW_CORE_DUMPS \
	"allow_core_lookups"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_LISTEN    "listen"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_REGULAR_EXPRESSIONS \
	"regular_expressions"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_EXTENDED_EXPRESSIONS \
	"extended_expressions"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_DESTINATION \
	"destination"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_FILE      "file"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_SYSLOG_FACILITY \
	"syslog_facility"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_STRIPPED_NAMES \
	"stripped_names"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_AUTH      "auth"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_AUTH_BADPASS \
	"auth_badpass"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_AUTH_GOODPASS \
	"auth_goodpass"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_CHECKRAD  "checkrad"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_MAX_ATTRIBUTES \
	"max_attributes"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_REJECT_DELAY \
	"reject_delay"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_PROXY_REQUESTS \
	"proxy_requests"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_START_SERVERS \
	"start_servers"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_MAX_SERVERS \
	"max_servers"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_MIN_SPARE_SERVERS \
	"min_spare_servers"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_MAX_SPARE_SERVERS \
	"max_spare_servers"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_MAX_QUEUE_SIZE \
	"max_queue_size"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_MAX_REQUESTS_PER_SERVER \
	"max_requests_per_server"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_CLIENT    "client"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_NETMASK   "netmask"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_SECRET    "secret"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_REQUIRE_MESSAGE_AUTHENTICATOR \
	"require_message_authenticator"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_SHORTNAME "shortname"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_NASTYPE   "nastype"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_VIRTUAL_SERVER \
	"virtual_server"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_COA_SERVER \
	"coa_server"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_DEFAULT_EAP_TYPE \
	"default_eap_type"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_TIMER_EXPIRE \
	"timer_expire"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_IGNORE_UNKNOWN_EAP_TYPES \
	"ignore_unknown_eap_types"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_CISCO_ACCOUNTING_USERNAME_BUG \
	"cisco_accounting_username_bug"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_MAX_SESSIONS \
	"max_sessions"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_TLS	"tls"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_ENABLE	"enable"
#define EZCFG_FREERADIUS2_OPT_KEYWORD_EAP	"eap"


/* ezcfg freeradius2 server nvram names */
#define EZCFG_FREERADIUS2_SERVER_IPADDR         "ipaddr"
#define EZCFG_FREERADIUS2_SERVER_PORT           "port"
#define EZCFG_FREERADIUS2_SERVER_NAME           "name"
#define EZCFG_FREERADIUS2_SERVER_MAX_REQUEST_TIME \
	"max_request_time"
#define EZCFG_FREERADIUS2_SERVER_CLEANUP_DELAY  "cleanup_delay"
#define EZCFG_FREERADIUS2_SERVER_MAX_REQUESTS   "max_requests"
#define EZCFG_FREERADIUS2_SERVER_LISTEN_NUM     "listen_num"
#define EZCFG_FREERADIUS2_SERVER_LISTEN         "listen"
#define EZCFG_FREERADIUS2_SERVER_HOSTNAME_LOOKUPS \
	"hostname_lookups"
#define EZCFG_FREERADIUS2_SERVER_ALLOW_CORE_DUMPS \
	"allow_core_lookups"
#define EZCFG_FREERADIUS2_SERVER_REGULAR_EXPRESSIONS \
	"regular_expressions"
#define EZCFG_FREERADIUS2_SERVER_EXTENDED_EXPRESSIONS \
	"extended_expressions"
#define EZCFG_FREERADIUS2_SERVER_LOG_DESTINATION \
	"log_destination"
#define EZCFG_FREERADIUS2_SERVER_LOG_SYSLOG_FACILITY \
	"log_syslog_facility"
#define EZCFG_FREERADIUS2_SERVER_LOG_STRIPPED_NAMES \
	"log_stripped_names"
#define EZCFG_FREERADIUS2_SERVER_LOG_AUTH2      "log_auth"
#define EZCFG_FREERADIUS2_SERVER_LOG_AUTH_BADPASS \
	"log_auth_badpass"
#define EZCFG_FREERADIUS2_SERVER_LOG_AUTH_GOODPASS \
	"log_auth_goodpass"
#define EZCFG_FREERADIUS2_SERVER_SECURITY_MAX_ATTRIBUTES \
	"security_max_attributes"
#define EZCFG_FREERADIUS2_SERVER_SECURITY_REJECT_DELAY \
	"security_reject_delay"
#define EZCFG_FREERADIUS2_SERVER_PROXY_REQUESTS \
	"proxy_requests"
#define EZCFG_FREERADIUS2_SERVER_START_SERVERS  "start_servers"
#define EZCFG_FREERADIUS2_SERVER_MAX_SERVERS    "max_servers"
#define EZCFG_FREERADIUS2_SERVER_MIN_SPARE_SERVERS \
	"min_spare_servers"
#define EZCFG_FREERADIUS2_SERVER_MAX_SPARE_SERVERS \
	"max_spare_servers"
#define EZCFG_FREERADIUS2_SERVER_MAX_QUEUE_SIZE \
	"max_queue_size"
#define EZCFG_FREERADIUS2_SERVER_MAX_REQUESTS_PER_SERVER \
	"max_requests_per_server"
#define EZCFG_FREERADIUS2_SERVER_DICTIONARY_ENTRY \
	"dictionary_entry"
#define EZCFG_FREERADIUS2_SERVER_DICTIONARY_ENTRY_NUM \
	"dictionary_entry_num"
#define EZCFG_FREERADIUS2_SERVER_CLIENT_ENTRY \
	"client_entry"
#define EZCFG_FREERADIUS2_SERVER_CLIENT_ENTRY_NUM \
	"client_entry_num"
#define EZCFG_FREERADIUS2_SERVER_PER_SOCKET_CLIENTS_ENTRY \
	"per_socket_clients_entry"
#define EZCFG_FREERADIUS2_SERVER_PER_SOCKET_CLIENTS_ENTRY_NUM \
	"per_socket_clients_entry_num"
#define EZCFG_FREERADIUS2_SERVER_EAP            "eap"

#define EZCFG_FREERADIUS2_SERVER_DEFAULT_IPADDR "192.168.1.1"
#define EZCFG_FREERADIUS2_SERVER_DEFAULT_PORT   "1812,1813"

#endif
