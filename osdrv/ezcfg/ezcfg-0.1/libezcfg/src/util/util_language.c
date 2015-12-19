/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_language.c
 *
 * Description  : system language settings
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-22   0.1       Write it from scrach
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
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

struct lc_pair {
	char *lc_name;
	char *lc_desc;
};

struct lc_pair ezcfg_support_langs[] = {
	{ "en_HK", "English (Hong Kong)" },
	{ "zh_CN", "Chinese (China)" },
};

int ezcfg_util_lang_get_length(void)
{
	return ARRAY_SIZE(ezcfg_support_langs);
}

char *ezcfg_util_lang_get_name_by_index(int i)
{
	return ezcfg_support_langs[i].lc_name;
}

char *ezcfg_util_lang_get_desc_by_index(int i)
{
	return ezcfg_support_langs[i].lc_desc;
}

char *ezcfg_util_lang_get_desc_by_name(char *name)
{
	size_t i;
	struct lc_pair *lcp;
	for (i = 0; i < ARRAY_SIZE(ezcfg_support_langs); i++) {
		lcp = &(ezcfg_support_langs[i]);
		if (strcmp(lcp->lc_name, name) == 0)
			return lcp->lc_desc;
	}
	return NULL;
}

