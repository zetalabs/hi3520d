/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_ctrl.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-02   0.1       Splite it from ezcfg.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_CTRL_H_
#define _EZCFG_PRIV_CTRL_H_

#include "ezcfg-types.h"

/* ctrl/ctrl.c - daemon runtime setup */
struct ezcfg_ctrl *ezcfg_ctrl_new_from_socket(struct ezcfg *ezcfg, const int family, const int proto, const char *local_socket_path, const char *remote_socket_path);
void ezcfg_ctrl_delete(struct ezcfg_ctrl *ezctrl);
int ezcfg_ctrl_connect(struct ezcfg_ctrl *ezctrl);
int ezcfg_ctrl_read(struct ezcfg_ctrl *ezctrl, void *buf, int len, int flags);
int ezcfg_ctrl_write(struct ezcfg_ctrl *ezctrl, const void *buf, int len, int flags);
void ezcfg_ctrl_delete(struct ezcfg_ctrl *ezctrl);
struct ezcfg_ctrl *ezcfg_ctrl_new(struct ezcfg *ezcfg);
struct ezcfg_socket *ezcfg_ctrl_get_socket(const struct ezcfg_ctrl *ezctrl);
bool ezcfg_ctrl_set_buffer(struct ezcfg_ctrl *ezctrl, char *buf, int buf_len);
char *ezcfg_ctrl_get_buffer(const struct ezcfg_ctrl *ezctrl);
int ezcfg_ctrl_get_buffer_len(const struct ezcfg_ctrl *ezctrl);
void ezcfg_ctrl_reset_attributes(struct ezcfg_ctrl *ezctrl);
int ezcfg_ctrl_handle_message(struct ezcfg_ctrl *ezctrl, char *output, int len, void *rte);
int ezcfg_ctrl_handle_ipc_message(char **argv, char *output, int len, void *rte);
#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
int ezcfg_ctrl_handle_upnp_message(char **argv, char *output, int len, void *rte);
#endif
#if (HAVE_EZBOX_SERVICE_EZCTP == 1)
int ezcfg_ctrl_handle_ezctp_message(char **argv, char *output, int len, void *rte);
#endif

#endif /* _EZCFG_PRIV_CTRL_H_ */

