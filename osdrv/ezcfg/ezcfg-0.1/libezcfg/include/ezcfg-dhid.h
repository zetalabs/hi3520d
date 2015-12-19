/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-dhid.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-06-12   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_DHID_H_
#define _EZCFG_DHID_H_

/* ezcfg nvram name prefix */
#define EZCFG_DHID_NVRAM_PREFIX           "dhid."

/* ezcfg dhid config key words */
#define EZCFG_DHID_KEYWORD_HOST_ID       "HostID"
#define EZCFG_DHID_KEYWORD_HOST_PASS     "HostPass"
#define EZCFG_DHID_KEYWORD_AUTH_P        "AuthP"
#define EZCFG_DHID_KEYWORD_AUTH_Q        "AuthQ"
#define EZCFG_DHID_KEYWORD_IS_ADDR       "ISAddr"
#define EZCFG_DHID_KEYWORD_REFRESH       "Refresh"
#define EZCFG_DHID_KEYWORD_ON_CMD        "OnCmd"
#define EZCFG_DHID_KEYWORD_OFF_CMD       "OffCmd"

/* ezcfg dhid auxiliary names */
#define EZCFG_DHID_ENTRY                 "entry"
#define EZCFG_DHID_ENTRY_NUM             "entry_num"
#define EZCFG_DHID_QRC_ENABLE            "qrc_enable"

/* ezcfg dhid control names */
#define EZCFG_DHID_ENTRY_0_HOST_ID       "entry.0.HostID"
#define EZCFG_DHID_ENTRY_0_QRC_ENABLE    "entry.0.qrc_enable"
#define EZCFG_DHID_ENTRY_0_HOST_PASS     "entry.0.HostPass"
#define EZCFG_DHID_ENTRY_0_AUTH_P0       "entry.0.AuthP0"
#define EZCFG_DHID_ENTRY_0_AUTH_P1       "entry.0.AuthP1"
#define EZCFG_DHID_ENTRY_0_AUTH_Q0       "entry.0.AuthQ0"
#define EZCFG_DHID_ENTRY_0_AUTH_Q1       "entry.0.AuthQ1"
#define EZCFG_DHID_ENTRY_0_IS_ADDR0      "entry.0.ISAddr0"
#define EZCFG_DHID_ENTRY_0_IS_ADDR1      "entry.0.ISAddr1"
#define EZCFG_DHID_ENTRY_0_REFRESH       "entry.0.Refresh"
#define EZCFG_DHID_ENTRY_0_ON_CMD        "entry.0.OnCmd"
#define EZCFG_DHID_ENTRY_0_OFF_CMD       "entry.0.OffCmd"

#endif
