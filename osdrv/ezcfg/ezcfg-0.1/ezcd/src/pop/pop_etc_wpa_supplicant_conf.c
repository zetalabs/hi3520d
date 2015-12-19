/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_wpa_supplicant_conf.c
 *
 * Description  : ezbox /etc/wpa_supplicant.conf file generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-08-02   0.1       Write it from scratch
 * ============================================================================
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>
#include <net/if.h>

#include "ezcd.h"

#if 0
#define DBG(format, args...) do {\
	FILE *fp = fopen("/tmp/wpa_conf.debug", "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif



int pop_etc_wpa_supplicant_conf(int flag)
{
        FILE *file = NULL;
	char wifi_nic[IFNAMSIZ];
	char name[64];
	char buf[128];
	char *s_name;
	int rc;

	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WIFI_LAN, IFNAME), wifi_nic, sizeof(wifi_nic));
	if (rc <= 0) {
		return (EXIT_FAILURE);
	}

	/* generate /etc/wpa_supplicant-$ifname.conf */
	snprintf(name, sizeof(name), "/etc/wpa_supplicant-%s.conf", wifi_nic);
	file = fopen(name, "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	s_name = name + strlen(EZCFG_WPA_SUPPLICANT_NVRAM_PREFIX);

	switch (flag) {
	case RC_ACT_START :
		/* Set the wpa_cli control interface */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, CTRL_INTERFACE));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc <= 0) {
			snprintf(buf, sizeof(buf), "/var/run/wpa_supplicant-%s.conf", wifi_nic);
			rc = ezcfg_api_nvram_set(name, buf);
		}
		fprintf(file, "%s=%s\n", s_name, buf);

		/* IEEE 802.1X/EAPOL version */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, EAPOL_VERSION));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", s_name, buf);
		}

		/* AP scanning/selection */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, AP_SCAN));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", s_name, buf);
		}

		/* EAP fast re-authentication */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, FAST_REAUTH));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", s_name, buf);
		}

		/* Driver interface parameters */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, DRIVER_PARAM));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=\"%s\"\n", s_name, buf);
		}

		/* Country code */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, COUNTRY));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", s_name, buf);
		}

		/* Maximum lifetime for PMKSA in seconds; default 43200 */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, DOT11_RSNA_CONFIG_PMK_LIFETIME));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", s_name, buf);
		}

		/* Threshold for reauthentication (percentage of PMK lifetime); default 70 */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, DOT11_RSNA_CONFIG_PMK_REAUTH_THRESHOLD));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", s_name, buf);
		}

		/* Timeout for security association negotiation in seconds; default 60 */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, DOT11_RSNA_CONFIG_SA_TIMEOUT));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", s_name, buf);
		}

		/* Wi-Fi Protected Setup (WPS) parameters */
		/* Universally Unique IDentifier (UUID; see RFC 4122) of the device */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, UUID));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", s_name, buf);
		}

		/* Device Name */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, DEVICE_NAME));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", s_name, buf);
		}

		/* Manufacturer */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, MANUFACTURER));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", s_name, buf);
		}

		/* Model Name */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, MODEL_NAME));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", s_name, buf);
		}

		/* Model Number */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, MODEL_NUMBER));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", s_name, buf);
		}

		/* Serial Number */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, SERIAL_NUMBER));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", s_name, buf);
		}

		/* Primary Device Type */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, DEVICE_TYPE));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", s_name, buf);
		}

		/* OS Version */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, OS_VERSION));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", s_name, buf);
		}

		/* Config Methods */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, CONFIG_METHODS));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", s_name, buf);
		}

		/* Credential processing */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, WPS_CRED_PROCESSING));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", s_name, buf);
		}

		/* Maximum number of BSS entries to keep in memory */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, BSS_MAX_COUNT));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", s_name, buf);
		}

		/* filter_ssids - SSID-based scan result filtering */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, FILTER_SSIDS));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s=%s\n", s_name, buf);
		}

		/* network block */
		fprintf(file, "network={\n");

		/* disabled */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_DISABLED));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* id_str: Network identifier string for external scripts */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_ID_STR));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* ssid: SSID (mandatory); */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_SSID));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* scan_ssid */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_SCAN_SSID));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* bssid: BSSID (optional); */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_BSSID));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* priority: priority group (integer) */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_PRIORITY));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* mode: IEEE 802.11 operation mode */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_MODE));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* frequency: Channel frequency in megahertz (MHz) for IBSS, */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_FREQUENCY));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* scan_freq: List of frequencies to scan */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_SCAN_FREQ));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* freq_list: Array of allowed frequencies */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_FREQ_LIST));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* proto: list of accepted protocols */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_PROTO));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* key_mgmt: list of accepted authenticated key management protocols */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_KEY_MGMT));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* auth_alg: list of allowed IEEE 802.11 authentication algorithms */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_AUTH_ALG));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* pairwise: list of accepted pairwise (unicast) ciphers for WPA */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_PAIRWISE));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* group: list of accepted group (broadcast/multicast) ciphers for WPA */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_GROUP));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* psk: WPA preshared key; 256-bit pre-shared key */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_PSK));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* eapol_flags: IEEE 802.1X/EAPOL options (bit field) */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_EAPOL_FLAGS));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* mixed_cell: This option can be used to configure whether so called mixed cells */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_MIXED_CELL));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* proactive_key_caching: Enable/disable opportunistic PMKSA caching for WPA2 */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_PROACTIVE_KEY_CACHING));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* wep_key0..3: Static WEP key */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_WEP_KEY0));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_WEP_KEY1));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_WEP_KEY2));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_WEP_KEY3));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* wep_tx_keyidx: Default WEP key index (TX) */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_WEP_TX_KEYIDX));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* peerkey: Whether PeerKey negotiation for direct links (IEEE 802.11e DLS) is allowed */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_PEERKEY));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* wpa_ptk_rekey: Maximum lifetime for PTK in seconds */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_WPA_PTK_REKEY));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* Following fields are only used with internal EAP implementation */
		/* eap: space-separated list of accepted EAP methods */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_EAP));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* identity: Identity string for EAP */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_IDENTITY));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", s_name, buf);
		}

		/* anonymous_identity: Anonymous identity string for EAP */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_ANONYMOUS_IDENTITY));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", s_name, buf);
		}

		/* password: Password string for EAP */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_PASSWORD));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", s_name, buf);
		}

		/* ca_cert: File path to CA certificate file (PEM/DER) */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_CA_CERT));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* ca_path: Directory path for CA certificate files (PEM) */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_CA_PATH));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* client_cert: File path to client certificate file (PEM/DER) */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_CLIENT_CERT));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* private_key: File path to client private key file (PEM/DER/PFX) */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_PRIVATE_KEY));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", s_name, buf);
		}

		/* private_key_passwd: Password for private key file */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_PRIVATE_KEY_PASSWD));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", s_name, buf);
		}

		/* dh_file: File path to DH/DSA parameters file (in PEM format) */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_DH_FILE));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* subject_match: Substring to be matched against the subject of the authentication server certificate */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_SUBJECT_MATCH));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", s_name, buf);
		}

		/* altsubject_match: Semicolon separated string of entries to be matched against
		 * the alternative subject name of the authentication server certificate */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_ALTSUBJECT_MATCH));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", s_name, buf);
		}

		/* phase1: Phase1 (outer authentication, i.e., TLS tunnel) parameters */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_PHASE1));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", s_name, buf);
		}

		/* phase2: Phase2 (inner authentication with TLS tunnel) parameters */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_PHASE2));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", s_name, buf);
		}

		/* Following certificate/private key fields are used in inner Phase2
		 * authentication when using EAP-TTLS or EAP-PEAP */
		/* ca_cert2: File path to CA certificate file */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_CA_CERT2));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* ca_path2: Directory path for CA certificate files (PEM) */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_CA_PATH2));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* client_cert2: File path to client certificate file (PEM/DER) */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_CLIENT_CERT2));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* private_key2: File path to client private key file (PEM/DER/PFX) */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_PRIVATE_KEY2));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", s_name, buf);
		}

		/* private_key2_passwd: Password for private key file */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_PRIVATE_KEY2_PASSWD));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", s_name, buf);
		}

		/* dh_file2: File path to DH/DSA parameters file (in PEM format) */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_DH_FILE2));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* subject_match2: Substring to be matched against the subject of the authentication server certificate */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_SUBJECT_MATCH2));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", s_name, buf);
		}

		/* altsubject_match2: Semicolon separated string of entries to be matched against
		 * the alternative subject name of the authentication server certificate */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_ALTSUBJECT_MATCH2));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", s_name, buf);
		}

		/* fragment_size: Maximum EAP fragment size in bytes (default 1398) */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_FRAGMENT_SIZE));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* EAP-FAST variables */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_PAC_FILE));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* fast_max_pac_list_len */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_FAST_MAX_PAC_LIST_LEN));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* fast_pac_format */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_FAST_PAC_FORMAT));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* eap_workaround */
		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, NETWORK_EAP_WORKAROUND));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", s_name, buf);
		}

		/* end of wireless network */
		fprintf(file, "}\n");

		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
