/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : nvram/nvram_propagators.c
 *
 * Description  : implement propagating Name-Value pairs
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2014-03-02   0.1       Write it from scratch
 * ============================================================================
 */

#include <string.h>
#include "ezcfg-private.h"
#include "ezcfg.h"

static bool dummy_nv_propagator(const char *name, const char *value, struct ezcfg_socket *sp)
{
	return true;
}

ezcfg_nv_propagator_t default_nvram_propagators[] = {
	/* default propagator */
        { "", NULL, dummy_nv_propagator },
};

bool ezcfg_nvram_propagate_value(struct ezcfg *ezcfg, char *name, char *value)
{
	size_t i;
	bool ret;
	ezcfg_nv_propagator_t p;

	ASSERT(ezcfg != NULL);
	ASSERT(name != NULL);
	ASSERT(value != NULL);

	/* value is propagated by default */
	ret = true;

	for(i = 0; i < ARRAY_SIZE(default_nvram_propagators); i++) {
		p = default_nvram_propagators[i];
		if (strcmp(p.name, name) == 0) {
			ret = p.handler(name, value, p.sp);
		}
	}

	return ret;
}

/* Public functions */
int ezcfg_nvram_get_num_default_nvram_propagators(void)
{
	return ARRAY_SIZE(default_nvram_propagators);
}

