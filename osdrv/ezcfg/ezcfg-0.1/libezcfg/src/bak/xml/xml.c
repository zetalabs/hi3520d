/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : xml/xml.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-08-01   0.1       Write it from scratch
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

struct elem_attribute {
  char *prefix;
  char *name;
  char *value;
  struct elem_attribute *next;
};

struct ezcfg_xml_element {
  char *prefix;
  char *name;
  char *content;
  int etag_index; /* etag index in root stack */
  struct elem_attribute *attr_head;
  struct elem_attribute *attr_tail;
};

struct ezcfg_xml {
  struct ezcfg *ezcfg;
  /* prolog part of the xml document */
  unsigned short version_major;
  unsigned short version_minor;
  unsigned short num_encoding_names; /* Number of supported encoding names */
  const char **encoding_names;
  unsigned short encoding_name_index;
  bool standalone;
  /* element part of the xml document */
  int max_elements;
  int num_elements;
  struct ezcfg_xml_element **root; /* stack for representing xml doc tree */
};

/* private function previous declaration */
static int parse_xml_document_element(
				      struct ezcfg_xml *xml,
				      char **pbuf,
				      int *plen,
				      const int pi,
				      const int si);

const char *default_character_encoding_names[] = {
  /* bad encoding string */
  NULL,
  /* http://www.iana.org/assignments/character-sets */
  "UTF-8",
  "UTF-16",
  "ISO-10646-UCS-2",
  "ISO-10646-UCS-4",
  "ISO-8859-1",
  "ISO-8859-2",
  "ISO-8859-3",
  "ISO-8859-4",
  "ISO-8859-5",
  "ISO-8859-6",
  "ISO-8859-7",
  "ISO-8859-8",
  "ISO-8859-9",
  "ISO-8859-10",
  "ISO-2022-JP",
  "Shift_JIS",
  "EUC-JP", 
  "GB2312",
};

/* Private functions */
static void delete_attribute(struct elem_attribute *a)
{
  ASSERT(a != NULL);

  /* prefix and name point to the same place 
   * if prefix is not NULL, free(prefix) 
   * else free(name) */
  if (a->prefix != NULL) {
    free(a->prefix);
  }
  else if (a->name != NULL) {
    free(a->name);
  }

  if (a->value != NULL) {
    free(a->value);
  }

  free(a);
}

static void delete_elements(struct ezcfg_xml *xml)
{
  int i;
  struct ezcfg_xml_element *p;
  for (i=0; i < xml->num_elements; i++) {
    p = xml->root[i];
    if (p == NULL)
      continue;

    /* clean etag pointer */
    if (p->etag_index > 0) {
      xml->root[p->etag_index] = NULL;
    }

    ezcfg_xml_element_delete(p);
    xml->root[i] = NULL;
  }
  xml->num_elements = 0;
}

static bool is_white_space(char c) {
  if (c == 0x20 || c == 0x09 || c == 0x0d || c == 0x0a) {
    return true;
  }
  return false;
}

static bool is_equal_sign(char c) {
  if (c == '=') {
    return true;
  }
  return false;
}

/* Char ::= #x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF] */
static bool is_char(char c) {
  if (c == 0x09 || c == 0x0d || c == 0x0a || c > 0x1f) {
    return true;
  }
  return false;
}


/* FIXME! not considerate non-ASCII charactors */
/* NameStartChar ::= ":" | [A-Z] | "_" | [a-z] | [#xC0-#xD6] | [#xD8-#xF6] | [#xF8-#x2FF] | [#x370-#x37D] | [#x37F-#x1FFF] | [#x200C-#x200D] | [#x2070-#x218F] | [#x2C00-#x2FEF] | [#x3001-#xD7FF] | [#xF900-#xFDCF] | [#xFDF0-#xFFFD] | [#x10000-#xEFFFF] */
static bool is_name_start_char(char c) {
  if (isalpha(c) != 0 ||
      c == ':' ||
      c == '_') {
    return true;
  }
  return false;
}

/* NameChar ::= NameStartChar | "-" | "." | [0-9] | #xB7 | [#x0300-#x036F] | [#x203F-#x2040] */
static bool is_name_char(char c) {
  if (is_name_start_char(c) ||
      c == '-' ||
      c == '.' ||
      isdigit(c) != 0) {
    return true;
  }
  return false;
}

static bool is_xml_element_start_tag(char *buf) {
  if (buf[0] == '<' && is_name_start_char(buf[1]) == true) {
    char *p = strstr(buf, ">"); /* find the start tag end part ">" */
    if (p == NULL) {
      return false;
    }
    return true;
  }
  return false;
}

static bool is_xml_element_end_tag(char *buf) {
  if (strncmp(buf, "</", 2) == 0 && is_name_start_char(buf[2]) == true) {
    char *p = strstr(buf, ">"); /* find the end tag end part ">" */
    if (p == NULL) {
      return false;
    }
    return true;
  }
  return false;
}

static bool is_xml_element_empty_element_tag(char *buf) {
  if (is_xml_element_start_tag(buf) == true) {
    char *p = strstr(buf, ">"); /* find the empty element tag end part "/>" */
    if (p == NULL) {
      return false;
    }
    if (*(p-1) == '/') {
      return true;
    }
  }
  return false;
}

static bool is_xml_entity_reference(char *buf) {
  char *p;
  if (buf[0] == '&' && is_name_start_char(buf[1]) == true) {
    p = buf+2;
    while (is_name_char(*p) == true) p++;
    if (*p == ';') {
      return true;
    }
  }
  return false;
}

/* CharRef ::= '&#' [0-9]+ ';' | '&#x' [0-9a-fA-F]+ ';' */

static bool is_xml_character_reference(char *buf) {
  char *p;
  if (strncmp(buf, "&#", 2) == 0) {
    p = buf+2;
    if (isdigit(*p) != 0) {
      while (isdigit(*p) != 0) p++;
      if (*p == ';') {
	return true;
      }
    }
    else if(*p == 'x') {
      p++;
      if (isxdigit(*p) != 0) {
	while (isxdigit(*p) != 0) p++;
	if (*p == ';') {
	  return true;
	}
      }
    }
  }
  return false;
}

static bool is_xml_cdata_section(char *buf) {
  if (strncmp(buf, "<![CDATA[", 9) == 0 && is_char(buf[9]) == true) {
    char *p = strstr(buf+10, "]]>"); /* find the cdata section tag end part "]]>" */
    if (p == NULL) {
      return false;
    }
    while (p > buf+9) {
      if (is_char(*p) == false) {
	return false;
      }
      p--;
    }
    return true;
  }
  return false;
}

static bool is_xml_declaration(char *buf) {
  if (strncmp(buf, "<?xml", 5) == 0 && is_white_space(buf[5])) {
    char *p = strstr(buf, "?>"); /* find the XML declaration tag end part "?>" */
    if (p == NULL) {
      return false;
    }
    return true;
  }
  return false;
}

static bool is_xml_document_type_declaration(char *buf) {
  if (strncmp(buf, "<!DOCTYPE", 9) == 0 && is_white_space(buf[9])) {
    char *p = strstr(buf, ">"); /* find the XML document type declaration tag end part ">" */
    if (p == NULL) {
      return false;
    }
    return true;
  }
  return false;
}

static bool is_xml_text_declaration(char *buf) {
  if (strncmp(buf, "<?xml", 5) == 0 && is_white_space(buf[5])) {
    char *p = strstr(buf, "?>"); /* find the XML text declaration tag end part "?>" */
    if (p == NULL) {
      return false;
    }
    return true;
  }
  return false;
}

static bool is_xml_comment(char *buf) {
  if (strncmp(buf, "<!--", 4) == 0) {
    char *p = strstr(buf, "-->"); /* find the XML comment tag end part "-->" */
    if (p == NULL) {
      return false;
    }
    if (*(p-1) == '-') {
      /* not allow "--->" as a XML comment tag end part */
      return false;
    }
    return true;
  }
  return false;
}

static bool is_xml_processing_instruction(char *buf) {
  if (strncmp(buf, "<?", 2) == 0 && is_name_start_char(buf[2]) == true) {
    char *p = buf+2;
    if(strncasecmp(p, "xml", 3) == 0 && (is_white_space(p[3]) || p[3] == '?')) {
      return false;
    }
    p = strstr(buf, "?>"); /* find the processing instruction tag end part "?>" */
    if (p == NULL) {
      return false;
    }
    return true;
  }
  return false;
}

static bool is_xml_misc(char *buf) {
  if (is_xml_comment(buf) == true) {
    return true;
  }
  else if (is_xml_processing_instruction(buf) == true) {
    return true;
  }
  else if (is_white_space(*buf) == true) {
    return true;
  }
  return false;
}

static bool is_xml_element(char *buf) {
  if (is_xml_element_start_tag(buf) == true) {
    return true;
  }
  else if (is_xml_element_empty_element_tag(buf) == true) {
    return true;
  }
  return false;
}

static bool is_xml_reference(char *buf) {
  if (is_xml_entity_reference(buf) == true) {
    return true;
  }
  else if (is_xml_character_reference(buf) == true) {
    return true;
  }
  return false;
}

static bool is_xml_content_markup(char *buf) {
  if (is_xml_element(buf) == true) {
    return true;
  }
  else if (is_xml_reference(buf) == true) {
    return true;
  }
  else if (is_xml_cdata_section(buf) == true) {
    return true;
  }
  else if (is_xml_processing_instruction(buf) == true) {
    return true;
  }
  else if (is_xml_comment(buf) == true) {
    return true;
  }
  return false;
}

/* XML 1.0 fifth edition section 2.4 */
static bool is_xml_markup(char *buf) {
  if (is_xml_element_start_tag(buf) == true) {
    return true;
  }
  else if (is_xml_element_end_tag(buf) == true) {
    return true;
  }
  else if (is_xml_element_empty_element_tag(buf) == true) {
    return true;
  }
  else if (is_xml_entity_reference(buf) == true) {
    return true;
  }
  else if (is_xml_character_reference(buf) == true) {
    return true;
  }
  else if (is_xml_comment(buf) == true) {
    return true;
  }
  else if (is_xml_cdata_section(buf) == true) {
    return true;
  }
  else if (is_xml_document_type_declaration(buf) == true) {
    return true;
  }
  else if (is_xml_processing_instruction(buf) == true) {
    return true;
  }
  else if (is_xml_declaration(buf) == true) {
    return true;
  }
  else if (is_xml_text_declaration(buf) == true) {
    return true;
  }
  return false;
}

static char *skip_white_space(char *buf)
{
  char *p = buf;
  while (is_white_space(*p) == true) p++;
  return p;
}

static unsigned short find_encoding_name_index(struct ezcfg_xml *xml, const char *enc_name)
{
  //struct ezcfg *ezcfg;
  int i;

  ASSERT(xml != NULL);
  ASSERT(enc_name != NULL);

  //ezcfg = xml->ezcfg;

  for (i = xml->num_encoding_names; i > 0; i--) {
    if (strcasecmp(xml->encoding_names[i], enc_name) == 0)
      return i;
  }
  return 0;
}

static bool parse_xml_comment(struct ezcfg_xml *xml, char **pbuf, int *plen)
{
  //struct ezcfg *ezcfg;
  char *p, *buf;
  int len;

  //ezcfg = xml->ezcfg;

  buf = *pbuf;
  len = *plen;

  /* Comment ::= '<!--' ((Char - '-') | ('-' (Char - '-')))* '-->' */
  p = buf+4; /* skip "<!--" */

  /* match comment end tag */
  p = strstr(p, "-->");
  if (p == NULL) {
    return false;
  }
  p += 3; /* skip "-->" */

  /* skip white space */
  p = skip_white_space(p);

  *pbuf = p;
  *plen = len - (buf - p);

  return true;
}

static bool parse_xml_processing_instruction(struct ezcfg_xml *xml, char **pbuf, int *plen)
{
  //struct ezcfg *ezcfg;
  char *p, *buf;
  int len;

  //ezcfg = xml->ezcfg;

  buf = *pbuf;
  len = *plen;

  /* Processing Instructions */
  /* PI ::= '<?' PITarget (S (Char* - (Char* '?>' Char*)))? '?>'
   * PITarget ::= Name - (('X' | 'x') ('M' | 'm') ('L' | 'l'))
   */
  p = buf+2; /* skip "<?" */

  /* match PI end tag */
  p = strstr(p, "?>");
  if (p == NULL) {
    return false;
  }
  p += 2; /* skip "?>" */

  /* skip white space */
  p = skip_white_space(p);

  *pbuf = p;
  *plen = len - (buf - p);

  return true;
}

static bool parse_xml_document_misc(struct ezcfg_xml *xml, char **pbuf, int *plen)
{
  //struct ezcfg *ezcfg;
  char *p, *buf;
  int len;

  //ezcfg = xml->ezcfg;

  buf = *pbuf;
  len = *plen;

  p = *pbuf;

  /* Misc ::= Comment | PI | S */
  if (is_xml_comment(p) == true) {
    if ( parse_xml_comment(xml, pbuf, plen) == false) {
      return false;
    }
  }
  else if (is_xml_processing_instruction(p) == true) {
    if ( parse_xml_processing_instruction(xml, pbuf, plen) == false) {
      return false;
    }
  }
  else if (is_white_space(*p) == true) {
    p = skip_white_space(p);
    *pbuf = p;
    *plen = len - (p - buf);
  }
  return true;
}

static bool parse_xml_prolog_xmldecl(struct ezcfg_xml *xml, char **pbuf, int *plen)
{
  //struct ezcfg *ezcfg;
  char *p, *buf;
  int len;

  //ezcfg = xml->ezcfg;

  buf = *pbuf;
  len = *plen;

  /* XMLDecl */
  p = buf+5; /* skip "<?xml" */
  p = skip_white_space(p);
  if (strncmp(p, "version", 7) == 0) {
    /* get version info */
    p += 7; /* skip "version" */
    /* XML 1.0 fifth edition. Eq ::= S? '=' S? */
    p = skip_white_space(p);
    if (is_equal_sign(*p) == true) {
      p++;
      p = skip_white_space(p);
      /* VersionNum part, force to 1.0 if incorrect */
      if (p[0] == '1' && p[1] == '.') {
	xml->version_major = 1;
	p += 2; /* skip "1." */
	xml->version_minor = 0;
	while (isdigit(*p) != 0) {
	  xml->version_minor *= 10;
	  xml->version_minor += (*p - '0');
	}
      }
      p = skip_white_space(p);
    }
  }
  if (strncmp(p, "encoding", 8) == 0) {
    /* get encoding info */
    p += 8; /* skip "encoding" */
    /* XML 1.0 fifth edition. Eq ::= S? '=' S? */
    p = skip_white_space(p);
    if (is_equal_sign(*p) == true) {
      p++;
      p = skip_white_space(p);
      /* EncName part, force to UTF-8 if incorrect */
      if (p[0] == '\"' || p[0] == '\'') {
	char str_tag;
	char *enc_name;

	str_tag = p[0];
	p++;
	enc_name = p;
	while (*p != str_tag && p < buf+len) p++;
	if (*p == str_tag) {
	  *p = '\0';
	  xml->encoding_name_index = find_encoding_name_index(xml, enc_name);
	  *p = str_tag;
	}
      }
      p = skip_white_space(p);
    }
  }
  if (strncmp(p, "standalone", 10) == 0) {
    /* get standalone info */
    p += 10; /* skip "standalone" */
    /* XML 1.0 fifth edition. Eq ::= S? '=' S? */
    p = skip_white_space(p);
    if (is_equal_sign(*p) == true) {
      p++;
      p = skip_white_space(p);
      /* EncName part, force to UTF-8 if incorrect */
      if (p[0] == '\"' || p[0] == '\'') {
	char str_tag;
	char *bool_str;

	str_tag = p[0];
	p++;
	bool_str = p;
	while (*p != str_tag && p < buf+len) p++;
	if (*p == str_tag) {
	  *p = '\0';
	  if (strcmp(bool_str, "yes") == 0) {
	    xml->standalone = true;
	  }
	  *p = str_tag;
	}
      }
      p = skip_white_space(p);
    }
  }

  /* match XMLDecl end tag */
  p = strstr(p, "?>");
  if (p == NULL) {
    return false;
  }
  p += 2; /* skip "?>" */

  /* skip white space */
  p = skip_white_space(p);

  *pbuf = p;
  *plen = len - (buf - p);

  return true;
}

static bool parse_xml_prolog_doctypedecl(struct ezcfg_xml *xml, char **pbuf, int *plen)
{
  //struct ezcfg *ezcfg;
  char *p, *buf;
  int len;

  //ezcfg = xml->ezcfg;

  buf = *pbuf;
  len = *plen;

  /* doctypedecl */
  p = buf+9; /* skip "<!DOCTYPE" */
  while (is_white_space(*p) == true) p++;

  /* match doctypedecl end tag */
  p = strstr(p, ">");
  if (p == NULL) {
    return false;
  }
  p += 1; /* skip ">" */

  /* skip white space */
  p = skip_white_space(p);

  *pbuf = p;
  *plen = len - (buf - p);

  return true;
}

static bool parse_xml_document_prolog(struct ezcfg_xml *xml, char **pbuf, int *plen)
{
  //struct ezcfg *ezcfg;

  ASSERT(xml != NULL);
  ASSERT(pbuf != NULL);
  ASSERT(plen != NULL);
  ASSERT(*pbuf != NULL);
  ASSERT(*plen > 0);

  //ezcfg = xml->ezcfg;

  /* prolog ::= XMLDecl? Misc* (doctypedecl Misc*)? */ 

  /* check prolog XMLDecl? */
  if (is_xml_declaration(*pbuf) == true) {
    if (parse_xml_prolog_xmldecl(xml, pbuf, plen) == false) {
      return false;
    }
  }

  /* check prolog Misc* part */
  while(is_xml_misc(*pbuf) == true) {
    if (parse_xml_document_misc(xml, pbuf, plen) == false) {
      return false;
    }
  }

  /* check prolog (doctypedecl Misc*)? */
  if (is_xml_document_type_declaration(*pbuf) == true) {
    if (parse_xml_prolog_doctypedecl(xml, pbuf, plen) == false) {
      return false;
    }

    while(is_xml_misc(*pbuf) == true) {
      if (parse_xml_document_misc(xml, pbuf, plen) == false) {
	return false;
      }
    }
  }

  return true;
}

/* return element index in xml->root */
static int parse_element_empty_elem_tag(
					struct ezcfg_xml *xml,
					char **pbuf,
					int *plen,
					const int pi,
					const int si) {

  struct ezcfg *ezcfg;
  char *p, *name, *value;
  char *start_tag_end;
  struct ezcfg_xml_element *elem;
  char c;
  char *buf;
  int len;
  int ei;

  ASSERT(xml != NULL);
  ASSERT(pbuf != NULL);
  ASSERT(plen != NULL);
  ASSERT(*pbuf != NULL);
  ASSERT(*plen > 0);

  ezcfg = xml->ezcfg;

  buf = *pbuf;
  len = *plen;

  /* It's an EmptyElemTag */
  /* EmptyElemTag ::= '<' Name (S Attribute)* S? '/>' */

  /* Name part */
  name = *pbuf+1; /* skip '<' */
  start_tag_end = strstr(name, "/>");
  if (start_tag_end == NULL) {
    return -1;
  }

  p = name+1;
  while(is_name_char(*p) == true && p < start_tag_end) p++;
  c = *p;
  *p = '\0';
  elem = ezcfg_xml_element_new(xml, name, NULL);
  if (elem == NULL) {
    err(ezcfg, "Cannot initialize xml root element\n");
    return -1;
  }

  *p = c;

  while(p < start_tag_end) {
    /* skip S */
    p = skip_white_space(p);

    /* Attribute part */
    /* Attribute ::= Name Eq AttValue */
    if (is_name_start_char(*p) == true) {
      name = p;
      p++;
      while(is_name_char(*p) == true) p++;

      if (is_white_space(*p) == true) { *p = '\0'; p++; }

      if (is_equal_sign(*p)) { *p = '\0'; p++; }
      else { ei = -1; goto exit; }

      p = skip_white_space(p);
      if (*p == '\"' || *p == '\'') {
	c = *p;
	value = p+1; /* skip \" or \' */
	p = strchr(value, c); /* find end tag for string */
	if (p == NULL) {
	  ei = -1;
	  goto exit;
	}
	*p = '\0';
	p++;
	if (ezcfg_xml_element_add_attribute(xml, elem, name, value, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL) == false) {
	  ei = -1;
	  goto exit;
	}
      }
      else { ei = -1; goto exit; }
    }
  }
  p += 2; /* skip "/>" */

  /* skip white space */
  p = skip_white_space(p);

  *pbuf = p;
  *plen = len - (p - buf);
  ei = ezcfg_xml_add_element(xml, pi, si, elem);
 exit:
  if (ei < 0) {
    ezcfg_xml_element_delete(elem);
  }
  return ei;
}

static int parse_element_stag(
			      struct ezcfg_xml *xml,
			      char **pbuf,
			      int *plen,
			      const int pi,
			      const int si) {

  struct ezcfg *ezcfg;
  char *p, *name, *value;
  char *start_tag_end;
  struct ezcfg_xml_element *elem;
  char c;
  char *buf;
  int len;
  int ei;

  ASSERT(xml != NULL);
  ASSERT(pbuf != NULL);
  ASSERT(plen != NULL);
  ASSERT(*pbuf != NULL);
  ASSERT(*plen > 0);

  ezcfg = xml->ezcfg;

  buf = *pbuf;
  len = *plen;

  /* STag ::= '<' Name (S Attribute)* S? '>' */

  /* Name part */
  name = *pbuf+1; /* skip '<' */
  start_tag_end = strstr(name, ">");
  if (start_tag_end == NULL) {
    return -1;
  }

  p = name+1;
  while(is_name_char(*p) == true && p < start_tag_end) p++;

  c = *p;
  *p = '\0';
  elem = ezcfg_xml_element_new(xml, name, NULL);
  *p = c;

  if (elem == NULL) {
    err(ezcfg, "Cannot initialize xml root element\n");
    return -1;
  }

  while(p < start_tag_end) {
    /* skip S */
    p = skip_white_space(p);

    /* Attribute part */
    /* Attribute ::= Name Eq AttValue */
    if (is_name_start_char(*p) == false) {
      /* format error! not Name start */
      ei = -1;
      goto exit;
    }

    name = p;
    p++;
    while(is_name_char(*p) == true) p++;

    if (is_white_space(*p) == true) { *p = '\0'; p++; }

    if (is_equal_sign(*p) == false) {
      /* format error! not Eq */
      ei = -1;
      goto exit;
    }

    *p = '\0';
    p++;

    p = skip_white_space(p);
    if (*p != '\"' && *p != '\'') {
      /* format error! not start with \" or \' */
      ei = -1;
      goto exit;
    }

    c = *p;
    value = p+1; /* skip \" or \' */
    p = strchr(value, c); /* find end tag for string */
    if (p == NULL) {
      ei = -1;
      goto exit;
    }
    *p = '\0';
    p++;
    if (ezcfg_xml_element_add_attribute(xml, elem, name, value, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL) == false) {
      ei = -1;
      goto exit;
    }
  }
  p++; /* skip '>' */

  /* skip white space */
  p = skip_white_space(p);

  *pbuf = p;
  *plen = len - (p - buf);
  ei = ezcfg_xml_add_element(xml, pi, si, elem);
 exit:
  if (ei < 0) {
    ezcfg_xml_element_delete(elem);
  }
  return ei;
}

static bool parse_xml_char_data(
				struct ezcfg_xml *xml,
				char **pbuf,
				int *plen,
				const int pi) {

  //struct ezcfg *ezcfg;
  struct ezcfg_xml_element *elem;
  char *p, *buf, *content;
  char c;
  int len;

  ASSERT(xml != NULL);
  ASSERT(xml->root != NULL);

  //ezcfg = xml->ezcfg;

  elem = xml->root[pi];

  if (elem == NULL) {
    return false;
  }

  buf = *pbuf;
  len = *plen;

  p = buf;
  while (is_xml_markup(p) == false) p++;
  c = *p;
  *p = '\0';

  content = strdup(*pbuf);
  *p = c;

  if (content == NULL) {
    return false;
  }

  if (elem->content != NULL) {
    free(elem->content);
  }
  elem->content = content;

  /* skip white space */
  p = skip_white_space(p);

  *pbuf = p;
  *plen = len - (p - buf);

  return true;
}

static bool parse_xml_reference(struct ezcfg_xml *xml, char **pbuf, int *plen)
{
  char *p, *buf;
  int len;

  buf = *pbuf;
  len = *plen;

  /* just skip it */
  p = strchr(buf, ';');
  if (p == NULL) {
    return false;
  }
  p++; /* skip ';' */

  /* skip white space */
  p = skip_white_space(p);

  *pbuf = p;
  *plen = len - (p - buf);

  return true;
}

static bool parse_xml_cdata_section(struct ezcfg_xml *xml, char **pbuf, int *plen)
{
  char *p, *buf;
  int len;

  buf = *pbuf;
  len = *plen;

  /* just skip it */
  p = strstr(buf, "]]>");
  if (p == NULL) {
    return false;
  }
  p += 3; /* skip "]]>" */

  /* skip white space */
  p = skip_white_space(p);

  *pbuf = p;
  *plen = len - (p - buf);

  return true;
}

static int parse_element_content(
				 struct ezcfg_xml *xml,
				 char **pbuf,
				 int *plen,
				 const int pi,
				 const int si) {

  //struct ezcfg *ezcfg;
  int ci; /* current element index */

  ASSERT(xml != NULL);

  //ezcfg = xml->ezcfg;
  ci = si;

  /* content ::= CharData? ((element | Reference | CDSect | PI | Comment) CharData?)* */
  if (is_xml_markup(*pbuf) == false) {
    if (parse_xml_char_data(xml, pbuf, plen, pi) == false) {
      return -1;
    }
  }

  while(is_xml_content_markup(*pbuf) == true) {
    if (is_xml_element(*pbuf) == true) {
      ci = parse_xml_document_element(xml, pbuf, plen, pi, ci);
      if (ci == -1) {
	return -1;
      }
    }
    else if (is_xml_reference(*pbuf) == true) {
      if (parse_xml_reference(xml, pbuf, plen) == false) {
	return -1;
      }
    }
    else if (is_xml_cdata_section(*pbuf) == true) {
      if (parse_xml_cdata_section(xml, pbuf, plen) == false) {
	return -1;
      }
    }
    else if (is_xml_processing_instruction(*pbuf) == true) {
      if (parse_xml_processing_instruction(xml, pbuf, plen) == false) {
	return -1;
      }
    }
    else if (is_xml_comment(*pbuf) == true) {
      if (parse_xml_comment(xml, pbuf, plen) == false) {
	return -1;
      }
    }

    if (is_xml_markup(*pbuf) == false) {
      if (parse_xml_char_data(xml, pbuf, plen, pi) == -1) {
	return -1;
      }
    }
  }

  return pi;
}

static int parse_element_etag(
			      struct ezcfg_xml *xml,
			      char **pbuf,
			      int *plen,
			      const int ei) {

  //struct ezcfg *ezcfg;
  char *p, *q, *name;
  char *end_tag_end;
  struct ezcfg_xml_element *elem;
  char c;
  char *buf;
  int len;

  ASSERT(xml != NULL);
  ASSERT(pbuf != NULL);
  ASSERT(plen != NULL);
  ASSERT(*pbuf != NULL);
  ASSERT(*plen > 0);


  ASSERT(xml != NULL);

  //ezcfg = xml->ezcfg;

  buf = *pbuf;
  len = *plen;
  elem = ezcfg_xml_get_element_by_index(xml, ei);

  /* ETag ::= '</' Name S? '>' */

  /* Name part */
  name = *pbuf+2; /* skip '</' */
  end_tag_end = strstr(name, ">");
  if (end_tag_end == NULL) {
    return -1;
  }

  p = name+1;
  while(is_name_char(*p) == true && p < end_tag_end) p++;
  c = *p;
  *p = '\0';

  if (elem->prefix != NULL) {
    q = elem->prefix + strlen(elem->prefix);
    *q = ':';
    if (strcmp(elem->prefix, name) != 0) {
      *p = c;
      *q = '\0';
      return -1;
    }
    *q = '\0';
  }
  else {
    if (strcmp(elem->name, name) != 0) {
      *p = c;
      return -1;
    }
  }

  *p = c;

  p = end_tag_end+1; /* skip '>' */

  /* skip white space */
  p = skip_white_space(p);

  *pbuf = p;
  *plen = len - (p - buf);

  return elem->etag_index;
}

static int parse_element_stag_content_etag(
					   struct ezcfg_xml *xml,
					   char **pbuf,
					   int *plen,
					   const int pi,
					   const int si) {

  //struct ezcfg *ezcfg;
  int stag_index;

  ASSERT(xml != NULL);
  ASSERT(pbuf != NULL);
  ASSERT(plen != NULL);
  ASSERT(*pbuf != NULL);
  ASSERT(*plen > 0);

  //ezcfg = xml->ezcfg;

  /* It's element ::= STag content ETag */
  stag_index = parse_element_stag(xml, pbuf, plen, pi, si);
  if (stag_index == -1) {
    return -1;
  }

  if (parse_element_content(xml, pbuf, plen, stag_index, -1) == -1) {
    return -1;
  }

  if (is_xml_element_end_tag(*pbuf) == false) {
    return -1;
  }

  if (parse_element_etag(xml, pbuf, plen, stag_index) == -1) {
    return -1;
  }

  return stag_index;
}

static int parse_xml_document_element(
				      struct ezcfg_xml *xml,
				      char **pbuf,
				      int *plen,
				      const int pi,
				      const int si) {

  //struct ezcfg *ezcfg;

  ASSERT(xml != NULL);
  ASSERT(pbuf != NULL);
  ASSERT(plen != NULL);
  ASSERT(*pbuf != NULL);
  ASSERT(*plen > 0);

  //ezcfg = xml->ezcfg;

  /* element ::= EmptyElemTag | STag content ETag */

  /* check element EmptyElemTag |STag start part */
  if (is_xml_element_start_tag(*pbuf) == true) {
    if (is_xml_element_empty_element_tag(*pbuf) == true) {
      /* It's an EmptyElemTag */
      /* EmptyElemTag ::= '<' Name (S Attribute)* S? '/>' */
      return parse_element_empty_elem_tag(xml, pbuf, plen, pi, si);
    }

    /* It's an STag */
    /* element ::= STag content ETag */
    return parse_element_stag_content_etag(xml, pbuf, plen, pi, si);
  }

  return -1;
}

/* Public functions */
void ezcfg_xml_delete(struct ezcfg_xml *xml)
{
  //struct ezcfg *ezcfg;

  ASSERT(xml != NULL);

  //ezcfg = xml->ezcfg;

  if (xml->root != NULL) {
    delete_elements(xml);
    free(xml->root);
  }

  free(xml);
}

/**
 * ezcfg_xml_new:
 * Create ezcfg xml parser data structure
 * Returns: a new ezcfg xml parser data structure
 **/
struct ezcfg_xml *ezcfg_xml_new(struct ezcfg *ezcfg)
{
  struct ezcfg_xml *xml;

  ASSERT(ezcfg != NULL);

  /* initialize xml parser data structure */
  xml = calloc(1, sizeof(struct ezcfg_xml));
  if (xml == NULL) {
    err(ezcfg, "initialize xml parser error.\n");
    return NULL;
  }

  xml->max_elements = EZCFG_XML_MAX_ELEMENTS * 2; /* must be times of 2 */
  xml->root=calloc(xml->max_elements, sizeof(struct ezcfg_xml_element *));
  if (xml->root == NULL) {
    err(ezcfg, "initialize xml element stack error.\n");
    free(xml);
    return NULL;
  }

  xml->ezcfg = ezcfg;
  xml->num_encoding_names = ARRAY_SIZE(default_character_encoding_names) - 1; /* first item is NULL */
  xml->encoding_names = default_character_encoding_names;

  return xml;
}

void ezcfg_xml_reset_attributes(struct ezcfg_xml *xml)
{
  //struct ezcfg *ezcfg;

  ASSERT(xml != NULL);

  //ezcfg = xml->ezcfg;

  if (xml->root != NULL)
    delete_elements(xml);

  xml->version_major = 0;
  xml->version_minor = 0;
  xml->encoding_name_index = 0;
  xml->standalone = false;
  xml->num_elements = 0;
}

int ezcfg_xml_get_max_elements(struct ezcfg_xml *xml)
{
  //struct ezcfg *ezcfg;

  ASSERT(xml != NULL);

  //ezcfg = xml->ezcfg;

  return xml->max_elements;
}

bool ezcfg_xml_set_max_elements(struct ezcfg_xml *xml, const int max_elements)
{
  struct ezcfg *ezcfg;
  struct ezcfg_xml_element **root; /* stack for representing xml doc tree */
  int i;

  ASSERT(xml != NULL);

  ezcfg = xml->ezcfg;

  if (max_elements <= xml->max_elements) {
    err(ezcfg, "must enlarge the xml stack.\n");
    return false;
  }

  root=realloc(xml->root, sizeof(struct ezcfg_xml_element *) * max_elements);
  if (root == NULL) {
    err(ezcfg, "can not realloc for xml->root.\n");
    return false;
  }

  /* initialize newly allocated memory which was uninitialized */
  for (i = xml->num_elements; i < max_elements; i++) {
    root[i] = NULL;
  }

  xml->root = root;
  xml->max_elements = max_elements;
  return true;
}

int ezcfg_xml_get_num_elements(struct ezcfg_xml *xml)
{
  //struct ezcfg *ezcfg;

  ASSERT(xml != NULL);

  //ezcfg = xml->ezcfg;

  return xml->num_elements;
}

void ezcfg_xml_element_delete(struct ezcfg_xml_element *elem)
{
  struct elem_attribute *a;
  ASSERT(elem != NULL);

  /* first delete all element's attributes */
  while (elem->attr_head != NULL) {
    a = elem->attr_head;
    elem->attr_head = a->next;
    delete_attribute(a);
  }

  /* prefix and name point to the same place 
   * if prefix is not NULL, free(prefix) 
   * else free(name) */
  if (elem->prefix != NULL) {
    free(elem->prefix);
  }
  else if (elem->name != NULL) {
    free(elem->name);
  }

  if (elem->content != NULL) {
    free(elem->content);
  }

  free(elem);
}

struct ezcfg_xml_element *ezcfg_xml_element_new(
						struct ezcfg_xml *xml,
						const char *name,
						const char *content) {

  struct ezcfg *ezcfg;
  struct ezcfg_xml_element *elem;
  char *p;

  ASSERT(xml != NULL);
  ASSERT(name != NULL);

  ezcfg = xml->ezcfg;

  elem = calloc(1, sizeof(struct ezcfg_xml_element));
  if (elem == NULL) {
    err(ezcfg, "calloc xml element error.\n");
    return NULL;
  }

  elem->name = strdup(name);
  if (elem->name == NULL) {
    err(ezcfg, "initialize xml element name error.\n");
    ezcfg_xml_element_delete(elem);
    return NULL;
  }
  /* find the prefix */
  p = strchr(elem->name, ':');
  if (p != NULL) {
    elem->prefix = elem->name;
    *p = '\0';
    p++;
    elem->name = p;
  }

  if (content != NULL) {
    elem->content = strdup(content);
    if (elem->content == NULL) {
      err(ezcfg, "initialize xml element content error.\n");
      ezcfg_xml_element_delete(elem);
      return NULL;
    }
  }
  return elem;
}

bool ezcfg_xml_element_add_attribute(
				     struct ezcfg_xml *xml,
				     struct ezcfg_xml_element *elem,
				     const char *name, const char *value, int pos) {

  struct ezcfg *ezcfg;
  struct elem_attribute *a;
  char *p;

  ASSERT(xml != NULL);
  ASSERT(elem != NULL);
  ASSERT(name != NULL);
  ASSERT(value != NULL);

  ezcfg = xml->ezcfg;

  a = calloc(1, sizeof(struct elem_attribute));
  if (a == NULL) {
    err(ezcfg, "no memory for element attribute\n");
    return false;
  }

  a->name = strdup(name);
  if (a->name == NULL) {
    err(ezcfg, "no memory for element attribute name\n");
    goto fail_exit;
  }
  /* find the prefix */
  p = strchr(a->name, ':');
  if (p != NULL) {
    a->prefix = a->name;
    *p = '\0';
    p++;
    a->name = p;
  }

  a->value = strdup(value);
  if (a->value == NULL) {
    err(ezcfg, "no memory for element attribute value\n");
    goto fail_exit;
  }

  if (pos == EZCFG_XML_ELEMENT_ATTRIBUTE_HEAD) {
    a->next = elem->attr_head;
    elem->attr_head = a;
    if (elem->attr_tail == NULL) {
      elem->attr_tail = a;
    }
  } else if (pos == EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL) {
    if (elem->attr_tail == NULL) {
      elem->attr_head = a;
      elem->attr_tail = a;
    } else {
      a->next = elem->attr_tail->next;
      elem->attr_tail->next = a;
      elem->attr_tail = a;
    }
  } else {
    err(ezcfg, "not support element attribute position\n");
    goto fail_exit;
  }
  return true;

 fail_exit:
  delete_attribute(a);
  return false;
}

int ezcfg_xml_add_element(
			  struct ezcfg_xml *xml,
			  const int pi, /* parent element index */
			  const int si, /* sibling element index */
			  struct ezcfg_xml_element *elem) {

  struct ezcfg *ezcfg;
  struct ezcfg_xml_element **root;
  struct ezcfg_xml_element *parent;
  struct ezcfg_xml_element *sibling;
  int i, max_elements;

  ASSERT(xml != NULL);
  //ASSERT(xml->num_elements+2 <= xml->max_elements);

  ezcfg = xml->ezcfg;

  if (xml->num_elements+2 > xml->max_elements) {
    max_elements = xml->max_elements + EZCFG_XML_ENLARGE_SIZE ;
    root=realloc(xml->root, sizeof(struct ezcfg_xml_element *) * max_elements);
    if (root == NULL) {
      err(ezcfg, "can not realloc for xml->root.\n");
      return -1;
    }
    /* initialize newly allocated memory which was uninitialized */
    for (i = xml->num_elements; i < max_elements; i++) {
      root[i] = NULL;
    }
    xml->root = root;
    xml->max_elements = max_elements;
  }

  root = xml->root;
  //i = 0;

  if (pi < 0) {
    parent = NULL;
  }
  else {
    parent = root[pi];
  }

  if (si < 0) {
    sibling = NULL;
  }
  else {
    sibling = root[si];
  }

  if (parent == NULL) {
    /* root element */
    if(xml->num_elements != 0) {
      err(ezcfg, "xml root element must be only one, xml->num_elements=[%d]\n", xml->num_elements);
      return -1;
    }
    i = 0;
    root[i] = elem;
    xml->num_elements++;
    elem->etag_index = i+1;
    root[i+1] = elem;
    xml->num_elements++;
  } else if (sibling == NULL) {
    /* parent element's first child */
    for (i = xml->num_elements-1; i > 0; i--) {
      /* move 2 steps */
      root[i+2] = root[i];

      /* change etag_index for the start element */
      if (root[i]->etag_index == i) {
	root[i]->etag_index = i+2;
      }
      /* find parent element end tag */
      if (root[i] == parent) {
	break;
      }
    }
    if(i == 0) {
      err(ezcfg, "parent element not found\n");
      return -1;
    }
    root[i] = elem;
    xml->num_elements++;
    elem->etag_index = i+1;
    root[i+1] = elem;
    xml->num_elements++;
  } else {
    /* sibling element's next */
    for (i = xml->num_elements-1; i > sibling->etag_index; i--) {
      /* move 2 steps */
      root[i+2] = root[i];

      /* change etag_index for the start element */
      if (root[i]->etag_index == i) {
	root[i]->etag_index = i+2;
      }
    }
    if(i == 0) {
      err(ezcfg, "sibling element not found\n");
      return -1;
    }
    i++; /* move to next element slot */
    root[i] = elem;
    xml->num_elements++;
    elem->etag_index = i+1;
    root[i+1] = elem;
    xml->num_elements++;
  }

  return i;
}

/* XML 1.0 fifth edition section 2.11 End-of-Line Handling */
int ezcfg_xml_normalize_document(struct ezcfg_xml *xml, char *buf, int len)
{
  //struct ezcfg *ezcfg;
  char *p, *q;

  //ezcfg = xml->ezcfg;

  p = buf;
  q = buf;

  while(*q != '\0') {
    if (*q == 0x0d) {
      *p = 0x0a;
      if (*(q+1) == 0x0a) {
	q++;
      }
    }
    else {
      *p = *q;
    }
    p++;
    q++;
  }

  /* \0-terminated the document */
  *p = '\0';

  return (p - buf);
}

bool ezcfg_xml_parse(struct ezcfg_xml *xml, char *buf, int len)
{
  //struct ezcfg *ezcfg;
  //struct ezcfg_xml_element **root;
  char *p;
  int i;

  ASSERT(xml != NULL);
  ASSERT(xml->root != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  //ezcfg = xml->ezcfg;
  //root = xml->root;

  /* clean up xml->root elements */
  delete_elements(xml);

  p = buf;
  i = len;

  /* normalize XML document */
  i = ezcfg_xml_normalize_document(xml, buf, len);
  if (i < 1) {
    return false;
  }

  /* XML document ::= prolog element Misc* */
  if (parse_xml_document_prolog(xml, &p, &i) == false) {
    return false;
  }

  /* parent = NULL, it's a root element */
  if (parse_xml_document_element(xml, &p, &i, -1, -1) == -1) {
    return false;
  }

  while(is_xml_misc(p) == true) {
    if (parse_xml_document_misc(xml, &p, &i) == false) {
      return false;
    }
  }

  return true;
}

int ezcfg_xml_get_message_length(struct ezcfg_xml *xml)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_xml_element **root;
  struct elem_attribute *a;
  int i, n, count;

  ASSERT(xml != NULL);
  ASSERT(xml->root != NULL);

  //ezcfg = xml->ezcfg;
  root = xml->root;

  count = 0;
  for (i = 0; i < xml->num_elements; i++) {
    if (root[i]->etag_index != i) {
      if ((root[i]->name)[0] == '\0') {
	/* special case for content is CharData in middle of element */
	/* <caption>This is Raphael's "Foligno" Madonna, painted in
	   <date>1511</date> - <date>1512</date>.
	   </caption>
	*/
	//snprintf(buf+strlen(buf), len-strlen(buf), "%s", root[i]->content);
	n = strlen(root[i]->content);
	count += n;
      }
      else {
	/* start tag */
	//snprintf(buf+strlen(buf), len-strlen(buf), "<%s", root[i]->name);
	count += 1; /* for "<" */
	if (root[i]->prefix == NULL) {
	  n = strlen(root[i]->name);
	}
	else {
	  n = strlen(root[i]->prefix); /* for prefix */
	  n += 1; /* for ':' */
	  n += strlen(root[i]->name); /* for name */
	}
	count += n;

	a = root[i]->attr_head;
	while(a != NULL) {
	  //snprintf(buf+strlen(buf), len-strlen(buf), " %s", a->name);
	  count += 1; /* for SP */
	  if (a->prefix == NULL) {
	    n = strlen(a->name);
	  }
	  else {
	    n = strlen(a->prefix);
	    n += 1; /* for ':' */
	    n += strlen(a->name);
	  }
	  count += n;

	  //snprintf(buf+strlen(buf), len-strlen(buf), "=\"%s\"", a->value);
	  count += 2; /* for "=\"" */
	  n = strlen(a->value);
	  count += n;
	  count += 1; /* for \" */

	  a = a->next;
	}
	if (root[i]->content == NULL && root[i]->etag_index == i+1) {
	  //snprintf(buf+strlen(buf), len-strlen(buf), "/>");
	  count += 2; /* for "/>" */
	}
	else {
	  //snprintf(buf+strlen(buf), len-strlen(buf), ">");
	  count += 1; /* for ">" */

	  if (root[i]->content != NULL) {
	    //snprintf(buf+strlen(buf), len-strlen(buf), "%s", root[i]->content);
	    n = strlen(root[i]->content);
	    count += n;
	  }
	}
      }
    }
    else {
      /* end tag */
      if (root[i]->content == NULL && root[i-1]->etag_index == i) {
	/* add new-line */
	//snprintf(p, l, "\n");
	count += 1; /* for "\n" */
      }
      else {
	//snprintf(buf+strlen(buf), len-strlen(buf), "</%s>\n", root[i]->name);
	count += 2; /* for "</" */
	if (root[i]->prefix == NULL) {
	  n = strlen(root[i]->name);
	}
	else {
	  n = strlen(root[i]->prefix);
	  n += 1; /* for ':' */
	  n += strlen(root[i]->name);
	}
	count += n;
	count += 2; /* for ">\n" */
      }
    }
  }
  return (count);
}

int ezcfg_xml_write_message(struct ezcfg_xml *xml, char *buf, int len)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_xml_element **root;
  struct elem_attribute *a;
  char *p;
  int i, n, l;

  ASSERT(xml != NULL);
  ASSERT(xml->root != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  //ezcfg = xml->ezcfg;
  root = xml->root;
  buf[0] = '\0';
  l = len;
  p = buf;

  for (i = 0; i < xml->num_elements; i++) {
    if (root[i]->etag_index != i) {
      if ((root[i]->name)[0] == '\0') {
	/* special case for content is CharData in middle of element */
	/* <caption>This is Raphael's "Foligno" Madonna, painted in
	   <date>1511</date> - <date>1512</date>.
	   </caption>
	*/
	//snprintf(buf+strlen(buf), len-strlen(buf), "%s", root[i]->content);
	n = snprintf(p, l, "%s", root[i]->content);
	p += n;
	l -= n;
      }
      else {
	/* start tag */
	//snprintf(buf+strlen(buf), len-strlen(buf), "<%s", root[i]->name);
	if (root[i]->prefix == NULL) {
	  n = snprintf(p, l, "<%s", root[i]->name);
	}
	else {
	  n = snprintf(p, l, "<%s:%s", root[i]->prefix, root[i]->name);
	}
	p += n;
	l -= n;

	a = root[i]->attr_head;
	while(a != NULL) {
	  //snprintf(buf+strlen(buf), len-strlen(buf), " %s", a->name);
	  if (a->prefix == NULL) {
	    n = snprintf(p, l, " %s", a->name);
	  }
	  else {
	    n = snprintf(p, l, " %s:%s", a->prefix, a->name);
	  }
	  p += n;
	  l -= n;

	  //snprintf(buf+strlen(buf), len-strlen(buf), "=\"%s\"", a->value);
	  n = snprintf(p, l, "=\"%s\"", a->value);
	  p += n;
	  l -= n;

	  a = a->next;
	}
	if (root[i]->content == NULL && root[i]->etag_index == i+1) {
	  //snprintf(buf+strlen(buf), len-strlen(buf), "/>");
	  n = snprintf(p, l, "/>");
	  p += n;
	  l -= n;

	}
	else {
	  //snprintf(buf+strlen(buf), len-strlen(buf), ">");
	  n = snprintf(p, l, ">");
	  p += n;
	  l -= n;

	  if (root[i]->content != NULL) {
	    //snprintf(buf+strlen(buf), len-strlen(buf), "%s", root[i]->content);
	    n = snprintf(p, l, "%s", root[i]->content);
	    p += n;
	    l -= n;
	  }
	}
      }
    }
    else {
      /* end tag */
      if (root[i]->content == NULL && root[i-1]->etag_index == i) {
	/* add new-line */
	n = snprintf(p, l, "\n");
	p += n;
	l -= n;
      }
      else {
	//snprintf(buf+strlen(buf), len-strlen(buf), "</%s>\n", root[i]->name);
	if (root[i]->prefix == NULL) {
	  n = snprintf(p, l, "</%s>\n", root[i]->name);
	}
	else {
	  n = snprintf(p, l, "</%s:%s>\n", root[i]->prefix, root[i]->name);
	}
	p += n;
	l -= n;
      }
    }
  }
  if (p == buf+len) {
    return -1;
  }
  return (p - buf);
}

int ezcfg_xml_get_element_index(struct ezcfg_xml *xml, const int pi, const int si, char *name)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_xml_element *elem;
  int i, pi_etag_index;
  char *p;

  ASSERT(xml != NULL);
  ASSERT(xml->root != NULL);
  ASSERT(pi >= -1);
  ASSERT(si >= -1);
  ASSERT(pi < xml->num_elements);
  ASSERT(si < xml->num_elements);

  //ezcfg = xml->ezcfg;

  /* find prefix */
  p = strchr(name, ':');
  if (p == NULL) {
    p = name;
  }
  else {
    p++;
  }

  /* find pi end tag index */
  elem = (pi == -1) ? xml->root[0] : xml->root[pi];
  pi_etag_index = elem->etag_index;

  /* find target element index */
  i = (si == -1) ? pi+1 : si+1;
  while (i < pi_etag_index) {
    elem = xml->root[i];
    if (strcmp(elem->name, p) == 0 &&
	(elem->etag_index != i)) {
      return i;
    }
    /* move to next sibling element slot */
    i = elem->etag_index + 1;
  }

  return -1;
}

struct ezcfg_xml_element *ezcfg_xml_get_element_by_index(struct ezcfg_xml *xml, const int i)
{
  //struct ezcfg *ezcfg;

  ASSERT(xml != NULL);
  ASSERT(xml->root != NULL);
  ASSERT(i < xml->num_elements);

  //ezcfg = xml->ezcfg;

  return xml->root[i];
}

bool ezcfg_xml_set_element_content_by_index(struct ezcfg_xml *xml, const int i, const char *content)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_xml_element *elem;
  char *p;

  ASSERT(xml != NULL);
  ASSERT(xml->root != NULL);
  ASSERT(i < xml->num_elements);

  //ezcfg = xml->ezcfg;

  elem = xml->root[i];
  p = strdup(content);
  if (p == NULL) {
    return false;
  }
  if (elem->content != NULL) {
    free(elem->content);
  }
  elem->content = p;
  return true;
}

char *ezcfg_xml_get_element_content_by_index(struct ezcfg_xml *xml, const int i)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_xml_element *elem;

  ASSERT(xml != NULL);
  ASSERT(xml->root != NULL);
  ASSERT(i < xml->num_elements);

  //ezcfg = xml->ezcfg;

  elem = xml->root[i];
  return elem->content;
}

int ezcfg_xml_get_element_etag_index_by_index(struct ezcfg_xml *xml, const int i)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_xml_element *elem;

  ASSERT(xml != NULL);
  ASSERT(xml->root != NULL);
  ASSERT(i < xml->num_elements);

  //ezcfg = xml->ezcfg;

  elem = xml->root[i];
  return elem->etag_index;
}
