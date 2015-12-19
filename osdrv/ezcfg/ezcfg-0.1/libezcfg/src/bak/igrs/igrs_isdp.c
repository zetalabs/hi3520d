/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : igrs/igrs_isdp.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-27   0.1       Write it from scratch
 * 2011-11-30   0.2       Rewrite it to conform with ISO/IEC 14543-5-1:2010
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

struct ezcfg_igrs_isdp {
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	unsigned short version_major;
	unsigned short version_minor;
	unsigned short message_type;
	char *host; /* Multicast channel and port reserved for ISDP */
	/* NOTIFY headers */
	char *cache_control; /* Used in advertisement mechanisms */
	char *location; /* A URL */
	char *nt; /* Notification Type */
	char *nts; /* Notification Sub Type. Single URI */
	char *server;
	char *usn; /* Unique Service Name */
	/* M-SEARCH headers */
	char *man; /* "isdp:discover" */
	char *mx; /* Maximum wait time in seconds */
	char *st; /* Search Target */
};

#if 0
static const char *message_type_strings[] = {
	/* bad Message Type string */
	NULL,
	/* 9.1 Device Advertisement */
	"DeviceOnlineAdvertisement",
	"DeviceOfflineAdvertisement",
	/* 9.2 Device Pipe Management */
	"CreatePipeRequest",
	"CreatePipeResponse",
	"AuthenticateRequest",
	"AuthenticateResponse",
	"AuthenticateResultRequest",
	"AuthenticateResultResponse",
	"CreatePipeResultRequest",
	"CreatePipeResultResponse",
	"DetachPipeNotify",
	"DeviceOnlineDetectionRequest",
	"DeviceOnlineDetectionResponse",
	/* 9.3 Detaild Device Description Document Retrieval */
	"GetDeviceDescriptionRequest",
	"GetDeviceDescriptionResponse",
	/* 9.4 Retrieve Detailed Device Description Document Based on Non-Secure Pipe */
	/* 9.5 Device Group Setup */
	"PeerDeviceGroupAdvertisement",
	"QuitPeerDeviceGroupNotify",
	"CentralisedDeviceGroupAdvertisement",
	"JoinCentralisedDeviceGroupRequest",
	"JoinCentralisedDeviceGroupResponse",
	"QuitCentralisedDeviceGroupAdvertisement",
	"QuitCentralisedDeviceGroupNotify",
	/* 9.6 Device Search */
	"SearchDeviceRequest",
	"SearchDeviceResponse",
	"SearchDeviceRequestOnDevice",
	"SearchDeviceResponseOnDevice",
	/* 9.7 Device Online/Offline Event Subscription */
	"SubscribeDeviceEventRequest",
	"RenewSubscriptionDeviceEventRequest",
	"SubscribeDeviceEventResponse",
	"UnSubscribeDeviceEventNotify",
	"NotifyDeviceEvent",
	/* 9.8 Device Group Search */
	"SearchDeviceGroupRequest",
	"SearchDeviceGroupResponse",
	/* 10.1 Service Online/Offline Advertisement */
	"ServiceOnlineAdvertisement",
	"ServiceOfflineAdvertisement",
	"RegisterServiceNotify",
	"UnRegisterServiceNotify",
	/* 10.2 Service Search */
	"SearchServiceRequest",
	"SearchServiceResponse",
	"SearchServiceRequestOnDevice",
	"SearchServiceResponseOnDevice",
	/* 10.3 Service Online/Offline Event Subscription */
	"SubscribeServiceEventRequest",
	"RenewSubscriptionServiceEventRequest",
	"SubscribeServiceEventResponse",
	"UnSubscribeDeviceEventNotify",
	"NotifyServiceEvent",
	/* 10.4 Service Description Document Retrieval */
	"GetServiceDescriptionRequest",
	"GetServiceDescriptionResponse",
	/* 10.5 Session */
	"CreateSessionRequest",
	"CreateSessionResponse",
	"DestroySessionNotify",
	"ApplySessionKeyRequest",
	"ApplySessionKeyResponse",
	"TransferSessionKeyRequest",
	"TransferSessionKeyResponse",
	/* 10.6 Service Invocation */
	"InvokeServiceRequest",
	"InvokeServiceResponse",
	"SendNotification",
};
#endif

/**
 * private functions
 **/

/**
 * Public functions
 **/
void ezcfg_igrs_isdp_delete(struct ezcfg_igrs_isdp *isdp)
{
	//struct ezcfg *ezcfg;

	ASSERT(isdp != NULL);

	//ezcfg = isdp->ezcfg;

	free(isdp);
}

/**
 * ezcfg_igrs_isdp_new:
 * Create ezcfg isdp protocol data structure
 * Returns: a new ezcfg isdp protocol data structure
 **/
struct ezcfg_igrs_isdp *ezcfg_igrs_isdp_new(struct ezcfg *ezcfg)
{
	struct ezcfg_igrs_isdp *isdp;

	ASSERT(ezcfg != NULL);

	/* initialize isdp protocol data structure */
	isdp = calloc(1, sizeof(struct ezcfg_igrs_isdp));
	if (isdp == NULL) {
		return NULL;
	}

	memset(isdp, 0, sizeof(struct ezcfg_igrs_isdp));
	isdp->ezcfg = ezcfg;
	//isdp->igrs = igrs;

	return isdp;
}

void ezcfg_igrs_isdp_dump(struct ezcfg_igrs_isdp *isdp)
{
	//struct ezcfg *ezcfg;

	ASSERT(isdp != NULL);

	//ezcfg = isdp->ezcfg;

}

