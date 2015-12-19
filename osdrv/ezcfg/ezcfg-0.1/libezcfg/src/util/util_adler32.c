/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : util/util_adler32.c
 *
 * Description  : compute the Adler-32 checksum of a data stream.
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-06   0.1       Write it from scratch
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

#include "ezcfg.h"
#include "ezcfg-private.h"

/* The Adler-32 checksum calculation */
#define MOD_ADLER 65521

/* data: Pointer to the data to be summed; len is in bytes */
uint32_t ezcfg_util_adler32(unsigned char *data, size_t len)
{
	uint32_t a = 1, b = 0;
	while (len > 0)
	{
		size_t tlen = len > 5550 ? 5550 : len;
		len -= tlen;
		do {
			a += *data++;
			b += a;
		} while (--tlen);

		a %= MOD_ADLER;
		b %= MOD_ADLER;
	}
	return (b << 16) | a;
}
