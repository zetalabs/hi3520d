/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-private.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2014-07-10   0.1       Split it from ezcfg-private.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_LOCALE_H_
#define _EZCFG_PRIV_LOCALE_H_

struct ezcfg_locale * ezcfg_locale_new(struct ezcfg *ezcfg);
int ezcfg_locale_delete(struct ezcfg_locale *locale);
int ezcfg_locale_set_name(struct ezcfg_locale *locale, char *name);
int ezcfg_locale_set_domain(struct ezcfg_locale *locale, char *domain);
int ezcfg_locale_set_dir(struct ezcfg_locale *locale, char *dir);
char * ezcfg_locale_text(struct ezcfg_locale *locale, char *msgid);

#endif /* _EZCFG_PRIV_LOCALE_H_ */
