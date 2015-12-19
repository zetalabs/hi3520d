/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_basic_agent.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2013-07-29   0.1       Write it from scratch
 * 2015-06-10   0.1       Rename it to ezcfg-priv_basic_agent.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_BASIC_AGENT_H_
#define _EZCFG_PRIV_BASIC_AGENT_H_

/* bitmap for agent state */
#define AGENT_STATE_STOPPED	0
#define AGENT_STATE_RUNNING	1
#define AGENT_STATE_STOPPING	2

#define AGENT_SOCKET_QUEUE_LENGTH	20
#define AGENT_MASTER_WAIT_TIME		5

/* basic/agent/agent.c */
struct ezcfg_agent *ezcfg_agent_start(struct ezcfg *ezcfg);
void ezcfg_agent_stop(struct ezcfg_agent *agent);
void ezcfg_agent_reload(struct ezcfg_agent *agent);
void ezcfg_agent_set_threads_max(struct ezcfg_agent *agent, int threads_max);

#endif /* _EZCFG_PRIV_BASIC_AGENT_H_ */
