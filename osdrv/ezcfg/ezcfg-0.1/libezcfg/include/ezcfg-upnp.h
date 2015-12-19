/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-upnp.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-09   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_UPNP_H_
#define _EZCFG_UPNP_H_

/* for IFNAMSIZ */
#include <net/if.h>

/*----------------------------*/
/* data structure */
typedef struct device_icon_s {
	/* icon elements */
	char *mimetype;
	int width;
	int height;
	int depth;
	char *url;
	/* link */
	struct device_icon_s *next;
} device_icon_t;

typedef struct upnp_service_s {
	/* service elements */
	char *serviceType;
	char *serviceId;
	char *SCPDURL;
	char *controlURL;
	char *eventSubURL;
	/* link */
	struct upnp_service_s *next;
} upnp_service_t;

typedef struct upnp_device_s {
	/* device elements */
	char *deviceType;
	char *friendlyName;
	char *manufacturer;
	char *manufacturerURL;
	char *modelDescription;
	char *modelName;
	char *modelNumber;
	char *modelURL;
	char *serialNumber;
	char *UDN;
	char *UPC;
	device_icon_t *iconList;
	upnp_service_t *serviceList;
	struct upnp_device_s *deviceList; /* first child node */
	char *presentationURL;
	/* link */
	struct upnp_device_s *next; /* sibling node */
	/* implement info */
	void *data;
} upnp_device_t;

typedef struct upnp_control_point_s {
	/* monitoring root devices */
	int root_dev_max_num;
	upnp_device_t device_template;
	upnp_device_t *root_devs;
} upnp_control_point_t;

typedef struct upnp_if_s {
	/* monitoring root devices */
	char ifname[IFNAMSIZ];
	int life_time;
	struct upnp_if_s *next;
} upnp_if_t;

struct ezcfg_upnp {
	struct ezcfg *ezcfg;

	int role; /* controlled device/control point */
	int device_type; /* standardizeddcps */
	upnp_if_t *ifs;

	/* UPnP info */
	int version_major; /* UPnP major version, must be 1 */
	int version_minor; /* UPnP minor version, should be 0 or 1 */

	char *URLBase; /* UPnP UDA 1.0 defined, UPnP UDA 1.1 deprecated */

	union {
		upnp_device_t dev;
		upnp_control_point_t cp;
	} u;

	struct ezcfg_upnp *next; /* link pointer */
};

/*----------------------------*/
/* ezcfg nvram name prefix */
#define EZCFG_UPNP_NVRAM_PREFIX            "upnp."

/* ezcfg UPnP role string */
#define EZCFG_UPNP_ROLE_UNKNOWN         0
#define EZCFG_UPNP_ROLE_DEVICE          1
#define EZCFG_UPNP_ROLE_CONTROL_POINT   2

#define EZCFG_UPNP_ROLE_DEVICE_STRING   "device"
#define EZCFG_UPNP_ROLE_CONTROL_POINT_STRING "control-point"


/* ezcfg UPnP device type string */
#define EZCFG_UPNP_DEV_TYPE_UNKNOWN         0
#define EZCFG_UPNP_DEV_TYPE_IGD1            1

#define EZCFG_UPNP_DEV_TYPE_IGD1_STRING     "igd1"

/* upnp conf path */
#define EZCFG_UPNP_CONF_ROOT_PATH       SYSCONFDIR "/ezcfg_upnpd"
#define EZCFG_UPNP_IGD1_CONF_PATH       EZCFG_UPNP_CONF_ROOT_PATH "/" EZCFG_UPNP_DEV_TYPE_IGD1_STRING

/* IGD1 config nvram name */
#define EZCFG_UPNP_IGD1_DESCRIPTION_PATH   "igd1_description_path"
#define EZCFG_UPNP_IGD1_ROOT_DEVICE_XML_FILE	EZCFG_UPNP_IGD1_CONF_PATH "/InternetGatewayDevice1.xml"
/* IGD device nvram name */
#define EZCFG_UPNP_IGD1_IGD_FRIENDLY_NAME	"igd1_IGD.friendlyName"
#define EZCFG_UPNP_IGD1_IGD_MANUFACTURER	"igd1_IGD.manufacturer"
#define EZCFG_UPNP_IGD1_IGD_MANUFACTURER_URL	"igd1_IGD.manufacturerURL"
#define EZCFG_UPNP_IGD1_IGD_MODEL_DESCRIPTION	"igd1_IGD.modelDescription"
#define EZCFG_UPNP_IGD1_IGD_MODEL_NAME		"igd1_IGD.modelName"
#define EZCFG_UPNP_IGD1_IGD_MODEL_NUMBER	"igd1_IGD.modelNumber"
#define EZCFG_UPNP_IGD1_IGD_MODEL_URL		"igd1_IGD.modelURL"
#define EZCFG_UPNP_IGD1_IGD_SERIAL_NUMBER	"igd1_IGD.serialNumber"
#define EZCFG_UPNP_IGD1_IGD_UDN			"igd1_IGD.UDN"
#define EZCFG_UPNP_IGD1_IGD_UPC			"igd1_IGD.UPC"
#define EZCFG_UPNP_IGD1_IGD_UPC			"igd1_IGD.UPC"
#define EZCFG_UPNP_IGD1_IGD_ICON_NUMBER		"igd1_IGD_icon_number"
#define EZCFG_UPNP_IGD1_IGD_ICON		"igd1_IGD.icon"
#define EZCFG_UPNP_IGD1_IGD_SERVICE_NUMBER	"igd1_IGD_service_number"
#define EZCFG_UPNP_IGD1_IGD_SERVICE		"igd1_IGD.service"
/* WANDevice device nvram name */
#define EZCFG_UPNP_IGD1_WAND_NUMBER		"igd1_WANDevice_number"
#define EZCFG_UPNP_IGD1_WAND			"igd1_WANDevice"
/* WANConnectionDevice device nvram name */
#define EZCFG_UPNP_IGD1_WANCD			"igd1_WANConnectionDevice"
/* LANDevice device nvram name */
#define EZCFG_UPNP_IGD1_LAND_NUMBER		"igd1_LANDevice_number"
#define EZCFG_UPNP_IGD1_LAND			"igd1_LANDevice"

/* Device name string */
#define EZCFG_UPNP_WAN_DEVCIE_STRING                    "WANDevice"
#define EZCFG_UPNP_LAN_DEVCIE_STRING                    "LANDevice"
#define EZCFG_UPNP_WAN_CONNECTION_DEVCIE_STRING         "WANConnectionDevice"
#define EZCFG_UPNP_NUMBER_STRING                        "number"
#define EZCFG_UPNP_ICON_NUMBER_STRING                   "icon_number"
#define EZCFG_UPNP_SERVICE_NUMBER_STRING                "service_number"
#define EZCFG_UPNP_WANCD_NUMBER_STRING                  "WANConnectionDevice_number"

/* UPnP Description XML string */
#define EZCFG_UPNP_DESC_SPEC_VERSION_ELEMENT_NAME       "specVersion"
#define EZCFG_UPNP_DESC_MAJOR_ELEMENT_NAME              "major"
#define EZCFG_UPNP_DESC_MINOR_ELEMENT_NAME              "minor"
#define EZCFG_UPNP_DESC_URL_BASE_ELEMENT_NAME           "URLBase"
#define EZCFG_UPNP_DESC_DEVICE_ELEMENT_NAME             "device"
#define EZCFG_UPNP_DESC_DEVICE_TYPE_ELEMENT_NAME        "deviceType"
#define EZCFG_UPNP_DESC_FRIENDLY_NAME_ELEMENT_NAME      "friendlyName"
#define EZCFG_UPNP_DESC_MANUFACTURER_ELEMENT_NAME       "manufacturer"
#define EZCFG_UPNP_DESC_MANUFACTURER_URL_ELEMENT_NAME   "manufacturerURL"
#define EZCFG_UPNP_DESC_MODEL_DESCRIPTION_ELEMENT_NAME  "modelDescription"
#define EZCFG_UPNP_DESC_MODEL_NAME_ELEMENT_NAME         "modelName"
#define EZCFG_UPNP_DESC_MODEL_NUMBER_ELEMENT_NAME       "modelNumber"
#define EZCFG_UPNP_DESC_MODEL_URL_ELEMENT_NAME          "modelURL"
#define EZCFG_UPNP_DESC_SERIAL_NUMBER_ELEMENT_NAME      "serialNumber"
#define EZCFG_UPNP_DESC_UDN_ELEMENT_NAME                "UDN"
#define EZCFG_UPNP_DESC_UPC_ELEMENT_NAME                "UPC"
#define EZCFG_UPNP_DESC_ICON_LIST_ELEMENT_NAME          "iconList"
#define EZCFG_UPNP_DESC_ICON_ELEMENT_NAME               "icon"
#define EZCFG_UPNP_DESC_MIME_TYPE_ELEMENT_NAME          "mimetype"
#define EZCFG_UPNP_DESC_WIDTH_ELEMENT_NAME              "width"
#define EZCFG_UPNP_DESC_HEIGHT_ELEMENT_NAME             "height"
#define EZCFG_UPNP_DESC_DEPTH_ELEMENT_NAME              "depth"
#define EZCFG_UPNP_DESC_URL_ELEMENT_NAME                "url"
#define EZCFG_UPNP_DESC_SERVICE_LIST_ELEMENT_NAME       "serviceList"
#define EZCFG_UPNP_DESC_SERVICE_ELEMENT_NAME            "service"
#define EZCFG_UPNP_DESC_SERVICE_TYPE_ELEMENT_NAME       "serviceType"
#define EZCFG_UPNP_DESC_SERVICE_ID_ELEMENT_NAME         "serviceId"
#define EZCFG_UPNP_DESC_SCPD_URL_ELEMENT_NAME           "SCPDURL"
#define EZCFG_UPNP_DESC_CONTROL_URL_ELEMENT_NAME        "controlURL"
#define EZCFG_UPNP_DESC_EVENT_SUB_URL_ELEMENT_NAME      "eventSubURL"
#define EZCFG_UPNP_DESC_DEVICE_LIST_ELEMENT_NAME        "deviceList"
#define EZCFG_UPNP_DESC_PRESENTATION_URL_ELEMENT_NAME   "presentationURL"

/* ezcfg upnp http methods */
#define EZCFG_UPNP_HTTP_METHOD_NOTIFY       "NOTIFY"
#define EZCFG_UPNP_HTTP_METHOD_MSEARCH      "M-SEARCH"
/* ezcfg upnp http headers */
#define EZCFG_UPNP_HTTP_HEADER_HOST                 "Host"
#define EZCFG_UPNP_HTTP_HEADER_CACHE_CONTROL        "Cache-Control"
#define EZCFG_UPNP_HTTP_HEADER_LOCATION             "Location"
#define EZCFG_UPNP_HTTP_HEADER_NT                   "NT"
#define EZCFG_UPNP_HTTP_HEADER_NTS                  "NTS"
#define EZCFG_UPNP_HTTP_HEADER_SERVER               "Server"
#define EZCFG_UPNP_HTTP_HEADER_USN                  "USN"
#define EZCFG_UPNP_HTTP_HEADER_MAN                  "Man"
#define EZCFG_UPNP_HTTP_HEADER_MX                   "MX"
#define EZCFG_UPNP_HTTP_HEADER_ST                   "ST"
#define EZCFG_UPNP_HTTP_HEADER_DATE                 "Date"
#define EZCFG_UPNP_HTTP_HEADER_EXT                  "Ext"

#endif /* _EZCFG_UPNP_H_ */
