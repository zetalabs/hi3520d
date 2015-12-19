/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/master_load_socket_conf.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-10-10   0.1       Split it from master.c
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
#include <sys/ipc.h>
#include <sys/sem.h>
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

/* don't remove nvram socket */
void ezcfg_master_load_socket_conf(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;
	struct ezcfg_socket *sp = NULL;
	struct ezcfg_socket *nvram_sp = NULL;
	struct ezcfg_socket **psp;
	char *p = NULL;
	int domain, type, proto;
	char address[256];
	int i;
	int socket_number = -1;

	if (master == NULL)
		return ;

	ezcfg = ezcfg_master_get_ezcfg(master);

	/* nvram fake socket */
	//nvram_sp = ezcfg_socket_fake_new(ezcfg, AF_LOCAL, SOCK_STREAM, EZCFG_PROTO_SOAP_HTTP, EZCFG_SOCK_NVRAM_PATH);
	p = ezcfg_common_get_sock_nvram_path(ezcfg);
	if ((p != NULL) && (*p != '\0')) {
		nvram_sp = ezcfg_socket_fake_new(ezcfg, AF_LOCAL, SOCK_STREAM, EZCFG_PROTO_SOAP_HTTP, p);
		if (nvram_sp == NULL) {
			err(ezcfg, "ezcfg_socket_new(nvram_sp)\n");
			return ;
		}
	}

	/* tag listening_sockets to need_delete = true; */
	sp = ezcfg_master_get_listening_sockets(master);
	while(sp != NULL) {
		if((nvram_sp != NULL) && (ezcfg_socket_compare(sp, nvram_sp) == false)) {
			ezcfg_socket_set_need_delete(sp, true);
		}
		sp = ezcfg_socket_get_next(sp);
	}

	/* delete unused fake sockets */
	if (nvram_sp != NULL) {
		ezcfg_socket_delete(nvram_sp);
	}

	/* first get the socket number */
	p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_SOCKET_NUMBER);
	if (p != NULL) {
		socket_number = atoi(p);
		free(p);
	}
	for (i = 0; i < socket_number; i++) {

		/* initialize */
		domain = -1;
		type = -1;
		proto = EZCFG_PROTO_UNKNOWN;
		address[0] = '\0';
		sp = NULL;

		/* socket domain */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_SOCKET, i, EZCFG_EZCFG_KEYWORD_DOMAIN);
		if (p != NULL) {
			domain = ezcfg_util_socket_domain_get_index(p);
			free(p);
		}

		/* socket type */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_SOCKET, i, EZCFG_EZCFG_KEYWORD_TYPE);
		if (p != NULL) {
			type = ezcfg_util_socket_type_get_index(p);
			free(p);
		}

		/* socket protocol */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_SOCKET, i, EZCFG_EZCFG_KEYWORD_PROTOCOL);
		if (p != NULL) {
			proto = ezcfg_util_socket_protocol_get_index(p);
			free(p);
		}

		/* socket address */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_SOCKET, i, EZCFG_EZCFG_KEYWORD_ADDRESS);
		if (p != NULL) {
			snprintf(address, sizeof(address), "%s", p);
			free(p);
			p = NULL;
		}
		if ((domain < 0) ||
		    (type < 0) ||
		    (proto == EZCFG_PROTO_UNKNOWN) ||
		    (address[0] == '\0')) {
			err(ezcfg, "socket setting error\n");
			continue;
		}

		sp = ezcfg_socket_fake_new(ezcfg, domain, type, proto, address);
		if (sp == NULL) {
			err(ezcfg, "init fake socket fail: %m\n");
			continue;
		}

	    	psp = ezcfg_master_get_p_listening_sockets(master);
	    	if (ezcfg_socket_list_in(psp, sp) == true) {
			info(ezcfg, "socket already up\n");
			/* don't delete this socket in listening_sockets */
			ezcfg_socket_list_set_need_delete(psp, sp, false);
			ezcfg_socket_delete(sp);
			continue;
		}

		/* not in listening sockets, create it now */
		/* first delete the fake socket */
		ezcfg_socket_delete(sp);
		/* then create the socket */
		sp = ezcfg_socket_new(ezcfg, domain, type, proto, address);
		if (sp == NULL) {
			err(ezcfg, "init socket fail: %m\n");
			continue;
		}

		if ((domain == AF_LOCAL) &&
		    (address[0] != '@')) {
			ezcfg_socket_set_need_unlink(sp, true);
		}

	    	psp = ezcfg_master_get_p_listening_sockets(master);
		if (ezcfg_socket_list_insert(psp, sp) < 0) {
			err(ezcfg, "insert listener socket fail: %m\n");
			ezcfg_socket_delete(sp);
			continue;
		}

		if (ezcfg_socket_enable_receiving(sp) < 0) {
			err(ezcfg, "enable socket [%s] receiving fail: %m\n", address);
	    		psp = ezcfg_master_get_p_listening_sockets(master);
			ezcfg_socket_list_delete_socket(psp, sp);
			continue;
		}

		if (ezcfg_socket_enable_listening(sp, ezcfg_master_get_sq_len(master)) < 0) {
			err(ezcfg, "enable socket [%s] listening fail: %m\n", address);
	    		psp = ezcfg_master_get_p_listening_sockets(master);
			ezcfg_socket_list_delete_socket(psp, sp);
			continue;
		}

		ezcfg_socket_set_close_on_exec(sp);
	}

	/* delete all sockets taged need_delete = true in need_listening_sockets */
	sp = ezcfg_master_get_listening_sockets(master);
	while(sp != NULL) {
		if(ezcfg_socket_get_need_delete(sp) == true) {
	    		psp = ezcfg_master_get_p_listening_sockets(master);
			ezcfg_socket_list_delete_socket(psp, sp);
			sp = ezcfg_master_get_listening_sockets(master);
		}
		else {
			sp = ezcfg_socket_get_next(sp);
		}
	}
}
