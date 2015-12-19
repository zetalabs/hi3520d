/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_base64.c
 *
 * Description  : implement RFC1113 base64 encode/decode algorithm
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-10   0.1       Write it from scrach
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

static const char b64_encode_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const char b64_decode_table[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* '+' */
	62,
	0, 0, 0,
	/* '/' */
	63,
	/* '0' - '9' */
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 
	0, 0, 0, 0, 0, 0, 0,
	/* 'A' - 'Z' */
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	0, 0, 0, 0, 0, 0,
	/* 'a' - 'z' */
	26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
};

int ezcfg_util_base64_encode(unsigned char *src, unsigned char *dst, size_t src_len, size_t dst_len)
{
	unsigned char c[3];
	int count = 0;
	size_t b64_div = src_len / 3;
	size_t b64_mod = src_len % 3;
	size_t i;

	if ((src == NULL) || (dst == NULL)) {
		return -1;
	}

	if (dst_len < ((b64_div + ((b64_mod + 2) / 3)) * 4 + 1)) {
		return -1;
	}
 
	for (i = 0; i < b64_div; i++) {
		c[0] = *src++;
		c[1] = *src++;
		c[2] = *src++;

		*dst++ = b64_encode_table[c[0] >> 2];
		*dst++ = b64_encode_table[((c[0] << 4) | (c[1] >> 4)) & 0x3f];
		*dst++ = b64_encode_table[((c[1] << 2) | (c[2] >> 6)) & 0x3f];
		*dst++ = b64_encode_table[c[2] & 0x3f];
		count += 4;
	}

	if (b64_mod == 1) {
		c[0] = *src++;
		*dst++ = b64_encode_table[(c[0] & 0xfc) >> 2];
		*dst++ = b64_encode_table[(c[0] & 0x03) << 4];
		*dst++ = '=';
		*dst++ = '=';
		count += 4;
	}
	else if (b64_mod == 2) {
		c[0] = *src++;
		c[1] = *src++;
		*dst++ = b64_encode_table[(c[0] & 0xfc) >> 2];
		*dst++ = b64_encode_table[((c[0] & 0x03) << 4) | ((c[1] & 0xf0) >> 4)];
		*dst++ = b64_encode_table[(c[1] & 0x0f) << 2];
		*dst++ = '=';
		count += 4;
	}

	*dst = '\0';
	return count;
}

int ezcfg_util_base64_decode(unsigned char *src, unsigned char *dst, size_t src_len, size_t dst_len)
{
	int32_t val;
	int count = 0;
	size_t i;

	if ((src == NULL) || (dst == NULL)) {
		return -1;
	}

	if ((src_len % 4) != 0) {
		return -1;
	}

	if (dst_len < ((src_len / 4) * 3 + 1)) {
		return -1;
	}

	for (i = 0; i < src_len; i += 4) {
		val = b64_decode_table[*src++] << 18;
		val += b64_decode_table[*src++] << 12;
		*dst++ = (val & 0x00ff0000) >> 16;
		count++;
 
		if (*src != '=') {
			val += b64_decode_table[*src++] << 6;
			*dst++ = (val & 0x0000ff00) >> 8;
			count++;
 
			if (*src != '=') {
				val += b64_decode_table[*src++];
				*dst++ = (val & 0x000000ff);
				count++;
			}
		}
	}

	*dst = '\0';
	return count;
}
