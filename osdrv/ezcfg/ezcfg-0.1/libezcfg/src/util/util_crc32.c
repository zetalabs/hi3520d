/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_crc32.c
 *
 * Description  : compute the CRC-32 of a data stream
 *
 * This file is to calculate the CRC-32 for u-boot environment.
 * The 32-bit CRC calculation based on the polynomial:
 * x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1.
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-05   0.1       Write it from scrach
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

#define CRC32_INITIAL_REMAINDER	0xffffffffUL
#define CRC32_FINAL_XOR_VALUE	0xffffffffUL
#define CRC32_POLYNOMIAL	0x04c11db7UL

static uint32_t crc32_reflect(uint32_t data, int bits)
{
	uint32_t value = 0;
	int i;

	/* swap bit 0 for bit 7, bit 1 for bit 6, ... */
	for (i = 1; i < (bits+1); i++) {
		if (data & 1) { value |= (1 << (bits - i)); }
		data >>= 1;
	}
	return value;
}

static void crc32_init_table(uint32_t *crc32_table, int len)
{
	int i, j;
	
	for (i = 0; i < len; i++) {
		crc32_table[i] = crc32_reflect(i, 8) << 24;
		for (j = 0; j < 8; j++) {
			crc32_table[i] = (crc32_table[i] << 1) ^ ((crc32_table[i] & (1 << 31)) ? CRC32_POLYNOMIAL : 0);
		}
		crc32_table[i] = crc32_reflect(crc32_table[i], 32);
	}
}

static void crc32_calc(uint32_t *crcp, unsigned char *data, int len)
{
	uint32_t crc32_table[256];

	crc32_init_table(crc32_table, ARRAY_SIZE(crc32_table));

	while (len--) {
		*crcp = (*crcp >> 8) ^ crc32_table[(*crcp & 0xff) ^ *data];
		data++;
	}
}

uint32_t ezcfg_util_crc32(unsigned char *data, size_t len)
{
	uint32_t crc32 = CRC32_INITIAL_REMAINDER;
	crc32_calc(&crc32, data, len);
	crc32 ^= CRC32_FINAL_XOR_VALUE;
	return crc32;
}
