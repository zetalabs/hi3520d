/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_ezcfg_httpd.c
 *
 * Description  : ezbox run ezcfg httpd service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-20   0.1       Write it from scratch
 * 2011-10-21   0.1       Modify it to use rcso framework
 * ============================================================================
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcd.h"

#if 0
#define DBG(format, args...) do {\
	char dbg_path[256]; \
	FILE *dbg_fp; \
	snprintf(dbg_path, 256, "/tmp/%d-debug.txt", getpid()); \
	dbg_fp = fopen(dbg_path, "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_ezcfg_httpd(int argc, char **argv)
#endif
{
	int rc = -1;
	int ip[4];
	char buf[256];
	int flag, ret = EXIT_FAILURE;
	struct ezcfg_arg_nvram_socket *ap1 = NULL;
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	struct ezcfg_arg_nvram_ssl *ap2 = NULL;
#endif

	if (argc < 3) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "ezcfg_httpd")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		goto func_exit;
	}

	buf[0] = '\0';
#if (HAVE_EZBOX_LAN_NIC == 1)
	if (strcmp(argv[1], "lan") == 0 &&
	    utils_service_binding_lan(NVRAM_SERVICE_OPTION(EZCFG, HTTPD_BINDING)) == true) {
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(LAN, IPADDR), buf, sizeof(buf));
		if (rc < 0) {
			return (EXIT_FAILURE);
		}
	} else
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
	if (strcmp(argv[1], "wan") == 0 &&
	    utils_service_binding_wan(NVRAM_SERVICE_OPTION(EZCFG, HTTPD_BINDING)) == true) {
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, IPADDR), buf, sizeof(buf));
		if (rc < 0) {
			return (EXIT_FAILURE);
		}
	} else
#endif
#if ((HAVE_EZBOX_LAN_NIC == 1) || (HAVE_EZBOX_WAN_NIC == 1))
	{
		return (EXIT_FAILURE);
	}
#endif

	rc = sscanf(buf, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
	if (rc != 4) {
		return (EXIT_FAILURE);
	}

	ap1 = ezcfg_api_arg_nvram_socket_new();
	if (ap1 == NULL) {
		goto func_exit;
	}

#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	ap2 = ezcfg_api_arg_nvram_ssl_new();
	if (ap2 == NULL) {
		goto func_exit;
	}
#endif

	flag = utils_get_rc_act_type(argv[2]);

	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_STOP :
		/* delete ezcfg httpd listening socket */
		snprintf(buf, sizeof(buf), "%d.%d.%d.%d:%s",
			ip[0], ip[1], ip[2], ip[3],
			EZCFG_PROTO_HTTP_PORT_NUMBER_STRING);
		rc = ezcfg_api_arg_nvram_socket_set_domain(ap1, EZCFG_SOCKET_DOMAIN_INET_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_socket_set_type(ap1, EZCFG_SOCKET_TYPE_STREAM_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_socket_set_protocol(ap1, EZCFG_SOCKET_PROTO_HTTP_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_socket_set_address(ap1, buf);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_nvram_remove_socket(ap1);

#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
		/* delete ezcfg httpd listening socket */
		snprintf(buf, sizeof(buf), "%d.%d.%d.%d:%s",
			ip[0], ip[1], ip[2], ip[3],
			EZCFG_PROTO_HTTPS_PORT_NUMBER_STRING);
		rc = ezcfg_api_arg_nvram_socket_set_domain(ap1, EZCFG_SOCKET_DOMAIN_INET_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_socket_set_type(ap1, EZCFG_SOCKET_TYPE_STREAM_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_socket_set_protocol(ap1, EZCFG_SOCKET_PROTO_HTTPS_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_socket_set_address(ap1, buf);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_nvram_remove_socket(ap1);

		/* SSL nvram settings */
		rc = ezcfg_api_arg_nvram_ssl_set_role(ap2, EZCFG_SSL_ROLE_SERVER_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_ssl_set_method(ap2, EZCFG_SSL_METHOD_SSLV23_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_ssl_set_socket_enable(ap2, "1");
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_ssl_set_socket_domain(ap2, EZCFG_SOCKET_DOMAIN_INET_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_ssl_set_socket_type(ap2, EZCFG_SOCKET_TYPE_STREAM_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_ssl_set_socket_protocol(ap2, EZCFG_SOCKET_PROTO_HTTPS_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_ssl_set_socket_address(ap2, buf);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_ssl_set_certificate_file(ap2, EZCFG_SSL_CERT_ROOT_PATH "/ezcfg_httpd.cert.pem");
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_ssl_set_certificate_chain_file(ap2, "");
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_ssl_set_private_key_file(ap2, EZCFG_SSL_PRIV_ROOT_PATH "/ezcfg_httpd.key.pem");
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_nvram_remove_ssl(ap2);
#endif

		/* restart ezcfg daemon */
		/* FIXME: do it in action config file */
#if 0
		if (rc >= 0) {
			rc_ezcd(RC_ACT_RELOAD);
		}
#endif
		if (flag == RC_ACT_STOP) {
			ret = EXIT_SUCCESS;
			break;
		}

		/* RC_ACT_RESTART fall through */

	case RC_ACT_START :
		rc = utils_nvram_cmp(NVRAM_SERVICE_OPTION(EZCFG, HTTPD_ENABLE), "1");
		if (rc < 0) {
			goto func_exit;
		}

		/* add ezcfg httpd listening socket */
		snprintf(buf, sizeof(buf), "%d.%d.%d.%d:%s",
			ip[0], ip[1], ip[2], ip[3],
			EZCFG_PROTO_HTTP_PORT_NUMBER_STRING);
		rc = ezcfg_api_arg_nvram_socket_set_domain(ap1, EZCFG_SOCKET_DOMAIN_INET_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_socket_set_type(ap1, EZCFG_SOCKET_TYPE_STREAM_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_socket_set_protocol(ap1, EZCFG_SOCKET_PROTO_HTTP_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_socket_set_address(ap1, buf);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_nvram_insert_socket(ap1);

#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
		/* add ezcfg httpd listening socket */
		snprintf(buf, sizeof(buf), "%d.%d.%d.%d:%s",
			ip[0], ip[1], ip[2], ip[3],
			EZCFG_PROTO_HTTPS_PORT_NUMBER_STRING);
		rc = ezcfg_api_arg_nvram_socket_set_domain(ap1, EZCFG_SOCKET_DOMAIN_INET_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_socket_set_type(ap1, EZCFG_SOCKET_TYPE_STREAM_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_socket_set_protocol(ap1, EZCFG_SOCKET_PROTO_HTTPS_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_socket_set_address(ap1, buf);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_nvram_insert_socket(ap1);

		/* SSL nvram settings */
		rc = ezcfg_api_arg_nvram_ssl_set_role(ap2, EZCFG_SSL_ROLE_SERVER_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_ssl_set_method(ap2, EZCFG_SSL_METHOD_SSLV23_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_ssl_set_socket_enable(ap2, "1");
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_ssl_set_socket_domain(ap2, EZCFG_SOCKET_DOMAIN_INET_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_ssl_set_socket_type(ap2, EZCFG_SOCKET_TYPE_STREAM_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_ssl_set_socket_protocol(ap2, EZCFG_SOCKET_PROTO_HTTPS_STRING);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_ssl_set_socket_address(ap2, buf);
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_ssl_set_certificate_file(ap2, EZCFG_SSL_CERT_ROOT_PATH "/ezcfg_httpd.cert.pem");
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_ssl_set_certificate_chain_file(ap2, "");
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_arg_nvram_ssl_set_private_key_file(ap2, EZCFG_SSL_PRIV_ROOT_PATH "/ezcfg_httpd.key.pem");
		if (rc < 0) {
			goto func_exit;
		}
		rc = ezcfg_api_nvram_insert_ssl(ap2);
#endif

		/* restart ezcfg daemon */
		/* FIXME: do it in config file */
#if 0
		if (rc >= 0) {
			rc_ezcd(RC_ACT_RELOAD);
		}
#endif
		ret = EXIT_SUCCESS;
		break;

	default:
		ret = EXIT_FAILURE;
		break;
	}

func_exit:
	if (ap1 != NULL) {
		ezcfg_api_arg_nvram_socket_delete(ap1);
	}

#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	if (ap2 != NULL) {
		ezcfg_api_arg_nvram_ssl_delete(ap2);
	}
#endif

	return (ret);
}
