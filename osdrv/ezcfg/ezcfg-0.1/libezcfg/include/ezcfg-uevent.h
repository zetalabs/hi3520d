/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-uevent.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-08-10   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_UEVENT_H_
#define _EZCFG_UEVENT_H_

#include "ezcfg.h"

struct ezcfg_uevent {
	struct ezcfg *ezcfg;
};

enum uevent_netlink_group {
	UEVENT_NLGRP_NONE = 0,
	UEVENT_NLGRP_KERNEL,
};

#endif /* _EZCFG_UEVENT_H_ */
