/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-wpa_supplicant.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-08-04   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_WPA_SUPPLICANT_H_
#define _EZCFG_WPA_SUPPLICANT_H_

/* ezcfg nvram name prefix */
#define EZCFG_WPA_SUPPLICANT_NVRAM_PREFIX            "wpa_supplicant."

/* hostapd wpa_supplicant related options */
#define EZCFG_WPA_SUPPLICANT_CTRL_INTERFACE         "ctrl_interface"
#define EZCFG_WPA_SUPPLICANT_EAPOL_VERSION          "eapol_version"
#define EZCFG_WPA_SUPPLICANT_AP_SCAN                "ap_scan"
#define EZCFG_WPA_SUPPLICANT_FAST_REAUTH            "fast_reauth"
#define EZCFG_WPA_SUPPLICANT_DRIVER_PARAM           "driver_param"
#define EZCFG_WPA_SUPPLICANT_COUNTRY                "country"
#define EZCFG_WPA_SUPPLICANT_DOT11_RSNA_CONFIG_PMK_LIFETIME \
	"dot11RSNAConfigPMKLifetime"
#define EZCFG_WPA_SUPPLICANT_DOT11_RSNA_CONFIG_PMK_REAUTH_THRESHOLD \
	"dot11RSNAConfigPMKReauthThreshold"
#define EZCFG_WPA_SUPPLICANT_DOT11_RSNA_CONFIG_SA_TIMEOUT \
	"dot11RSNAConfigSATimeout"
#define EZCFG_WPA_SUPPLICANT_UUID                   "uuid"
#define EZCFG_WPA_SUPPLICANT_DEVICE_NAME            "device_name"
#define EZCFG_WPA_SUPPLICANT_MANUFACTURER           "manufacturer"
#define EZCFG_WPA_SUPPLICANT_MODEL_NAME             "model_name"
#define EZCFG_WPA_SUPPLICANT_MODEL_NUMBER           "model_number"
#define EZCFG_WPA_SUPPLICANT_SERIAL_NUMBER          "serial_number"
#define EZCFG_WPA_SUPPLICANT_DEVICE_TYPE            "device_type"
#define EZCFG_WPA_SUPPLICANT_OS_VERSION             "os_version"
#define EZCFG_WPA_SUPPLICANT_CONFIG_METHODS         "config_methods"
#define EZCFG_WPA_SUPPLICANT_WPS_CRED_PROCESSING    "wps_cred_processing"
#define EZCFG_WPA_SUPPLICANT_BSS_MAX_COUNT          "bss_max_count"
#define EZCFG_WPA_SUPPLICANT_FILTER_SSIDS           "filter_ssids"
#define EZCFG_WPA_SUPPLICANT_NETWORK_DISABLED       "disabled"
#define EZCFG_WPA_SUPPLICANT_NETWORK_ID_STR         "id_str"
#define EZCFG_WPA_SUPPLICANT_NETWORK_SSID           "ssid"
#define EZCFG_WPA_SUPPLICANT_NETWORK_SCAN_SSID      "scan_ssid"
#define EZCFG_WPA_SUPPLICANT_NETWORK_BSSID          "bssid"
#define EZCFG_WPA_SUPPLICANT_NETWORK_PRIORITY       "priority"
#define EZCFG_WPA_SUPPLICANT_NETWORK_MODE           "mode"
#define EZCFG_WPA_SUPPLICANT_NETWORK_FREQUENCY      "frequency"
#define EZCFG_WPA_SUPPLICANT_NETWORK_SCAN_FREQ      "scan_freq"
#define EZCFG_WPA_SUPPLICANT_NETWORK_FREQ_LIST      "freq_list"
#define EZCFG_WPA_SUPPLICANT_NETWORK_PROTO          "proto"
#define EZCFG_WPA_SUPPLICANT_NETWORK_KEY_MGMT       "key_mgmt"
#define EZCFG_WPA_SUPPLICANT_NETWORK_AUTH_ALG       "auth_alg"
#define EZCFG_WPA_SUPPLICANT_NETWORK_PAIRWISE       "pairwise"
#define EZCFG_WPA_SUPPLICANT_NETWORK_GROUP          "group"
#define EZCFG_WPA_SUPPLICANT_NETWORK_PSK            "psk"
#define EZCFG_WPA_SUPPLICANT_NETWORK_EAPOL_FLAGS    "eapol_flags"
#define EZCFG_WPA_SUPPLICANT_NETWORK_MIXED_CELL     "mixed_cell"
#define EZCFG_WPA_SUPPLICANT_NETWORK_PROACTIVE_KEY_CACHING \
	"proactive_key_caching"
#define EZCFG_WPA_SUPPLICANT_NETWORK_WEP_KEY0       "wep_key0"
#define EZCFG_WPA_SUPPLICANT_NETWORK_WEP_KEY1       "wep_key1"
#define EZCFG_WPA_SUPPLICANT_NETWORK_WEP_KEY2       "wep_key2"
#define EZCFG_WPA_SUPPLICANT_NETWORK_WEP_KEY3       "wep_key3"
#define EZCFG_WPA_SUPPLICANT_NETWORK_WEP_TX_KEYIDX  "wep_tx_keyidx"
#define EZCFG_WPA_SUPPLICANT_NETWORK_PEERKEY        "peerkey"
#define EZCFG_WPA_SUPPLICANT_NETWORK_WPA_PTK_REKEY  "wpa_ptk_rekey"
#define EZCFG_WPA_SUPPLICANT_NETWORK_EAP            "eap"
#define EZCFG_WPA_SUPPLICANT_NETWORK_IDENTITY       "identity"
#define EZCFG_WPA_SUPPLICANT_NETWORK_ANONYMOUS_IDENTITY \
	"anonymous_identity"
#define EZCFG_WPA_SUPPLICANT_NETWORK_PASSWORD       "password"
#define EZCFG_WPA_SUPPLICANT_NETWORK_CA_CERT        "ca_cert"
#define EZCFG_WPA_SUPPLICANT_NETWORK_CA_PATH        "ca_path"
#define EZCFG_WPA_SUPPLICANT_NETWORK_CLIENT_CERT    "client_cert"
#define EZCFG_WPA_SUPPLICANT_NETWORK_PRIVATE_KEY    "private_key"
#define EZCFG_WPA_SUPPLICANT_NETWORK_PRIVATE_KEY_PASSWD \
	"private_key_passwd"
#define EZCFG_WPA_SUPPLICANT_NETWORK_DH_FILE        "dh_file"
#define EZCFG_WPA_SUPPLICANT_NETWORK_SUBJECT_MATCH  "subject_match"
#define EZCFG_WPA_SUPPLICANT_NETWORK_ALTSUBJECT_MATCH \
	"altsubject_match"
#define EZCFG_WPA_SUPPLICANT_NETWORK_PHASE1         "phase1"
#define EZCFG_WPA_SUPPLICANT_NETWORK_PHASE2         "phase2"
#define EZCFG_WPA_SUPPLICANT_NETWORK_CA_CERT2       "ca_cert2"
#define EZCFG_WPA_SUPPLICANT_NETWORK_CA_PATH2       "ca_path2"
#define EZCFG_WPA_SUPPLICANT_NETWORK_CLIENT_CERT2   "client_cert2"
#define EZCFG_WPA_SUPPLICANT_NETWORK_PRIVATE_KEY2   "private_key2"
#define EZCFG_WPA_SUPPLICANT_NETWORK_PRIVATE_KEY2_PASSWD \
	"private_key2_passwd"
#define EZCFG_WPA_SUPPLICANT_NETWORK_DH_FILE2       "dh_file2"
#define EZCFG_WPA_SUPPLICANT_NETWORK_SUBJECT_MATCH2 "subject_match2"
#define EZCFG_WPA_SUPPLICANT_NETWORK_ALTSUBJECT_MATCH2 \
	"altsubject_match2"
#define EZCFG_WPA_SUPPLICANT_NETWORK_FRAGMENT_SIZE  "fragment_size"
#define EZCFG_WPA_SUPPLICANT_NETWORK_PAC_FILE       "pac_file"
#define EZCFG_WPA_SUPPLICANT_NETWORK_FAST_MAX_PAC_LIST_LEN \
	"fast_max_pac_list_len"
#define EZCFG_WPA_SUPPLICANT_NETWORK_FAST_PAC_FORMAT \
	"fast_pac_format"
#define EZCFG_WPA_SUPPLICANT_NETWORK_EAP_WORKAROUND "eap_workaround"

#endif
