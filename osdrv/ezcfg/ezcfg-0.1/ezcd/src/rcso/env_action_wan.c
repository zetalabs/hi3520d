/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : env_action_wan.c
 *
 * Description  : ezbox env agent runs network WAN config service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-04   0.1       Write it from scratch
 * 2011-10-07   0.2       Modify it to use rcso framework
 * 2012-12-25   0.3       Modify it to use agent action framework
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
#include <net/if.h>

#include "ezcd.h"
#include "pop_func.h"

#if 0
#define DBG(format, args...) do {		\
    FILE *fp = fopen("/dev/kmsg", "a");		\
    if (fp) {					\
      fprintf(fp, format, ## args);		\
      fclose(fp);				\
    }						\
  } while(0)
#else
#define DBG(format, args...)
#endif

static void config_wan_static(void)
{
  char name[32];
  char buf[256];
  int i;
  int rc;

  /* IP address */
  rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, STATIC_IPADDR), buf, sizeof(buf));
  if (rc >= 0) {
    rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, IPADDR), buf);
    if (rc < 0) {
      DBG("ezcfg_api_nvram_set(%s=%s) error.\n", NVRAM_SERVICE_OPTION(WAN, IPADDR), buf);
    }
  }

  /* netmask */
  rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, STATIC_NETMASK), buf, sizeof(buf));
  if (rc >= 0) {
    rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, NETMASK), buf);
    if (rc < 0) {
      DBG("ezcfg_api_nvram_set(%s=%s) error.\n", NVRAM_SERVICE_OPTION(WAN, NETMASK), buf);
    }
  }

  /* gateway IP address */
  rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, STATIC_GATEWAY), buf, sizeof(buf));
  if (rc >= 0) {
    rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, GATEWAY), buf);
    if (rc < 0) {
      DBG("ezcfg_api_nvram_set(%s=%s) error.\n", NVRAM_SERVICE_OPTION(WAN, GATEWAY), buf);
    }
  }

  /* DNS */
  for(i=1; i<=3; i++) {
    snprintf(name, sizeof(name), "%s%d", NVRAM_SERVICE_OPTION(WAN, STATIC_DNS), i);
    rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
    if (rc >= 0) {
      snprintf(name, sizeof(name), "%s%d", NVRAM_SERVICE_OPTION(WAN, DNS), i);
      rc = ezcfg_api_nvram_set(name, buf);
      if (rc < 0) {
	DBG("ezcfg_api_nvram_set(%s=%s) error.\n", name, buf);
      }
    }
  }

  pop_etc_resolv_conf(RC_ACT_START);

  /* start WAN interface binding services */
#if 0
  rc_wan_services(RC_ACT_START);
#endif
}

static void deconfig_wan_static(void)
{
  char name[32];
  int i;
  int rc;

  pop_etc_resolv_conf(RC_ACT_STOP);

  /* IP address */
  rc = ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(WAN, IPADDR));
  if (rc < 0) {
    DBG("ezcfg_api_nvram_unset(%s) error.\n", NVRAM_SERVICE_OPTION(WAN, IPADDR));
  }

  /* netmask */
  rc = ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(WAN, NETMASK));
  if (rc < 0) {
    DBG("ezcfg_api_nvram_unset(%s) error.\n", NVRAM_SERVICE_OPTION(WAN, NETMASK));
  }

  /* gateway IP address */
  rc = ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(WAN, GATEWAY));
  if (rc < 0) {
    DBG("ezcfg_api_nvram_unset(%s) error.\n", NVRAM_SERVICE_OPTION(WAN, GATEWAY));
  }

  /* DNS */
  for(i=1; i<=3; i++) {
    snprintf(name, sizeof(name), "%s%d", NVRAM_SERVICE_OPTION(WAN, DNS), i);
    rc = ezcfg_api_nvram_unset(name);
    if (rc < 0) {
      DBG("ezcfg_api_nvram_unset(%s) error.\n", name);
    }
  }
}

static int start_wan(void)
{
  char wan_ifname[IFNAMSIZ];
  char buf[256];
  int wan_type;
  int rc;

  wan_type = utils_get_wan_type();
  if (wan_type == WAN_TYPE_UNKNOWN)
    return (EXIT_FAILURE);

  switch (wan_type) {
  case WAN_TYPE_DHCP :
    rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, DHCP_IFNAME), wan_ifname, sizeof(wan_ifname));
    if (rc <= 0)
      return (EXIT_FAILURE);

    /* start DHCP client process */
    snprintf(buf, sizeof(buf), "%s %s &", CMD_IFUP, wan_ifname);
    utils_system(buf);

    break;

  case WAN_TYPE_STATIC :
    rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, STATIC_IFNAME), wan_ifname, sizeof(wan_ifname));
    if (rc <= 0)
      return (EXIT_FAILURE);

    /* config Static IP address */
    snprintf(buf, sizeof(buf), "%s %s", CMD_IFUP, wan_ifname);
    utils_system(buf);

    config_wan_static();

    break;

  case WAN_TYPE_PPPOE :
    rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, PPPOE_DEVICE_NAME), wan_ifname, sizeof(wan_ifname));
    if (rc <= 0)
      return (EXIT_FAILURE);

    /* config PPPoE device interface */
    snprintf(buf, sizeof(buf), "%s link set %s up", CMD_IP, wan_ifname);
    utils_system(buf);

    /* start PPP daemon process */
    /* FIXME: do it in action file */
#if 0
    snprintf(buf, sizeof(buf), "%s %s wan start", CMD_RC, "ppp_rp_pppoe");
    utils_system(buf);
#endif

    break;
  }

  return (EXIT_SUCCESS);
}

static int stop_wan(void)
{
  char wan_ifname[IFNAMSIZ];
  char buf[256];
  int wan_type;
  int rc;

  wan_type = utils_get_wan_type();
  if (wan_type == WAN_TYPE_UNKNOWN)
    return (EXIT_FAILURE);

  /* stop WAN interface binding services */
#if 0
  rc_wan_services(RC_ACT_STOP);
#endif

  switch (wan_type) {
  case WAN_TYPE_DHCP :
    rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, DHCP_IFNAME), wan_ifname, sizeof(wan_ifname));
    if (rc <= 0)
      return (EXIT_FAILURE);

    /* stop DHCP client process */
    snprintf(buf, sizeof(buf), "%s %s", CMD_IFDOWN, wan_ifname);
    utils_system(buf);

    break;

  case WAN_TYPE_STATIC :
    rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, STATIC_IFNAME), wan_ifname, sizeof(wan_ifname));
    if (rc <= 0)
      return (EXIT_FAILURE);

    deconfig_wan_static();

    /* deconfig Static IP address */
    snprintf(buf, sizeof(buf), "%s %s", CMD_IFDOWN, wan_ifname);
    utils_system(buf);

    break;

  case WAN_TYPE_PPPOE :
    rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, PPPOE_DEVICE_NAME), wan_ifname, sizeof(wan_ifname));
    if (rc <= 0)
      return (EXIT_FAILURE);

    /* deconfig PPPoE device interface */
    snprintf(buf, sizeof(buf), "%s link set %s down", CMD_IP, wan_ifname);
    utils_system(buf);

    break;

  }

  return (EXIT_SUCCESS);
}

#ifdef _EXEC_
int main(int argc, char **argv)
#else
  int env_action_wan(int argc, char **argv)
#endif
{
  int flag, ret;

  if (argc < 2) {
    return (EXIT_FAILURE);
  }

  if (strcmp(argv[0], "wan")) {
    return (EXIT_FAILURE);
  }

  if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
    return (EXIT_FAILURE);
  }

  flag = utils_get_rc_act_type(argv[1]);

  switch (flag) {
  case RC_ACT_RESTART :
  case RC_ACT_STOP :
    /* bring down WAN interface and deconfig it */
    ret = stop_wan();
    if (flag == RC_ACT_STOP) {
      break;
    }

    if (ret != EXIT_SUCCESS)
      return ret;

    /* RC_ACT_RESTART fall through */
  case RC_ACT_BOOT :
  case RC_ACT_START :
    /* bring up WAN interface and config it */
    ret = start_wan();
    break;

  default :
    ret = EXIT_FAILURE;
    break;
  }

  return ret;
}
