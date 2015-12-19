/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_tzdata.c
 *
 * Description  : system timezone data settings
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

struct tz_pair {
	char *tz_name;
	char *tz_desc;
};

struct tz_triple {
	char *tz_name;
	char *tz_desc;
	struct tz_pair *tz_list;
};

static struct tz_pair ezcfg_africa_locations[] = {
	{ "Algeria", "Algeria" },
	{ NULL, NULL },
};

static struct tz_pair ezcfg_america_locations[] = {
	{ "Anguilla", "Anguilla" },
	{ NULL, NULL },
};

static struct tz_pair ezcfg_antarctica_locations[] = {
	{ "McMurdo", "McMurdo Station, Ross Island" },
	{ NULL, NULL },
};

static struct tz_pair ezcfg_arctic_locations[] = {
	{ "Longyearbyen", "Svalbard & Jan Mayen" },
	{ NULL, NULL },
};

static struct tz_pair ezcfg_asia_locations[] = {
	{ "Shanghai", "east China - Beijing, Guangdong, Shanghai, etc." },
	{ "Hong_Kong", "Hong Kong" },
	{ NULL, NULL },
};

static struct tz_pair ezcfg_atlantic_locations[] = {
	{ "Bermuda", "Bermuda" },
	{ NULL, NULL },
};

static struct tz_pair ezcfg_australia_locations[] = {
	{ "Lord_Howe", "Lord Howe Island" },
	{ NULL, NULL },
};

static struct tz_pair ezcfg_europe_locations[] = {
	{ "Mariehamn", "Aaland Islands" },
	{ NULL, NULL },
};

static struct tz_pair ezcfg_indian_locations[] = {
	{ "Chagos", "British Indian Ocean Territory" },
	{ NULL, NULL },
};

static struct tz_pair ezcfg_pacific_locations[] = {
	{ "Chatham", "Chatham Islands" },
	{ NULL, NULL },
};

static struct tz_pair ezcfg_none_locations[] = {
	{ "GST-10", "GST-10" },
	{ NULL, NULL },
};

static struct tz_triple ezcfg_support_areas[] = {
	{ "Africa", "Africa", ezcfg_africa_locations },
	{ "America", "America", ezcfg_america_locations },
	{ "Antarctica", "Antarctica", ezcfg_antarctica_locations },
	{ "Arctic", "Arctic Ocean", ezcfg_arctic_locations },
	{ "Asia", "Asia", ezcfg_asia_locations },
	{ "Atlantic", "Atlantic Ocean", ezcfg_atlantic_locations },
	{ "Australia", "Australia", ezcfg_australia_locations },
	{ "Europe", "Europe", ezcfg_europe_locations },
	{ "Indian", "Indian Ocean", ezcfg_indian_locations },
	{ "Pacific", "Pacific Ocean", ezcfg_pacific_locations },
	{ "none", "Posix TZ format", ezcfg_none_locations },
};

int ezcfg_util_tzdata_get_area_length(void)
{
	return ARRAY_SIZE(ezcfg_support_areas);
}

char *ezcfg_util_tzdata_get_area_name_by_index(int i)
{
	return ezcfg_support_areas[i].tz_name;
}

char *ezcfg_util_tzdata_get_area_desc_by_index(int i)
{
	return ezcfg_support_areas[i].tz_desc;
}

char *ezcfg_util_tzdata_get_area_desc_by_name(char *name)
{
	size_t i;
	for (i = 0; i < ARRAY_SIZE(ezcfg_support_areas); i++) {
		if (strcmp(ezcfg_support_areas[i].tz_name, name) == 0)
			return ezcfg_support_areas[i].tz_desc;
	}
	return NULL;
}

int ezcfg_util_tzdata_get_location_length(char *area)
{
	size_t i, j;
	struct tz_pair *tzp;
	for (i = 0; i < ARRAY_SIZE(ezcfg_support_areas); i++) {
		if (strcmp(ezcfg_support_areas[i].tz_name, area) == 0) {
			j = 0;
			tzp = ezcfg_support_areas[i].tz_list;
			while (tzp->tz_name != NULL) {
				tzp++;
				j++;
			}
			return j;
		}
	}
	return -1;
}

char *ezcfg_util_tzdata_get_location_name_by_index(char *area, int i)
{
	size_t j;
	struct tz_pair *tzp;
	for (j = 0; j < ARRAY_SIZE(ezcfg_support_areas); j++) {
		if (strcmp(ezcfg_support_areas[j].tz_name, area) == 0) {
			tzp = ezcfg_support_areas[j].tz_list;
			return tzp[i].tz_name;
		}
	}
	return NULL;
}

char *ezcfg_util_tzdata_get_location_desc_by_index(char *area, int i)
{
	size_t j;
	struct tz_pair *tzp;
	for (j = 0; j < ARRAY_SIZE(ezcfg_support_areas); j++) {
		if (strcmp(ezcfg_support_areas[j].tz_name, area) == 0) {
			tzp = ezcfg_support_areas[j].tz_list;
			return tzp[i].tz_desc;
		}
	}
	return NULL;
}

char *ezcfg_util_tzdata_get_location_desc_by_name(char *area, char *name)
{
	size_t j;
	struct tz_pair *tzp;
	for (j = 0; j < ARRAY_SIZE(ezcfg_support_areas); j++) {
		if (strcmp(ezcfg_support_areas[j].tz_name, area) == 0) {
			tzp = ezcfg_support_areas[j].tz_list;
			while(tzp->tz_name != NULL) {
				if (strcmp(tzp->tz_name, name) == 0) {
					return tzp->tz_desc;
				}
				tzp++;
			}
		}
	}
	return NULL;
}

bool ezcfg_util_tzdata_check_area_location(char *area, char *location)
{
	return (ezcfg_util_tzdata_get_location_desc_by_name(area, location) != NULL);
}

