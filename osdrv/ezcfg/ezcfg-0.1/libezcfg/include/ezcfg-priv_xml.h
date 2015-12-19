/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_xml.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-13   0.1       Split it from ezcfg-private.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_XML_H_
#define _EZCFG_PRIV_XML_H_

#include "ezcfg-types.h"

/* xml/xml.c */
void ezcfg_xml_delete(struct ezcfg_xml *xml);
struct ezcfg_xml *ezcfg_xml_new(struct ezcfg *ezcfg);
void ezcfg_xml_reset_attributes(struct ezcfg_xml *xml);
int ezcfg_xml_get_max_elements(struct ezcfg_xml *xml);
bool ezcfg_xml_set_max_elements(struct ezcfg_xml *xml, const int max_elements);
int ezcfg_xml_get_num_elements(struct ezcfg_xml *xml);
int ezcfg_xml_normalize_document(struct ezcfg_xml *xml, char *buf, int len);
bool ezcfg_xml_parse(struct ezcfg_xml *xml, char *buf, int len);
int ezcfg_xml_get_message_length(struct ezcfg_xml *xml);
int ezcfg_xml_write_message(struct ezcfg_xml *xml, char *buf, int len);
void ezcfg_xml_element_delete(struct ezcfg_xml_element *elem);
struct ezcfg_xml_element *ezcfg_xml_element_new(
        struct ezcfg_xml *xml,
        const char *name, const char *content);
int ezcfg_xml_add_element(
        struct ezcfg_xml *xml,
        const int pi,
        const int si,
        struct ezcfg_xml_element *elem);
int ezcfg_xml_get_element_index(struct ezcfg_xml *xml, const int pi, const int si, char *name);
bool ezcfg_xml_element_add_attribute(
        struct ezcfg_xml *xml,
        struct ezcfg_xml_element *elem,
        const char *name, const char *value, int pos);
struct ezcfg_xml_element *ezcfg_xml_get_element_by_index(struct ezcfg_xml *xml, const int index);
bool ezcfg_xml_set_element_content_by_index(struct ezcfg_xml *xml, const int index, const char *content);
char *ezcfg_xml_get_element_content_by_index(struct ezcfg_xml *xml, const int index);
int ezcfg_xml_get_element_etag_index_by_index(struct ezcfg_xml *xml, const int index);

#endif /* _EZCFG_PRIV_XML_H_ */
