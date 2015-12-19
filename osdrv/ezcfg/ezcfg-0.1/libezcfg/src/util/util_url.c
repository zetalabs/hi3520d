/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : util/util_url.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-01-13   0.1       Split it from worker_http.c
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#if 1
#define DBG(format, args...) do { \
	char path[256]; \
	FILE *fp; \
	snprintf(path, 256, "/tmp/%d-debug.txt", getpid()); \
	fp = fopen(path, "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

#define HEXTOI(x) (isdigit(x) ? x - '0' : x - 'W')

size_t ezcfg_util_url_decode(const char *src, size_t src_len,
	char *dst, size_t dst_len, int is_form_url_encoded)
{
	size_t i, j;
	int a, b;

	for (i = j = 0; i < src_len && j < dst_len - 1; i++, j++) {
		if (src[i] == '%' &&
		    isxdigit(* (const unsigned char *) (src + i + 1)) &&
		    isxdigit(* (const unsigned char *) (src + i + 2))) {
			a = tolower(* (const unsigned char *) (src + i + 1));
			b = tolower(* (const unsigned char *) (src + i + 2));
			dst[j] = (char) ((HEXTOI(a) << 4) | HEXTOI(b));
			i += 2;
		} else if (is_form_url_encoded && src[i] == '+') {
			dst[j] = ' ';
		} else {
			dst[j] = src[i];
		}
	}

	dst[j] = '\0'; // Null-terminate the destination

	return j;
}

void ezcfg_util_url_remove_double_dots_and_double_slashes(char *s)
{
	char *p = s;

	while (*s != '\0') {
		*p++ = *s++;
		if (s[-1] == '/' || s[-1] == '\\') {
			/* skip all following slashes and backslashes */
			while (*s == '/' || *s == '\\') {
				s++;
			}

			/* skip all double-dots */
			while (*s == '.' && s[1] == '.') {
				s += 2;
			}
		}
	}
	*p = '\0';
}
