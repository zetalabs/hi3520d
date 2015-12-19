/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-igrs.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-29   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_IGRS_H_
#define _EZCFG_IGRS_H_

#include "ezcfg.h"
#include "ezcfg-http.h"

struct ezcfg_igrs_msg_op {
	char *name;
	bool (*build_fn)(struct ezcfg_igrs *igrs);
	int (*write_fn)(struct ezcfg_igrs *igrs, char *buf, int len);
	int (*handle_fn)(struct ezcfg_igrs *igrs);
};

struct ezcfg_igrs {
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;

	/* IGRS info */
	unsigned short version_major; /* IGRS major version, must be 1 */
	unsigned short version_minor; /* IGRS minor version, must be 0 */

	unsigned short num_message_types; /* Number of supported message types */
	const struct ezcfg_igrs_msg_op *message_type_ops;
	unsigned short message_type_index; /* index for message type string */

	char source_device_id[EZCFG_UUID_STRING_LEN+1]; /* +1 for \0-terminated */
	char target_device_id[EZCFG_UUID_STRING_LEN+1]; /* +1 for \0-terminated */

	unsigned int source_client_id; /* 0 reserved */
	unsigned int target_client_id; /* 0 reserved */
	unsigned int target_service_id; /* 0 reserved */
	unsigned int source_service_id; /* 0 reserved */
	unsigned int sequence_id; /* 0 reserved */
	unsigned int acknowledge_id; /* 0 reserved */
	unsigned short return_code; /* 0 reserved */

	char *source_user_id; /* string, max length is 127 */
	char *service_security_id;

	char *invoke_args;

	char *host; /* Multicast channel and port reserved for ISDP */
	/* NOTIFY headers */
	char *cache_control; /* Used in advertisement mechanisms */
	char *location; /* A URL */
	char *nt; /* Notification Type */
	char *nts; /* Notification Sub Type. Single URI */
	char *server;
	char *usn; /* Unique Service Name */
	/* M-SEARCH headers */
	char *man; /* "ssdp:discover" */
	char *mx; /* Maximum wait time in seconds */
	char *st; /* Search Target */

	struct ezcfg_igrs *next; /* linklist */
};

/* igrs/igrs_create_session.c */
bool ezcfg_igrs_build_create_session_request(struct ezcfg_igrs *igrs);
int ezcfg_igrs_write_create_session_request(struct ezcfg_igrs *igrs, char *buf, int len);
int ezcfg_igrs_handle_create_session_request(struct ezcfg_igrs *igrs);

/* igrs/igrs_invoke_service.c */
bool ezcfg_igrs_build_invoke_service_request(struct ezcfg_igrs *igrs);
int ezcfg_igrs_write_invoke_service_request(struct ezcfg_igrs *igrs, char *buf, int len);
int ezcfg_igrs_handle_invoke_service_request(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_build_invoke_service_response(struct ezcfg_igrs *igrs);
int ezcfg_igrs_write_invoke_service_response(struct ezcfg_igrs *igrs, char *buf, int len);
int ezcfg_igrs_handle_invoke_service_response(struct ezcfg_igrs *igrs);

/* ezcfg igrs http methods */
#define EZCFG_IGRS_HTTP_METHOD_POST         "POST"
#define EZCFG_IGRS_HTTP_METHOD_POST_EXT     "M-POST"
/* ezcfg igrs http headers */
#define EZCFG_IGRS_HTTP_HEADER_HOST                 "Host"
#define EZCFG_IGRS_HTTP_HEADER_CONTENT_TYPE         "Content-Type"
#define EZCFG_IGRS_HTTP_HEADER_CONTENT_LENGTH       "Content-Length"
#define EZCFG_IGRS_HTTP_HEADER_MAN                  "Man"
#define EZCFG_IGRS_HTTP_HEADER_01_IGRS_VERSION      "01-IGRSVersion"
#define EZCFG_IGRS_HTTP_HEADER_01_IGRS_MESSAGE_TYPE "01-IGRSMessageType"
#define EZCFG_IGRS_HTTP_HEADER_01_TARGET_DEVICE_ID  "01-TargetDeviceId"
#define EZCFG_IGRS_HTTP_HEADER_01_SOURCE_DEVICE_ID  "01-SourceDeviceId"
#define EZCFG_IGRS_HTTP_HEADER_01_SEQUENCE_ID       "01-SequenceId"
#define EZCFG_IGRS_HTTP_HEADER_02_SOAP_ACTION       "02-SoapAction"

/* ezcfg igrs soap element names */
#define EZCFG_IGRS_SOAP_ENVELOPE_ELEMENT_NAME             "SOAP-ENV:Envelope"
#define EZCFG_IGRS_SOAP_BODY_ELEMENT_NAME                 "SOAP-ENV:Body"
#define EZCFG_IGRS_SOAP_SESSION_ELEMENT_NAME              "Session"
#define EZCFG_IGRS_SOAP_SOURCE_CLIENT_ID_ELEMENT_NAME     "SourceClientId"
#define EZCFG_IGRS_SOAP_TARGET_CLIENT_ID_ELEMENT_NAME     "TargetClientId"
#define EZCFG_IGRS_SOAP_SOURCE_SERVICE_ID_ELEMENT_NAME    "SourceServiceId"
#define EZCFG_IGRS_SOAP_TARGET_SERVICE_ID_ELEMENT_NAME    "TargetServiceId"
#define EZCFG_IGRS_SOAP_SEQUENCE_ID_ELEMENT_NAME          "SequenceId"
#define EZCFG_IGRS_SOAP_ACKNOWLEDGE_ID_ELEMENT_NAME       "AcknowledgeId"
#define EZCFG_IGRS_SOAP_RETURN_CODE_ELEMENT_NAME          "ReturnCode"
#define EZCFG_IGRS_SOAP_USER_INFO_ELEMENT_NAME            "UserInfo"
#define EZCFG_IGRS_SOAP_SOURCE_USER_ID_ELEMENT_NAME       "SourceUserId"
#define EZCFG_IGRS_SOAP_SERVICE_SECURITY_ID_ELEMENT_NAME  "ServiceSecurityId"
#define EZCFG_IGRS_SOAP_TOKEN_ELEMENT_NAME                "Token"
#define EZCFG_IGRS_SOAP_INVOKE_ARGS_ELEMENT_NAME          "InvokeArguments"

/* ezcfg igrs soap element attributes */
#define EZCFG_IGRS_SOAP_ENVELOPE_ATTR_NS_NAME	"xmlns:SOAP-ENV"
//#define EZCFG_IGRS_SOAP_ENVELOPE_ATTR_NS_VALUE	"http://schemas.xmlsoap.org/soap/envelope/"
#define EZCFG_IGRS_SOAP_ENVELOPE_ATTR_NS_VALUE	"http://www.w3.org/2002/12/soap-envelope"
#define EZCFG_IGRS_SOAP_ENVELOPE_ATTR_ENC_NAME	"SOAP-ENV:encodingStyle"
#define EZCFG_IGRS_SOAP_ENVELOPE_ATTR_ENC_VALUE	"http://schemas.xmlsoap.org/soap/encoding/"
#define EZCFG_IGRS_SOAP_SESSION_ATTR_NS_NAME		"xmlns"
#define EZCFG_IGRS_SOAP_SESSION_ATTR_NS_VALUE	"http://www.igrs.org/spec1.0"

/* ezcfg igrs message types */
/* 9.1 Device Advertisement */
#define EZCFG_IGRS_MSG_DEVICE_ONLINE_ADVERTISEMENT   "DeviceOnlineAdvertisement"
#define EZCFG_IGRS_MSG_DEVICE_OFFLINE_ADVERTISEMENT  "DeviceOfflineAdvertisement"
/* 9.2 Device Pipe Management */
#define EZCFG_IGRS_MSG_CREATE_PIPE_REQUEST               "CreatePipeRequest"
#define EZCFG_IGRS_MSG_CREATE_PIPE_RESPONSE              "CreatePipeResponse"
#define EZCFG_IGRS_MSG_AUTHENTICATE_REQUEST              "AuthenticateRequest"
#define EZCFG_IGRS_MSG_AUTHENTICATE_RESPONSE             "AuthenticateResponse"
#define EZCFG_IGRS_MSG_AUTHENTICATE_RESULT_REQUEST       "AuthenticateResultRequest"
#define EZCFG_IGRS_MSG_AUTHENTICATE_RESULT_RESPONSE      "AuthenticateResultResponse"
#define EZCFG_IGRS_MSG_CREATE_PIPE_RESULT_REQUEST        "CreatePipeResultRequest"
#define EZCFG_IGRS_MSG_CREATE_PIPE_RESULT_RESPONSE       "CreatePipeResultResponse"
#define EZCFG_IGRS_MSG_DETACH_PIPE_NOTIFY                "DetachPipeNotify"
#define EZCFG_IGRS_MSG_DEVICE_ONLINE_DETECTION_REQUEST   "DeviceOnlineDetectionRequest"
#define EZCFG_IGRS_MSG_DEVICE_ONLINE_DETECTION_RESPONSE  "DeviceOnlineDetectionResponse"
/* 9.3 Detaild Device Description Document Retrieval */
#define EZCFG_IGRS_MSG_GET_DEVICE_DESCRIPTION_REQUEST   "GetDeviceDescriptionRequest"
#define EZCFG_IGRS_MSG_GET_DEVICE_DESCRIPTION_RESPONSE  "GetDeviceDescriptionResponse"
/* 9.4 Retrieve Detailed Device Description Document Based on Non-Secure Pipe */
/* 9.5 Device Group Setup */
#define EZCFG_IGRS_MSG_PEER_DEVICE_GROUP_ADVERTISEMENT              "PeerDeviceGroupAdvertisement"
#define EZCFG_IGRS_MSG_QUIT_PEER_DEVICE_GROUP_NOTIFY                "QuitPeerDeviceGroupNotify"
#define EZCFG_IGRS_MSG_CENTRALISED_DEVICE_GROUP_ADVERTISEMENT       "CentralisedDeviceGroupAdvertisement"
#define EZCFG_IGRS_MSG_JOIN_CENTRALISED_DEVICE_GROUP_REQUEST        "JoinCentralisedDeviceGroupRequest"
#define EZCFG_IGRS_MSG_JOIN_CENTRALISED_DEVICE_GROUP_RESPONSE       "JoinCentralisedDeviceGroupResponse"
#define EZCFG_IGRS_MSG_QUIT_CENTRALISED_DEVICE_GROUP_ADVERTISEMENT  "QuitCentralisedDeviceGroupAdvertisement"
#define EZCFG_IGRS_MSG_QUIT_CENTRALISED_DEVICE_GROUP_NOTIFY         "QuitCentralisedDeviceGroupNotify"
/* 9.6 Device Search */
#define EZCFG_IGRS_MSG_SEARCH_DEVICE_REQUEST              "SearchDeviceRequest"
#define EZCFG_IGRS_MSG_SEARCH_DEVICE_RESPONSE             "SearchDeviceResponse"
#define EZCFG_IGRS_MSG_SEARCH_DEVICE_REQUEST_ON_DEVICE    "SearchDeviceRequestOnDevice"
#define EZCFG_IGRS_MSG_SEARCH_DEVICE_RESPONSE_ON_DEVICE   "SearchDeviceResponseOnDevice"
/* 9.7 Device Online/Offline Event Subscription */
#define EZCFG_IGRS_MSG_SUBSCRIBE_DEVICE_EVENT_REQUEST           "SubscribeDeviceEventRequest"
#define EZCFG_IGRS_MSG_RENEW_SUBSCRIPTION_DEVICE_EVENT_REQUEST  "RenewSubscriptionDeviceEventRequest"
#define EZCFG_IGRS_MSG_SUBSCRIBE_DEVICE_EVENT_RESPONSE          "SubscribeDeviceEventResponse"
#define EZCFG_IGRS_MSG_UNSUBSCRIBE_DEVICE_EVENT_NOTIFY          "UnSubscribeDeviceEventNotify"
#define EZCFG_IGRS_MSG_NOTIFY_DEVICE_EVENT                      "NotifyDeviceEvent"
/* 9.8 Device Group Search */
#define EZCFG_IGRS_MSG_SEARCH_DEVICE_GROUP_REQUEST    "SearchDeviceGroupRequest"
#define EZCFG_IGRS_MSG_SEARCH_DEVICE_GROUP_RESPONSE   "SearchDeviceGroupResponse"
/* 10.1 Service Online/Offline Advertisement */
#define EZCFG_IGRS_MSG_SERVICE_ONLINE_ADVERTISEMENT   "ServiceOnlineAdvertisement"
#define EZCFG_IGRS_MSG_SERVICE_OFFLINE_ADVERTISEMENT  "ServiceOfflineAdvertisement"
#define EZCFG_IGRS_MSG_REGISTER_SERVICE_NOTIFY        "RegisterServiceNotify"
#define EZCFG_IGRS_MSG_UNREGISTER_SERVICE_NOTIFY      "UnRegisterServiceNotify"
/* 10.2 Service Search */
#define EZCFG_IGRS_MSG_SEARCH_SERVICE_REQUEST             "SearchServiceRequest"
#define EZCFG_IGRS_MSG_SEARCH_SERVICE_RESPONSE            "SearchServiceResponse"
#define EZCFG_IGRS_MSG_SEARCH_SERVICE_REQUEST_ON_DEVICE   "SearchServiceRequestOnDevice"
#define EZCFG_IGRS_MSG_SEARCH_SERVICE_RESPONSE_ON_DEVICE  "SearchServiceResponseOnDevice"
/* 10.3 Service Online/Offline Event Subscription */
#define EZCFG_IGRS_MSG_SUBSCRIBE_SERVICE_EVENT_REQUEST           "SubscribeServiceEventRequest"
#define EZCFG_IGRS_MSG_RENEW_SUBSCRIPTION_SERVICE_EVENT_REQUEST  "RenewSubscriptionServiceEventRequest"
#define EZCFG_IGRS_MSG_SUBSCRIBE_SERVICE_EVENT_RESPONSE          "SubscribeServiceEventResponse"
#define EZCFG_IGRS_MSG_UNSUBSCRIBE_DEVICE_EVENT_NOTIFY           "UnSubscribeDeviceEventNotify"
#define EZCFG_IGRS_MSG_NOTIFY_SERVICE_EVENT                      "NotifyServiceEvent"
/* 10.4 Service Description Document Retrieval */
#define EZCFG_IGRS_MSG_GET_SERVICE_DESCRIPTION_REQUEST   "GetServiceDescriptionRequest"
#define EZCFG_IGRS_MSG_GET_SERVICE_DESCRIPTION_RESPONSE  "GetServiceDescriptionResponse"
/* 10.5 Session */
#define EZCFG_IGRS_MSG_CREATE_SESSION_REQUEST         "CreateSessionRequest"
#define EZCFG_IGRS_MSG_CREATE_SESSION_RESPONSE        "CreateSessionResponse"
#define EZCFG_IGRS_MSG_DESTROY_SESSION_NOTIFY         "DestroySessionNotify"
#define EZCFG_IGRS_MSG_APPLY_SESSION_KEY_REQUEST      "ApplySessionKeyRequest"
#define EZCFG_IGRS_MSG_APPLY_SESSION_KEY_RESPONSE     "ApplySessionKeyResponse"
#define EZCFG_IGRS_MSG_TRANSFER_SESSION_KEY_REQUEST   "TransferSessionKeyRequest"
#define EZCFG_IGRS_MSG_TRANSFER_SESSION_KEY_RESPONSE  "TransferSessionKeyResponse"
/* 10.6 Service Invocation */
#define EZCFG_IGRS_MSG_INVOKE_SERVICE_REQUEST   "InvokeServiceRequest"
#define EZCFG_IGRS_MSG_INVOKE_SERVICE_RESPONSE  "InvokeServiceResponse"
#define EZCFG_IGRS_MSG_SEND_NOTIFICATION        "SendNotification"


/* ezcfg igrs soap actions */
/* 9.3 Detaild Device Description Document Retrieval */
#define EZCFG_IGRS_SOAP_ACTION_GET_DEVICE_DESCRIPTION_REQUEST   "IGRS-GetDeviceDescription-Request"
#define EZCFG_IGRS_SOAP_ACTION_GET_DEVICE_DESCRIPTION_RESPONSE  "IGRS-GetDeviceDescription-Response"
/* 9.4 Retrieve Detailed Device Description Document Based on Non-Secure Pipe */
/* 9.5 Device Group Setup */
#define EZCFG_IGRS_SOAP_ACTION_PEER_DEVICE_GROUP_ADVERTISEMENT              "IGRS-PeerDeviceGroup-Advertisement"
#define EZCFG_IGRS_SOAP_ACTION_CENTRALISED_DEVICE_GROUP_ADVERTISEMENT       "IGRS-CentralisedDeviceGroup-Advertisement"
/* 9.6 Device Search */
#define EZCFG_IGRS_SOAP_ACTION_SEARCH_DEVICE_RESPONSE             "IGRS-SearchDevice-Response"
#define EZCFG_IGRS_SOAP_ACTION_SEARCH_DEVICE_REQUEST_ON_DEVICE    "IGRS-SearchDevice-Request-OnDevice"
#define EZCFG_IGRS_SOAP_ACTION_SEARCH_DEVICE_RESPONSE_ON_DEVICE   "IGRS-SearchDevice-Response-OnDevice"
/* 9.7 Device Online/Offline Event Subscription */
#define EZCFG_IGRS_SOAP_ACTION_SUBSCRIBE_DEVICE_EVENT_REQUEST           "IGRS-SubscribeDeviceEvent-Request"
#define EZCFG_IGRS_SOAP_ACTION_RENEW_SUBSCRIPTION_DEVICE_EVENT_REQUEST  "IGRS-RenewSubscriptionDeviceEvent-Request"
#define EZCFG_IGRS_SOAP_ACTION_SUBSCRIBE_DEVICE_EVENT_RESPONSE          "IGRS-SubscribeDeviceEvent-Response"
#define EZCFG_IGRS_SOAP_ACTION_UNSUBSCRIBE_DEVICE_EVENT_NOTIFY          "IGRS-UnSubscribeDeviceEvent-Notify"
#define EZCFG_IGRS_SOAP_ACTION_DEVICE_EVENT_NOTIFY                      "IGRS-DeviceEvent-Notify"
/* 9.8 Device Group Search */
#define EZCFG_IGRS_SOAP_ACTION_SEARCH_DEVICE_GROUP_RESPONSE  "IGRS-SearchDeviceGroup-Response"
/* 10.1 Service Online/Offline Advertisement */
#define EZCFG_IGRS_SOAP_ACTION_REGISTER_SERVICE_NOTIFY        "IGRS-RegisterService-Notify"
#define EZCFG_IGRS_SOAP_ACTION_UNREGISTER_SERVICE_NOTIFY      "IGRS-UnRegisterService-Notify"
/* 10.2 Service Search */
#define EZCFG_IGRS_SOAP_ACTION_SEARCH_SERVICE_RESPONSE            "IGRS-SearchService-Response"
#define EZCFG_IGRS_SOAP_ACTION_SEARCH_SERVICE_REQUEST_ON_DEVICE   "IGRS-SearchService-Request-OnDevice"
#define EZCFG_IGRS_SOAP_ACTION_SEARCH_SERVICE_RESPONSE_ON_DEVICE  "IGRS-SearchService-Response-OnDevice"
/* 10.3 Service Online/Offline Event Subscription */
#define EZCFG_IGRS_SOAP_ACTION_SUBSCRIBE_SERVICE_EVENT_REQUEST           "IGRS-SubscribeServiceEvent-Request"
#define EZCFG_IGRS_SOAP_ACTION_RENEW_SUBSCRIPTION_SERVICE_EVENT_REQUEST  "IGRS-RenewSubscriptionServiceEvent-Request"
#define EZCFG_IGRS_SOAP_ACTION_SUBSCRIBE_SERVICE_EVENT_RESPONSE          "IGRS-SubscribeServiceEvent-Response"
#define EZCFG_IGRS_SOAP_ACTION_UNSUBSCRIBE_DEVICE_EVENT_NOTIFY           "IGRS-UnSubscribeDeviceEvent-Notify"
#define EZCFG_IGRS_SOAP_ACTION_SERVICE_EVENT_NOTIFY                      "IGRS-ServiceEvent-Notify"
/* 10.4 Service Description Document Retrieval */
#define EZCFG_IGRS_SOAP_ACTION_GET_SERVICE_DESCRIPTION_REQUEST   "IGRS-GetServiceDescription-Request"
#define EZCFG_IGRS_SOAP_ACTION_GET_SERVICE_DESCRIPTION_RESPONSE  "IGRS-GetServiceDescription-Response"
/* 10.5 Session */
#define EZCFG_IGRS_SOAP_ACTION_CREATE_SESSION_REQUEST         "IGRS-CreateSession-Request"
#define EZCFG_IGRS_SOAP_ACTION_CREATE_SESSION_RESPONSE        "IGRS-CreateSession-Response"
#define EZCFG_IGRS_SOAP_ACTION_DESTROY_SESSION_NOTIFY         "IGRS-DestroySession-Notify"
#define EZCFG_IGRS_SOAP_ACTION_APPLY_SESSION_KEY_REQUEST      "IGRS-ApplySessionKey-Request"
#define EZCFG_IGRS_SOAP_ACTION_APPLY_SESSION_KEY_RESPONSE     "IGRS-ApplySessionKey-Response"
#define EZCFG_IGRS_SOAP_ACTION_TRANSFER_SESSION_KEY_REQUEST   "IGRS-TransferSessionKey-Request"
#define EZCFG_IGRS_SOAP_ACTION_TRANSFER_SESSION_KEY_RESPONSE  "IGRS-TransferSessionKey-Response"
/* 10.6 Service Invocation */
#define EZCFG_IGRS_SOAP_ACTION_INVOKE_SERVICE_REQUEST   "IGRS-InvokeService-Request"
#define EZCFG_IGRS_SOAP_ACTION_INVOKE_SERVICE_RESPONSE  "IGRS-InvokeService-Response"
#define EZCFG_IGRS_SOAP_ACTION_SEND_NOTIFICATION        "IGRS-Send-Notification"

#endif /* _EZCFG_IGRS_H_ */
