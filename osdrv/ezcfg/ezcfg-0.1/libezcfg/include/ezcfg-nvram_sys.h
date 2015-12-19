/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-sys.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-17   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_NVRAM_SYS_H_
#define _EZCFG_NVRAM_SYS_H_

/* ezcfg nvram name prefix */
#define EZCFG_NVRAM_PREFIX_SYS                  "sys."

/* ezcfg system names */
#define EZCFG_NVRAM_SYS_DEVICE_NAME             "device_name"
#define EZCFG_NVRAM_SYS_SERIAL_NUMBER           "serial_number"
#define EZCFG_NVRAM_SYS_DISTRO_NAME             "distro_name"
#define EZCFG_NVRAM_SYS_HARDWARE_ARCH           "hardware_arch"
#define EZCFG_NVRAM_SYS_HARDWARE_VERSION        "hardware_version"
#define EZCFG_NVRAM_SYS_SOFTWARE_VERSION        "software_version"
#define EZCFG_NVRAM_SYS_SOFTWARE_REVISION       "software_revision"

#define EZCFG_NVRAM_SYS_HDD_DEVICE              "hdd_device"
#define EZCFG_NVRAM_SYS_BOOT_ARGS               "boot_args"

#define EZCFG_NVRAM_SYS_UPGRADE_IMAGE           "upgrade_image"

#if 0
#define EZCFG_SYS_LANGUAGE                "language"
#define EZCFG_SYS_TZ_AREA                 "tz_area"
#define EZCFG_SYS_TZ_LOCATION             "tz_location"

#define EZCFG_SYS_RESTORE_DEFAULTS        "restore_defaults"

#define EZCFG_SYS_NICS                    "nics"

#define EZCFG_SYS_ETH_LAN_NIC             "eth_lan_nic"
#define EZCFG_SYS_WIFI_LAN_NIC            "wifi_lan_nic"
#define EZCFG_SYS_BR_LAN_NICS             "br_lan_nics"
#define EZCFG_SYS_LAN_NIC                 "lan_nic"
#define EZCFG_SYS_ETH_WAN_NIC             "eth_wan_nic"
#define EZCFG_SYS_ADLS_WAN_NIC            "adsl_wan_nic"
#define EZCFG_SYS_VDLS_WAN_NIC            "vdsl_wan_nic"
#define EZCFG_SYS_WIFI_WAN_NIC            "wifi_wan_nic"
#define EZCFG_SYS_WAN_NIC                 "wan_nic"
#endif

#define EZCFG_NVRAM_SYS_DATA_DEV                "data_dev"
#define EZCFG_NVRAM_SYS_DATA_DEV_KEY            "data_dev_key"
#define EZCFG_NVRAM_SYS_SWAP_DEV                "swap_dev"
#define EZCFG_NVRAM_SYS_MODULES                 "modules"
#define EZCFG_NVRAM_SYS_IPTABLES_MODULES        "iptables_modules"
#define EZCFG_NVRAM_SYS_LD_LIBRARY_PATH         "ld_library_path"

#endif
