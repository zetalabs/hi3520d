/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : socket/socket.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-12   0.1       Write it from scratch
 * 2015-06-08   0.2       Use namespace nvram configuration
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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <linux/netlink.h>
#include <netdb.h>

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-uevent.h"

/*
 * unified socket address. For IPv6 support, add IPv6 address structure
 * in the union u.
 */
struct usa {
  socklen_t len;
  union {
    struct sockaddr sa;
    struct sockaddr_un sun;
    struct sockaddr_in sin;
    struct sockaddr_nl snl;
#if (HAVE_EZBOX_EZCFG_IPV6 == 1)
    struct sockaddr_in6 sin6;
#endif
  } u;
};

/*
 * unified multicast request.
 */
struct umreq {
  union {
    struct ip_mreq       group;     /* IPv4 multicast group */
#if (HAVE_EZBOX_EZCFG_IPV6 == 1)
    struct ipv6_mreq     groupv6;   /* IPv6 multicast group */
#endif
  } u;
};

/*
 * structure used to describe listening socket, or socket which was
 * accept()-ed by the master thread and queued for future handling
 * by the worker thread.
 */
struct ezcfg_socket {
  struct ezcfg *ezcfg;
  struct ezcfg_socket *next;      /* Linkage                      */
  int                  sock;      /* Listening socket             */
  int                  proto;     /* Communication protocol 	*/
  int                  backlog;   /* Listening queue length 	*/
  int                  domain;
  int                  type;
  struct usa           lsa;       /* Local socket address         */
  struct usa           rsa;       /* Remote socket address        */
  struct umreq         mreq;      /* Multicast group              */
  bool                 need_unlink;    /* Need to unlink socket node 	*/
  bool                 need_delete;    /* Need to delete socket node 	*/
  char *               buffer;
  int                  buffer_len;
};


/**
 * private functions
 */

static int set_non_blocking_mode(int sock)
{
  int flags;

  flags = fcntl(sock, F_GETFL, 0);
  fcntl(sock, F_SETFL, flags | O_NONBLOCK);

  return 0;
}

static void close_socket_gracefully(int sock) {
  char buf[1024];
  int n;

  /* Send FIN to the client */
  shutdown(sock, 1);
  set_non_blocking_mode(sock);

  /* Read and discard pending data. If we do not do that and close the
   * socket, the data in the send buffer may be discarded.
   */
  do {
    n = read(sock, buf, sizeof(buf));
  } while (n > 0);

  /* Now we know that our FIN is ACK-ed, safe to close */
  close(sock);
}

/**
 * create_socket:
 * Create ezcfg unified socket.
 * Returns: a new ezcfg socket
 **/
static struct ezcfg_socket *create_socket(struct ezcfg *ezcfg,
  const int domain, const int type, const int proto,
  const char *laddr, const char *raddr, bool create_sockfd)
{
  struct ezcfg_socket *sp = NULL;
  struct usa *usa = NULL;
  char *addr = NULL;
  char *port = NULL;
  int sock_protocol = -1;
  int buf_size = 16 * 1024 * 1024;

  ASSERT(ezcfg != NULL);
  ASSERT((laddr != NULL) || (raddr != NULL));

  if (ezcfg_util_socket_is_supported_domain(domain) == false) {
    err(ezcfg, "unknown socket family %d\n", domain);
    return NULL;
  }

  if (ezcfg_util_socket_is_supported_protocol(proto) == false) {
    err(ezcfg, "unknown communication protocol %d\n", proto);
    return NULL;
  }

  /* initialize socket */
  if ((sp = calloc(1, sizeof(struct ezcfg_socket))) == NULL) {
    err(ezcfg, "calloc socket fail: %m\n");
    return NULL;
  }
  memset(sp, 0, sizeof(struct ezcfg_socket));
  sp->sock = -1;
  sp->ezcfg = ezcfg;
  sp->proto = proto;
  sp->domain = domain;
  sp->type = type;

  /* FIXME: should change sock_protocol w/r proto */
  if (proto == EZCFG_PROTO_UEVENT) {
    sock_protocol = NETLINK_KOBJECT_UEVENT;
  }
  else {
    sock_protocol = 0;
  }

  switch (domain) {
  case AF_LOCAL:
    /* create socket ? */
    if (create_sockfd == true) {
      sp->sock = socket(AF_LOCAL, type, sock_protocol);
      if (sp->sock < 0) {
	err(ezcfg, "socket error\n");
	goto fail_exit;
      }
    }
    usa = &(sp->lsa);
    usa->u.sun.sun_family = AF_LOCAL;
    strcpy(usa->u.sun.sun_path, laddr);
    usa->len = offsetof(struct sockaddr_un, sun_path) + strlen(usa->u.sun.sun_path);
    /* translate leading '@' to abstract namespace */
    if (usa->u.sun.sun_path[0] == '@') {
      usa->u.sun.sun_path[0] = '\0';
    }
    usa = &(sp->rsa);
    usa->u.sun.sun_family = AF_LOCAL;
    strcpy(usa->u.sun.sun_path, raddr);
    usa->len = offsetof(struct sockaddr_un, sun_path) + strlen(usa->u.sun.sun_path);
    /* translate leading '@' to abstract namespace */
    if (usa->u.sun.sun_path[0] == '@') {
      usa->u.sun.sun_path[0] = '\0';
    }
    break;

  case AF_INET:
    /* create socket ? */
    if (create_sockfd == true) {
      sp->sock = socket(AF_INET, type, sock_protocol);
      if (sp->sock < 0) {
	err(ezcfg, "socket error\n");
	goto fail_exit;
      }
    }
    if (laddr) {
      addr = strdup(laddr);
      if (addr == NULL) {
        err(ezcfg, "can not alloc addr.\n");
        goto fail_exit;
      }
      port = strchr(addr, ':');
      if (port) {
        *port = '\0';
        port++;
      }
      usa = &(sp->lsa);
      usa->u.sin.sin_family = AF_INET;
      usa->u.sin.sin_addr.s_addr = inet_addr(addr);
      if (port) {
        usa->u.sin.sin_port = htons((uint16_t)atoi(port));
      }
      else {
        usa->u.sin.sin_port = htons(0);
      }
      usa->len = sizeof(usa->u.sin);
      free(addr);
      addr = NULL;
    }
    if (raddr) {
      addr = strdup(raddr);
      if (addr == NULL) {
        err(ezcfg, "can not alloc addr.\n");
        goto fail_exit;
      }
      port = strchr(addr, ':');
      if (port) {
        *port = '\0';
        port++;
      }
      usa = &(sp->rsa);
      usa->u.sin.sin_family = AF_INET;
      usa->u.sin.sin_addr.s_addr = inet_addr(addr);
      if (port) {
        usa->u.sin.sin_port = htons((uint16_t)atoi(port));
      }
      else {
        usa->u.sin.sin_port = htons(0);
      }
      usa->len = sizeof(usa->u.sin);
      free(addr);
      addr = NULL;
    }

#if 0
    if (ezcfg_util_socket_is_multicast_address(proto, addr) == true) {
      int reuse = 1;
      char *if_addr, *if_port;
      if_addr = strchr(port, '@');
      if (if_addr == NULL) {
	err(ezcfg, "muticast socket_path format error.\n");
	goto fail_exit;
      }
      *if_addr = '\0';
      if_addr++;
      if_port = strchr(if_addr, ':');
      if (if_port != NULL) {
	*if_port = '\0';
	if_port++;
      }
      else {
	/* set to multicast port */
	if_port = port;
      }
      usa->u.sin.sin_addr.s_addr = inet_addr(if_addr);
      usa->u.sin.sin_port = htons((uint16_t)atoi(if_port));
      usa->len = sizeof(usa->u.sin);

      /* set remote pear info */
      usa = &(sp->rsa);
      usa->u.sin.sin_family = AF_INET;
      usa->u.sin.sin_addr.s_addr = inet_addr(addr);
      usa->u.sin.sin_port = htons((uint16_t)atoi(port));
      usa->len = sizeof(usa->u.sin);

      /*
       * enable SO_REUSEADDR to allow multiple instances of this
       * application to receive copies of the multicast datagrams.
       */
      if (create_sockfd == true) {
	if(setsockopt(sp->sock, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0) {
	  err(ezcfg, "setting SO_REUSEADDR error\n");
	  goto fail_exit;
	}
      }

      /*
       * join the multicast group [addr](239.255.255.250) on the [if_addr] interface.
       * note that this IP_ADD_MEMBERSHIP option must be
       * called for each local interface over which the multicast
       * datagrams are to be received. */
      sp->mreq.u.group.imr_multiaddr.s_addr = inet_addr(addr);
      sp->mreq.u.group.imr_interface.s_addr = inet_addr(if_addr);
    }
#endif
    break;

#if (HAVE_EZBOX_EZCFG_IPV6 == 1)
  case AF_INET6:
    /* create socket ? */
    if (create_sockfd == true) {
      sp->sock = socket(AF_INET6, type, sock_protocol);
      if (sp->sock < 0) {
	err(ezcfg, "socket error\n");
	goto fail_exit;
      }
    }

    if (laddr) {
      struct addrinfo hints;
      struct addrinfo *result;
      int s;

      if (*laddr != '[') {
        err(ezcfg, "laddr format error, missing [.\n");
        goto fail_exit;
      }
      /* skip '[' */
      addr = strdup(laddr+1);
      if (addr == NULL) {
        err(ezcfg, "can not alloc addr.\n");
        goto fail_exit;
      }
      port = strchr(addr, ']');
      if (port == NULL) {
        err(ezcfg, "laddr format error, missing ].\n");
        goto fail_exit;
      }
      *port = '\0';
      port++;
      if (*port == ':') {
        *port = '\0';
        port++;
      }
      else {
        port = NULL;
      }

      /* Obtain address(es) matching host/port */
      memset(&hints, 0, sizeof(struct addrinfo));
      hints.ai_family = AF_INET6;
      hints.ai_socktype = type;
      hints.ai_flags = 0;
      hints.ai_protocol = 0;          /* Any protocol */

      s = getaddrinfo(addr, port, &hints, &result);
      if (s != 0) {
        err(ezcfg, "getaddrinfo: %s\n", gai_strerror(s));
        goto fail_exit;
      }
      if (result->ai_next != NULL) {
        err(ezcfg, "getaddrinfo: ambiguity socket address\n");
        freeaddrinfo(result);
        goto fail_exit;
      }

      usa = &(sp->lsa);
      usa->u.sin6.sin6_family = AF_INET6;
      memcpy(&(usa->u.sa), result->ai_addr, result->ai_addrlen);
      freeaddrinfo(result);

      usa->len = sizeof(usa->u.sin6);
      free(addr);
      addr = NULL;
    }

    if (raddr) {
      struct addrinfo hints;
      struct addrinfo *result;
      int s;

      if (*laddr != '[') {
        err(ezcfg, "laddr format error, missing [.\n");
        goto fail_exit;
      }
      /* skip '[' */
      addr = strdup(raddr+1);
      if (addr == NULL) {
        err(ezcfg, "can not alloc addr.\n");
        goto fail_exit;
      }
      port = strchr(addr, ']');
      if (port == NULL) {
        err(ezcfg, "laddr format error, missing ].\n");
        goto fail_exit;
      }
      *port = '\0';
      port++;
      if (*port == ':') {
        *port = '\0';
        port++;
      }
      else {
        port = NULL;
      }

      /* Obtain address(es) matching host/port */
      memset(&hints, 0, sizeof(struct addrinfo));
      hints.ai_family = AF_INET6;
      hints.ai_socktype = type;
      hints.ai_flags = 0;
      hints.ai_protocol = 0;          /* Any protocol */

      s = getaddrinfo(addr, port, &hints, &result);
      if (s != 0) {
        err(ezcfg, "getaddrinfo: %s\n", gai_strerror(s));
        goto fail_exit;
      }
      if (result->ai_next != NULL) {
        err(ezcfg, "getaddrinfo: ambiguity socket address\n");
        freeaddrinfo(result);
        goto fail_exit;
      }

      usa = &(sp->rsa);
      usa->u.sin6.sin6_family = AF_INET6;
      memcpy(&(usa->u.sa), result->ai_addr, result->ai_addrlen);
      freeaddrinfo(result);

      usa->len = sizeof(usa->u.sin6);
      free(addr);
      addr = NULL;
    }

#if 0
    usa = &(sp->lsa);
    usa->u.sin6.sin6_family = AF_INET6;
    if (ezcfg_util_socket_is_multicast_address(proto, addr) == true) {
      int reuse = 1;
      char *if_addr, *if_port;
      struct addrinfo hints;
      struct addrinfo *result;
      int s;
      if_addr = strchr(port, '@');
      if (if_addr == NULL) {
	err(ezcfg, "muticast socket_path format error, missing @.\n");
	goto fail_exit;
      }
      *if_addr = '\0';
      if_addr++;
      if (*if_addr != '[') {
	err(ezcfg, "muticast socket_path format error, missing [.\n");
	goto fail_exit;
      }
      if_port = strchr(if_addr, ']');
      if (if_port == NULL) {
	err(ezcfg, "muticast socket_path format error, missing ].\n");
	goto fail_exit;
      }
      *if_port = '\0';
      if_port++;
      if (*if_port == ':') {
	*if_port = '\0';
	if_port++;
      }
      else {
	/* set to multicast port */
	if_port = port;
      }

      /* Obtain address(es) matching host/port */
      memset(&hints, 0, sizeof(struct addrinfo));
      hints.ai_family = AF_INET6;
      hints.ai_socktype = type;
      hints.ai_flags = 0;
      hints.ai_protocol = 0;          /* Any protocol */

      s = getaddrinfo(if_addr, if_port, &hints, &result);
      if (s != 0) {
	err(ezcfg, "getaddrinfo: %s\n", gai_strerror(s));
	goto fail_exit;
      }
      if (result->ai_next != NULL) {
	err(ezcfg, "getaddrinfo: ambiguity socket address\n");
	freeaddrinfo(result);
	goto fail_exit;
      }

      memcpy(&(usa->u.sa), result->ai_addr, result->ai_addrlen);
      freeaddrinfo(result);

      usa->len = sizeof(usa->u.sin6);

      /* set remote pear info */
      usa = &(sp->rsa);
      usa->u.sin6.sin6_family = AF_INET6;

      /* Obtain address(es) matching host/port */
      memset(&hints, 0, sizeof(struct addrinfo));
      hints.ai_family = AF_INET6;
      hints.ai_socktype = type;
      hints.ai_flags = 0;
      hints.ai_protocol = 0;          /* Any protocol */

      s = getaddrinfo(addr, port, &hints, &result);
      if (s != 0) {
	err(ezcfg, "getaddrinfo: %s\n", gai_strerror(s));
	goto fail_exit;
      }
      if (result->ai_next != NULL) {
	err(ezcfg, "getaddrinfo: ambiguity socket address\n");
	freeaddrinfo(result);
	goto fail_exit;
      }

      memcpy(&(usa->u.sa), result->ai_addr, result->ai_addrlen);
      freeaddrinfo(result);

      usa->len = sizeof(usa->u.sin6);

      /*
       * enable SO_REUSEADDR to allow multiple instances of this
       * application to receive copies of the multicast datagrams.
       */
      if (create_sockfd == true) {
	if(setsockopt(sp->sock, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0) {
	  err(ezcfg, "setting SO_REUSEADDR error\n");
	  goto fail_exit;
	}
      }

      /*
       * join the multicast group [addr](ff02::c) on the [if_addr] interface.
       * note that this IPV6_ADD_MEMBERSHIP option must be
       * called for each local interface over which the multicast
       * datagrams are to be received. */
      usa = &(sp->rsa);
      memcpy(&(sp->mreq.u.groupv6.ipv6mr_multiaddr), &(usa->u.sin6), usa->len);
      usa = &(sp->lsa);
      sp->mreq.u.groupv6.ipv6mr_interface = usa->u.sin6.sin6_scope_id;
    }
#endif
    break;
#endif

  case AF_NETLINK:
    /* create socket ? */
    if (create_sockfd == true) {
      sp->sock = socket(AF_NETLINK, type, sock_protocol);
      if (sp->sock < 0) {
	err(ezcfg, "socket error\n");
	goto fail_exit;
      }
      /* set receive buffer size */
      if (setsockopt(sp->sock, SOL_SOCKET, SO_RCVBUFFORCE, &buf_size, sizeof(buf_size))) {
	buf_size = 106496;
	if (setsockopt(sp->sock, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(buf_size))) {
	  err(ezcfg, "socket set receive buffer size error\n");
	  goto fail_exit;
	}
      }
    }
    if (laddr) {
      usa = &(sp->lsa);
      usa->u.snl.nl_family = AF_NETLINK;
      //usa->u.snl.nl_groups = 1;
      if (strcmp(laddr, "kernel") == 0) {
        usa->u.snl.nl_groups = UEVENT_NLGRP_KERNEL;
      }
      else {
        usa->u.snl.nl_groups = UEVENT_NLGRP_NONE;
      }
      //usa->u.snl.nl_pid = getpid();
      if (create_sockfd == true) {
        int err = 0;
        //const int on = 1;
        struct sockaddr_nl snl;
        socklen_t addrlen;

      /*
       * get the address the kernel has assigned us
       * it is usually, but not necessarily the pid
       */
        addrlen = sizeof(struct sockaddr_nl);
        err = getsockname(sp->sock, (struct sockaddr *)&snl, &addrlen);
        if (err == 0)
          usa->u.snl.nl_pid = snl.nl_pid;

        /* enable receiving of sender credentials */
        //setsockopt(sp->sock, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on));
      }
      usa->len = sizeof(usa->u.snl);
    }
    if (raddr) {
      usa = &(sp->rsa);
      usa->u.snl.nl_family = AF_NETLINK;
      //usa->u.snl.nl_groups = 1;
      if (strcmp(raddr, "kernel") == 0) {
        usa->u.snl.nl_groups = UEVENT_NLGRP_KERNEL;
      }
      else {
        usa->u.snl.nl_groups = UEVENT_NLGRP_NONE;
      }
      //usa->u.snl.nl_pid = getpid();
      if (create_sockfd == true) {
        int err = 0;
        //const int on = 1;
        struct sockaddr_nl snl;
        socklen_t addrlen;

      /*
       * get the address the kernel has assigned us
       * it is usually, but not necessarily the pid
       */
        addrlen = sizeof(struct sockaddr_nl);
        err = getsockname(sp->sock, (struct sockaddr *)&snl, &addrlen);
        if (err == 0)
          usa->u.snl.nl_pid = snl.nl_pid;

        /* enable receiving of sender credentials */
        //setsockopt(sp->sock, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on));
      }
      usa->len = sizeof(usa->u.snl);
    }
    break;

  default:
    err(ezcfg, "bad family [%d]\n", domain);
    goto fail_exit;
    break;
  }

  return sp;

 fail_exit:
  if (addr != NULL) {
    free(addr);
  }

  if (sp != NULL) {
    if (sp->sock >= 0) {
      close(sp->sock);
    }
    free(sp);
  }
  return NULL;
}

/**
 * public functions
 */

/**
 * ezcfg_socket_del:
 * Delete ezcfg unified socket.
 * Returns:
 **/
int ezcfg_socket_del(struct ezcfg_socket *sp)
{
  struct ezcfg *ezcfg;

  ASSERT(sp != NULL);

  ezcfg = sp->ezcfg;

  if (sp->sock >= 0) {
    close(sp->sock);
    /* also remove the filesystem node */
    if (sp->domain == AF_LOCAL && sp->need_unlink == true) {
			
      if (unlink(sp->lsa.u.sun.sun_path) == -1) {
	err(ezcfg, "unlink fail: %m\n");
      }
    }
  }
  if (sp->buffer != NULL) {
    free(sp->buffer);
  }
  free(sp);

  /* decrease ezcfg library context reference */
  if (ezcfg_dec_ref(ezcfg) != EZCFG_RET_OK) {
    EZDBG("ezcfg_dec_ref() failed\n");
  }
  return EZCFG_RET_OK;
}

/**
 * ezcfg_socket_new:
 * Create ezcfg unified socket.
 * Returns: a new ezcfg socket
 **/
struct ezcfg_socket *ezcfg_socket_new(struct ezcfg *ezcfg, char *ns)
{
  int domain = -1, type = -1, proto = -1;
  char *pdomain, *ptype, *pproto, *pladdr, *praddr;
  char *laddr = NULL;
  char *raddr = NULL;
  char name[EZCFG_NAME_MAX] = "";
  struct ezcfg_linked_list *list = NULL;
  struct ezcfg_nv_pair *data = NULL;
  int ret = EZCFG_RET_FAIL;
  int i = 0, list_length = 0;
  struct ezcfg_socket *sp = NULL;
  char *val = NULL;

  ASSERT(ezcfg != NULL);

  /* increase ezcfg library context reference */
  if (ezcfg_inc_ref(ezcfg) != EZCFG_RET_OK) {
    EZDBG("%s(%d) <6>pid=[%d] ezcfg_inc_ref() failed\n", __func__, __LINE__, getpid());
    return NULL;
  }

  list = ezcfg_linked_list_new(ezcfg,
    ezcfg_nv_pair_del_handler,
    ezcfg_nv_pair_cmp_handler);
  if (list == NULL) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    goto exit_fail;
  }

  /* domain */
  ret = ezcfg_util_snprintf_ns_name(name, sizeof(name), ns, NVRAM_NAME(SOCKET, DOMAIN));
  if (ret != EZCFG_RET_OK) {
    goto exit_fail;
  }
  data = ezcfg_nv_pair_new(name, NULL);
  if (data == NULL) {
    goto exit_fail;
  }
  if (ezcfg_linked_list_append(list, data) != EZCFG_RET_OK) {
    goto exit_fail;
  }
  pdomain = ezcfg_nv_pair_get_n(data);
  data = NULL;

  /* type */
  ret = ezcfg_util_snprintf_ns_name(name, sizeof(name), ns, NVRAM_NAME(SOCKET, TYPE));
  if (ret != EZCFG_RET_OK) {
    goto exit_fail;
  }
  data = ezcfg_nv_pair_new(name, NULL);
  if (data == NULL) {
    goto exit_fail;
  }
  if (ezcfg_linked_list_append(list, data) != EZCFG_RET_OK) {
    goto exit_fail;
  }
  ptype = ezcfg_nv_pair_get_n(data);
  data = NULL;

  /* protocol */
  ret = ezcfg_util_snprintf_ns_name(name, sizeof(name), ns, NVRAM_NAME(SOCKET, PROTOCOL));
  if (ret != EZCFG_RET_OK) {
    goto exit_fail;
  }
  data = ezcfg_nv_pair_new(name, NULL);
  if (data == NULL) {
    goto exit_fail;
  }
  if (ezcfg_linked_list_append(list, data) != EZCFG_RET_OK) {
    goto exit_fail;
  }
  pproto = ezcfg_nv_pair_get_n(data);
  data = NULL;

  /* local address */
  ret = ezcfg_util_snprintf_ns_name(name, sizeof(name), ns, NVRAM_NAME(SOCKET, LOCAL_ADDRESS));
  if (ret != EZCFG_RET_OK) {
    goto exit_fail;
  }
  data = ezcfg_nv_pair_new(name, NULL);
  if (data == NULL) {
    goto exit_fail;
  }
  if (ezcfg_linked_list_append(list, data) != EZCFG_RET_OK) {
    goto exit_fail;
  }
  pladdr = ezcfg_nv_pair_get_n(data);
  data = NULL;

  /* remote address */
  ret = ezcfg_util_snprintf_ns_name(name, sizeof(name), ns, NVRAM_NAME(SOCKET, REMOTE_ADDRESS));
  if (ret != EZCFG_RET_OK) {
    goto exit_fail;
  }
  data = ezcfg_nv_pair_new(name, NULL);
  if (data == NULL) {
    goto exit_fail;
  }
  if (ezcfg_linked_list_append(list, data) != EZCFG_RET_OK) {
    goto exit_fail;
  }
  praddr = ezcfg_nv_pair_get_n(data);
  data = NULL;

  /* get these value from nvram */
  ret = ezcfg_common_get_nvram_entries(ezcfg, list);
  if (ret != EZCFG_RET_OK) {
    goto exit_fail;
  }

  /* parse settings */
  list_length = ezcfg_linked_list_get_length(list);
  for (i = 1; i < list_length+1; i++) {
    data = (struct ezcfg_nv_pair *)ezcfg_linked_list_get_node_data_by_index(list, i);
    if (data == NULL) {
      goto exit_fail;
    }
    if (ezcfg_nv_pair_get_n(data) == pdomain) {
      val = ezcfg_nv_pair_get_v(data);
      if (val == NULL) {
        goto exit_fail;
      }
      domain = atoi(val);
    }
    else if (ezcfg_nv_pair_get_n(data) == ptype) {
      val = ezcfg_nv_pair_get_v(data);
      if (val == NULL) {
        goto exit_fail;
      }
      type = atoi(val);
    }
    else if (ezcfg_nv_pair_get_n(data) == pproto) {
      val = ezcfg_nv_pair_get_v(data);
      if (val == NULL) {
        goto exit_fail;
      }
      proto = atoi(val);
    }
    else if (ezcfg_nv_pair_get_n(data) == pladdr) {
      val = ezcfg_nv_pair_get_v(data);
      if (*val != '\0') {
        laddr = strdup(val);
        if (laddr == NULL) {
          data = NULL;
          goto exit_fail;
        }
      }
    }
    else if (ezcfg_nv_pair_get_n(data) == praddr) {
      val = ezcfg_nv_pair_get_v(data);
      if (*val != '\0') {
        raddr = strdup(val);
        if (raddr == NULL) {
          data = NULL;
          goto exit_fail;
        }
      }
    }
  }
  data = NULL;

  /* cleanup list */
  ezcfg_linked_list_del(list);
  list = NULL;

  sp = create_socket(ezcfg, domain, type, proto, laddr, raddr, true);
  free(laddr);
  laddr = NULL;
  free(raddr);
  raddr = NULL;
  if (sp == NULL) {
    /* decrease ezcfg library context reference */
    if (ezcfg_dec_ref(ezcfg) != EZCFG_RET_OK) {
      EZDBG("%s(%d) <6>pid=[%d] ezcfg_dec_ref() failed\n", __func__, __LINE__, getpid());
    }
  }
  return sp;

exit_fail:
  EZDBG("%s(%d)\n", __func__, __LINE__);
  if (laddr) {
    free(laddr);
  }
  if (raddr) {
    free(raddr);
  }
  if (data) {
    ezcfg_nv_pair_del(data);
  }
  if (list) {
    ezcfg_linked_list_del(list);
  }
  /* decrease ezcfg library context reference */
  if (ezcfg_dec_ref(ezcfg) != EZCFG_RET_OK) {
    EZDBG("%s(%d) <6>pid=[%d] ezcfg_dec_ref() failed\n", __func__, __LINE__, getpid());
  }
  return NULL;
}

/**
 * ezcfg_socket_fake_new:
 * Create ezcfg unified socket.
 * Returns: a new ezcfg socket
 **/
struct ezcfg_socket *ezcfg_socket_fake_new(struct ezcfg *ezcfg,
   const int domain, const int type, const int proto,
   const char *laddr, const char *raddr)
{
  return create_socket(ezcfg, domain, type, proto, laddr, raddr, false);
}

struct ezcfg *ezcfg_socket_get_ezcfg(const struct ezcfg_socket *sp)
{
  ASSERT(sp != NULL);
  return sp->ezcfg;
}

bool ezcfg_socket_set_sock(struct ezcfg_socket *sp, const int sock)
{
  ASSERT(sp != NULL);
  sp->sock = sock;
  return true;
}

int ezcfg_socket_get_sock(const struct ezcfg_socket *sp)
{
  ASSERT(sp != NULL);
  return sp->sock;
}

bool ezcfg_socket_set_proto(struct ezcfg_socket *sp, const int proto)
{
  ASSERT(sp != NULL);
  sp->proto = proto;
  return true;
}

int ezcfg_socket_get_proto(const struct ezcfg_socket *sp)
{
  ASSERT(sp != NULL);
  return sp->proto;
}

bool ezcfg_socket_set_domain(struct ezcfg_socket *sp, const int domain)
{
  ASSERT(sp != NULL);
  sp->domain = domain;
  return true;
}

int ezcfg_socket_get_domain(const struct ezcfg_socket *sp)
{
  ASSERT(sp != NULL);
  return sp->domain;
}

bool ezcfg_socket_set_type(struct ezcfg_socket *sp, const int type)
{
  ASSERT(sp != NULL);
  sp->type = type;
  return true;
}

int ezcfg_socket_get_type(const struct ezcfg_socket *sp)
{
  ASSERT(sp != NULL);
  return sp->type;
}

bool ezcfg_socket_set_buffer(struct ezcfg_socket *sp, char *buf, int buf_len)
{
  char *p;

  ASSERT(sp != NULL);
  ASSERT(buf != NULL);
  ASSERT(buf_len >= 0);

  p = malloc(buf_len);
  if (p == NULL) {
    return false;
  }

  memcpy(p, buf, buf_len);

  if (sp->buffer != NULL) {
    free(sp->buffer);
  }
  sp->buffer = p;
  sp->buffer_len = buf_len;

  return true;
}

char *ezcfg_socket_get_buffer(const struct ezcfg_socket *sp)
{
  ASSERT(sp != NULL);
  return sp->buffer;
}

int ezcfg_socket_get_buffer_len(const struct ezcfg_socket *sp)
{
  ASSERT(sp != NULL);
  return sp->buffer_len;
}

struct ezcfg_socket *ezcfg_socket_get_next(const struct ezcfg_socket *sp)
{
  ASSERT(sp != NULL);
  return sp->next;
}

int ezcfg_socket_get_local_socket_len(struct ezcfg_socket *sp)
{
  ASSERT(sp != NULL);
  return sp->lsa.len;
}

char *ezcfg_socket_get_local_socket_path(struct ezcfg_socket *sp, char *addr, int size)
{
  ASSERT(sp != NULL);
  ASSERT(addr != NULL);
  ASSERT(size > 0);
  if (snprintf(addr, size, "%s", sp->lsa.u.sun.sun_path) < size)
    return addr;
  else
    return NULL;
}

char *ezcfg_socket_get_local_socket_ip(struct ezcfg_socket *sp)
{
  ASSERT(sp != NULL);
  return inet_ntoa(sp->lsa.u.sin.sin_addr);
}

char *ezcfg_socket_get_group_interface_ip(struct ezcfg_socket *sp)
{
  ASSERT(sp != NULL);
  return inet_ntoa(sp->mreq.u.group.imr_interface);
}

int ezcfg_socket_get_remote_socket_len(struct ezcfg_socket *sp)
{
  ASSERT(sp != NULL);
  return sp->rsa.len;
}

char *ezcfg_socket_get_remote_socket_path(struct ezcfg_socket *sp, char *addr, int size)
{
  ASSERT(sp != NULL);
  ASSERT(addr != NULL);
  ASSERT(size > 0);
  if (snprintf(addr, size, "%s", sp->rsa.u.sun.sun_path) < size)
    return addr;
  else
    return NULL;
}

char *ezcfg_socket_get_mcast_socket_path(struct ezcfg_socket *sp, char *addr, int size)
{
  ASSERT(sp != NULL);
  ASSERT(addr != NULL);
  ASSERT(size > 0);

  if (sp->domain == AF_INET) {
    if (inet_ntop(sp->domain, &(sp->mreq.u.group.imr_multiaddr),
		  addr, sizeof(struct sockaddr_in)) == NULL)
      return NULL;
    else
      return addr;
  }
#if (HAVE_EZBOX_EZCFG_IPV6 == 1)
  else if (sp->domain == AF_INET6) {
    *addr='[';
    if (inet_ntop(sp->domain, &(sp->mreq.u.groupv6.ipv6mr_multiaddr),
		  addr+1, sizeof(struct sockaddr_in6)) == NULL) {
      return NULL;
    }
    strcat(addr, "]");
    return addr;
  }
#endif
  else {
    return NULL;
  }
}

/**
 * ezcfg_socket_calloc:
 * Allocate ezcfg socket buffer.
 * Returns:
 **/
struct ezcfg_socket *ezcfg_socket_calloc(struct ezcfg *ezcfg, int size)
{
  struct ezcfg_socket *sp;
  int i;

  ASSERT(ezcfg != NULL);
  ASSERT(size > 0);

  sp = calloc(size, sizeof(struct ezcfg_socket));
  if (sp == NULL) {
    err(ezcfg, "calloc fail: %m\n");
    return NULL;
  }
  for (i = 0; i < size; i++) {
    sp[i].sock = -1;
    sp[i].ezcfg = ezcfg;
  }

  return sp;
}

/**
 * ezcfg_socket_list_delete_socket:
 * Delete a special socket from socket list.
 * Returns: new list header store in list.
 * Returns: true if delete sp in list, otherwise false.
 **/
bool ezcfg_socket_list_delete_socket(struct ezcfg_socket **list, struct ezcfg_socket *sp)
{
  struct ezcfg_socket *prev, *cur;

  ASSERT(list != NULL);

  if (sp == NULL) {
    return true;
  }

  cur = *list;
  if (sp == cur) {
    *list = cur->next;
    ezcfg_socket_del(cur);
    return true;
  }

  prev = cur;
  cur = cur->next;
  while (cur != NULL) {
    if (sp == cur) {
      prev->next = cur->next;
      ezcfg_socket_del(cur);
      return true;
    }
    prev = cur;
    cur = cur->next;
  }
  return false;
}

/**
 * ezcfg_socket_list_delete:
 * Delete ezcfg socket list linked with sp.
 * Returns:
 **/
void ezcfg_socket_list_delete(struct ezcfg_socket **list)
{
  struct ezcfg_socket *cur;

  ASSERT(list != NULL);

  cur = *list;
  while (cur != NULL) {
    *list = cur->next;
    ezcfg_socket_del(cur);
    cur = *list;
  }
}

/**
 * ezcfg_socket_list_insert:
 * Add socket to list header.
 * Returns:
 **/
int ezcfg_socket_list_insert(struct ezcfg_socket **list, struct ezcfg_socket *sp)
{
  ASSERT(list != NULL);
  ASSERT(sp != NULL);

  sp->next = *list;
  *list = sp;
  return 0;
}

bool ezcfg_socket_list_in(struct ezcfg_socket **list, struct ezcfg_socket *sp)
{
  struct ezcfg *ezcfg;
  struct ezcfg_socket *cur;

  ASSERT(list != NULL);
  ASSERT(sp != NULL);

  ezcfg = sp->ezcfg;

  cur = *list;
  while (cur != NULL) {
    if (ezcfg_socket_compare(sp, cur) == true) {
      info(ezcfg, "find match socket\n");
      return true;
    }
    cur = cur->next;
  }
  return false;
}

struct ezcfg_socket * ezcfg_socket_list_next(struct ezcfg_socket **list)
{
  ASSERT(list != NULL);

  return (*list)->next;
}

bool ezcfg_socket_list_set_need_delete(struct ezcfg_socket **list, struct ezcfg_socket *sp, bool need_delete)
{
  struct ezcfg *ezcfg;
  struct ezcfg_socket *cur;
  bool ret = false;

  ASSERT(list != NULL);
  ASSERT(sp != NULL);

  ezcfg = sp->ezcfg;

  cur = *list;
  while (cur != NULL) {
    if (ezcfg_socket_compare(sp, cur) == true) {
      info(ezcfg, "find match socket\n");
      cur->need_delete = need_delete;
      ret = true;
    }
    cur = cur->next;
  }
  return ret;
}

/**
 * ezcfg_socket_binding:
 * @sp: the listening socket which should receive events
 * Binds the @sp to the event source.
 * Returns: 0 on success, otherwise a negative error value.
 */
int ezcfg_socket_binding(struct ezcfg_socket *sp)
{
  int err = 0;
  struct usa *usa = NULL;
  struct ezcfg *ezcfg;

  ASSERT(sp != NULL);

  ezcfg = sp->ezcfg;
  usa = &(sp->lsa);

  switch(sp->domain) {
  case AF_LOCAL:
    err = bind(sp->sock,
	       (struct sockaddr *)&usa->u.sun, usa->len);
    break;

  case AF_INET:
    err = bind(sp->sock,
	       (struct sockaddr *)&usa->u.sin, usa->len);
    break;

#if (HAVE_EZBOX_EZCFG_IPV6 == 1)
  case AF_INET6:
    err = bind(sp->sock,
	       (struct sockaddr *)&usa->u.sin6, usa->len);
    break;
#endif

  case AF_NETLINK:
    err = bind(sp->sock,
	       (struct sockaddr *)&usa->u.snl, usa->len);
    break;

  default:
    err(ezcfg, "unknown family [%d]\n", sp->domain);
    return -EINVAL;
  }

  if (err < 0) {
    return err;
  }

  return 0;
}

/**
 * ezcfg_socket_enable_receiving:
 * @sp: the listening socket which should receive events
 * Binds the @sp to the event source.
 * Returns: 0 on success, otherwise a negative error value.
 */
int ezcfg_socket_enable_receiving(struct ezcfg_socket *sp)
{
  int err = 0;
  struct usa *usa = NULL;
  struct ezcfg *ezcfg;

  ASSERT(sp != NULL);

  ezcfg = sp->ezcfg;
  usa = &(sp->lsa);

  switch(sp->domain) {
  case AF_LOCAL:
    err = bind(sp->sock,
	       (struct sockaddr *)&usa->u.sun, usa->len);
    break;

  case AF_INET:
    if (sp->proto == EZCFG_PROTO_UPNP_SSDP) {
      usa->u.sin.sin_addr.s_addr = INADDR_ANY;
    }

    err = bind(sp->sock,
	       (struct sockaddr *)&usa->u.sin, usa->len);
    if (err < 0) {
      break;
    }

    if (sp->proto == EZCFG_PROTO_UPNP_SSDP) {
      err = setsockopt(sp->sock,
		       IPPROTO_IP, IP_ADD_MEMBERSHIP,
		       (char *)&(sp->mreq.u.group), sizeof(sp->mreq.u.group));
    }

    break;

  case AF_NETLINK:
    err = bind(sp->sock,
	       (struct sockaddr *)&usa->u.snl, usa->len);
    break;

  default:
    err(ezcfg, "unknown family [%d]\n", sp->domain);
    return -EINVAL;
  }

  if (err < 0) {
    return err;
  }

  /* enable receiving of sender credentials */
  //setsockopt(sp->sock, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on));
  return 0;
}

/**
 * ezcfg_socket_enable_listening:
 * @sp: the listening socket which start receiving events
 * Makes the @sp listening to the event source.
 * Returns: 0 on success, otherwise a negative error value.
 */
int ezcfg_socket_enable_listening(struct ezcfg_socket *sp, int backlog)
{
  int err = 0;
  struct ezcfg *ezcfg;

  ASSERT(sp != NULL);
  ASSERT(backlog > 0);

  ezcfg = sp->ezcfg;
  sp->backlog = backlog;

  switch(sp->domain) {
  case AF_LOCAL:
  case AF_INET:
#if (HAVE_EZBOX_EZCFG_IPV6 == 1)
  case AF_INET6:
#endif
    if (sp->type == SOCK_STREAM) {
      err = listen(sp->sock, backlog);
    }
    else if (sp->type == SOCK_DGRAM) {
      info(ezcfg, "SOCK_DGRAM not need listen\n");
    }
    break;

  case AF_NETLINK:
    if (sp->proto == EZCFG_PROTO_UEVENT) {
      info(ezcfg, "UEVENT not need listen\n");
    }
    break;

  default:
    err(ezcfg, "unknown family [%d]\n", sp->domain);
    return -EINVAL;
  }

  if (err < 0) {
    err(ezcfg, "enable listening error.\n");
    return err;
  }

  /* enable receiving of sender credentials */
  //setsockopt(sp->sock, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on));
  return 0;
}

/**
 * ezcfg_socket_enable_again:
 * @sp: the listening socket which will re-enable
 * Makes the @sp listening to the event source.
 * Returns: 0 on success, otherwise a negative error value.
 */
int ezcfg_socket_enable_again(struct ezcfg_socket *sp)
{
  int err = 0;
  //struct ezcfg *ezcfg;
  int sock_protocol = -1;

  ASSERT(sp != NULL);

  //ezcfg = sp->ezcfg;

  ezcfg_socket_close_sock(sp);

  /* FIXME: should change sock_protocol w/r sp->proto */
  if (sp->proto == EZCFG_PROTO_UEVENT) {
    sock_protocol = NETLINK_KOBJECT_UEVENT;
  }
  else {
    sock_protocol = 0;
  }

  sp->sock = socket(sp->domain, sp->type, sock_protocol);
  if (sp->sock < 0) {
    return -1;
  }

  err = ezcfg_socket_enable_receiving(sp);
  if (err < 0) {
    return err;
  }

  if (sp->backlog > 0) {
    err = ezcfg_socket_enable_listening(sp, sp->backlog);
    if (err < 0) {
      return err;
    }
  }

  ezcfg_socket_set_close_on_exec(sp);

  return 0;
}

/**
 * ezcfg_socket_enable_sending:
 * @sp: the socket which should send packets
 * Returns: 0 on success, otherwise a negative error value.
 */
int ezcfg_socket_enable_sending(struct ezcfg_socket *sp)
{
  int err = 0;
  struct usa *usa = NULL;
  struct ezcfg *ezcfg;

  ASSERT(sp != NULL);

  ezcfg = sp->ezcfg;
  usa = &(sp->rsa);

  switch(sp->domain) {
  case AF_LOCAL:
    break;

  case AF_INET:
    if (sp->proto == EZCFG_PROTO_UPNP_SSDP) {
      char loopch = 0;

      /* Disable loopback so you do not receive your own datagrams. */
      err = setsockopt(sp->sock, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch));
      if (err < 0) {
	break;
      }

      /* Set local interface for outbound multicast datagrams. */
      /* The IP address specified must be associated with a local, */
      /* multicast capable interface. */
      err = setsockopt(sp->sock,
		       IPPROTO_IP, IP_MULTICAST_IF,
		       (char *)&(sp->mreq.u.group.imr_interface), sizeof(sp->mreq.u.group.imr_interface));
      if (err < 0) {
	break;
      }

      err = connect(sp->sock, (struct sockaddr *)&(usa->u.sin), usa->len);
    }
    break;

#if (HAVE_EZBOX_EZCFG_IPV6 == 1)
  case AF_INET6:
    if (sp->proto == EZCFG_PROTO_UPNP_SSDP) {
      char loopch = 0;

      /* Disable loopback so you do not receive your own datagrams. */
      err = setsockopt(sp->sock, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch));
      if (err < 0) {
	break;
      }

      /* Set local interface for outbound multicast datagrams. */
      /* The IP address specified must be associated with a local, */
      /* multicast capable interface. */
      err = setsockopt(sp->sock,
		       IPPROTO_IPV6, IPV6_MULTICAST_IF,
		       (char *)&(sp->mreq.u.group.imr_interface), sizeof(sp->mreq.u.group.imr_interface));
      if (err < 0) {
	break;
      }

      err = connect(sp->sock, (struct sockaddr *)&(usa->u.sin6), usa->len);
    }
    break;
#endif

  case AF_NETLINK:
    break;

  default:
    err(ezcfg, "unknown family [%d]\n", sp->domain);
    return -EINVAL;
  }

  if (err < 0) {
    return err;
  }

  return 0;
}

bool ezcfg_socket_compare(struct ezcfg_socket *sp1, struct ezcfg_socket *sp2)
{
  struct usa *usa1 = NULL, *usa2 = NULL;

  ASSERT(sp1 != NULL);
  ASSERT(sp2 != NULL);

  usa1 = &(sp1->lsa);
  usa2 = &(sp2->lsa);
  if ((sp1->proto == sp2->proto) &&
      (sp1->domain == sp2->domain) &&
      (sp1->type == sp2->type) &&
      (usa1->len == usa2->len)) {
    if (memcmp(&(usa1->u.sa), &(usa2->u.sa), usa1->len) == 0) {
      return true;
    }
  }
  return false;
}

/**
 * ezcfg_socket_set_receive_buffer_size:
 * @sp: the socket which should receive events
 * @size: the size in bytes
 * Set the size of the kernel socket buffer. This call needs the
 * appropriate privileges to succeed.
 * Returns: 0 on success, otherwise -1 on error.
 */
int ezcfg_socket_set_receive_buffer_size(struct ezcfg_socket *sp, int size)
{
  if (sp == NULL)
    return -1;
  if (setsockopt(sp->sock, SOL_SOCKET, SO_RCVBUFFORCE, &size, sizeof(size)) < 0)
    return -1;
  return 0;
}

int ezcfg_socket_queue_get_socket(const struct ezcfg_socket *queue, int pos, struct ezcfg_socket *sp)
{
  *sp = queue[pos];
  return 0;
}

int ezcfg_socket_queue_set_socket(struct ezcfg_socket *queue, int pos, const struct ezcfg_socket *sp)
{
  queue[pos] = *sp;
  return 0;
}

static struct ezcfg_socket *new_accepted_socket_stream(const struct ezcfg_socket *listener)
{
  struct ezcfg_socket *accepted;
  struct ezcfg *ezcfg;
  int domain;

  ezcfg = listener->ezcfg;

  accepted = calloc(1, sizeof(struct ezcfg_socket));
  if (accepted == NULL) {
    err(ezcfg, "calloc fail: %m\n");
    return NULL;
  }
  memset(accepted, 0, sizeof(struct ezcfg_socket));
  accepted->ezcfg = ezcfg;
  accepted->sock = -1;
  accepted->proto = listener->proto;
  accepted->lsa = listener->lsa;
  accepted->rsa = listener->rsa;
  accepted->mreq = listener->mreq;
  accepted->need_unlink = listener->need_unlink;
  domain = listener->domain;
  accepted->domain = domain;
  accepted->type = listener->type;
  accepted->buffer = NULL;

  switch(domain) {
  case AF_LOCAL:
    accepted->rsa.len = sizeof(accepted->rsa.u.sun);
    accepted->sock = accept(listener->sock,
			    &(accepted->rsa.u.sa),
			    &(accepted->rsa.len));
    if (accepted->sock == -1) {
      free(accepted);
      return NULL;
    }
    break;

  case AF_INET:
    accepted->rsa.len = sizeof(accepted->rsa.u.sin);
    accepted->sock = accept(listener->sock,
			    &(accepted->rsa.u.sa),
			    &(accepted->rsa.len));
    if (accepted->sock == -1) {
      free(accepted);
      return NULL;
    }
    break;

#if (HAVE_EZBOX_EZCFG_IPV6 == 1)
  case AF_INET6:
    accepted->rsa.len = sizeof(accepted->rsa.u.sin6);
    accepted->sock = accept(listener->sock,
			    &(accepted->rsa.u.sa),
			    &(accepted->rsa.len));
    if (accepted->sock == -1) {
      free(accepted);
      return NULL;
    }
    break;
#endif

#if 0
  case AF_NETLINK:
    accepted->rsa.len = sizeof(accepted->rsa.u.snl);
    accepted->sock = accept(listener->sock,
			    &(accepted->rsa.u.sa),
			    &(accepted->rsa.len));
    if (accepted->sock == -1) {
      free(accepted);
      return NULL;
    }
    break;
#endif

  default:
    err(ezcfg, "unknown socket family [%d]\n", domain);
    free(accepted);
    return NULL;
  }

  return accepted;
}

static struct ezcfg_socket *new_accepted_socket_datagram(const struct ezcfg_socket *listener)
{
  struct ezcfg_socket *accepted;
  struct ezcfg *ezcfg;
  int domain, type, sock_protocol;

  ezcfg = listener->ezcfg;

  accepted = calloc(1, sizeof(struct ezcfg_socket));
  if (accepted == NULL) {
    err(ezcfg, "calloc fail: %m\n");
    return NULL;
  }
  memset(accepted, 0, sizeof(struct ezcfg_socket));
  accepted->ezcfg = ezcfg;
  accepted->sock = -1;
  accepted->proto = listener->proto;
  accepted->lsa = listener->lsa;
  accepted->rsa = listener->rsa;
  accepted->mreq = listener->mreq;
  accepted->need_unlink = listener->need_unlink;
  domain = listener->domain;
  type = listener->type;
  accepted->domain = domain;
  accepted->type = type;
  accepted->buffer = NULL;

  /* FIXME: should change sock_protocol w/r proto */
  if (accepted->proto == EZCFG_PROTO_UEVENT) {
    sock_protocol = NETLINK_KOBJECT_UEVENT;
  }
  else {
    sock_protocol = 0;
  }

  switch(domain) {
  case AF_INET:
#if (HAVE_EZBOX_EZCFG_IPV6 == 1)
  case AF_INET6:
#endif
    accepted->sock = socket(domain, type, sock_protocol);
    if (accepted->sock == -1) {
      free(accepted);
      return NULL;
    }
    break;

  default:
    info(ezcfg, "not handling socket family [%d]\n", domain);
    break;
  }

  return accepted;
}

struct ezcfg_socket *ezcfg_socket_new_accepted_socket(const struct ezcfg_socket *listener)
{
  //struct ezcfg *ezcfg;

  ASSERT(listener != NULL);
  //ezcfg = listener->ezcfg;

  switch (listener->proto) {
  case EZCFG_PROTO_UEVENT :
    return new_accepted_socket_datagram(listener);
    break;
  case EZCFG_PROTO_UPNP_SSDP :
    return new_accepted_socket_datagram(listener);
    break;
  default :
    return new_accepted_socket_stream(listener);
    break;
  }
}

void ezcfg_socket_close_sock(struct ezcfg_socket *sp)
{
  //struct ezcfg *ezcfg;

  ASSERT(sp != NULL);

  //ezcfg = sp->ezcfg;

  if (sp->sock >= 0) {
    close_socket_gracefully(sp->sock);
    sp->sock = -1;
  }
}

void ezcfg_socket_set_close_on_exec(struct ezcfg_socket *sp)
{
  ASSERT(sp != NULL);

  if (sp->sock >= 0) {
    fcntl(sp->sock, F_SETFD, FD_CLOEXEC);
  }
}

void ezcfg_socket_set_need_unlink(struct ezcfg_socket *sp, bool need_unlink)
{
  ASSERT(sp != NULL);

  sp->need_unlink = need_unlink;
}

void ezcfg_socket_set_need_delete(struct ezcfg_socket *sp, bool need_delete)
{
  ASSERT(sp != NULL);

  sp->need_delete = need_delete;
}

bool ezcfg_socket_get_need_delete(struct ezcfg_socket *sp)
{
  ASSERT(sp != NULL);

  return (sp->need_delete);
}

bool ezcfg_socket_sync_lsa(struct ezcfg_socket *dsp, const struct ezcfg_socket *ssp)
{
  //struct ezcfg *ezcfg;

  ASSERT(dsp != NULL);
  ASSERT(ssp != NULL);
  //ezcfg = dsp->ezcfg;

  if (dsp->proto != ssp->proto) {
    return false;
  }

  dsp->lsa = ssp->lsa;

  return true;
}

bool ezcfg_socket_sync_rsa(struct ezcfg_socket *dsp, const struct ezcfg_socket *ssp)
{
  //struct ezcfg *ezcfg;

  ASSERT(dsp != NULL);
  ASSERT(ssp != NULL);
  //ezcfg = dsp->ezcfg;

  if (dsp->proto != ssp->proto) {
    return false;
  }

  dsp->rsa = ssp->rsa;

  return true;
}

int ezcfg_socket_connect_remote(struct ezcfg_socket *sp)
{
  int err = 0;
  struct usa *usa;
  struct ezcfg *ezcfg;

  ASSERT(sp != NULL);

  ezcfg = sp->ezcfg;
  usa = &(sp->rsa);

  switch (sp->domain) {
  case AF_LOCAL:
    err = connect(sp->sock, (struct sockaddr *)&usa->u.sun, usa->len);
    break;
  default:
    err(ezcfg, "bad family [%d]\n", sp->domain);
    return -EINVAL;
  }

  if (err < 0) {
    return err;
  }

  /* enable receiving of sender credentials */
  //setsockopt(sp->sock, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on));
  return 0;
}

int ezcfg_socket_read(struct ezcfg_socket *sp, void *buf, int len, int flags)
{
  struct ezcfg *ezcfg;
  char * p;
  int status, n;
  int sock;
  struct usa *rsa;

  ASSERT(sp != NULL);
  ASSERT(buf != NULL);
  ASSERT(len >= 0);

  ezcfg = sp->ezcfg;
  p = buf;
  status = 0;
  sock = sp->sock;
  memset(buf, '\0', len);
  rsa = &(sp->rsa);

  while (status == 0) {
    /* handle inet dgram */
    if ((sp->domain == AF_INET) && (sp->type == SOCK_DGRAM)) {
      n = recvfrom(sock, p + status, len - status, flags,
		   (struct sockaddr *)&(rsa->u.sin), &(rsa->len));
    }
#if (HAVE_EZBOX_EZCFG_IPV6 == 1)
    else if ((sp->domain == AF_INET6) && (sp->type == SOCK_DGRAM)) {
      n = recvfrom(sock, p + status, len - status, flags,
		   (struct sockaddr *)&(rsa->u.sin6), &(rsa->len));
    }
#endif
    else {
      n = read(sock, p + status, len - status);
    }

    if (n < 0) {
      if (errno == EPIPE) {
	info(ezcfg, "pipe error: %m\n");
	return -EPIPE;
      }
      else if (errno == EINTR || errno == EAGAIN) {
	info(ezcfg, "interrupted: %m\n");
	continue;
      }
      else {
	err(ezcfg, "read fail: %m\n");
	return -errno;
      }
    }

    if (n == 0) {
      info(ezcfg, "remote end closed connection: %m\n");
      p = buf;
      p[len-1] = '\0';
      break;
    }
    status += n;
  }

  return status;
}

int ezcfg_socket_write(struct ezcfg_socket *sp, const void *buf, int len, int flags)
{
  struct ezcfg *ezcfg;
  const char *p;
  int status, n;
  int sock;
  struct usa *rsa;

  ASSERT(sp != NULL);
  ASSERT(buf != NULL);
  ASSERT(len >= 0);

  ezcfg = sp->ezcfg;
  p = buf;
  status = 0;
  sock = sp->sock;
  rsa = &(sp->rsa);

  while (status != len) {
    /* handle inet dgram */
    if ((sp->domain == AF_INET) && (sp->type == SOCK_DGRAM)) {
      n = sendto(sock, p + status, len - status, flags,
		 (struct sockaddr *)&(rsa->u.sin), rsa->len);
    }
#if (HAVE_EZBOX_EZCFG_IPV6 == 1)
    else if ((sp->domain == AF_INET6) && (sp->type == SOCK_DGRAM)) {
      n = sendto(sock, p + status, len - status, flags,
		 (struct sockaddr *)&(rsa->u.sin6), rsa->len);
    }
#endif
    else {
      n = write(sock, p + status, len - status);
    }
    if (n < 0) {
      if (errno == EPIPE) {
	info(ezcfg, "remote end closed connection: %m\n");
	return -EPIPE;
      }
      else if (errno == EINTR || errno == EAGAIN) {
	info(ezcfg, "interrupted: %m\n");
	continue;
      }
      else {
	err(ezcfg, "write fail: %m\n");
	return -errno;
      }
    }
    status += n;
  }

  return status;
}
