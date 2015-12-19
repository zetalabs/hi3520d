/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : uuid/uuid.c
 *
 * Description  : implement UUID functions (RFC 4122)
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-08-06   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

/* set the following to the number of 100ns ticks of the actual
   resolution of your system's clock, right now we use 1usec = 10*100ns */
#define UUIDS_PER_TICK 10

/* time offset between UUID and Unix Epoch time according to standards.
   (UUID UTC base time is October 15, 1582
    Unix UTC base time is January  1, 1970) */
#define UUID_TIMEOFFSET 0x01B21DD213814000LL

struct ezcfg_uuid {
	struct ezcfg *ezcfg;
	int version; /* UUID version : 1 - 5 */

	unsigned int time_low; /* bits 0-31 of time field */
	unsigned short time_mid; /* bits 32-47 of time field */
	unsigned short time_hi_and_version; /* bits 48-59 of time field plus 4 bit version */
	unsigned char clock_seq_low; /* bits 0-7 of clock sequence field */
	unsigned char clock_seq_hi_and_reserved; /* bits 8-13 of clock sequence field plus 2 bit variant */
	unsigned char node[6]; /* bits 0-47 of node MAC address */

	union {
		unsigned char *mac; /* UUID version 1: time, clock and node based */
		void *md5; /* UUID version 3: name based with MD5 */
		void *prng; /* UUID version 4: random number based */
		void *sha1; /* UUID version 5: name based with SHA-1 */
        } u;

	char *store_name; /* stable store name in ezcfg nvram */
};

/*
 * Private functions
 */

static bool get_generator_state(char uuid_str[EZCFG_UUID_STRING_LEN+1], unsigned long long *ts, unsigned long *clock_seq, unsigned char node[6])
{
	if (uuid_str[0] == '\0')
		return false;

	return true;
}

static bool gen_uuid_v1(struct ezcfg_uuid *uuid)
{
	struct ezcfg *ezcfg;
	struct timeval time_now;
	unsigned long clock_seq;
	unsigned char store_node[6];
	char uuid_str[EZCFG_UUID_STRING_LEN+1];
	unsigned long long ts, store_ts;
	bool is_state_good;

	ezcfg = uuid->ezcfg;

	/* read stable store UUID generator state */
	uuid_str[0] = '\0';
	is_state_good = get_generator_state(uuid_str, &store_ts, &clock_seq, store_node);

	/* Get the current time */
	if (gettimeofday(&time_now, NULL) == -1) {
		err(ezcfg, "can not gettimeofday.\n");
		return false;
	}
	/* convert to 64-bit
	 * Offset between UUID formatted times and Unix formatted times.
	 * UUID UTC base time is October 15, 1582.
	 * Unix base time is January 1, 1970.
	 */
	ts = ( time_now.tv_sec * 10000000 ) + ( time_now.tv_usec * 10 ) + UUID_TIMEOFFSET;

	/* Get the current node ID */
	srandom(time_now.tv_usec);
	ezcfg_uuid_v1_enforce_multicast_mac(uuid);

	/* If the state was unavailable (e.g., non-existent or corrupted),
	 * or the saved node ID is different than the current node ID
	 */
	if (is_state_good == false || memcmp(store_node, uuid->node, 6) != 0) {
		clock_seq = rand();
	}
	/* If the state was available, but the saved timestamp is later than
	 * the current timestamp
	 */
	else if (is_state_good == true && ts < store_ts) {
		clock_seq++;
	}

	/* Format a UUID from the current timestamp, clock sequence, and node
	 * ID values 
	 */
	uuid->time_low = (unsigned long)(ts & 0xFFFFFFFF);
	uuid->time_mid = (unsigned short)((ts >> 32) & 0xFFFF);
	uuid->time_hi_and_version = (unsigned short)((ts >> 48) & 0x0FFF);
	uuid->time_hi_and_version |= (1 << 12);
	uuid->clock_seq_low = clock_seq & 0xFF;
	uuid->clock_seq_hi_and_reserved = (clock_seq & 0x3F00) >> 8;
	uuid->clock_seq_hi_and_reserved |= 0x80;
	memcpy(uuid->node, uuid->u.mac, 6);
 
	/* Save the state */
	ezcfg_uuid_export_str(uuid, uuid_str, sizeof(uuid_str));
	return true;
}

/*
 * Public functions
 */

bool ezcfg_uuid_delete(struct ezcfg_uuid *uuid)
{
	//struct ezcfg *ezcfg;

	ASSERT(uuid != NULL);

	//ezcfg = uuid->ezcfg;

	switch(uuid->version) {
	case 1:
		if (uuid->u.mac != NULL)
			free(uuid->u.mac);
		break;
	case 3:
		if (uuid->u.md5 != NULL)
			free(uuid->u.md5);
		break;
	case 4:
		if (uuid->u.prng != NULL)
			free(uuid->u.prng);
		break;
	case 5:
		if (uuid->u.sha1 != NULL)
			free(uuid->u.sha1);
		break;
	}

	free(uuid);
	return true;
}

struct ezcfg_uuid *ezcfg_uuid_new(struct ezcfg *ezcfg, int version)
{
	struct ezcfg_uuid *uuid;
	time_t t;

	ASSERT(ezcfg != NULL);

	uuid = calloc(1, sizeof(struct ezcfg_uuid));
	if (uuid == NULL) {
		err(ezcfg, "can not calloc uuid\n");
		return NULL;
	}

	memset(uuid, 0, sizeof(struct ezcfg_uuid));
	uuid->ezcfg = ezcfg;
	uuid->version = version;

	switch(uuid->version) {
	case 1:
		uuid->u.mac = calloc(6, sizeof(unsigned char));
		if (uuid->u.mac == NULL) {
			err(ezcfg, "no enough memory for uuid mac.\n");
			goto fail_exit;
		}
		break;
	case 3:
		uuid->u.md5 = calloc(6, sizeof(unsigned char));
		if (uuid->u.md5 == NULL) {
			err(ezcfg, "no enough memory for uuid md5.\n");
			goto fail_exit;
		}
		break;
	case 4:
		uuid->u.prng = calloc(6, sizeof(unsigned char));
		if (uuid->u.prng == NULL) {
			err(ezcfg, "no enough memory for uuid prng.\n");
			goto fail_exit;
		}
		break;
	case 5:
		uuid->u.sha1 = calloc(6, sizeof(unsigned char));
		if (uuid->u.sha1 == NULL) {
			err(ezcfg, "no enough memory for uuid sha1.\n");
			goto fail_exit;
		}
		break;
	default:
		err(ezcfg, "unknown uuid version.\n");
		goto fail_exit;
	}
	/* set random seed */
	srand((unsigned)time(&t));
	return uuid;

fail_exit:
	ezcfg_uuid_delete(uuid);
	return NULL;
}

int ezcfg_uuid_get_version(struct ezcfg_uuid *uuid)
{
	//struct ezcfg *ezcfg;

	ASSERT(uuid != NULL);

	//ezcfg = uuid->ezcfg;

	return uuid->version;
}

bool ezcfg_uuid_set_store_name(struct ezcfg_uuid *uuid, const char *store_name)
{
	struct ezcfg *ezcfg;
	char *p;

	ASSERT(uuid != NULL);
	ASSERT(store_name != NULL);

	ezcfg = uuid->ezcfg;

	p = strdup(store_name);
	if (p == NULL) {
		err(ezcfg, "unable allocate store name.\n");
		return false;
	}

	if (uuid->store_name != NULL) {
		free(uuid->store_name);
	}
	uuid->store_name = p;
	return true;
}

char * ezcfg_uuid_get_store_name(struct ezcfg_uuid *uuid)
{
	//struct ezcfg *ezcfg;

	ASSERT(uuid != NULL);

	//ezcfg = uuid->ezcfg;

	return uuid->store_name;
}

bool ezcfg_uuid_generate(struct ezcfg_uuid *uuid)
{
	struct ezcfg *ezcfg;

	ASSERT(uuid != NULL);

	ezcfg = uuid->ezcfg;

	switch(uuid->version) {
	case 1:
		/* no stable store info for UUID v1 */
		return gen_uuid_v1(uuid);
		break;

	case 3:
		break;

	case 4:
		break;

	case 5:
		break;

	default:
		err(ezcfg, "uuid version is incorrect.\n");
		return false;
	}

	return true;
}

bool ezcfg_uuid_export_str(struct ezcfg_uuid *uuid, char *buf, int len)
{
	struct ezcfg *ezcfg;

	ASSERT(uuid != NULL);
	ASSERT(buf != NULL);
	ASSERT(len > EZCFG_UUID_STRING_LEN);

	ezcfg = uuid->ezcfg;

	if (snprintf(buf, EZCFG_UUID_STRING_LEN+1,
	         "%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
	         (unsigned long)uuid->time_low,
	         (unsigned int)uuid->time_mid,
	         (unsigned int)uuid->time_hi_and_version,
	         (unsigned int)uuid->clock_seq_hi_and_reserved,
	         (unsigned int)uuid->clock_seq_low,
	         (unsigned int)uuid->node[0],
	         (unsigned int)uuid->node[1],
	         (unsigned int)uuid->node[2],
	         (unsigned int)uuid->node[3],
	         (unsigned int)uuid->node[4],
	         (unsigned int)uuid->node[5]) != EZCFG_UUID_STRING_LEN) {
		err(ezcfg, "export str error.\n");
		return false;
	}
	return true;
}

bool ezcfg_uuid_v1_set_mac(struct ezcfg_uuid *uuid, unsigned char *mac, int len)
{
	struct ezcfg *ezcfg;

	ASSERT(uuid != NULL);
	ASSERT(mac != NULL);
	ASSERT(len >= 6);

	ezcfg = uuid->ezcfg;

	if (uuid->version != 1) {
		err(ezcfg, "uuid version should be 1 for set_mac.\n");
		return false;
	}

	memcpy(uuid->u.mac, mac, 6);

	return true;
}

bool ezcfg_uuid_v1_enforce_multicast_mac(struct ezcfg_uuid *uuid)
{
	struct ezcfg *ezcfg;
	int i;

	ASSERT(uuid != NULL);

	ezcfg = uuid->ezcfg;

	if (uuid->version != 1) {
		err(ezcfg, "uuid version should be 1 for enforce_multicast_mac.\n");
		return false;
	}

	for(i=0; i<6; i++) {
		uuid->u.mac[i] = random() & 0xFF;
	}

	/* set to local multicast MAC address */
	uuid->u.mac[0] |= 0x03;
	
	return true;
}
