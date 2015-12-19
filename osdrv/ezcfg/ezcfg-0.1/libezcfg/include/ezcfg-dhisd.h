/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-dhisd.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-10-03   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_DHISD_H_
#define _EZCFG_DHISD_H_

/* ezcfg nvram name prefix */
#define EZCFG_DHISD_NVRAM_PREFIX          "dhisd."

/* ezcfg dhisd config key words */
#define EZCFG_DHISD_KEYWORD_LOG_FILE      "LogFile"
#define EZCFG_DHISD_KEYWORD_PID_FILE      "PidFile"
#define EZCFG_DHISD_KEYWORD_BIND_ADDRESS  "BindAddress"
#define EZCFG_DHISD_KEYWORD_BIND_PORT     "BindPort"
#define EZCFG_DHISD_KEYWORD_DB_FILE       "DBFile"

#define EZCFG_DHISD_KEYWORD_MYSQL_SERVER  "MySQLServer"
#define EZCFG_DHISD_KEYWORD_MYSQL_USER    "MySQLUser"
#define EZCFG_DHISD_KEYWORD_MYSQL_PASSWORD "MySQLPassword"
#define EZCFG_DHISD_KEYWORD_MYSQL_DBASE   "MySQLDBase"

#define EZCFG_DHISD_KEYWORD_HOST_ID       "HostID"
#define EZCFG_DHISD_KEYWORD_HOST_NAME     "HostName"
#define EZCFG_DHISD_KEYWORD_HOST_PASS     "HostPass"
#define EZCFG_DHISD_KEYWORD_AUTH_N        "AuthN"
#define EZCFG_DHISD_KEYWORD_ON_CMD        "OnCmd"
#define EZCFG_DHISD_KEYWORD_OFF_CMD       "OffCmd"

/* ezcfg dhisd config names */
#define EZCFG_DHISD_LOG_FILE              "LogFile"
#define EZCFG_DHISD_PID_FILE              "PidFile"
#define EZCFG_DHISD_BIND_ADDRESS          "BindAddress"
#define EZCFG_DHISD_BIND_PORT             "BindPort"
#define EZCFG_DHISD_DB_FILE               "DBFile"

#define EZCFG_DHISD_MYSQL_SERVER          "MySQLServer"
#define EZCFG_DHISD_MYSQL_USER            "MySQLUser"
#define EZCFG_DHISD_MYSQL_PASSWORD        "MySQLPassword"
#define EZCFG_DHISD_MYSQL_DBASE           "MySQLDBase"

/* ezcfg dhisd auxiliary names */
#define EZCFG_DHISD_DB_ENTRY              "db_entry"
#define EZCFG_DHISD_DB_ENTRY_NUM          "db_entry_num"
#define EZCFG_DHISD_QRC_ENABLE            "qrc_enable"

/* ezcfg dhisd control names */
#define EZCFG_DHISD_DB_ENTRY_0_HOST_ID    "db_entry.0.HostID"
#define EZCFG_DHISD_DB_ENTRY_0_HOST_NAME  "db_entry.0.HostName"
#define EZCFG_DHISD_DB_ENTRY_0_QRC_ENABLE "db_entry.0.qrc_enable"
#define EZCFG_DHISD_DB_ENTRY_0_HOST_PASS  "db_entry.0.HostPass"
#define EZCFG_DHISD_DB_ENTRY_0_AUTH_N0    "db_entry.0.AuthN0"
#define EZCFG_DHISD_DB_ENTRY_0_AUTH_N1    "db_entry.0.AuthN1"
#define EZCFG_DHISD_DB_ENTRY_0_AUTH_N2    "db_entry.0.AuthN2"
#define EZCFG_DHISD_DB_ENTRY_0_AUTH_N3    "db_entry.0.AuthN3"
#define EZCFG_DHISD_DB_ENTRY_0_ON_CMD     "db_entry.0.OnCmd"
#define EZCFG_DHISD_DB_ENTRY_0_OFF_CMD    "db_entry.0.OffCmd"

#endif
