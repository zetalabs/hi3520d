/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_dillo_dillorc.c
 *
 * Description  : ezbox /etc/dillo/dillorc file generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-07-01   0.1       Write it from scratch
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
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcd.h"

int pop_etc_dillo_dillorc(int flag)
{
        FILE *file = NULL;
	char name[32];
	char buf[64];
	int rc, ret;

	/* generate /etc/dillo/dillorc */
	file = fopen("/etc/dillo/dillorc", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	switch (flag) {
	case RC_ACT_START :
		/* Set the desired initial browser size */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, GEOMETRY));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, GEOMETRY), buf);
		}

		/* Change this if you want to have text-only browsing from the start. */
		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, LOAD_IMAGES), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, LOAD_IMAGES), "NO");
		}

		/* Change this if you want to disable loading of CSS stylesheets initially. */
		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, LOAD_STYLESHEETS), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, LOAD_STYLESHEETS), "NO");
		}

		/* Change this if you want to disable parsing of embedded CSS initially. */
		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, PARSE_EMBEDDED_CSS), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, PARSE_EMBEDDED_CSS), "NO");
		}

		/* How should Dillo restrict automatic requests */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, FILTER_AUTO_REQUESTS));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, FILTER_AUTO_REQUESTS), buf);
		}

		/* Change the buffering scheme for drawing */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, BUFFERED_DRAWING));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, BUFFERED_DRAWING), buf);
		}

		/* Set your default directory for download/save operations */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, SAVE_DIR));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, SAVE_DIR), buf);
		}

		/* Default fonts */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, FONT_SERIF));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=\"%s\"\n", SERVICE_OPTION(DILLO, FONT_SERIF), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, FONT_SANS_SERIF));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=\"%s\"\n", SERVICE_OPTION(DILLO, FONT_SANS_SERIF), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, FONT_CURSIVE));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=\"%s\"\n", SERVICE_OPTION(DILLO, FONT_CURSIVE), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, FONT_FANTASY));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=\"%s\"\n", SERVICE_OPTION(DILLO, FONT_FANTASY), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, FONT_MONOSPACE));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=\"%s\"\n", SERVICE_OPTION(DILLO, FONT_MONOSPACE), buf);
		}

		/* All font sizes are scaled by this value */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, FONT_FACTOR));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, FONT_FACTOR), buf);
		}

		/* Maximum font size in pixels */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, FONT_MAX_SIZE));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, FONT_MAX_SIZE), buf);
		}

		/* Minimum font size in pixels */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, FONT_MIN_SIZE));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, FONT_MIN_SIZE), buf);
		}

		/* Show tooltip popups for UI and for HTML title attributes */
		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, SHOW_TOOLTIP), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, SHOW_TOOLTIP), "NO");
		}

		/* If you prefer more accurate HTML bug diagnosis over better rendering */
		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, W3C_PLUS_HEURISTICS), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, W3C_PLUS_HEURISTICS), "NO");
		}

		/* Set the start page */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, START_PAGE));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=\"%s\"\n", SERVICE_OPTION(DILLO, START_PAGE), buf);
		}

		/* Set the home location */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, HOME));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=\"%s\"\n", SERVICE_OPTION(DILLO, HOME), buf);
		}

		/* Set the URL used by the web search dialog. */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, SEARCH_URL));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=\"%s\"\n", SERVICE_OPTION(DILLO, SEARCH_URL), buf);
		}

		/* If set, dillo will ask web servers to send pages in this language. */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, HTTP_LANGUAGE));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=\"%s\"\n", SERVICE_OPTION(DILLO, HTTP_LANGUAGE), buf);
		}

		/* Maximum number of simultaneous TCP connections to a single server or proxy. */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, HTTP_MAX_CONNS));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=\"%s\"\n", SERVICE_OPTION(DILLO, HTTP_MAX_CONNS), buf);
		}

		/* Set the proxy information for http. */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, HTTP_PROXY));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=\"%s\"\n", SERVICE_OPTION(DILLO, HTTP_PROXY), buf);
		}

		/* If you need to provide a  user/password pair for the proxy,                  
		 * set the proxy user name here and Dillo will ask for the password later.
		 */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, HTTP_PROXYUSER));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=\"%s\"\n", SERVICE_OPTION(DILLO, HTTP_PROXYUSER), buf);
		}

		/* Set the domains to access without proxy */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, NO_PROXY));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=\"%s\"\n", SERVICE_OPTION(DILLO, NO_PROXY), buf);
		}

		/* Set the HTTP Referer (sic) header. */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, HTTP_REFERER));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, HTTP_REFERER), buf);
		}

		/* Set the HTTP User-Agent header. */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, HTTP_USER_AGENT));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=\"%s\"\n", SERVICE_OPTION(DILLO, HTTP_USER_AGENT), buf);
		}

		/* Set the background color */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, BG_COLOR));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, BG_COLOR), buf);
		}

		/* If your eyes suffer with white backgrounds, change this. */
		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, ALLOW_WHITE_BG), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, ALLOW_WHITE_BG), "NO");
		}

		/* When set to YES, the page author's visited link color may be overridden                             
		 * to allow better contrast with text/links/background
		 */
		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, CONTRAST_VISITED_COLOR), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, CONTRAST_VISITED_COLOR), "NO");
		}

		/* Size of dillo panel (used to enlarge the browsing area) */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DILLO, PANEL_SIZE));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, PANEL_SIZE), buf);
		}

		/* When set to YES, the page author's visited link color may be overridden                             
		 * to allow better contrast with text/links/background
		 */
		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, SMALL_ICONS), "1") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, SMALL_ICONS), "YES");
		}

		/* Here you can choose to hide some widgets of the dillo panel... */
		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, SHOW_BACK), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, SHOW_BACK), "NO");
		}

		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, SHOW_FORW), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, SHOW_FORW), "NO");
		}

		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, SHOW_HOME), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, SHOW_HOME), "NO");
		}

		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, SHOW_RELOAD), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, SHOW_RELOAD), "NO");
		}

		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, SHOW_SAVE), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, SHOW_SAVE), "NO");
		}

		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, SHOW_STOP), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, SHOW_STOP), "NO");
		}

		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, SHOW_BOOKMARKS), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, SHOW_BOOKMARKS), "NO");
		}

		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, SHOW_TOOLS), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, SHOW_TOOLS), "NO");
		}

		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, SHOW_FILEMENU), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, SHOW_FILEMENU), "NO");
		}

		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, SHOW_CLEAR_URL), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, SHOW_CLEAR_URL), "NO");
		}

		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, SHOW_URL), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, SHOW_URL), "NO");
		}

		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, SHOW_SEARCH), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, SHOW_SEARCH), "NO");
		}

		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, SHOW_HELP), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, SHOW_HELP), "NO");
		}

		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, SHOW_PROGRESS_BOX), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, SHOW_PROGRESS_BOX), "NO");
		}

		/* Start dillo with the panels hidden? */
		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, FULLWINDOW_START), "1") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, FULLWINDOW_START), "YES");
		}

		/* When filling out forms, our default behaviour is to submit on enterpress,                           
		 * but only when there's a single text entry (to avoid incomplete submits).
		 */
		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, ENTERPRESS_FORCES_SUBMIT), "1") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, ENTERPRESS_FORCES_SUBMIT), "YES");
		}

		/* A mouse's middle click over a link opens a new Tab. */
		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, MIDDLE_CLICK_OPENS_NEW_TAB), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, MIDDLE_CLICK_OPENS_NEW_TAB), "NO");
		}

		/* Mouse middle click by default drives drag-scrolling. */
		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, MIDDLE_CLICK_DRAGS_PAGE), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, MIDDLE_CLICK_DRAGS_PAGE), "NO");
		}

		/* Focus follows new Tabs. */
		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, FOCUS_NEW_TAB), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, FOCUS_NEW_TAB), "NO");
		}

		/* Generic messages (mainly for debugging specific parts) */
		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, SHOW_MSG), "0") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, SHOW_MSG), "NO");
		}

		/* Accepted by the W3C validator but "strongly discouraged" by the SPEC. */
		if (utils_nvram_cmp(NVRAM_SERVICE_OPTION(DILLO, SHOW_EXTRA_WARNINGS), "1") == 0) {
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DILLO, SHOW_EXTRA_WARNINGS), "YES");
		}

		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}

	fclose(file);
	return (ret);
}
