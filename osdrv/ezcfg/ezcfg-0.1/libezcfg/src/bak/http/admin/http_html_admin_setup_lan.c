/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/admin/http_html_admin_setup_lan.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-04   0.1       Write it from scratch
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
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
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
static int set_html_main_setup_lan_dhcpd(
					 struct ezcfg_http_html_admin *admin,
					 struct ezcfg_locale *locale,
					 int pi, int si)
{
  struct ezcfg *ezcfg;
  struct ezcfg_nvram *nvram;
  struct ezcfg_html *html;
  int content_index, child_index;
  int p_index;
  int input_index, select_index;
  char buf[1024];
  char name[32];
  char *p = NULL;
  int i;
  int ret = -1;

  ASSERT(admin != NULL);
  ASSERT(pi > 1);

  ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
  nvram = ezcfg_http_html_admin_get_nvram(admin);
  html = ezcfg_http_html_admin_get_html(admin);

  content_index = pi;
  child_index = si;

  /* <h3>DHCP Server Setting</h3> */
  child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_H3_ELEMENT_NAME, ezcfg_locale_text(locale, "DHCP Server Setting"));
  if (child_index < 0) {
    err(ezcfg, "ezcfg_html_add_body_child error.\n");
    goto func_exit;
  }

  /* <p>Service Switch : </p> */
  snprintf(buf, sizeof(buf), "%s%s",
	   ezcfg_locale_text(locale, "Service Switch"),
	   ezcfg_locale_text(locale, " : "));
  child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_html_add_body_child error.\n");
    goto func_exit;
  }

  /* save <p> index */
  p_index = child_index;

  /* <p>Service Switch : <select name="lan_dhcpd_enable"></select></p> */
  child_index = -1;
  select_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_SELECT_ELEMENT_NAME, NULL);
  if (select_index < 0) {
    err(ezcfg, "ezcfg_html_add_body_child error.\n");
    goto func_exit;
  }
  ezcfg_html_add_body_child_attribute(html, select_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_ENABLE), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

  /* <p>Service Switch : <select name="lan_dhcpd_enable"><option value="1" selected="selected">Enabled</option></select></p> */
  buf[0] = '\0';
  ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_ENABLE), &p);
  if (p != NULL) {
    snprintf(buf, sizeof(buf), "%s", p);
    free(p);
  }
  child_index = -1;
  for (i = 0; i < 2; i++) {
    char tmp[2];
    snprintf(tmp, sizeof(tmp), "%d", i);
    child_index = ezcfg_html_add_body_child(html, select_index, child_index, EZCFG_HTML_OPTION_ELEMENT_NAME, ezcfg_locale_text(locale, ezcfg_util_text_get_service_switch(i == 1)));
    if (child_index < 0) {
      err(ezcfg, "ezcfg_html_add_body_child error.\n");
      goto func_exit;
    }
    ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, tmp, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    if (strcmp(tmp, buf) == 0) {
      ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    }
  }

  /* restore <p> index */
  child_index = p_index;

  if (strcmp(buf, "1") == 0) {
    char tmp[2];
    /* <p>Start IP Address : </p> */
    snprintf(buf, sizeof(buf), "%s%s",
	     ezcfg_locale_text(locale, "Start IP Address"),
	     ezcfg_locale_text(locale, " : "));
    child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_html_add_body_child error.\n");
      goto func_exit;
    }

    /* <p>Start IP Address : <input type="text" maxlength="15" name="lan_dhcpd_start_ipaddr" value=""/></p> */
    /* save <p> index */
    p_index = child_index;
    child_index = -1;

    input_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
    if (input_index < 0) {
      err(ezcfg, "ezcfg_html_add_body_child error.\n");
      goto func_exit;
    }
    ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_TEXT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME, "15", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_START_IPADDR), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_START_IPADDR), &p);
    if (p != NULL) {
      ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, p, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
      free(p);
    }

    /* restore <p> index */
    child_index = p_index;

    /* <p>End IP Address : </p> */
    snprintf(buf, sizeof(buf), "%s%s",
	     ezcfg_locale_text(locale, "End IP Address"),
	     ezcfg_locale_text(locale, " : "));
    child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_html_add_body_child error.\n");
      goto func_exit;
    }

    /* <p>End IP Address : <input type="text" maxlength="15" name="lan_dhcpd_end_ipaddr" value=""/></p> */
    /* save <p> index */
    p_index = child_index;
    child_index = -1;

    input_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
    if (input_index < 0) {
      err(ezcfg, "ezcfg_html_add_body_child error.\n");
      goto func_exit;
    }
    ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_TEXT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME, "15", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_END_IPADDR), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_END_IPADDR), &p);
    if (p != NULL) {
      ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, p, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
      free(p);
    }

    /* restore <p> index */
    child_index = p_index;

    /* <p>Client Lease Time : </p> */
    snprintf(buf, sizeof(buf), "%s%s",
	     ezcfg_locale_text(locale, "Client Lease Time"),
	     ezcfg_locale_text(locale, " : "));
    child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_html_add_body_child error.\n");
      goto func_exit;
    }

    /* <p>Client Lease Time : <input type="text" maxlength="15" name="lan_dhcpd_lease" value=""/></p> */
    /* save <p> index */
    p_index = child_index;
    child_index = -1;

    input_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, ezcfg_locale_text(locale, " Seconds"));
    if (input_index < 0) {
      err(ezcfg, "ezcfg_html_add_body_child error.\n");
      goto func_exit;
    }
    ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_TEXT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME, "5", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_SIZE_ATTRIBUTE_NAME, "5", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_LEASE), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_LEASE), &p);
    if (p != NULL) {
      ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, p, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
      free(p);
    }

    /* restore <p> index */
    child_index = p_index;

    /* <p>Use WAN DNS Servers : </p> */
    snprintf(buf, sizeof(buf), "%s%s",
	     ezcfg_locale_text(locale, "Use WAN DNS Servers"),
	     ezcfg_locale_text(locale, " : "));
    child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_html_add_body_child error.\n");
      goto func_exit;
    }

    /* <p>Use WAN DNS Servers : <input type="radio" name="lan_dhcpd_wan_dns_enable" value="1">Yes</input><input type="radio" name="lan_dhcpd_wan_dns_enable" value="0">No</input></p> */
    /* save <p> index */
    p_index = child_index;
    child_index = -1;
    tmp[0] = '\0';
    ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_WAN_DNS_ENABLE), &p);
    if (p != NULL) {
      snprintf(tmp, sizeof(tmp), "%s", p);
      free(p);
    }

    snprintf(buf, sizeof(buf), " %s ",
	     ezcfg_locale_text(locale, "Yes"));
    input_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, buf);
    if (input_index < 0) {
      err(ezcfg, "ezcfg_html_add_body_child error.\n");
      goto func_exit;
    }
    ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_RADIO, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_WAN_DNS_ENABLE), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, "1", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    if (strcmp(tmp, "1") == 0) {
      ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_CHECKED_ATTRIBUTE_NAME, EZCFG_HTML_CHECKED_ATTRIBUTE_NAME, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    }

    snprintf(buf, sizeof(buf), " %s ",
	     ezcfg_locale_text(locale, "No"));
    input_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, buf);
    if (input_index < 0) {
      err(ezcfg, "ezcfg_html_add_body_child error.\n");
      goto func_exit;
    }
    ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_RADIO, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_WAN_DNS_ENABLE), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, "0", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    if (strcmp(tmp, "0") == 0) {
      ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_CHECKED_ATTRIBUTE_NAME, EZCFG_HTML_CHECKED_ATTRIBUTE_NAME, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    }

    /* restore <p> index */
    child_index = p_index;

    /* set DHCP DNS server manually */
    if (strcmp(tmp, "0") == 0) {
      for (i = 1; i <= 3; i++) {
	snprintf(name, sizeof(name), "%s%d", NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_DNS), i);
	/* <p>Static DNS 1 : </p> */
	snprintf(buf, sizeof(buf), "%s %d%s",
		 ezcfg_locale_text(locale, "Static DNS"), i,
		 ezcfg_locale_text(locale, " : "));
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (child_index < 0) {
	  err(ezcfg, "ezcfg_html_add_body_child error.\n");
	  goto func_exit;
	}

	/* <p>Static DNS 1 : <input type="text" maxlength="15" name="lan_dhcpd_dns1" value=""/></p> */
	/* save <p> index */
	p_index = child_index;
	child_index = -1;

	input_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
	if (input_index < 0) {
	  err(ezcfg, "ezcfg_html_add_body_child error.\n");
	  goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_TEXT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME, "15", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, name, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_nvram_get_entry_value(nvram, name, &p);
	if (p != NULL) {
	  ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, p, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	  free(p);
	}

	/* restore <p> index */
	child_index = p_index;
      }
    }

    /* <p><a href="/admin/reserve_dhcp_client">DHCP Client Reservation</a></p> */
    child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, NULL);
    if (child_index < 0) {
      err(ezcfg, "ezcfg_html_add_body_child error.\n");
      goto func_exit;
    }
    /* save <p> index */
    p_index = child_index;
    child_index = -1;

    child_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_A_ELEMENT_NAME, ezcfg_locale_text(locale, "DHCP Client Reservation"));
    if (child_index < 0) {
      err(ezcfg, "ezcfg_html_add_body_child error.\n");
      goto func_exit;
    }

    ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_HREF_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "reserve_dhcp_client", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    /* restore <p> index */
    child_index = p_index;
  }

  si = child_index;

  /* must return main index */
  ret = si;

 func_exit:
  return ret;
}

static int set_html_main_setup_lan(
				   struct ezcfg_http_html_admin *admin,
				   struct ezcfg_locale *locale,
				   int pi, int si)
{
  struct ezcfg *ezcfg;
  struct ezcfg_nvram *nvram;
  struct ezcfg_html *html;
  int content_index, child_index;
  int p_index;
  int input_index;
  char buf[1024];
  char *p = NULL;
  int ret = -1;

  ASSERT(admin != NULL);
  ASSERT(pi > 1);

  ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
  nvram = ezcfg_http_html_admin_get_nvram(admin);
  html = ezcfg_http_html_admin_get_html(admin);

  /* <div id="main"> */
  si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_DIV_ELEMENT_NAME, NULL);
  if (si < 0) {
    err(ezcfg, "ezcfg_html_add_body_child error.\n");
    goto func_exit;
  }
  ezcfg_html_add_body_child_attribute(html, si, EZCFG_HTML_ID_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_MAIN, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

  /* <div id="menu"> */
  child_index = ezcfg_http_html_admin_set_html_menu(admin, si, -1);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_http_html_admin_set_html_menu.\n");
    goto func_exit;
  }

  /* <div id="content"> */
  content_index = ezcfg_html_add_body_child(html, si, child_index, EZCFG_HTML_DIV_ELEMENT_NAME, NULL);
  if (content_index < 0) {
    err(ezcfg, "ezcfg_html_add_body_child error.\n");
    goto func_exit;
  }
  ezcfg_html_add_body_child_attribute(html, content_index, EZCFG_HTML_ID_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_CONTENT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

  child_index = -1;
  /* <h3>LAN Setup</h3> */
  child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_H3_ELEMENT_NAME, ezcfg_locale_text(locale, "LAN Setup"));
  if (child_index < 0) {
    err(ezcfg, "ezcfg_html_add_body_child error.\n");
    goto func_exit;
  }

  /* <p>IP Address : </p> */
  snprintf(buf, sizeof(buf), "%s%s",
	   ezcfg_locale_text(locale, "IP Address"),
	   ezcfg_locale_text(locale, " : "));
  child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_html_add_body_child error.\n");
    goto func_exit;
  }

  /* <p>IP Address : <input type="text" maxlength="15" name="lan_ipaddr" value=""/></p> */
  /* save <p> index */
  p_index = child_index;
  child_index = -1;

  input_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
  if (input_index < 0) {
    err(ezcfg, "ezcfg_html_add_body_child error.\n");
    goto func_exit;
  }
  ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_TEXT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
  ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME, "15", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
  ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(LAN, IPADDR), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
  ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(LAN, IPADDR), &p);
  if (p != NULL) {
    ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, p, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    free(p);
  }

  /* restore <p> index */
  child_index = p_index;

  /* <p>Subnet Mask : </p> */
  snprintf(buf, sizeof(buf), "%s%s",
	   ezcfg_locale_text(locale, "Subnet Mask"),
	   ezcfg_locale_text(locale, " : "));
  child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_html_add_body_child error.\n");
    goto func_exit;
  }

  /* <p>Subnet Mask : <input type="text" maxlength="15" name="lan_netmask" value=""/></p> */
  /* save <p> index */
  p_index = child_index;
  child_index = -1;

  input_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
  if (input_index < 0) {
    err(ezcfg, "ezcfg_html_add_body_child error.\n");
    goto func_exit;
  }
  ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_TEXT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
  ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME, "15", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
  ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(LAN, NETMASK), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
  ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(LAN, NETMASK), &p);
  if (p != NULL) {
    ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, p, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
    free(p);
  }

  /* restore <p> index */
  child_index = p_index;

  buf[0] = '\0';
  ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(RC, DNSMASQ_ENABLE), &p);
  if (p != NULL) {
    snprintf(buf, sizeof(buf), "%s", p);
    free(p);
  }

  if (strcmp(buf, "1") == 0) {
    child_index = set_html_main_setup_lan_dhcpd(admin, locale, content_index, child_index);
    if (child_index < 0) {
      err(ezcfg, "set_html_main_setup_lan_dhcpd error.\n");
      goto func_exit;
    }
  }

  /* <br /> */
  child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_BR_ELEMENT_NAME, NULL);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_html_add_body_child error.\n");
    goto func_exit;
  }

  /* <input> buttons part */
  child_index = ezcfg_http_html_admin_set_html_button(admin, content_index, child_index);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_http_html_admin_set_html_button.\n");
    goto func_exit;
  }

  /* must return main index */
  ret = si;

 func_exit:
  return ret;
}

static int build_admin_setup_lan_response(struct ezcfg_http_html_admin *admin)
{
  struct ezcfg *ezcfg;
  struct ezcfg_html *html = NULL;
  struct ezcfg_locale *locale = NULL;
  int head_index, body_index, child_index;
  int container_index, form_index;
  int rc = 0;
	
  ASSERT(admin != NULL);

  ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);

  /* set locale info */
  locale = ezcfg_locale_new(ezcfg);
  if (locale != NULL) {
    ezcfg_locale_set_domain(locale, EZCFG_HTTP_HTML_ADMIN_SETUP_LAN_DOMAIN);
    ezcfg_locale_set_dir(locale, EZCFG_HTTP_HTML_LANG_DIR);
  }

  html = ezcfg_html_new(ezcfg);

  if (html == NULL) {
    err(ezcfg, "can not alloc html.\n");
    rc = -1;
    goto func_exit;
  }

  /* set admin->html */
  ezcfg_http_html_admin_set_html(admin, html);

  /* clean HTML structure info */
  ezcfg_html_reset_attributes(html);

  /* build HTML */
  ezcfg_html_set_version_major(html, 4);
  ezcfg_html_set_version_minor(html, 1);

  /* HTML root */
  rc = ezcfg_html_set_root(html, EZCFG_HTML_HTML_ELEMENT_NAME);
  if (rc < 0) {
    err(ezcfg, "ezcfg_html_set_root.\n");
    rc = -1;
    goto func_exit;
  }

  /* HTML Head */
  head_index = ezcfg_html_set_head(html, EZCFG_HTML_HEAD_ELEMENT_NAME);
  if (head_index < 0) {
    err(ezcfg, "ezcfg_html_set_head error.\n");
    rc = -1;
    goto func_exit;
  }

  /* set admin common Head */
  child_index = ezcfg_http_html_admin_set_html_common_head(admin, head_index, -1);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_http_html_admin_set_html_common_head error.\n");
    rc = -1;
    goto func_exit;
  }

  /* HTML Title */
  child_index = ezcfg_html_add_head_child(html, head_index, child_index, EZCFG_HTML_TITLE_ELEMENT_NAME, ezcfg_locale_text(locale, "Setup LAN"));
  if (child_index < 0) {
    err(ezcfg, "ezcfg_html_add_head_child error.\n");
    rc = -1;
    goto func_exit;
  }

  /* HTML Body */
  body_index = ezcfg_html_set_body(html, EZCFG_HTML_BODY_ELEMENT_NAME);
  if (body_index < 0) {
    err(ezcfg, "ezcfg_html_set_body error.\n");
    rc = -1;
    goto func_exit;
  }

  /* HTML div container */
  container_index = ezcfg_html_add_body_child(html, body_index, -1, EZCFG_HTML_DIV_ELEMENT_NAME, NULL);
  if (container_index < 0) {
    err(ezcfg, "ezcfg_html_add_body_child error.\n");
    rc = -1;
    goto func_exit;
  }
  ezcfg_html_add_body_child_attribute(html, container_index, EZCFG_HTML_ID_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_CONTAINER, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

  /* HTML form */
  form_index = ezcfg_html_add_body_child(html, container_index, -1, EZCFG_HTML_FORM_ELEMENT_NAME, NULL);
  if (form_index < 0) {
    err(ezcfg, "ezcfg_html_add_body_child error.\n");
    rc = -1;
    goto func_exit;
  }
  ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, "setup_lan", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
  ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_METHOD_ATTRIBUTE_NAME, "post", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
  ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_ACTION_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "setup_lan", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

  /* HTML div head */
  child_index = ezcfg_http_html_admin_set_html_head(admin, form_index, -1);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_http_html_admin_set_html_head error.\n");
    rc = -1;
    goto func_exit;
  }

  /* HTML div main */
  child_index = set_html_main_setup_lan(admin, locale, form_index, child_index);
  if (child_index < 0) {
    err(ezcfg, "set_html_main_setup_lan error.\n");
    rc = -1;
    goto func_exit;
  }

  /* HTML div foot */
  child_index = ezcfg_http_html_admin_set_html_foot(admin, form_index, child_index);
  if (child_index < 0) {
    err(ezcfg, "ezcfg_http_html_admin_set_html_foot error.\n");
    rc = -1;
    goto func_exit;
  }

  /* set return value */
  rc = 0;
 func_exit:
  if (locale != NULL)
    ezcfg_locale_delete(locale);

  return rc;
}

static bool do_admin_setup_lan_action(struct ezcfg_http_html_admin *admin)
{
  //struct ezcfg *ezcfg;
  //struct ezcfg_link_list *list;
  bool ret = false;

  //ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
  //list = ezcfg_http_html_admin_get_post_list(admin);

  if (ezcfg_http_html_admin_get_action(admin) == HTTP_HTML_ADMIN_ACT_SAVE) {
    ret = ezcfg_http_html_admin_save_settings(admin);
  }
  return ret;
}

static bool handle_admin_setup_lan_post(struct ezcfg_http_html_admin *admin)
{
  //struct ezcfg *ezcfg;
  bool ret = false;

  //ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);

  if (ezcfg_http_html_admin_handle_post_data(admin) == true) {
    ret = do_admin_setup_lan_action(admin);
  }
  return ret;
}

/**
 * Public functions
 **/
int ezcfg_http_html_admin_setup_lan_handler(struct ezcfg_http_html_admin *admin)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_http *http;
  int ret = -1;

  ASSERT(admin != NULL);

  //ezcfg = ezcfg_http_html_admin_get_ezcfg(admin);
  http = ezcfg_http_html_admin_get_http(admin);

  /* admin setup_lan uri=[/admin/setup_lan] */
  if (ezcfg_http_request_method_cmp(http, EZCFG_HTTP_METHOD_POST) == 0) {
    /* do post handling */
    handle_admin_setup_lan_post(admin);
  }

  ret = build_admin_setup_lan_response(admin);
  return ret;
}
