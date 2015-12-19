/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/admin/http_html_admin_layout_css.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-13   0.1       Write it from scratch
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

#if (HAVE_EZBOX_EZCFG_NLS == 1)
#include <locale.h>
#include <libintl.h>
#endif

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-http.h"
#include "ezcfg-html.h"
#include "ezcfg-http_html_admin.h"

/**
 * Private functions
 **/

static int build_admin_layout_css_response(struct ezcfg_http_html_admin *admin)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_css *css;
	char buf[256];
	char *msg = NULL;
	int msg_len;
	int n;
	int rc = 0;
	bool ret;
	
	ASSERT(admin != NULL);

	ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
	http = ezcfg_http_html_admin_get_http(admin);

	css = ezcfg_css_new(ezcfg);
	if (css == NULL) {
		err(ezcfg, "can not alloc css.\n");
		rc = -1;
		goto func_exit;
	}

	/* build CSS */
	ezcfg_css_set_version_major(css, 2);
	ezcfg_css_set_version_minor(css, 1);

	/* body {
	 *   position: relative;
	 *   background: black;
	 *   margin: 0;
	 *   padding: 0;
	 * }
	 */
	rc = ezcfg_css_add_rule_set(css, EZCFG_HTML_BODY_ELEMENT_NAME, "position", "relative");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}
	rc = ezcfg_css_add_rule_set(css, EZCFG_HTML_BODY_ELEMENT_NAME, "background", "black");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}
	rc = ezcfg_css_add_rule_set(css, EZCFG_HTML_BODY_ELEMENT_NAME, "margin", "0");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}
	rc = ezcfg_css_add_rule_set(css, EZCFG_HTML_BODY_ELEMENT_NAME, "padding", "0");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	/* div#container {
	 *   width: 100%;
	 * }
	 */
	snprintf(buf, sizeof(buf), "%s#%s", EZCFG_HTML_DIV_ELEMENT_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_CONTAINER);
	rc = ezcfg_css_add_rule_set(css, buf, "width", "100%");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	/* div#head {
	 *   width: 800px;
	 *   margin: auto;
	 *   height: auto;
	 *   padding-top: 1px;
	 *   padding-bottom: 1px;
	 *   padding-left: 5px;
	 *   padding-right: 5px;
	 *   text-align: center;
	 *   background: #FFCC99;
	 * }
	 */   
	snprintf(buf, sizeof(buf), "%s#%s", EZCFG_HTML_DIV_ELEMENT_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_HEAD);
	rc = ezcfg_css_add_rule_set(css, buf, "width", "800px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "margin", "auto");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "height", "auto");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "padding-top", "1px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "padding-bottom", "1px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "padding-left", "5px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "padding-right", "5px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "text-align", "center");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "background", "#FFCC99");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	/* div#main {
	 *   width: 800px;
	 *   margin: auto;
	 *   height: auto;
	 *   padding-top: 1px;
	 *   padding-bottom: 1px;
	 *   padding-left: 5px;
	 *   padding-right: 5px;
	 *   background: #CCFF00;
	 *   overflow: hidden;
	 * }
	 */   
	snprintf(buf, sizeof(buf), "%s#%s", EZCFG_HTML_DIV_ELEMENT_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_MAIN);
	rc = ezcfg_css_add_rule_set(css, buf, "width", "800px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "margin", "auto");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "height", "auto");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "padding-top", "1px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "padding-bottom", "1px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "padding-left", "5px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "padding-right", "5px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "background", "#CCFF00");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "overflow", "hidden");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	/* div#foot {
	 *   width: 800px;
	 *   margin: auto;
	 *   height: auto;
	 *   padding-top: 1px;
	 *   padding-bottom: 1px;
	 *   padding-left: 5px;
	 *   padding-right: 5px;
	 *   text-align: right;
	 *   clear: both;
	 *   background: #00FFFF;
	 * }
	 */   
	snprintf(buf, sizeof(buf), "%s#%s", EZCFG_HTML_DIV_ELEMENT_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_FOOT);
	rc = ezcfg_css_add_rule_set(css, buf, "width", "800px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "margin", "auto");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "height", "auto");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "padding-top", "1px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "padding-bottom", "1px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "padding-left", "5px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "padding-right", "5px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "text-align", "right");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "clear", "both");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "background", "#00FFFF");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	/* div#menu {
	 *   width: 180px;
	 *   text-align: left;
	 *   float: left;
	 *   clear: left;
	 *   overflow: hidden;
	 * }
	 */   
	snprintf(buf, sizeof(buf), "%s#%s", EZCFG_HTML_DIV_ELEMENT_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_MENU);
	rc = ezcfg_css_add_rule_set(css, buf, "width", "180px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "text-align", "left");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "float", "left");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "clear", "left");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "overflow", "hidden");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	/* div#content {
	 *   width: 580px;
	 *   text-align: left;
	 *   float: right;
	 *   clear: right;
	 *   overflow: hidden;
	 * }
	 */   
	snprintf(buf, sizeof(buf), "%s#%s", EZCFG_HTML_DIV_ELEMENT_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_CONTENT);
	rc = ezcfg_css_add_rule_set(css, buf, "width", "580px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "text-align", "left");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "float", "right");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "clear", "right");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "overflow", "hidden");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	/* div#button {
	 *   width: 800px;
	 *   margin: 0 auto;
	 *   height: auto;
	 *   text-align: right;
	 *   clear: both;
	 *   overflow: hidden;
	 * }
	 */ 
#if 0
	snprintf(buf, sizeof(buf), "%s#%s", EZCFG_HTML_DIV_ELEMENT_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_BUTTON);
	rc = ezcfg_css_add_rule_set(css, buf, "width", "800px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "margin", "0 auto");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "height", "auto");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "text-align", "right");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "clear", "both");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "overflow", "hidden");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}
#endif

	/* div#menu ul {
	 *   padding: 0;
	 *   margin: 0;
	 */
	snprintf(buf, sizeof(buf), "%s#%s %s", EZCFG_HTML_DIV_ELEMENT_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_MENU, EZCFG_HTML_UL_ELEMENT_NAME);
	rc = ezcfg_css_add_rule_set(css, buf, "margin", "0");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "padding", "0");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	/* div#menu ul li a {
	 *   display: block;
	 *   text-align: center;
	 *   padding: 5px 10px;
	 *   margin: 0 0 1px;
	 *   border-width: 0; 
	 *   text-decoration: none;
	 *   color: #FFC;
	 *   background: #444;
	 *   border-right: 5px solid #505050;}
	 */
	snprintf(buf, sizeof(buf), "%s#%s %s %s %s", EZCFG_HTML_DIV_ELEMENT_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_MENU, EZCFG_HTML_UL_ELEMENT_NAME, EZCFG_HTML_LI_ELEMENT_NAME, EZCFG_HTML_A_ELEMENT_NAME);
	rc = ezcfg_css_add_rule_set(css, buf, "display", "block");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "text-align", "center");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "padding", "5px 10px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "margin", "0 0 1px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "border-width", "0");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "text-decoration", "none");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "color", "#FFC");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "background", "#444");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "border-right", "5px solid #505050");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	/* div#menu ul li a:hover {
	 *   color: #411;
	 *   background: #AAA;
	 *   border-right: 5px double white;}
	 */    
	snprintf(buf, sizeof(buf), "%s#%s %s %s %s:%s", EZCFG_HTML_DIV_ELEMENT_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_MENU, EZCFG_HTML_UL_ELEMENT_NAME, EZCFG_HTML_LI_ELEMENT_NAME, EZCFG_HTML_A_ELEMENT_NAME, "hover");
	rc = ezcfg_css_add_rule_set(css, buf, "color", "#411");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "background", "#AAA");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "border-right", "5px double white");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	/* div#menu ul li a span {display: none;} */
	snprintf(buf, sizeof(buf), "%s#%s %s %s %s %s", EZCFG_HTML_DIV_ELEMENT_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_MENU, EZCFG_HTML_A_ELEMENT_NAME, EZCFG_HTML_UL_ELEMENT_NAME, EZCFG_HTML_LI_ELEMENT_NAME, EZCFG_HTML_SPAN_ELEMENT_NAME);
	rc = ezcfg_css_add_rule_set(css, buf, "display", "none");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	/* div#menu ul li a:hover span {
	 *   display: block;
	 *   top: 180px;
	 *   left: 0;
	 *   width: 125px;
	 *   padding: 5px;
	 *   margin: 10px;
	 *   z-index: 100;
	 *   color: #AAA;
	 *   background: black;
	 *   text-align: center;}
	 */
	snprintf(buf, sizeof(buf), "%s#%s %s %s %s:%s %s", EZCFG_HTML_DIV_ELEMENT_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_MENU, EZCFG_HTML_UL_ELEMENT_NAME, EZCFG_HTML_LI_ELEMENT_NAME, EZCFG_HTML_A_ELEMENT_NAME, "hover", EZCFG_HTML_SPAN_ELEMENT_NAME);
	rc = ezcfg_css_add_rule_set(css, buf, "display", "block");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "top", "180px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "left", "0");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "width", "125px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "padding", "5px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "margin", "10px");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "z-index", "100");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "color", "#AAA");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "background", "black");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	rc = ezcfg_css_add_rule_set(css, buf, "text-align", "center");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	/* div#menu a span {display: none;} */
	snprintf(buf, sizeof(buf), "%s#%s %s %s", EZCFG_HTML_DIV_ELEMENT_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_MENU, EZCFG_HTML_A_ELEMENT_NAME, EZCFG_HTML_SPAN_ELEMENT_NAME);
	rc = ezcfg_css_add_rule_set(css, buf, "display", "none");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}

	/* p.warning {color: red;} */
	snprintf(buf, sizeof(buf), "%s.%s", EZCFG_HTML_P_ELEMENT_NAME, EZCFG_HTTP_HTML_ADMIN_P_CLASS_WARNING);
	rc = ezcfg_css_add_rule_set(css, buf, "color", "red");
	if (rc < 0) {
		err(ezcfg, "ezcfg_css_add_rule_set.\n");
		rc = -1;
		goto func_exit;
	}


	msg_len = ezcfg_css_get_message_length(css);
	if (msg_len < 0) {
		err(ezcfg, "ezcfg_css_get_message_length\n");
		rc = -1;
		goto func_exit;
	}
	msg_len++; /* one more for '\0' */
	msg = (char *)malloc(msg_len);
	if (msg == NULL) {
		err(ezcfg, "malloc error.\n");
		rc = -1;
		goto func_exit;
	}

	memset(msg, 0, msg_len);
	n = ezcfg_css_write_message(css, msg, msg_len);
	if (n < 0) {
		err(ezcfg, "ezcfg_css_write_message\n");
		rc = -1;
		goto func_exit;
	}

	/* FIXME: name point to http->request_uri !!!
         * never reset http before using name */
	/* clean http structure info */
	ezcfg_http_reset_attributes(http);
	ezcfg_http_set_status_code(http, 200);
	ezcfg_http_set_state_response(http);

	if (ezcfg_http_set_message_body(http, msg, n) == NULL) {
		err(ezcfg, "ezcfg_http_set_message_body\n");
		rc = -1;
		goto func_exit;
	}

	ret = ezcfg_http_html_admin_set_http_css_common_header(admin);
	if (ret == false) {
		err(ezcfg, "ezcfg_http_html_admin_set_http_css_common_header error.\n");
		rc = -1;
		goto func_exit;
	}

	/* set return value */
	rc = 0;
func_exit:
	if (css != NULL)
		ezcfg_css_delete(css);

	if (msg != NULL)
		free(msg);

	return rc;
}

/**
 * Public functions
 **/

int ezcfg_http_html_admin_layout_css_handler(struct ezcfg_http_html_admin *admin)
{
	//struct ezcfg *ezcfg;
	int ret = -1;

	ASSERT(admin != NULL);

	//ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);

	/* admin layout_css uri=[/admin/layout_css] */
	ret = build_admin_layout_css_response(admin);
	return ret;
}

