/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : nvram/nvram_validators.c
 *
 * Description  : implement validating Name-Value RAM settings
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-25   0.1       Write it from scratch
 * ============================================================================
 */

#include <string.h>
#include "ezcfg-private.h"
#include "ezcfg.h"

static bool sys_language_validator(const char *value)
{
	return true;
}

static bool ui_tz_area_validator(const char *value)
{
	return true;
}

static bool ui_tz_location_validator(const char *value)
{
	return true;
}

static bool wan_type_validator(const char *value)
{
	if ((strcmp(value, "dhcp") == 0) ||
	    (strcmp(value, "static") == 0) ||
#if (HAVE_EZBOX_WAN_PPPOE == 1)
	    (strcmp(value, "pppoe") == 0) ||
#endif
#if (HAVE_EZBOX_WAN_PPTP == 1)
	    (strcmp(value, "pptp") == 0) ||
#endif
#if (HAVE_EZBOX_WAN_L2TP == 1)
	    (strcmp(value, "l2tp") == 0) ||
#endif
	    (strcmp(value, "disabled") == 0)
	)
		return true;
	else
		return false;
}

ezcfg_nv_validator_t default_nvram_validators[] = {
	/* System configuration */
	{ NVRAM_SERVICE_OPTION(EZCFG, SYS_LANGUAGE), sys_language_validator },

	/* UI configuration */
	{ NVRAM_SERVICE_OPTION(UI, TZ_AREA), ui_tz_area_validator },
	{ NVRAM_SERVICE_OPTION(UI, TZ_LOCATION), ui_tz_location_validator },

	/* WAN TCP/IP parameters */
	{ NVRAM_SERVICE_OPTION(WAN, TYPE), wan_type_validator },
};


bool ezcfg_nvram_validate_value(struct ezcfg *ezcfg, char *name, char *value)
{
	size_t i;
	bool ret;
	ezcfg_nv_validator_t v;

	ASSERT(ezcfg != NULL);
	ASSERT(name != NULL);
	ASSERT(value != NULL);

	/* value is valid by default */
 	ret = true;

	for(i = 0; i < ARRAY_SIZE(default_nvram_validators); i++) {
		v = default_nvram_validators[i];
		if (strcmp(v.name, name) == 0) {
			ret = v.handler(value);
			break;
		}
	}

	return ret;
}

/* Public functions */
int ezcfg_nvram_get_num_default_nvram_validators(void)
{
	return ARRAY_SIZE(default_nvram_validators);
}

