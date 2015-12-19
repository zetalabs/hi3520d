/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : upnp/upnp_ssdp.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-09   0.1       Write it from scratch
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
#include "ezcfg-upnp.h"

#if 1
#define DBG(format, args...) do { \
	char path[256]; \
	FILE *fp; \
	snprintf(path, 256, "/tmp/%d-debug.txt", getpid()); \
	fp = fopen(path, "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

typedef struct upnp_ssdp_device_param_s {
	int upnp_version_major;
	int upnp_version_minor;
	int upnp_device_type;
	int life_time;
	char *host_ipaddr;
	int host_port;
	char *NT;
	char *NTS;
	char *ST;
	char *UDN;
} upnp_ssdp_device_param_t;

typedef struct upnp_ssdp_cp_param_s {
	int upnp_version_major;
	int upnp_version_minor;
	int upnp_device_type;
	int wait_time;
	char *host_ipaddr;
	int host_port;
	char *MAN;
	char *ST;
} upnp_ssdp_cp_param_t;

struct ezcfg_upnp_ssdp {
	struct ezcfg *ezcfg;
	struct ezcfg_socket *sp;
	struct ezcfg_http *http;
	struct ezcfg_upnp *upnp;
	char *priv_data;
};

/* for HTTP/1.1 request methods */
static const char *upnp_ssdp_method_strings[] = {
	/* bad method string */
	NULL ,
	/* UPnP SSDP used methods */
	EZCFG_UPNP_HTTP_METHOD_NOTIFY ,
	EZCFG_UPNP_HTTP_METHOD_MSEARCH ,
};

/* for HTTP/1.1 known header */
static const char *upnp_ssdp_header_strings[] = {
	/* bad header string */
	NULL ,
	/* UPnP SSDP known HTTP headers */
	EZCFG_UPNP_HTTP_HEADER_HOST ,
	EZCFG_UPNP_HTTP_HEADER_CACHE_CONTROL ,
	EZCFG_UPNP_HTTP_HEADER_LOCATION ,
	EZCFG_UPNP_HTTP_HEADER_NT ,
	EZCFG_UPNP_HTTP_HEADER_NTS ,
	EZCFG_UPNP_HTTP_HEADER_SERVER ,
	EZCFG_UPNP_HTTP_HEADER_USN ,
	EZCFG_UPNP_HTTP_HEADER_MAN ,
	EZCFG_UPNP_HTTP_HEADER_MX ,
	EZCFG_UPNP_HTTP_HEADER_ST ,
	EZCFG_UPNP_HTTP_HEADER_DATE ,
	EZCFG_UPNP_HTTP_HEADER_EXT ,
};

/**
 * private functions
 **/
static bool upnp_send_ssdp_alive(
	struct ezcfg_socket *sp,
	struct ezcfg_http *http,
	upnp_ssdp_device_param_t *param)
{
	struct ezcfg *ezcfg;
	char buf[256];
	char ipaddr[64];
	char *msg;
	size_t msg_len;
	int rc;

	ezcfg = ezcfg_socket_get_ezcfg(sp);

	/* reset HTTP data structure */
	ezcfg_http_reset_attributes(http);

	/* build HTTP request line */
	ezcfg_http_set_request_method(http, EZCFG_UPNP_HTTP_METHOD_NOTIFY);
	ezcfg_http_set_request_uri(http, "*");
	ezcfg_http_set_version_major(http, 1);
	ezcfg_http_set_version_minor(http, 1);
	ezcfg_http_set_state_request(http);

	if (ezcfg_socket_get_mcast_socket_path(sp, ipaddr, sizeof(ipaddr)) == NULL) {
		return false;
	}

	/* Host: 239.255.255.250:1900 */
	snprintf(buf, sizeof(buf), "%s:%s", ipaddr,
	         EZCFG_PROTO_UPNP_SSDP_PORT_NUMBER_STRING);
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_HOST, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	/* Cache-Control: max-age=1800 */
	snprintf(buf, sizeof(buf), "max-age=%d", param->life_time);
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_CACHE_CONTROL, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	/* Location: http://192.168.1.1:60080/igd1/InternetGatewayDevice1.xml */
	snprintf(buf, sizeof(buf), "http://%s:%d%s",
	         param->host_ipaddr, param->host_port,
	         ezcfg_util_upnp_get_device_type_description_path(param->upnp_device_type));
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_LOCATION, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	/* NT: Notification Type, for root device */
	snprintf(buf, sizeof(buf), "%s", param->NT);
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_NT, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	/* NTS: Notification Sub Type, must be "ssdp:alive" */
	snprintf(buf, sizeof(buf), "%s", param->NTS);
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_NTS, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	/* Server: OS/version UPnP/1.0 product/version */
	snprintf(buf, sizeof(buf), "Linux/2.6 UPnP/%d.%d ezbox/1.0",
	         param->upnp_version_major, param->upnp_version_minor);
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_SERVER, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	/* USN: Unique Service Name */
	if (strcmp(param->NT, param->UDN) == 0) {
		/* USN ::= uuid:device-UUID */
		snprintf(buf, sizeof(buf), "%s", param->UDN);
	}
	else {
		/* USN ::= uuid:device-UUID::NT */
		snprintf(buf, sizeof(buf), "%s::%s", param->UDN, param->NT);
	}
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_USN, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	rc = ezcfg_http_get_message_length(http);
	if (rc < 0) {
		err(ezcfg, "HTTP message length error.\n");
		return false;
	}
	msg_len = rc+1; /* one more for '\0' */

	if (msg_len <= sizeof(buf)) {
		msg = buf;
	}
	else {
		msg = malloc(msg_len);
		if (msg == NULL) {
			err(ezcfg, "HTTP malloc msg buffer error.\n");
			return false;
		}
	}

	memset(msg, 0, msg_len);
	msg_len = ezcfg_http_write_message(http, msg, msg_len);

	ezcfg_socket_write(sp, msg, msg_len, 0);

	if (msg != buf) {
		free(msg);
	}

	return true;
}

static bool upnp_send_ssdp_byebye(
	struct ezcfg_socket *sp,
	struct ezcfg_http *http,
	upnp_ssdp_device_param_t *param)
{
	struct ezcfg *ezcfg;
	char buf[256];
	char ipaddr[64];
	char *msg;
	size_t msg_len;
	int rc;

	ezcfg = ezcfg_socket_get_ezcfg(sp);

	/* reset HTTP data structure */
	ezcfg_http_reset_attributes(http);

	/* build HTTP request line */
	ezcfg_http_set_request_method(http, EZCFG_UPNP_HTTP_METHOD_NOTIFY);
	ezcfg_http_set_request_uri(http, "*");
	ezcfg_http_set_version_major(http, 1);
	ezcfg_http_set_version_minor(http, 1);
	ezcfg_http_set_state_request(http);

	if (ezcfg_socket_get_mcast_socket_path(sp, ipaddr, sizeof(ipaddr)) == NULL) {
		return false;
	}

	/* Host: 239.255.255.250:1900 */
	snprintf(buf, sizeof(buf), "%s:%s", ipaddr,
	         EZCFG_PROTO_UPNP_SSDP_PORT_NUMBER_STRING);
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_HOST, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	/* NT: Notification Type, for root device */
	snprintf(buf, sizeof(buf), "%s", param->NT);
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_NT, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	/* NTS: Notification Sub Type, must be "ssdp:byebye" */
	snprintf(buf, sizeof(buf), "%s", param->NTS);
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_NTS, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	/* USN: Unique Service Name */
	if (strcmp(param->NT, param->UDN) == 0) {
		/* USN ::= uuid:device-UUID */
		snprintf(buf, sizeof(buf), "%s", param->UDN);
	}
	else {
		/* USN ::= uuid:device-UUID::NT */
		snprintf(buf, sizeof(buf), "%s::%s", param->UDN, param->NT);
	}
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_USN, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	rc = ezcfg_http_get_message_length(http);
	if (rc < 0) {
		err(ezcfg, "HTTP message length error.\n");
		return false;
	}
	msg_len = rc+1; /* one more for '\0' */

	if (msg_len <= sizeof(buf)) {
		msg = buf;
	}
	else {
		msg = malloc(msg_len);
		if (msg == NULL) {
			err(ezcfg, "HTTP malloc msg buffer error.\n");
			return false;
		}
	}

	memset(msg, 0, msg_len);
	msg_len = ezcfg_http_write_message(http, msg, msg_len);

	ezcfg_socket_write(sp, msg, msg_len, 0);

	if (msg != buf) {
		free(msg);
	}

	return true;
}

static bool upnp_send_ssdp_discover(
	struct ezcfg_socket *sp,
	struct ezcfg_http *http,
	upnp_ssdp_cp_param_t *param)
{
	struct ezcfg *ezcfg;
	char buf[256];
	char ipaddr[256];
	char *msg;
	size_t msg_len;
	int rc;

	ezcfg = ezcfg_socket_get_ezcfg(sp);

	/* reset HTTP data structure */
	ezcfg_http_reset_attributes(http);

	/* build HTTP request line */
	ezcfg_http_set_request_method(http, EZCFG_UPNP_HTTP_METHOD_MSEARCH);
	ezcfg_http_set_request_uri(http, "*");
	ezcfg_http_set_version_major(http, 1);
	ezcfg_http_set_version_minor(http, 1);
	ezcfg_http_set_state_request(http);

	if (ezcfg_socket_get_mcast_socket_path(sp, ipaddr, sizeof(ipaddr)) == NULL) {
		return false;
	}

	/* Host: 239.255.255.250:1900 */
	snprintf(buf, sizeof(buf), "%s:%s", ipaddr,
	         EZCFG_PROTO_UPNP_SSDP_PORT_NUMBER_STRING);
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_HOST, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	/* MAN: required by HTTP Extension Framework */
	snprintf(buf, sizeof(buf), "\"%s\"", param->MAN);
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_MAN, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	/* MX: Maximum wait time in seconds */
	snprintf(buf, sizeof(buf), "%d", param->wait_time);
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_MX, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	/* ST: Search Target */
	snprintf(buf, sizeof(buf), "%s", param->ST);
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_ST, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	rc = ezcfg_http_get_message_length(http);
	if (rc < 0) {
		err(ezcfg, "HTTP message length error.\n");
		return false;
	}
	msg_len = rc+1; /* one more for '\0' */

	if (msg_len <= sizeof(buf)) {
		msg = buf;
	}
	else {
		msg = malloc(msg_len);
		if (msg == NULL) {
			err(ezcfg, "HTTP malloc msg buffer error.\n");
			return false;
		}
	}

	memset(msg, 0, msg_len);
	msg_len = ezcfg_http_write_message(http, msg, msg_len);

	ezcfg_socket_write(sp, msg, msg_len, 0);

	if (msg != buf) {
		free(msg);
	}

	return true;
}

static bool upnp_send_ssdp_respond(
	struct ezcfg_socket *sp,
	struct ezcfg_http *http,
	upnp_ssdp_device_param_t *param)
{
	struct ezcfg *ezcfg;
	char buf[256];
	char *msg;
	size_t msg_len;
	int rc;
	time_t t;
	struct tm *tmp;

	ezcfg = ezcfg_socket_get_ezcfg(sp);

	/* reset HTTP data structure */
	ezcfg_http_reset_attributes(http);

	/* build HTTP response line */
	ezcfg_http_set_version_major(http, 1);
	ezcfg_http_set_version_minor(http, 1);
	ezcfg_http_set_status_code(http, 200);
	ezcfg_http_set_state_response(http);

	/* Cache-Control: max-age=1800 */
	snprintf(buf, sizeof(buf), "max-age=%d", param->life_time);
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_CACHE_CONTROL, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	/* Date: Thu, 01 Jan 1970 00:00:45 GMT */
	t = time(NULL);
	tmp = localtime(&t);
	if (tmp == NULL) {
		err(ezcfg, "localtime error.\n");
		return false;
	}

	if (strftime(buf, sizeof(buf), "%a, %d %b %Y %T GMT", tmp) == 0) {
		err(ezcfg, "strftime returned 0\n");
		return false;
	}

	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_DATE, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	/* EXT: Required by HTTP Extension Framework */
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_EXT, "") == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	/* Location: http://192.168.1.1:60080/igd1/InternetGatewayDevice1.xml */
	snprintf(buf, sizeof(buf), "http://%s:%d%s",
	         param->host_ipaddr, param->host_port,
	         ezcfg_util_upnp_get_device_type_description_path(param->upnp_device_type));
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_LOCATION, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	/* Server: OS/version UPnP/1.0 product/version */
	snprintf(buf, sizeof(buf), "Linux/2.6 UPnP/%d.%d ezbox/1.0",
	         param->upnp_version_major, param->upnp_version_minor);
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_SERVER, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	/* ST: Search Target */
	snprintf(buf, sizeof(buf), "%s", param->ST);
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_ST, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	/* USN: Unique Service Name */
	if (strcmp(param->ST, param->UDN) == 0) {
		/* USN ::= uuid:device-UUID */
		snprintf(buf, sizeof(buf), "%s", param->UDN);
	}
	else {
		/* USN ::= uuid:device-UUID::ST */
		snprintf(buf, sizeof(buf), "%s::%s", param->UDN, param->ST);
	}
	if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_USN, buf) == false) {
		err(ezcfg, "HTTP add header error.\n");
		return false;
	}

	rc = ezcfg_http_get_message_length(http);
	if (rc < 0) {
		err(ezcfg, "HTTP message length error.\n");
		return false;
	}
	msg_len = rc+1; /* one more for '\0' */

	if (msg_len <= sizeof(buf)) {
		msg = buf;
	}
	else {
		msg = malloc(msg_len);
		if (msg == NULL) {
			err(ezcfg, "HTTP malloc msg buffer error.\n");
			return false;
		}
	}

	memset(msg, 0, msg_len);
	msg_len = ezcfg_http_write_message(http, msg, msg_len);

	ezcfg_socket_write(sp, msg, msg_len, 0);

	if (msg != buf) {
		free(msg);
	}

	return true;
}

/**
 * Public functions
 **/
void ezcfg_upnp_ssdp_delete(struct ezcfg_upnp_ssdp *ssdp)
{
	//struct ezcfg *ezcfg;

	ASSERT(ssdp != NULL);

	//ezcfg = ssdp->ezcfg;

	if (ssdp->http != NULL) {
		ezcfg_http_delete(ssdp->http);
	}

	if (ssdp->priv_data != NULL) {
		free(ssdp->priv_data);
	}

	free(ssdp);
}

/**
 * ezcfg_upnp_ssdp_new:
 * Create ezcfg ssdp protocol data structure
 * Returns: a new ezcfg ssdp protocol data structure
 **/
struct ezcfg_upnp_ssdp *ezcfg_upnp_ssdp_new(struct ezcfg *ezcfg)
{
	struct ezcfg_upnp_ssdp *ssdp;

	ASSERT(ezcfg != NULL);

	/* initialize ssdp protocol data structure */
	ssdp = calloc(1, sizeof(struct ezcfg_upnp_ssdp));
	if (ssdp == NULL) {
		return NULL;
	}

	memset(ssdp, 0, sizeof(struct ezcfg_upnp_ssdp));

	ssdp->http = ezcfg_http_new(ezcfg);
	if (ssdp->http == NULL) {
		goto fail_exit;
	}
	ssdp->priv_data = NULL;

	ssdp->ezcfg = ezcfg;
	ezcfg_http_set_method_strings(ssdp->http, upnp_ssdp_method_strings, ARRAY_SIZE(upnp_ssdp_method_strings) - 1);
	ezcfg_http_set_known_header_strings(ssdp->http, upnp_ssdp_header_strings, ARRAY_SIZE(upnp_ssdp_header_strings) - 1);

	return ssdp;

fail_exit:
	ezcfg_upnp_ssdp_delete(ssdp);
	return NULL;
}

/**
 * ezcfg_upnp_ssdp_set_upnp:
 **/
bool ezcfg_upnp_ssdp_set_upnp(struct ezcfg_upnp_ssdp *ssdp, struct ezcfg_upnp *upnp)
{
	ASSERT(ssdp != NULL);
	ASSERT(upnp != NULL);

	ssdp->upnp = upnp;

	return true;
}

/**
 * ezcfg_upnp_ssdp_get_http:
 **/
struct ezcfg_http *ezcfg_upnp_ssdp_get_http(struct ezcfg_upnp_ssdp *ssdp)
{
	ASSERT(ssdp != NULL);

	return ssdp->http;
}

/**
 * ezcfg_upnp_ssdp_set_priv_data:
 **/
bool ezcfg_upnp_ssdp_set_priv_data(struct ezcfg_upnp_ssdp *ssdp, const char *data)
{
	char *p;

	ASSERT(ssdp != NULL);
	ASSERT(data != NULL);

	p = strdup(data);
	if (p == NULL) {
		return false;
	}
	if (ssdp->priv_data != NULL) {
		free(ssdp->priv_data);
	}
	ssdp->priv_data = p;
	return true;
}

/**
 * for controlled device
 * ezcfg_upnp_ssdp_notify_alive:
 **/
bool ezcfg_upnp_ssdp_notify_alive(struct ezcfg_upnp_ssdp *ssdp)
{
	struct ezcfg *ezcfg;
	struct ezcfg_upnp *upnp;
	struct ezcfg_http *http;
	struct ezcfg_socket *sp = NULL;
	int domain, type, proto;
	char socket_path[128];
	upnp_if_t *ifp;
	char ip[INET_ADDRSTRLEN];
	upnp_ssdp_device_param_t param;
	upnp_service_t *usp;
	upnp_device_t *udp;
	upnp_device_t **device_queue = NULL, **p_udp;
	int qi = 0, queue_size = 0;

	ASSERT(ssdp != NULL);

	ezcfg = ssdp->ezcfg;
	upnp = ssdp->upnp;
	http = ssdp->http;

	domain = ezcfg_util_socket_domain_get_index(EZCFG_SOCKET_DOMAIN_INET_STRING);
	type = ezcfg_util_socket_type_get_index(EZCFG_SOCKET_TYPE_DGRAM_STRING);
	proto = ezcfg_util_socket_protocol_get_index(EZCFG_SOCKET_PROTO_UPNP_SSDP_STRING);

	while(upnp != NULL) {
		/* check device role */
		if (upnp->role != EZCFG_UPNP_ROLE_DEVICE) {
			upnp = upnp->next;
			continue;
		}

		ifp = upnp->ifs;
		while(ifp != NULL) {
			if (ezcfg_util_if_get_ipaddr(ifp->ifname, ip) == true) {
				snprintf(socket_path, sizeof(socket_path), "%s:%s@%s",
					EZCFG_PROTO_UPNP_SSDP_MCAST_IPADDR_STRING,
					EZCFG_PROTO_UPNP_SSDP_PORT_NUMBER_STRING, ip);

				sp = ezcfg_socket_new(ezcfg, domain, type, proto, socket_path);
				if (sp == NULL) {
					return false;
				}
				ezcfg_socket_enable_sending(sp);

				/* for root device, NT ::= upnp:rootdevice */
				/* initialize SSDP param data structure */
				param.upnp_version_major = upnp->version_major;
				param.upnp_version_minor = upnp->version_minor;
				param.upnp_device_type = upnp->device_type;
				param.life_time = ifp->life_time;
				param.host_ipaddr = ip;
				param.host_port = EZCFG_PROTO_UPNP_HTTP_PORT_NUMBER;
				param.NT = "upnp:rootdevice";
				param.NTS = "ssdp:alive";
				param.UDN = upnp->u.dev.UDN;
				upnp_send_ssdp_alive(sp, http, &param);

				udp = &(upnp->u.dev);
				while (udp != NULL) {
					/* for device, NT ::= uuid:device-UUID */
					/* initialize SSDP param data structure */
					param.upnp_version_major = upnp->version_major;
					param.upnp_version_minor = upnp->version_minor;
					param.upnp_device_type = upnp->device_type;
					param.life_time = ifp->life_time;
					param.host_ipaddr = ip;
					param.host_port = EZCFG_PROTO_UPNP_HTTP_PORT_NUMBER;
					param.NT = udp->UDN;
					param.NTS = "ssdp:alive";
					param.UDN = udp->UDN;
					upnp_send_ssdp_alive(sp, http, &param);

					/* for device, NT ::= urn:schemas-upnp-org:device:deviceType:v or
					 * NT ::= urn:domain-name:device:deviceType:v */
					/* initialize SSDP param data structure */
					param.upnp_version_major = upnp->version_major;
					param.upnp_version_minor = upnp->version_minor;
					param.upnp_device_type = upnp->device_type;
					param.life_time = ifp->life_time;
					param.host_ipaddr = ip;
					param.host_port = EZCFG_PROTO_UPNP_HTTP_PORT_NUMBER;
					param.NT = udp->deviceType;
					param.NTS = "ssdp:alive";
					param.UDN = udp->UDN;
					upnp_send_ssdp_alive(sp, http, &param);

					/* for service list */
					usp = udp->serviceList;
					while (usp != NULL) {
						/* for service type, NT ::= urn:schemas-upnp-org:service:serviceType:v or
						 * NT ::= urn:domain-name:service:serviceType:v */
						/* initialize SSDP param data structure */
						param.upnp_version_major = upnp->version_major;
						param.upnp_version_minor = upnp->version_minor;
						param.upnp_device_type = upnp->device_type;
						param.life_time = ifp->life_time;
						param.host_ipaddr = ip;
						param.host_port = EZCFG_PROTO_UPNP_HTTP_PORT_NUMBER;
						param.NT = usp->serviceType;
						param.NTS = "ssdp:alive";
						param.UDN = udp->UDN;
						upnp_send_ssdp_alive(sp, http, &param);

						usp = usp->next;
					}

					/* add embedded device list to queue */
					if (udp->deviceList != NULL) {
						p_udp = realloc(device_queue, (queue_size+1)*sizeof(upnp_device_t *));
						if (p_udp != NULL) {
							device_queue = p_udp;
							device_queue[queue_size] = udp->deviceList;
							queue_size++;
						}
					}

					/* for sibling device */
					if (udp->next != NULL) {
						udp = udp->next;
					}
					else {
						if (qi < queue_size) {
							udp = device_queue[qi];
							qi++;
						}
						else {
							udp = NULL;
						}
					}
				}

				/* free device_queue */
				if (device_queue != NULL) {
					free(device_queue);
				}

				/* finish sending SSDP advertisement */
				ezcfg_socket_delete(sp);
				sp = NULL;
			}
			/* check next interface */
			ifp = ifp->next;
		}

		/* check next UPnP node */
		upnp = upnp->next;
	}

	return true;
}

/**
 * for controlled device
 * ezcfg_upnp_ssdp_notify_byebye:
 **/
bool ezcfg_upnp_ssdp_notify_byebye(struct ezcfg_upnp_ssdp *ssdp)
{
	struct ezcfg *ezcfg;
	struct ezcfg_upnp *upnp;
	struct ezcfg_http *http;
	struct ezcfg_socket *sp = NULL;
	int domain, type, proto;
	char socket_path[128];
	upnp_if_t *ifp;
	char ip[INET_ADDRSTRLEN];
	upnp_ssdp_device_param_t param;
	upnp_service_t *usp;
	upnp_device_t *udp;
	upnp_device_t **device_queue = NULL, **p_udp;
	int qi = 0, queue_size = 0;

	ASSERT(ssdp != NULL);

	ezcfg = ssdp->ezcfg;
	upnp = ssdp->upnp;
	http = ssdp->http;

	domain = ezcfg_util_socket_domain_get_index(EZCFG_SOCKET_DOMAIN_INET_STRING);
	type = ezcfg_util_socket_type_get_index(EZCFG_SOCKET_TYPE_DGRAM_STRING);
	proto = ezcfg_util_socket_protocol_get_index(EZCFG_SOCKET_PROTO_UPNP_SSDP_STRING);

	while(upnp != NULL) {
		/* check device role */
		if (upnp->role != EZCFG_UPNP_ROLE_DEVICE) {
			upnp = upnp->next;
			continue;
		}

		ifp = upnp->ifs;
		while(ifp != NULL) {
			if (ezcfg_util_if_get_ipaddr(ifp->ifname, ip) == true) {
				snprintf(socket_path, sizeof(socket_path), "%s:%s@%s",
					EZCFG_PROTO_UPNP_SSDP_MCAST_IPADDR_STRING,
					EZCFG_PROTO_UPNP_SSDP_PORT_NUMBER_STRING, ip);

				sp = ezcfg_socket_new(ezcfg, domain, type, proto, socket_path);
				if (sp == NULL) {
					return false;
				}
				ezcfg_socket_enable_sending(sp);

				/* for root device, NT ::= upnp:rootdevice */
				/* initialize SSDP param data structure */
				param.upnp_version_major = upnp->version_major;
				param.upnp_version_minor = upnp->version_minor;
				param.upnp_device_type = upnp->device_type;
				param.life_time = ifp->life_time;
				param.host_ipaddr = ip;
				param.host_port = EZCFG_PROTO_UPNP_HTTP_PORT_NUMBER;
				param.NT = "upnp:rootdevice";
				param.NTS = "ssdp:byebye";
				param.UDN = upnp->u.dev.UDN;
				upnp_send_ssdp_byebye(sp, http, &param);

				udp = &(upnp->u.dev);
				while (udp != NULL) {
					/* for device, NT ::= uuid:device-UUID */
					/* initialize SSDP param data structure */
					param.upnp_version_major = upnp->version_major;
					param.upnp_version_minor = upnp->version_minor;
					param.upnp_device_type = upnp->device_type;
					param.life_time = ifp->life_time;
					param.host_ipaddr = ip;
					param.host_port = EZCFG_PROTO_UPNP_HTTP_PORT_NUMBER;
					param.NT = udp->UDN;
					param.NTS = "ssdp:byebye";
					param.UDN = udp->UDN;
					upnp_send_ssdp_byebye(sp, http, &param);

					/* for device, NT ::= urn:schemas-upnp-org:device:deviceType:v or
					 * NT ::= urn:domain-name:device:deviceType:v */
					/* initialize SSDP param data structure */
					param.upnp_version_major = upnp->version_major;
					param.upnp_version_minor = upnp->version_minor;
					param.upnp_device_type = upnp->device_type;
					param.life_time = ifp->life_time;
					param.host_ipaddr = ip;
					param.host_port = EZCFG_PROTO_UPNP_HTTP_PORT_NUMBER;
					param.NT = udp->deviceType;
					param.NTS = "ssdp:byebye";
					param.UDN = udp->UDN;
					upnp_send_ssdp_byebye(sp, http, &param);

					/* for service list */
					usp = udp->serviceList;
					while (usp != NULL) {
						/* for service type, NT ::= urn:schemas-upnp-org:service:serviceType:v or
						 * NT ::= urn:domain-name:service:serviceType:v */
						/* initialize SSDP param data structure */
						param.upnp_version_major = upnp->version_major;
						param.upnp_version_minor = upnp->version_minor;
						param.upnp_device_type = upnp->device_type;
						param.life_time = ifp->life_time;
						param.host_ipaddr = ip;
						param.host_port = EZCFG_PROTO_UPNP_HTTP_PORT_NUMBER;
						param.NT = usp->serviceType;
						param.NTS = "ssdp:byebye";
						param.UDN = udp->UDN;
						upnp_send_ssdp_byebye(sp, http, &param);

						usp = usp->next;
					}

					/* add embedded device list to queue */
					if (udp->deviceList != NULL) {
						p_udp = realloc(device_queue, (queue_size+1)*sizeof(upnp_device_t *));
						if (p_udp != NULL) {
							device_queue = p_udp;
							device_queue[queue_size] = udp->deviceList;
							queue_size++;
						}
					}

					/* for sibling device */
					if (udp->next != NULL) {
						udp = udp->next;
					}
					else {
						if (qi < queue_size) {
							udp = device_queue[qi];
							qi++;
						}
						else {
							udp = NULL;
						}
					}
				}

				/* free device_queue */
				if (device_queue != NULL) {
					free(device_queue);
				}

				/* finish sending SSDP advertisement */
				ezcfg_socket_delete(sp);
				sp = NULL;
			}
			ifp = ifp->next;
		}

		/* check next UPnP node */
		upnp = upnp->next;
	}

	return true;
}

/**
 * for control point
 * ezcfg_upnp_ssdp_msearch_request:
 **/
bool ezcfg_upnp_ssdp_msearch_request(struct ezcfg_upnp_ssdp *ssdp)
{
	struct ezcfg *ezcfg;
	struct ezcfg_upnp *upnp;
	struct ezcfg_http *http;
	struct ezcfg_socket *sp = NULL;
	int domain, type, proto;
	char socket_path[128];
	upnp_if_t *ifp;
	char ip[INET_ADDRSTRLEN];
	upnp_ssdp_cp_param_t param;

	ASSERT(ssdp != NULL);
	ASSERT(ssdp->priv_data != NULL);

	ezcfg = ssdp->ezcfg;
	upnp = ssdp->upnp;
	http = ssdp->http;

	domain = ezcfg_util_socket_domain_get_index(EZCFG_SOCKET_DOMAIN_INET_STRING);
	type = ezcfg_util_socket_type_get_index(EZCFG_SOCKET_TYPE_DGRAM_STRING);
	proto = ezcfg_util_socket_protocol_get_index(EZCFG_SOCKET_PROTO_UPNP_SSDP_STRING);

	while(upnp != NULL) {
		/* check control point role */
		if (upnp->role != EZCFG_UPNP_ROLE_CONTROL_POINT) {
			upnp = upnp->next;
			continue;
		}

		ifp = upnp->ifs;
		while(ifp != NULL) {
			if (ezcfg_util_if_get_ipaddr(ifp->ifname, ip) == true) {
				snprintf(socket_path, sizeof(socket_path), "%s:%s@%s:%s",
					EZCFG_PROTO_UPNP_SSDP_MCAST_IPADDR_STRING,
					EZCFG_PROTO_UPNP_SSDP_PORT_NUMBER_STRING,
					ip, EZCFG_PROTO_UPNP_SSDP_PORT_NUMBER_STRING);

				sp = ezcfg_socket_new(ezcfg, domain, type, proto, socket_path);
				if (sp == NULL) {
					return false;
				}

				ezcfg_socket_binding(sp);
				ezcfg_socket_enable_sending(sp);

				/* initialize SSDP param data structure */
				param.upnp_version_major = upnp->version_major;
				param.upnp_version_minor = upnp->version_minor;
				param.upnp_device_type = upnp->device_type;
				param.wait_time = 3;
				param.host_ipaddr = ip;
				param.host_port = EZCFG_PROTO_UPNP_HTTP_PORT_NUMBER;
				param.MAN = "ssdp:discover";
				param.ST = ssdp->priv_data;
				upnp_send_ssdp_discover(sp, http, &param);

				/* finish sending SSDP search request */
				ezcfg_socket_delete(sp);
				sp = NULL;
			}
			ifp = ifp->next;
		}

		/* check next UPnP node */
		upnp = upnp->next;
	}

	return true;
}

/**
 * for controlled device
 * ezcfg_upnp_ssdp_msearch_response:
 **/
bool ezcfg_upnp_ssdp_msearch_response(struct ezcfg_upnp_ssdp *ssdp, struct ezcfg_socket *sp)
{
	//struct ezcfg *ezcfg;
	struct ezcfg_upnp *upnp;
	struct ezcfg_http *http;
	upnp_if_t *ifp;
	char ip[INET_ADDRSTRLEN];
	char if_ip[INET_ADDRSTRLEN];
	upnp_ssdp_device_param_t param;
	upnp_service_t *usp;
	upnp_device_t *udp;
	upnp_device_t **device_queue = NULL, **p_udp;
	int qi = 0, queue_size = 0;
	char *st = NULL;

	ASSERT(ssdp != NULL);
	ASSERT(sp != NULL);

	//ezcfg = ssdp->ezcfg;
	upnp = ssdp->upnp;
	http = ssdp->http;
	/* FIXME: http will be mangle by upnp_send_ssdp_respond() */
	st = strdup(ezcfg_http_get_header_value(http, EZCFG_UPNP_HTTP_HEADER_ST));
	if (st == NULL) {
		return false;
	}
	/* get socket interface ip */
	snprintf(if_ip, sizeof(if_ip), "%s", ezcfg_socket_get_group_interface_ip(sp));

	while(upnp != NULL) {
		/* check device role */
		if (upnp->role != EZCFG_UPNP_ROLE_DEVICE) {
			upnp = upnp->next;
			continue;
		}

		ifp = upnp->ifs;
		while(ifp != NULL) {
			if ((ezcfg_util_if_get_ipaddr(ifp->ifname, ip) == true) &&
			    (strcmp(if_ip, ip) == 0)) {
				/* for root device, ST ::= upnp:rootdevice */
				/* initialize SSDP param data structure */
				param.upnp_version_major = upnp->version_major;
				param.upnp_version_minor = upnp->version_minor;
				param.upnp_device_type = upnp->device_type;
				param.life_time = ifp->life_time;
				param.host_ipaddr = ip;
				param.host_port = EZCFG_PROTO_UPNP_HTTP_PORT_NUMBER;
				param.ST = "upnp:rootdevice";
				param.UDN = upnp->u.dev.UDN;
				if ((strcmp(st, "ssdp:all") == 0) ||
				    (strcmp(st, param.ST) == 0)) {
					upnp_send_ssdp_respond(sp, http, &param);
				}

				udp = &(upnp->u.dev);
				while (udp != NULL) {
					/* for device, NT ::= uuid:device-UUID */
					/* initialize SSDP param data structure */
					param.upnp_version_major = upnp->version_major;
					param.upnp_version_minor = upnp->version_minor;
					param.upnp_device_type = upnp->device_type;
					param.life_time = ifp->life_time;
					param.host_ipaddr = ip;
					param.host_port = EZCFG_PROTO_UPNP_HTTP_PORT_NUMBER;
					param.ST = udp->UDN;
					param.UDN = udp->UDN;
					if ((strcmp(st, "ssdp:all") == 0) ||
					    (strcmp(st, param.ST) == 0)) {
						upnp_send_ssdp_respond(sp, http, &param);
					}

					/* for device, NT ::= urn:schemas-upnp-org:device:deviceType:v or
					 * NT ::= urn:domain-name:device:deviceType:v */
					/* initialize SSDP param data structure */
					param.upnp_version_major = upnp->version_major;
					param.upnp_version_minor = upnp->version_minor;
					param.upnp_device_type = upnp->device_type;
					param.life_time = ifp->life_time;
					param.host_ipaddr = ip;
					param.host_port = EZCFG_PROTO_UPNP_HTTP_PORT_NUMBER;
					param.ST = udp->deviceType;
					param.UDN = udp->UDN;
					if ((strcmp(st, "ssdp:all") == 0) ||
					    (strcmp(st, param.ST) == 0)) {
						upnp_send_ssdp_respond(sp, http, &param);
					}

					/* for service list */
					usp = udp->serviceList;
					while (usp != NULL) {
						/* for service type, NT ::= urn:schemas-upnp-org:service:serviceType:v or
						 * NT ::= urn:domain-name:service:serviceType:v */
						/* initialize SSDP param data structure */
						param.upnp_version_major = upnp->version_major;
						param.upnp_version_minor = upnp->version_minor;
						param.upnp_device_type = upnp->device_type;
						param.life_time = ifp->life_time;
						param.host_ipaddr = ip;
						param.host_port = EZCFG_PROTO_UPNP_HTTP_PORT_NUMBER;
						param.ST = usp->serviceType;
						param.UDN = udp->UDN;
						if ((strcmp(st, "ssdp:all") == 0) ||
						    (strcmp(st, param.ST) == 0)) {
							upnp_send_ssdp_respond(sp, http, &param);
						}

						usp = usp->next;
					}

					/* add embedded device list to queue */
					if (udp->deviceList != NULL) {
						p_udp = realloc(device_queue, (queue_size+1)*sizeof(upnp_device_t *));
						if (p_udp != NULL) {
							device_queue = p_udp;
							device_queue[queue_size] = udp->deviceList;
							queue_size++;
						}
					}

					/* for sibling device */
					if (udp->next != NULL) {
						udp = udp->next;
					}
					else {
						if (qi < queue_size) {
							udp = device_queue[qi];
							qi++;
						}
						else {
							udp = NULL;
						}
					}
				}
				/* finish sending SSDP search response */
			}
			ifp = ifp->next;
		}

		/* check next UPnP node */
		upnp = upnp->next;
	}

	free(st);
	return true;
}

void ezcfg_upnp_ssdp_reset_attributes(struct ezcfg_upnp_ssdp *ssdp)
{
	//struct ezcfg *ezcfg;

	ASSERT(ssdp != NULL);

	//ezcfg = ssdp->ezcfg;

	ezcfg_http_reset_attributes(ssdp->http);
}
