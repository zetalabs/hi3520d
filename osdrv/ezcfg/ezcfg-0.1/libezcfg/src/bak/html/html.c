/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : html/html.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-03-25   0.1       Write it from scratch
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
#include "ezcfg-html.h"

struct ezcfg_html {
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;

	/* HTML version info */
	unsigned short version_major;
	unsigned short version_minor;

	/* HTML root element */
	int root_index; /* HTML root element index in xml-root */

	/* HTML Head Section */
	int head_index; /* HTML HEAD element index in xml-root */

	/* HTML Body Section */
	int body_index; /* HTML BODY element index in xml->root */
};

/**
 * Public functions
 **/
void ezcfg_html_delete(struct ezcfg_html *html)
{
	//struct ezcfg *ezcfg;

	ASSERT(html != NULL);

	//ezcfg = html->ezcfg;

	if (html->xml != NULL)
		ezcfg_xml_delete(html->xml);

	free(html);
}

/**
 * ezcfg_html_new:
 * Create ezcfg html info builder data structure
 * Returns: a new ezcfg html info builder data structure
 **/
struct ezcfg_html *ezcfg_html_new(struct ezcfg *ezcfg)
{
	struct ezcfg_html *html;

	ASSERT(ezcfg != NULL);

	/* initialize html info builder data structure */
	html = calloc(1, sizeof(struct ezcfg_html));
	if (html == NULL) {
		err(ezcfg, "initialize html builder error.\n");
		return NULL;
	}

	memset(html, 0, sizeof(struct ezcfg_html));

	html->xml = ezcfg_xml_new(ezcfg);
	if (html->xml == NULL) {
		ezcfg_html_delete(html);
		return NULL;
	}

	html->root_index = -1;
	html->head_index = -1;
	html->body_index = -1;
	html->ezcfg = ezcfg;

	return html;
}

void ezcfg_html_reset_attributes(struct ezcfg_html *html)
{
	//struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;

	ASSERT(html != NULL);
	ASSERT(html->xml != NULL);

	//ezcfg = html->ezcfg;
	xml = html->xml;

	ezcfg_xml_reset_attributes(xml);

	html->version_major = 0;
	html->version_minor = 0;

	html->root_index = -1;
	html->head_index = -1;
	html->body_index = -1;
}

struct ezcfg *ezcfg_html_get_ezcfg(struct ezcfg_html *html)
{
	ASSERT(html != NULL);

	return html->ezcfg;
}

unsigned short ezcfg_html_get_version_major(struct ezcfg_html *html)
{
	//struct ezcfg *ezcfg;

	ASSERT(html != NULL);

	//ezcfg = html->ezcfg;

	return html->version_major;
}

unsigned short ezcfg_html_get_version_minor(struct ezcfg_html *html)
{
	//struct ezcfg *ezcfg;

	ASSERT(html != NULL);

	//ezcfg = html->ezcfg;

	return html->version_minor;
}

bool ezcfg_html_set_version_major(struct ezcfg_html *html, unsigned short major)
{
	//struct ezcfg *ezcfg;

	ASSERT(html != NULL);

	//ezcfg = html->ezcfg;

	html->version_major = major;

	return true;
}

bool ezcfg_html_set_version_minor(struct ezcfg_html *html, unsigned short minor)
{
	//struct ezcfg *ezcfg;

	ASSERT(html != NULL);

	//ezcfg = html->ezcfg;

	html->version_minor = minor;

	return true;
}

int ezcfg_html_get_max_nodes(struct ezcfg_html *html)
{
	//struct ezcfg *ezcfg;

	ASSERT(html != NULL);

	//ezcfg = html->ezcfg;

	/* html, head, body */
	return ezcfg_xml_get_max_elements(html->xml) - 6;
}

bool ezcfg_html_set_max_nodes(struct ezcfg_html *html, const int max_nodes)
{
	//struct ezcfg *ezcfg;

	ASSERT(html != NULL);

	//ezcfg = html->ezcfg;

	/* html, head, body */
	return ezcfg_xml_set_max_elements(html->xml, max_nodes + 6);
}

int ezcfg_html_set_root(struct ezcfg_html *html, const char *name)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;
	struct ezcfg_xml_element *elem;

	ASSERT(html != NULL);
	ASSERT(html->xml != NULL);
	ASSERT(name != NULL);

	ezcfg = html->ezcfg;
	xml = html->xml;

	elem = ezcfg_xml_element_new(xml, name, NULL);

	if (elem == NULL) {
		err(ezcfg, "Cannot initialize html root\n");
		return -1;
	}

	html->root_index = ezcfg_xml_add_element(xml, -1, -1, elem);
	if (html->root_index < 0) {
		ezcfg_xml_element_delete(elem);
	}
	return html->root_index;
}

bool ezcfg_html_add_root_attribute(struct ezcfg_html *html, const char *name, const char *value, int pos) {
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;
	struct ezcfg_xml_element *elem;

	ASSERT(html != NULL);
	ASSERT(name != NULL);
	ASSERT(value != NULL);

	ezcfg = html->ezcfg;
	xml = html->xml;

	if (html->root_index < 0) {
		err(ezcfg, "no html root element!\n");
		return false;
	}

	elem = ezcfg_xml_get_element_by_index(xml, html->root_index);
	if (elem == NULL) {
		err(ezcfg, "html root element not set correct!\n");
		return false;
	}

	return ezcfg_xml_element_add_attribute(xml, elem, name, value, pos);
}

int ezcfg_html_set_head(struct ezcfg_html *html, const char *name)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;
	struct ezcfg_xml_element *root, *elem;

	ASSERT(html != NULL);
	ASSERT(html->xml != NULL);
	ASSERT(name != NULL);

	ezcfg = html->ezcfg;
	xml = html->xml;

	elem = ezcfg_xml_element_new(xml, name, NULL);

	if (elem == NULL) {
		err(ezcfg, "Cannot initialize html head\n");
		return -1;
	}

	if (html->root_index < 0) {
		err(ezcfg, "no root for html head\n");
		return -1;
	}

	root = ezcfg_xml_get_element_by_index(xml, html->root_index);
	if (root == NULL) {
		err(ezcfg, "html root is invalid\n");
		return -1;
	}

	html->head_index = ezcfg_xml_add_element(xml, html->root_index, -1, elem);
	if (html->head_index < 0) {
		ezcfg_xml_element_delete(elem);
	}
	return html->head_index;
}

int ezcfg_html_get_head_index(struct ezcfg_html *html)
{
	//struct ezcfg *ezcfg;

	ASSERT(html != NULL);

	//ezcfg = html->ezcfg;
	return html->head_index;
}

int ezcfg_html_add_head_child(struct ezcfg_html *html, const int pi, const int si, const char *name, const char *content)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;
	struct ezcfg_xml_element *parent, *sibling, *elem;
	int ei;

	ASSERT(html != NULL);
	ASSERT(html->xml != NULL);
	ASSERT(name != NULL);
	ASSERT(pi > 0);

	ezcfg = html->ezcfg;
	xml = html->xml;

	elem = ezcfg_xml_element_new(xml, name, content);

	if (elem == NULL) {
		err(ezcfg, "Cannot initialize html head\n");
		return -1;
	}

	if (html->head_index < 0) {
		err(ezcfg, "no head for html child\n");
		return -1;
	}

	parent = ezcfg_xml_get_element_by_index(xml, pi);
	if (parent == NULL) {
		err(ezcfg, "html parent index is invalid\n");
		return -1;
	}

	sibling = NULL;
	if (si > 0) {
		sibling = ezcfg_xml_get_element_by_index(xml, si);
		if (sibling == NULL) {
			err(ezcfg, "html sibling index is invalid\n");
			return -1;
		}
	}

	ei = ezcfg_xml_add_element(xml, pi, si, elem);
	if (ei < 0) {
		ezcfg_xml_element_delete(elem);
	}
	return ei;
}

bool ezcfg_html_add_head_child_attribute(struct ezcfg_html *html, int ei, const char *name, const char *value, int pos) {
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;
	struct ezcfg_xml_element *elem;

	ASSERT(html != NULL);
	ASSERT(ei > 0);
	ASSERT(name != NULL);
	ASSERT(value != NULL);

	ezcfg = html->ezcfg;
	xml = html->xml;

	if (html->root_index < 0) {
		err(ezcfg, "no html root element!\n");
		return false;
	}

	elem = ezcfg_xml_get_element_by_index(xml, ei);
	if (elem == NULL) {
		err(ezcfg, "html element index is invalid!\n");
		return false;
	}

	return ezcfg_xml_element_add_attribute(xml, elem, name, value, pos);
}

int ezcfg_html_set_body(struct ezcfg_html *html, const char *name)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;
	struct ezcfg_xml_element *root, *elem;

	ASSERT(html != NULL);
	ASSERT(html->xml != NULL);
	ASSERT(name != NULL);

	ezcfg = html->ezcfg;
	xml = html->xml;

	elem = ezcfg_xml_element_new(xml, name, NULL);

	if (elem == NULL) {
		err(ezcfg, "Cannot initialize html body\n");
		return -1;
	}

	if (html->root_index < 0) {
		err(ezcfg, "no root for html body\n");
		return -1;
	}

	root = ezcfg_xml_get_element_by_index(xml, html->root_index);
	if (root == NULL) {
		err(ezcfg, "html root is invalid\n");
		return -1;
	}

	html->body_index = ezcfg_xml_add_element(xml, html->root_index, html->head_index, elem);
	if (html->body_index < 0) {
		ezcfg_xml_element_delete(elem);
	}
	return html->body_index;
}

int ezcfg_html_get_body_index(struct ezcfg_html *html)
{
	//struct ezcfg *ezcfg;

	ASSERT(html != NULL);

	//ezcfg = html->ezcfg;
	return html->body_index;
}

int ezcfg_html_add_body_child(struct ezcfg_html *html, const int pi, const int si, const char *name, const char *content)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;
	struct ezcfg_xml_element *parent, *sibling, *elem;
	int ei;

	ASSERT(html != NULL);
	ASSERT(html->xml != NULL);
	ASSERT(name != NULL);
	ASSERT(pi > 0);

	ezcfg = html->ezcfg;
	xml = html->xml;

	elem = ezcfg_xml_element_new(xml, name, content);

	if (elem == NULL) {
		err(ezcfg, "Cannot initialize html body\n");
		return -1;
	}

	if (html->body_index < 0) {
		err(ezcfg, "no body for html child\n");
		return -1;
	}

	parent = ezcfg_xml_get_element_by_index(xml, pi);
	if (parent == NULL) {
		err(ezcfg, "html parent index is invalid\n");
		return -1;
	}

	sibling = NULL;
	if (si > 0) {
		sibling = ezcfg_xml_get_element_by_index(xml, si);
		if (sibling == NULL) {
			err(ezcfg, "html sibling index is invalid\n");
			return -1;
		}
	}

	ei = ezcfg_xml_add_element(xml, pi, si, elem);
	if (ei < 0) {
		ezcfg_xml_element_delete(elem);
	}
	return ei;
}

bool ezcfg_html_add_body_child_attribute(struct ezcfg_html *html, int ei, const char *name, const char *value, int pos) {
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;
	struct ezcfg_xml_element *elem;

	ASSERT(html != NULL);
	ASSERT(ei > 0);
	ASSERT(name != NULL);
	ASSERT(value != NULL);

	ezcfg = html->ezcfg;
	xml = html->xml;

	if (html->root_index < 0) {
		err(ezcfg, "no html root element!\n");
		return false;
	}

	elem = ezcfg_xml_get_element_by_index(xml, ei);
	if (elem == NULL) {
		err(ezcfg, "html element index is invalid!\n");
		return false;
	}

	return ezcfg_xml_element_add_attribute(xml, elem, name, value, pos);
}

int ezcfg_html_get_element_index(struct ezcfg_html *html, const int pi, const int si, char *name)
{
	//struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;

	ASSERT(html != NULL);
	ASSERT(html->xml != NULL);
	ASSERT(name != NULL);
	ASSERT(pi >= -1);

	//ezcfg = html->ezcfg;
	xml = html->xml;

	return ezcfg_xml_get_element_index(xml, pi, si, name);
}

char *ezcfg_html_get_element_content_by_index(struct ezcfg_html *html, const int i)
{
	//struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;

	ASSERT(html != NULL);
	ASSERT(html->xml != NULL);
	ASSERT(i > 0);

	//ezcfg = html->ezcfg;
	xml = html->xml;

	return ezcfg_xml_get_element_content_by_index(xml, i);
}

bool ezcfg_html_parse(struct ezcfg_html *html, char *buf, int len)
{
	/* buf must be \0-terminated */
	//struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;

	ASSERT(html != NULL);
	ASSERT(html->xml != NULL);
	ASSERT(buf != NULL);
	ASSERT(len > 0);

	//ezcfg = html->ezcfg;
	xml = html->xml;

	if (ezcfg_xml_parse(xml, buf, len) == false) {
		return false;
	}

	html->root_index = 0;

	if (ezcfg_xml_get_element_index(xml, -1, -1, EZCFG_HTML_HTML_ELEMENT_NAME) != 0) {
		return false;
	}

	html->body_index = ezcfg_xml_get_element_index(xml, html->root_index, -1, EZCFG_HTML_BODY_ELEMENT_NAME);

	if (html->body_index < 1) {
		return false;
	}

	return true;
}

int ezcfg_html_get_message_length(struct ezcfg_html *html)
{
	//struct ezcfg *ezcfg;
	char buf[256];
	int ret;

	ASSERT(html != NULL);

	//ezcfg = html->ezcfg;

	snprintf(buf, sizeof(buf), "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML %d.%02d//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n", html->version_major, html->version_minor);

	ret = ezcfg_xml_get_message_length(html->xml);

	if (ret < 0) {
		return ret;
	}
	else {
		return ret+strlen(buf);
	}
}

int ezcfg_html_write_message(struct ezcfg_html *html, char *buf, int len)
{
	//struct ezcfg *ezcfg;
	int dtd_len, ret;

	ASSERT(html != NULL);
	ASSERT(buf != NULL);
	ASSERT(len > 0);

	//ezcfg = html->ezcfg;

	/* first write HTML DTD */
	snprintf(buf, len, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML %d.%02d//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n", html->version_major, html->version_minor);
	dtd_len = strlen(buf);

	/* then write HTML document */
	ret = ezcfg_xml_write_message(html->xml, buf+dtd_len, len-dtd_len);
	if (ret < 0) {
		return ret;
	}
	else {
		return ret+dtd_len;
	}
}
