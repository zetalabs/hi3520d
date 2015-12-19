/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_sha1.c
 *
 * Description  : implement RFC 3174 - US Secure Hash Algorithm 1 (SHA1) algorithm
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2013-04-01   0.1       Write it from scrach
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

/*
 *  Define the circular shift macro
 */
#define SHA1CircularShift(bits,word) \
	((((word) << (bits)) & 0xFFFFFFFF) | \
	((word) >> (32-(bits))))


static void sha1_process_md(unsigned char block[64], uint32_t md[5])
{
	uint32_t w[80]; /* reusable round buffer */
	size_t t;
	uint32_t a, b, c, d, e;
	uint32_t temp;
	/* Constants defined in SHA-1 */
	const uint32_t sha1_k[] =
	{
		0x5A827999,
		0x6ED9EBA1,
		0x8F1BBCDC,
		0xCA62C1D6,
	};

	/* initialize the first 16 words in the array W */
	for(t = 0; t < 16; t++) {
		w[t]  = ((uint32_t) block[t * 4]) << 24;
		w[t] |= ((uint32_t) block[t * 4 + 1]) << 16;
		w[t] |= ((uint32_t) block[t * 4 + 2]) << 8;
		w[t] |= ((uint32_t) block[t * 4 + 3]);
	}

	for (t = 16; t < 80; t++) {
		w[t] = SHA1CircularShift(1,w[t-3] ^ w[t-8] ^ w[t-14] ^ w[t-16]);
	}

	a = md[0];
	b = md[1];
	c = md[2];
	d = md[3];
	e = md[4];

	for (t = 0; t < 20; t++) {
		temp = SHA1CircularShift(5, a) + ((b & c) | ((~b) & d)) + e + w[t] + sha1_k[0];
		temp &= 0xFFFFFFFF;
		e = d;
		d = c;
		c = SHA1CircularShift(30, b);
		b = a;
		a = temp;
	}

	for (t = 20; t < 40; t++) {
		temp = SHA1CircularShift(5, a) + (b ^ c ^ d) + e + w[t] + sha1_k[1];
		temp &= 0xFFFFFFFF;
		e = d;
		d = c;
		c = SHA1CircularShift(30, b);
		b = a;
		a = temp;
	}

	for (t = 40; t < 60; t++) {
		temp = SHA1CircularShift(5, a) + ((b & c) | (b & d) | (c & d)) + e + w[t] + sha1_k[2];
		temp &= 0xFFFFFFFF;
		e = d;
		d = c;
		c = SHA1CircularShift(30, b);
		b = a;
		a = temp;
	}

	for (t = 60; t < 80; t++) {
		temp = SHA1CircularShift(5, a) + (b ^ c ^ d) + e + w[t] + sha1_k[3];
		temp &= 0xFFFFFFFF;
		e = d;
		d = c;
		c = SHA1CircularShift(30, b);
		b = a;
		a = temp;
	}

	md[0] = (md[0] + a) & 0xFFFFFFFF;
	md[1] = (md[1] + b) & 0xFFFFFFFF;
	md[2] = (md[2] + c) & 0xFFFFFFFF;
	md[3] = (md[3] + d) & 0xFFFFFFFF;
	md[4] = (md[4] + e) & 0xFFFFFFFF;
}

int ezcfg_util_sha1(unsigned char *src, unsigned char *dst, size_t src_len, size_t dst_len)
{
	int count = 0;
	size_t i;
	unsigned char block[64]; /* 512-bit message blocks */
	size_t block_idx = 0;
	uint32_t len_lo = 0;
	uint32_t len_hi = 0;
	uint32_t md[5] = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };

	if ((src == NULL) || (dst == NULL)) {
		return -1;
	}

	if (src_len < 1) {
		return -1;
	}

	if (dst_len < 20) {
		return -1;
	}

	for (i = 0; i < src_len; i++) {
		block[block_idx++] = (src[i] & 0xFF);

		len_lo += 8;
		/* force it to 32 bits */
		len_lo &= 0xFFFFFFFF;
		if (len_lo == 0) {
			len_hi++;
			/* force it to 32 bits */
			len_hi &= 0xFFFFFFFF;
			if (len_hi == 0) {
				/* message is too long */
				return -1;
			}
		}

		if (block_idx == 64) {
			sha1_process_md(block, md);
			block_idx = 0;
		}
	}

	if (block_idx > 55) {
		block[block_idx++] = 0x80;
		while (block_idx < 64) {
			block[block_idx++] = 0;
		}

		sha1_process_md(block, md);

		while(block_idx < 56) {
			block[block_idx++] = 0;
		}
	}
	else {
		block[block_idx++] = 0x80;
		while (block_idx < 56) {
			block[block_idx++] = 0;
		}
	}

	block[56] = (len_hi >> 24) & 0xFF;
	block[57] = (len_hi >> 16) & 0xFF;
	block[58] = (len_hi >> 8) & 0xFF;
	block[59] = (len_hi) & 0xFF;
	block[60] = (len_lo >> 24) & 0xFF;
	block[61] = (len_lo >> 16) & 0xFF;
	block[62] = (len_lo >> 8) & 0xFF;
	block[63] = (len_lo) & 0xFF;

	sha1_process_md(block, md);

	count = sizeof(md);
	memcpy(dst, md, count);

	return count;
}
