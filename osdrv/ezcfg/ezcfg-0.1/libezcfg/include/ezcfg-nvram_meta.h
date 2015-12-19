/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-nvram_meta.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2014-03-31   0.2       Prepare for new NVRAM model
 * ============================================================================
 */

#ifndef _EZCFG_NVRAM_META_H_
#define _EZCFG_NVRAM_META_H_

/* ezcfg meta nvram name prefix */
#define EZCFG_NVRAM_PREFIX_META                  "meta."

/* ezcfg meta nvram names */
#define EZCFG_NVRAM_META_CONFIG_FILE             "config_file"
#define EZCFG_NVRAM_META_SEM_EZCFG_PATH          "sem_ezcfg_path"
#define EZCFG_NVRAM_META_SHM_EZCFG_PATH          "shm_ezcfg_path"
#define EZCFG_NVRAM_META_SHM_EZCFG_NVRAM_QUEUE_LENGTH \
  "shm_ezcfg_nvram_queue_length"
#define EZCFG_NVRAM_META_SHM_EZCFG_RC_QUEUE_LENGTH \
  "shm_ezcfg_rc_queue_length"
#define EZCFG_NVRAM_META_SOCK_NVRAM_PATH         "sock_nvram_path"
#define EZCFG_NVRAM_META_NVRAM_NODE_LIST_SIZE    "nvram_node_list_size"
#define EZCFG_NVRAM_META_LOG_FILE_PATH           "log_file_path"

#endif
