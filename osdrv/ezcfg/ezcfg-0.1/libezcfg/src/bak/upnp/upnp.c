/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : upnp/upnp.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-09   0.1       Write it from scratch
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
#include <netinet/in.h>

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-upnp.h"

#if 1
#define DBG(format, args...) do { \
	char dbg_path[256]; \
	FILE *dbg_fp; \
	snprintf(dbg_path, 256, "/tmp/%d-debug.txt", getpid()); \
	dbg_fp = fopen(dbg_path, "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

/**
 * private functions
 **/
static void upnp_device_icon_delete(device_icon_t *ip)
{
	if (ip->mimetype != NULL)
		free(ip->mimetype);

	if (ip->url != NULL)
		free(ip->url);

	free(ip);
}

static void upnp_device_delete_icon_list(device_icon_t **list)
{
	device_icon_t *ip;
	ip = *list;
	while (ip != NULL) {
		*list = ip->next;
		upnp_device_icon_delete(ip);
		ip = *list;
	}
}

static void upnp_service_delete(upnp_service_t *sp)
{
	if (sp->serviceType != NULL)
		free(sp->serviceType);

	if (sp->serviceId != NULL)
		free(sp->serviceId);

	if (sp->SCPDURL != NULL)
		free(sp->SCPDURL);

	if (sp->controlURL != NULL)
		free(sp->controlURL);

	if (sp->eventSubURL != NULL)
		free(sp->eventSubURL);

	free(sp);
}

static void upnp_device_delete_service_list(upnp_service_t **list)
{
	upnp_service_t *sp;
	sp = *list;
	while (sp != NULL) {
		*list = sp->next;
		upnp_service_delete(sp);
		sp = *list;
	}
}

static void upnp_device_delete_list(upnp_device_t **list);

static void upnp_delete_device(upnp_device_t dev)
{
	if (dev.deviceType != NULL)
		free(dev.deviceType);

	if (dev.friendlyName != NULL)
		free(dev.friendlyName);

	if (dev.manufacturer != NULL)
		free(dev.manufacturer);

	if (dev.manufacturerURL != NULL)
		free(dev.manufacturerURL);

	if (dev.modelDescription != NULL)
		free(dev.modelDescription);

	if (dev.modelName != NULL)
		free(dev.modelName);

	if (dev.modelNumber != NULL)
		free(dev.modelNumber);

	if (dev.modelURL != NULL)
		free(dev.modelURL);

	if (dev.serialNumber != NULL)
		free(dev.serialNumber);

	if (dev.UDN != NULL)
		free(dev.UDN);

	if (dev.UPC != NULL)
		free(dev.UPC);

	if (dev.iconList != NULL)
		upnp_device_delete_icon_list(&(dev.iconList));

	if (dev.serviceList != NULL)
		upnp_device_delete_service_list(&(dev.serviceList));

	if (dev.deviceList != NULL)
		upnp_device_delete_list(&(dev.deviceList));

	if (dev.presentationURL != NULL)
		free(dev.presentationURL);
}

static void upnp_device_delete_list(upnp_device_t **list)
{
	upnp_device_t *dp;
	dp = *list;
	while (dp != NULL) {
		*list = dp->next;
		upnp_delete_device(*dp);
		free(dp);
		dp = *list;
	}
}

static void upnp_delete_control_point(upnp_control_point_t cp)
{
	return;
}

static void upnp_delete_if_list(upnp_if_t **list)
{
	upnp_if_t *ip;
	ip = *list;
	while (ip != NULL) {
		*list = ip->next;
		free(ip);
		ip = *list;
	}
}

static bool upnp_device_parse_icon_list(device_icon_t **list, struct ezcfg_xml *xml, const int root_idx)
{
	device_icon_t *ip;
	char *p;
	int root_etag_idx, icon_idx;
	int pi, si, ci;

	root_etag_idx = ezcfg_xml_get_element_etag_index_by_index(xml, root_idx);
	/* find first <icon>, RECOMMENDED */
	icon_idx = -1;
	icon_idx = ezcfg_xml_get_element_index(xml, root_idx, icon_idx, EZCFG_UPNP_DESC_ICON_ELEMENT_NAME);
	while((icon_idx > root_idx) && (icon_idx < root_etag_idx)) {
		ip = malloc(sizeof(device_icon_t));
		if (ip == NULL) {
			goto fail_exit;
		}
		memset(ip, 0, sizeof(device_icon_t));
		/* put to icon list */
		ip->next = *list;
		*list = ip;

		pi = icon_idx; si = -1;
		/* get <mimetype>, REQUIRED */
		ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_MIME_TYPE_ELEMENT_NAME);
		if (ci < pi) {
			goto fail_exit;
		}
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p == NULL) {
			goto fail_exit;
		}
		ip->mimetype = strdup(p);
		if (ip->mimetype == NULL) {
			goto fail_exit;
		}

		/* get <width>, REQUIRED */
		ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_WIDTH_ELEMENT_NAME);
		if (ci < pi) {
			goto fail_exit;
		}
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p == NULL) {
			goto fail_exit;
		}
		ip->width = atoi(p);

		/* get <height>, REQUIRED */
		ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_HEIGHT_ELEMENT_NAME);
		if (ci < pi) {
			goto fail_exit;
		}
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p == NULL) {
			goto fail_exit;
		}
		ip->height = atoi(p);

		/* get <depth>, REQUIRED */
		ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_DEPTH_ELEMENT_NAME);
		if (ci < pi) {
			goto fail_exit;
		}
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p == NULL) {
			goto fail_exit;
		}
		ip->depth = atoi(p);

		/* get <url>, REQUIRED */
		ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_URL_ELEMENT_NAME);
		if (ci < pi) {
			goto fail_exit;
		}
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p == NULL) {
			goto fail_exit;
		}
		ip->url = strdup(p);
		if (ip->url == NULL) {
			goto fail_exit;
		}

		/* move to next <icon> section */
		icon_idx = ezcfg_xml_get_element_index(xml, root_idx, icon_idx, EZCFG_UPNP_DESC_ICON_ELEMENT_NAME);
	}

	return true;

fail_exit:
	upnp_device_delete_icon_list(list);
	*list = NULL;
	return false;
}

static bool upnp_device_parse_service_list(upnp_service_t **list, struct ezcfg_xml *xml, const int root_idx)
{
	upnp_service_t *sp;
	char *p;
	int root_etag_idx, service_idx;
	int pi, si, ci;

	root_etag_idx = ezcfg_xml_get_element_etag_index_by_index(xml, root_idx);
	/* find first <service>, OPTIONAL */
	service_idx = -1;
	service_idx = ezcfg_xml_get_element_index(xml, root_idx, service_idx, EZCFG_UPNP_DESC_SERVICE_ELEMENT_NAME);
	while((service_idx > root_idx) && (service_idx < root_etag_idx)) {
		sp = malloc(sizeof(upnp_service_t));
		if (sp == NULL) {
			goto fail_exit;
		}
		memset(sp, 0, sizeof(upnp_service_t));
		/* put to service list */
		sp->next = *list;
		*list = sp;

		pi = service_idx; si = -1;
		/* get <serviceType>, REQUIRED */
		ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_SERVICE_TYPE_ELEMENT_NAME);
		if (ci < pi) {
			goto fail_exit;
		}
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p == NULL) {
			goto fail_exit;
		}
		sp->serviceType = strdup(p);
		if (sp->serviceType == NULL) {
			goto fail_exit;
		}

		/* get <serviceId>, REQUIRED */
		ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_SERVICE_ID_ELEMENT_NAME);
		if (ci < pi) {
			goto fail_exit;
		}
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p == NULL) {
			goto fail_exit;
		}
		sp->serviceId = strdup(p);
		if (sp->serviceId == NULL) {
			goto fail_exit;
		}

		/* get <SCPDURL>, REQUIRED */
		ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_SCPD_URL_ELEMENT_NAME);
		if (ci < pi) {
			goto fail_exit;
		}
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p == NULL) {
			goto fail_exit;
		}
		sp->SCPDURL = strdup(p);
		if (sp->SCPDURL == NULL) {
			goto fail_exit;
		}

		/* get <controlURL>, REQUIRED */
		ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_CONTROL_URL_ELEMENT_NAME);
		if (ci < pi) {
			goto fail_exit;
		}
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p == NULL) {
			goto fail_exit;
		}
		sp->controlURL = strdup(p);
		if (sp->controlURL == NULL) {
			goto fail_exit;
		}

		/* get <eventSubURL>, REQUIRED */
		ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_EVENT_SUB_URL_ELEMENT_NAME);
		if (ci < pi) {
			goto fail_exit;
		}
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p == NULL) {
			goto fail_exit;
		}
		sp->eventSubURL = strdup(p);
		if (sp->eventSubURL == NULL) {
			goto fail_exit;
		}

		/* move to next <service> section */
		service_idx = ezcfg_xml_get_element_index(xml, root_idx, service_idx, EZCFG_UPNP_DESC_SERVICE_ELEMENT_NAME);
	}

	return true;

fail_exit:
	upnp_device_delete_service_list(list);
	*list = NULL;
	return false;
}

static bool upnp_device_parse_device(upnp_device_t *dp, struct ezcfg_xml *xml, const int root_idx);

static bool upnp_device_parse_device_list(upnp_device_t **list, struct ezcfg_xml *xml, const int root_idx)
{
	upnp_device_t *dp;
	int root_etag_idx, device_idx;

	root_etag_idx = ezcfg_xml_get_element_etag_index_by_index(xml, root_idx);
	/* find first <service>, OPTIONAL */
	device_idx = -1;
	device_idx = ezcfg_xml_get_element_index(xml, root_idx, device_idx, EZCFG_UPNP_DESC_DEVICE_ELEMENT_NAME);
	while((device_idx > root_idx) && (device_idx < root_etag_idx)) {
		dp = malloc(sizeof(upnp_device_t));
		if (dp == NULL) {
			goto fail_exit;
		}
		memset(dp, 0, sizeof(upnp_device_t));
		/* put to service list */
		dp->next = *list;
		*list = dp;

		if (upnp_device_parse_device(dp, xml, device_idx) == false) {
			goto fail_exit;
		}

		/* move to next <device> section */
		device_idx = ezcfg_xml_get_element_index(xml, root_idx, device_idx, EZCFG_UPNP_DESC_DEVICE_ELEMENT_NAME);
	}

	return true;

fail_exit:
	upnp_device_delete_list(list);
	*list = NULL;
	return false;
}

static bool upnp_device_parse_device(upnp_device_t *dp, struct ezcfg_xml *xml, const int root_idx)
{
	char *p;
	int pi, si, ci;

	pi = root_idx; si = -1;
	/* get <deviceType>, REQUIRED */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_DEVICE_TYPE_ELEMENT_NAME);
	if (ci < pi) {
		return false;
	}
	p = ezcfg_xml_get_element_content_by_index(xml, ci);
	if (p == NULL) {
		return false;
	}
	dp->deviceType = strdup(p);
	if (dp->deviceType == NULL) {
		return false;
	}

	/* get <friendlyName>, REQUIRED */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_FRIENDLY_NAME_ELEMENT_NAME);
	if (ci < pi) {
		return false;
	}
	p = ezcfg_xml_get_element_content_by_index(xml, ci);
	if (p == NULL) {
		return false;
	}
	dp->friendlyName = strdup(p);
	if (dp->friendlyName == NULL) {
		return false;
	}

	/* get <manufacturer>, REQUIRED */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_MANUFACTURER_ELEMENT_NAME);
	if (ci < pi) {
		return false;
	}
	p = ezcfg_xml_get_element_content_by_index(xml, ci);
	if (p == NULL) {
		return false;
	}
	dp->manufacturer = strdup(p);
	if (dp->manufacturer == NULL) {
		return false;
	}

	/* get <manufacturerURL>, OPTIONAL */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_MANUFACTURER_URL_ELEMENT_NAME);
	if (ci > pi) {
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p != NULL) {
			dp->manufacturerURL = strdup(p);
			if (dp->manufacturerURL == NULL) {
				return false;
			}
		}
	}

	/* get <modelDescription>, RECOMMENDED */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_MODEL_DESCRIPTION_ELEMENT_NAME);
	if (ci > pi) {
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p != NULL) {
			dp->modelDescription = strdup(p);
			if (dp->modelDescription == NULL) {
				return false;
			}
		}
	}

	/* get <modelName>, REQUIRED */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_MODEL_NAME_ELEMENT_NAME);
	if (ci < pi) {
		return false;
	}
	p = ezcfg_xml_get_element_content_by_index(xml, ci);
	if (p == NULL) {
		return false;
	}
	dp->modelName = strdup(p);
	if (dp->modelName == NULL) {
		return false;
	}

	/* get <modelNumber>, RECOMMENDED */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_MODEL_NUMBER_ELEMENT_NAME);
	if (ci > pi) {
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p != NULL) {
			dp->modelNumber = strdup(p);
			if (dp->modelNumber == NULL) {
				return false;
			}
		}
	}

	/* get <modelURL>, OPTIONAL */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_MODEL_URL_ELEMENT_NAME);
	if (ci > pi) {
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p != NULL) {
			dp->modelURL = strdup(p);
			if (dp->modelURL == NULL) {
				return false;
			}
		}
	}

	/* get <serialNumber>, RECOMMENDED */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_SERIAL_NUMBER_ELEMENT_NAME);
	if (ci > pi) {
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p != NULL) {
			dp->serialNumber = strdup(p);
			if (dp->serialNumber == NULL) {
				return false;
			}
		}
	}

	/* get <UDN>, REQUIRED */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_UDN_ELEMENT_NAME);
	if (ci < pi) {
		return false;
	}
	p = ezcfg_xml_get_element_content_by_index(xml, ci);
	if (p == NULL) {
		return false;
	}
	dp->UDN = strdup(p);
	if (dp->UDN == NULL) {
		return false;
	}

	/* get <UPC>, OPTIONAL */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_UPC_ELEMENT_NAME);
	if (ci > pi) {
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p != NULL) {
			dp->UPC = strdup(p);
			if (dp->UPC == NULL) {
				return false;
			}
		}
	}

	/* get <iconList>, Required if and only if device has one or more icons. */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_ICON_LIST_ELEMENT_NAME);
	if (ci > pi) {
		if (upnp_device_parse_icon_list(&(dp->iconList), xml, ci) == false) {
			return false;
		}
	}

	/* get <serviceList>, OPTIONAL */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_SERVICE_LIST_ELEMENT_NAME);
	if (ci > pi) {
		if (upnp_device_parse_service_list(&(dp->serviceList), xml, ci) == false) {
			return false;
		}
		do {
			upnp_service_t *sp = dp->serviceList;
			while (sp != NULL) {
				sp = sp->next;
			}
		} while (0);
	}

	/* get <deviceList>, REQUIRED if and only if root device has embedded devices. */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_DEVICE_LIST_ELEMENT_NAME);
	if (ci > pi) {
		if (upnp_device_parse_device_list(&(dp->deviceList), xml, ci) == false) {
			return false;
		}
	}

	/* get <presentationURL>, RECOMMENDED */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_PRESENTATION_URL_ELEMENT_NAME);
	if (ci > pi) {
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p != NULL) {
			dp->presentationURL = strdup(p);
			if (dp->presentationURL == NULL) {
				return false;
			}
		}
	}

	return true;
}

static bool upnp_fill_info(struct ezcfg_upnp *upnp, struct ezcfg_xml *xml)
{
	int pi, si, ci;
	char *p;

	/* get <specVersion> */
	pi = 0; si = -1;
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_SPEC_VERSION_ELEMENT_NAME);
	if (ci < pi) {
		return false;
	}

	/* get <major> */
	pi = ci; si = -1;
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_MAJOR_ELEMENT_NAME);
	if (ci < pi) {
		return false;
	}
	p = ezcfg_xml_get_element_content_by_index(xml, ci);
	if (p == NULL) {
		return false;
	}
	upnp->version_major = atoi(p);
	if (upnp->version_major != 1) {
		return false;
	}

	/* get <minor> */
	si = ci;
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_MINOR_ELEMENT_NAME);
	if (ci < pi) {
		return false;
	}
	p = ezcfg_xml_get_element_content_by_index(xml, ci);
	if (p == NULL) {
		return false;
	}
	upnp->version_minor = atoi(p);
	if ((upnp->version_minor != 0) &&
	    (upnp->version_minor != 1)) {
		return false;
	}

	/* get <URLBase> FIXME: UDA-1.0 only!!!*/
	if ((upnp->version_major == 1) &&
	    (upnp->version_minor == 0)) {
		pi = 0; si = -1;
		ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_URL_BASE_ELEMENT_NAME);
		if (ci > pi) {
			p = ezcfg_xml_get_element_content_by_index(xml, ci);
			if (p != NULL) {
				upnp->URLBase = strdup(p);
				if (upnp->URLBase == NULL) {
					return false;
				}
			}
		}
	}

	/* get <device> */
	pi = 0; si = -1;
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_DEVICE_ELEMENT_NAME);
	if (ci < pi) {
		return false;
	}

	/* parse <device> section */
	if (upnp->role == EZCFG_UPNP_ROLE_DEVICE) {
		return upnp_device_parse_device(&((upnp->u).dev), xml, ci);
	}
	else if (upnp->role == EZCFG_UPNP_ROLE_CONTROL_POINT) {
		return upnp_device_parse_device(&((upnp->u).cp.device_template), xml, ci);
	}

	return false;
}

/**
 * Public functions
 **/
void ezcfg_upnp_delete(struct ezcfg_upnp *upnp)
{
	//struct ezcfg *ezcfg;

	ASSERT(upnp != NULL);

	//ezcfg = upnp->ezcfg;

	if (upnp->role == EZCFG_UPNP_ROLE_DEVICE) {
		upnp_delete_device(upnp->u.dev);
	}
	else if (upnp->role == EZCFG_UPNP_ROLE_CONTROL_POINT) {
		upnp_delete_control_point(upnp->u.cp);
	}

	upnp_delete_if_list(&(upnp->ifs));

	if (upnp->URLBase != NULL)
		free(upnp->URLBase);

	free(upnp);
}

/**
 * ezcfg_upnp_new:
 * Create ezcfg upnp protocol data structure
 * Returns: a new ezcfg upnp protocol data structure
 **/
struct ezcfg_upnp *ezcfg_upnp_new(struct ezcfg *ezcfg)
{
	struct ezcfg_upnp *upnp;

	ASSERT(ezcfg != NULL);

	/* initialize upnp protocol data structure */
	upnp = malloc(sizeof(struct ezcfg_upnp));
	if (upnp == NULL) {
		return NULL;
	}
	memset(upnp, 0, sizeof(struct ezcfg_upnp));
	upnp->ezcfg = ezcfg;
	upnp->role = EZCFG_UPNP_ROLE_UNKNOWN;
	upnp->ifs = NULL;
	upnp->URLBase = NULL;
	upnp->next = NULL;

	return upnp;
}

bool ezcfg_upnp_set_role(struct ezcfg_upnp *upnp, int role)
{
	ASSERT(upnp != NULL);

	upnp->role = role;

	return true;
}

bool ezcfg_upnp_set_device_type(struct ezcfg_upnp *upnp, int type)
{
	ASSERT(upnp != NULL);

	upnp->device_type = type;

	return true;
}

bool ezcfg_upnp_if_list_insert(struct ezcfg_upnp *upnp, char ifname[IFNAMSIZ], int life_time)
{
	upnp_if_t *ifp;

	ASSERT(upnp != NULL);

	/* initialize upnp_if_t data structure */
	ifp = malloc(sizeof(upnp_if_t));
	if (ifp == NULL)
		return false;

	memset(ifp, 0, sizeof(upnp_if_t));
	strncpy(ifp->ifname, ifname, IFNAMSIZ);
	ifp->life_time = life_time;
	ifp->next = upnp->ifs;
	upnp->ifs = ifp;
	return true;
}

bool ezcfg_upnp_list_insert(struct ezcfg_upnp **list, struct ezcfg_upnp *upnp)
{
	ASSERT(list != NULL);
	ASSERT(upnp != NULL);

	upnp->next = *list;
	*list = upnp;
	return true;
}

void ezcfg_upnp_list_delete(struct ezcfg_upnp **list)
{
	struct ezcfg_upnp *up;

	ASSERT(list != NULL);

	up = *list;
	while (up != NULL) {
		*list = up->next;
		ezcfg_upnp_delete(up);
		up = *list;
	}
}

bool ezcfg_upnp_parse_description(struct ezcfg_upnp *upnp, const char *path)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml = NULL;
	struct stat stat_buf;
	char *buf = NULL;
	int buf_len;
	FILE *fp = NULL;
	bool ret = false;

	ASSERT(upnp != NULL);
	ASSERT(path != NULL);

	if (stat(path, &stat_buf) < 0)
		return false;

	buf_len = stat_buf.st_size + 1;
	buf = malloc(buf_len);
	if (buf == NULL) {
		goto func_out;
	}

	memset(buf, 0, buf_len);

	fp = fopen(path, "r");
	if (fp == NULL) {
		goto func_out;
	}

	if (fread(buf, 1, stat_buf.st_size, fp) != (size_t)stat_buf.st_size) {
		goto func_out;
	}
	buf[buf_len -1] = '\0';

	ezcfg = upnp->ezcfg;

	xml = ezcfg_xml_new(ezcfg);
	if (xml == NULL) {
		goto func_out;
	}
	
	if (ezcfg_xml_parse(xml, buf, buf_len) == false) {
		goto func_out;
	}

	/* fill UPnP info */
	if (upnp_fill_info(upnp, xml) == false) {
		goto func_out;
	}

	ret = true;

func_out:
	if (fp != NULL) {
		fclose(fp);
	}

	if (buf != NULL) {
		free(buf);
	}

	if (xml != NULL) {
		ezcfg_xml_delete(xml);
	}

	return ret;
}
