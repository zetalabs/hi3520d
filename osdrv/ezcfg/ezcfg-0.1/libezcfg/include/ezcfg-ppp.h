/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-ppp.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-07-07   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_PPP_H_
#define _EZCFG_PPP_H_

/* ezcfg nvram name prefix */
#define EZCFG_PPP_NVRAM_PREFIX             "ppp."

/* ezcfg ppp genenal options names */
#define EZCFG_PPP_OPT_KEYWORD_DEBUG         "debug"
#define EZCFG_PPP_OPT_KEYWORD_KDEBUG        "kdebug"
#define EZCFG_PPP_OPT_KEYWORD_NODETACH      "nodetach"
#define EZCFG_PPP_OPT_KEYWORD_UPDETACH      "updetach"
#define EZCFG_PPP_OPT_KEYWORD_HOLDOFF       "holdoff"
#define EZCFG_PPP_OPT_KEYWORD_IDLE          "idle"
#define EZCFG_PPP_OPT_KEYWORD_MAXCONNECT    "maxconnect"
#define EZCFG_PPP_OPT_KEYWORD_DOMAIN        "domain"
#define EZCFG_PPP_OPT_KEYWORD_PERSIST       "persist"
#define EZCFG_PPP_OPT_KEYWORD_NOPERSIST     "nopersist"
#define EZCFG_PPP_OPT_KEYWORD_DEMAND        "demand"
#define EZCFG_PPP_OPT_KEYWORD_LOGFILE       "logfile"
#define EZCFG_PPP_OPT_KEYWORD_LOGFD         "logfd"
#define EZCFG_PPP_OPT_KEYWORD_NOLOG         "nolog"
#define EZCFG_PPP_OPT_KEYWORD_NOLOGFD       "nologfd"
#define EZCFG_PPP_OPT_KEYWORD_LINKNAME      "linkname"
#define EZCFG_PPP_OPT_KEYWORD_IFNAME        "ifname"
#define EZCFG_PPP_OPT_KEYWORD_MAXFAIL       "maxfail"
#define EZCFG_PPP_OPT_KEYWORD_KTUNE         "ktune"
#define EZCFG_PPP_OPT_KEYWORD_NOKTUNE       "noktune"
#define EZCFG_PPP_OPT_KEYWORD_CONNECT_DELAY \
	"connect-delay"
#define EZCFG_PPP_OPT_KEYWORD_UNIT          "unit"
#define EZCFG_PPP_OPT_KEYWORD_DUMP          "dump"
#define EZCFG_PPP_OPT_KEYWORD_DRYRUN        "dryrun"
#define EZCFG_PPP_OPT_KEYWORD_CHILD_TIMEOUT "child-timeout"
#define EZCFG_PPP_OPT_KEYWORD_IP_UP_SCRIPT  "ip-up-script"
#define EZCFG_PPP_OPT_KEYWORD_IP_DOWN_SCRIPT \
	"ip-down-script"
#define EZCFG_PPP_OPT_KEYWORD_IPV6_UP_SCRIPT \
	"ipv6-up-script"
#define EZCFG_PPP_OPT_KEYWORD_IPV6_DOWN_SCRIPT \
	"ipv6-down-script"
#define EZCFG_PPP_OPT_KEYWORD_MULTILINK     "multilink"
#define EZCFG_PPP_OPT_KEYWORD_NOMULTILINK   "nomultilink"
#define EZCFG_PPP_OPT_KEYWORD_BUNDLE        "bundle"
#define EZCFG_PPP_OPT_KEYWORD_MP            "mp"
#define EZCFG_PPP_OPT_KEYWORD_NOMP          "nomp"
#define EZCFG_PPP_OPT_KEYWORD_PLUGIN        "plugin"
#define EZCFG_PPP_OPT_KEYWORD_PASS_FILTER   "pass-filter"
#define EZCFG_PPP_OPT_KEYWORD_ACTIVE_FILTER "active-filter"
#define EZCFG_PPP_OPT_KEYWORD_PRECOMPILED_PASS_FILTER \
	"precompiled-pass-filter"
#define EZCFG_PPP_OPT_KEYWORD_PRECOMPILED_ACTIVE_FILTER \
	"precompiled-active-filter"
#define EZCFG_PPP_OPT_KEYWORD_MAXOCTETS     "maxoctets"
#define EZCFG_PPP_OPT_KEYWORD_MO            "mo"
#define EZCFG_PPP_OPT_KEYWORD_MO_DIRECTION  "mo-direction"
#define EZCFG_PPP_OPT_KEYWORD_MO_TIMEOUT    "mo-timeout"

/* ezcfg ppp auth options names */
#define EZCFG_PPP_OPT_KEYWORD_AUTH          "auth"
#define EZCFG_PPP_OPT_KEYWORD_NOAUTH        "noauth"
#define EZCFG_PPP_OPT_KEYWORD_REQUIRE_PAP   "require-pap"
#define EZCFG_PPP_OPT_KEYWORD_PLUS_PAP      "+pap"
#define EZCFG_PPP_OPT_KEYWORD_REQUIRE_CHAP  "require-chap"
#define EZCFG_PPP_OPT_KEYWORD_PLUS_CHAP     "+chap"
#define EZCFG_PPP_OPT_KEYWORD_REQUIRE_MSCHAP "require-mschap"
#define EZCFG_PPP_OPT_KEYWORD_PLUS_MSCHAP   "+mschap"
#define EZCFG_PPP_OPT_KEYWORD_REQUIRE_MSCHAP_V2 \
	"require-mschap-v2"
#define EZCFG_PPP_OPT_KEYWORD_PLUS_MSCHAP_V2 "+mschap-v2"
#define EZCFG_PPP_OPT_KEYWORD_REFUSE_PAP    "refuse-pap"
#define EZCFG_PPP_OPT_KEYWORD_MINUS_PAP     "-pap"
#define EZCFG_PPP_OPT_KEYWORD_REFUSE_CHAP   "refuse-chap"
#define EZCFG_PPP_OPT_KEYWORD_MINUS_CHAP    "-chap"
#define EZCFG_PPP_OPT_KEYWORD_REFUSE_MSCHAP "refuse-mschap"
#define EZCFG_PPP_OPT_KEYWORD_MINUS_MSCHAP   "-mschap"
#define EZCFG_PPP_OPT_KEYWORD_REFUSE_MSCHAP_V2 \
	"refuse-mschap-v2"
#define EZCFG_PPP_OPT_KEYWORD_MINUS_MSCHAP_V2 \
	"-mschap-v2"
#define EZCFG_PPP_OPT_KEYWORD_REQUIRE_EAP   "require-eap"
#define EZCFG_PPP_OPT_KEYWORD_REFUSE_EAP    "refuse-eap"
#define EZCFG_PPP_OPT_KEYWORD_NAME          "name"
#define EZCFG_PPP_OPT_KEYWORD_PLUS_UA       "+ua"
#define EZCFG_PPP_OPT_KEYWORD_USER          "user"
#define EZCFG_PPP_OPT_KEYWORD_PASSWORD      "password"
#define EZCFG_PPP_OPT_KEYWORD_USEHOSTNAME   "usehostname"
#define EZCFG_PPP_OPT_KEYWORD_REMOTENAME    "remotename"
#define EZCFG_PPP_OPT_KEYWORD_LOGIN         "login"
#define EZCFG_PPP_OPT_KEYWORD_ENABLE_SESSION "enable-session"
#define EZCFG_PPP_OPT_KEYWORD_PAPCRYPT      "papcrypt"
#define EZCFG_PPP_OPT_KEYWORD_PRIVGROUP     "privgroup"
#define EZCFG_PPP_OPT_KEYWORD_ALLOW_IP      "allow-ip"
#define EZCFG_PPP_OPT_KEYWORD_REMOTENUMBER  "remotenumber"
#define EZCFG_PPP_OPT_KEYWORD_ALLOW_NUMBER  "allow-number"

/* ezcfg ppp lcp options names */
#define EZCFG_PPP_OPT_KEYWORD_MINUS_ALL     "-all"
#define EZCFG_PPP_OPT_KEYWORD_NOACCOMP      "noaccomp"
#define EZCFG_PPP_OPT_KEYWORD_MINUS_AC      "-ac"
#define EZCFG_PPP_OPT_KEYWORD_ASYNCMAP      "asyncmap"
#define EZCFG_PPP_OPT_KEYWORD_MINUS_AS      "-as"
#define EZCFG_PPP_OPT_KEYWORD_DEFAULT_ASYNCMAP \
	"default-asyncmap"
#define EZCFG_PPP_OPT_KEYWORD_MINUS_AM      "-am"
#define EZCFG_PPP_OPT_KEYWORD_NOMAGIC       "nomagic"
#define EZCFG_PPP_OPT_KEYWORD_MINUS_MN      "-mn"
#define EZCFG_PPP_OPT_KEYWORD_MRU           "mru"
#define EZCFG_PPP_OPT_KEYWORD_DEFAULT_MRU   "default-mru"
#define EZCFG_PPP_OPT_KEYWORD_MINUS_MRU     "-mru"
#define EZCFG_PPP_OPT_KEYWORD_MTU           "mtu"
#define EZCFG_PPP_OPT_KEYWORD_NOPCOMP       "nopcomp"
#define EZCFG_PPP_OPT_KEYWORD_MINUS_PC      "-pc"
#define EZCFG_PPP_OPT_KEYWORD_PASSIVE       "passive"
#define EZCFG_PPP_OPT_KEYWORD_MINUS_P       "-p"
#define EZCFG_PPP_OPT_KEYWORD_SILENT        "silent"
#define EZCFG_PPP_OPT_KEYWORD_LCP_ECHO_FAILURE \
	"lcp-echo-failure"
#define EZCFG_PPP_OPT_KEYWORD_LCP_ECHO_INTERVAL \
	"lcp-echo-interval"
#define EZCFG_PPP_OPT_KEYWORD_LCP_RESTART   "lcp-restart"
#define EZCFG_PPP_OPT_KEYWORD_LCP_MAX_TERMINATE \
	"lcp-max-terminate"
#define EZCFG_PPP_OPT_KEYWORD_LCP_MAX_CONFIGURE \
	"lcp-max-configure"
#define EZCFG_PPP_OPT_KEYWORD_LCP_MAX_FAILURE \
	"lcp-max-failure"
#define EZCFG_PPP_OPT_KEYWORD_RECEIVE_ALL   "receive-all"
#define EZCFG_PPP_OPT_KEYWORD_MRRU          "mrru"
#define EZCFG_PPP_OPT_KEYWORD_MPSHORTSEQ    "mpshortseq"
#define EZCFG_PPP_OPT_KEYWORD_NOMPSHORTSEQ  "nompshortseq"
#define EZCFG_PPP_OPT_KEYWORD_ENDPOINT      "endpoint"
#define EZCFG_PPP_OPT_KEYWORD_NOENDPOINT    "noendpoint"

/* ezcfg ppp pap options names */
#define EZCFG_PPP_OPT_KEYWORD_HIDE_PASSWORD "hide-password"
#define EZCFG_PPP_OPT_KEYWORD_SHOW_PASSWORD "show-password"
#define EZCFG_PPP_OPT_KEYWORD_PAP_RESTART   "pap-restart"
#define EZCFG_PPP_OPT_KEYWORD_PAP_MAX_AUTHREQ \
	"pap-max-authreq"
#define EZCFG_PPP_OPT_KEYWORD_PAP_TIMEOUT   "pap-timeout"

/* ezcfg ppp chap options names */
#define EZCFG_PPP_OPT_KEYWORD_CHAP_RESTART  "chap-restart"
#define EZCFG_PPP_OPT_KEYWORD_CHAP_MAX_CHALLENGE \
	"chap-max-challenge"
#define EZCFG_PPP_OPT_KEYWORD_CHAP_INTERVAL "chap-interval"
#define EZCFG_PPP_OPT_KEYWORD_CHAPMS_STRIP_DOMAIN \
	"chapms-strip-domain"

/* ezcfg ppp mschap options names */
#define EZCFG_PPP_OPT_KEYWORD_MS_LANMAN     "ms-lanman"
#define EZCFG_PPP_OPT_KEYWORD_MSCHAP_CHALLENGE \
	"mschap-challenge"
#define EZCFG_PPP_OPT_KEYWORD_MSCHAP2_PEER_CHALLENGE \
	"mschap2-peer-challenge"

/* ezcfg ppp eap options names */
#define EZCFG_PPP_OPT_KEYWORD_EAP_RESTART   "eap-restart"
#define EZCFG_PPP_OPT_KEYWORD_EAP_MAX_SREQ  "eap-max-sreq"
#define EZCFG_PPP_OPT_KEYWORD_EAP_TIMEOUT   "eap-timeout"
#define EZCFG_PPP_OPT_KEYWORD_EAP_MAX_RREQ  "eap-max-rreq"
#define EZCFG_PPP_OPT_KEYWORD_EAP_INTERVAL  "eap-interval"
#define EZCFG_PPP_OPT_KEYWORD_SRP_INTERVAL  "srp-interval"
#define EZCFG_PPP_OPT_KEYWORD_SRP_PN_SECRET "srp-pn-secret"
#define EZCFG_PPP_OPT_KEYWORD_SRP_USE_PSEUDONYM \
	"srp-use-pseudonym"

/* ezcfg ppp ipcp options names */
#define EZCFG_PPP_OPT_KEYWORD_NOIP          "noip"
#define EZCFG_PPP_OPT_KEYWORD_MINUS_IP      "-ip"
#define EZCFG_PPP_OPT_KEYWORD_NOVJ          "novj"
#define EZCFG_PPP_OPT_KEYWORD_MINUS_VJ      "-vj"
#define EZCFG_PPP_OPT_KEYWORD_NOVJCCOMP     "novjccomp"
#define EZCFG_PPP_OPT_KEYWORD_NINUS_VJCCOMP "-vjccomp"
#define EZCFG_PPP_OPT_KEYWORD_VJ_MAX_SLOTS  "vj-max-slots"
#define EZCFG_PPP_OPT_KEYWORD_IPCP_ACCEPT_LOCAL \
	"ipcp-accept-local"
#define EZCFG_PPP_OPT_KEYWORD_IPCP_ACCEPT_REMOTE \
	"ipcp-accept-remote"
#define EZCFG_PPP_OPT_KEYWORD_IPPARAM       "ipparam"
#define EZCFG_PPP_OPT_KEYWORD_NOIPDEFAULT   "noipdefault"
#define EZCFG_PPP_OPT_KEYWORD_MS_DNS        "ms-dns"
#define EZCFG_PPP_OPT_KEYWORD_MS_WINS       "ms-wins"
#define EZCFG_PPP_OPT_KEYWORD_IPCP_RESTART  "ipcp-restart"
#define EZCFG_PPP_OPT_KEYWORD_IPCP_MAX_TERMINATE \
	"ipcp-max-terminate"
#define EZCFG_PPP_OPT_KEYWORD_IPCP_MAX_CONFIGURE \
	"ipcp-max-configure"
#define EZCFG_PPP_OPT_KEYWORD_IPCP_MAX_FAILURE \
	"ipcp-max-failure"
#define EZCFG_PPP_OPT_KEYWORD_DEFAULTROUTE  "defaultroute"
#define EZCFG_PPP_OPT_KEYWORD_NODEFAULTROUTE "nodefaultroute"
#define EZCFG_PPP_OPT_KEYWORD_MINUS_DEFAULTROUTE \
	"-defaultroute"
#define EZCFG_PPP_OPT_KEYWORD_REPLACEDEFAULTROUTE \
	"replacedefaultroute"
#define EZCFG_PPP_OPT_KEYWORD_NOREPLACEDEFAULTROUTE \
	"noreplacedefaultroute"
#define EZCFG_PPP_OPT_KEYWORD_PROXYARP      "proxyarp"
#define EZCFG_PPP_OPT_KEYWORD_NOPROXYARP    "noproxyarp"
#define EZCFG_PPP_OPT_KEYWORD_MINUS_PROXYARP "-proxyarp"
#define EZCFG_PPP_OPT_KEYWORD_USEPEERDNS    "usepeerdns"
#define EZCFG_PPP_OPT_KEYWORD_NETMASK       "netmask"
#define EZCFG_PPP_OPT_KEYWORD_IPCP_NO_ADDRESSES \
	"ipcp-no-addresses"
#define EZCFG_PPP_OPT_KEYWORD_IPCP_NO_ADDRESS \
	"ipcp-no-address"
#define EZCFG_PPP_OPT_KEYWORD_NOREMOTEIP    "noremoteip"
#define EZCFG_PPP_OPT_KEYWORD_NOSENDIP      "nosendip"
#define EZCFG_PPP_OPT_KEYWORD_IP_ADDRESSES  "IP addresses"

/* ezcfg ppp ipv6cp options names */
#define EZCFG_PPP_OPT_KEYWORD_IPV6          "ipv6"
#define EZCFG_PPP_OPT_KEYWORD_PLUS_IPV6     "+ipv6"
#define EZCFG_PPP_OPT_KEYWORD_NOIPV6        "noipv6"
#define EZCFG_PPP_OPT_KEYWORD_MINUS_IPV6    "-ipv6"
#define EZCFG_PPP_OPT_KEYWORD_IPV6CP_ACCEPT_LOCAL \
	"ipv6cp-accept-local"
#define EZCFG_PPP_OPT_KEYWORD_IPV6CP_USE_IPADDR \
	"ipv6cp-use-ipaddr"
#define EZCFG_PPP_OPT_KEYWORD_IPV6CP_USE_PERSISTENT \
	"ipv6cp-use-persistent"
#define EZCFG_PPP_OPT_KEYWORD_IPV6CP_RESTART \
	"ipv6cp-restart"
#define EZCFG_PPP_OPT_KEYWORD_IPV6CP_MAX_TERMINATE \
	"ipv6cp-max-terminate"
#define EZCFG_PPP_OPT_KEYWORD_IPV6CP_MAX_CONFIGURE \
	"ipv6cp-max-configure"
#define EZCFG_PPP_OPT_KEYWORD_IPV6CP_MAX_FAILURE \
	"ipv6cp-max-failure"

/* ezcfg ppp tty options names */
#define EZCFG_PPP_OPT_KEYWORD_LOCK          "lock"
#define EZCFG_PPP_OPT_KEYWORD_NOLOCK        "nolock"
#define EZCFG_PPP_OPT_KEYWORD_INIT          "init"
#define EZCFG_PPP_OPT_KEYWORD_CONNECT       "connect"
#define EZCFG_PPP_OPT_KEYWORD_DISCONNECT    "disconnect"
#define EZCFG_PPP_OPT_KEYWORD_WELCOME       "welcome"
#define EZCFG_PPP_OPT_KEYWORD_PTY           "pty"
#define EZCFG_PPP_OPT_KEYWORD_NOTTY         "notty"
#define EZCFG_PPP_OPT_KEYWORD_SOCKET        "socket"
#define EZCFG_PPP_OPT_KEYWORD_CRTSCTS       "crtscts"
#define EZCFG_PPP_OPT_KEYWORD_CDTRCTS       "cdtrcts"
#define EZCFG_PPP_OPT_KEYWORD_NOCRTSCTS     "nocrtscts"
#define EZCFG_PPP_OPT_KEYWORD_MINUS_CCRTSCTS \
#define EZCFG_PPP_OPT_KEYWORD_NOCDTRCTS     "nocdtrcts"
#define EZCFG_PPP_OPT_KEYWORD_XONXOFF       "xonxoff"
#define EZCFG_PPP_OPT_KEYWORD_MODEM         "modem"
#define EZCFG_PPP_OPT_KEYWORD_LOCAL         "local"
#define EZCFG_PPP_OPT_KEYWORD_SYNC          "sync"
#define EZCFG_PPP_OPT_KEYWORD_DATARATE      "datarate"
#define EZCFG_PPP_OPT_KEYWORD_ESCAPE        "escape"

/* ezcfg ppp plugin rp-pppoe options names */
#define EZCFG_PPP_OPT_KEYWORD_DEVICE_NAME   "device name"
#define EZCFG_PPP_OPT_KEYWORD_RP_PPPOE_SERVICE \
	"rp_pppoe_service"
#define EZCFG_PPP_OPT_KEYWORD_RP_PPPOE_AC   "rp_pppoe_ac"
#define EZCFG_PPP_OPT_KEYWORD_RP_PPPOE_SESS "rp_pppoe_sess"
#define EZCFG_PPP_OPT_KEYWORD_RP_PPPOE_VERBOSE \
	"rp_pppoe_verbose"
#define EZCFG_PPP_OPT_KEYWORD_PPPOE_MAC     "pppoe-mac"

/* pap secrets */
#define EZCFG_PPP_PAP_SECRET_ENTRY          "pap_secret_entry"
#define EZCFG_PPP_PAP_SECRET_ENTRY_NUM      "pap_secret_entry_num"
/* chap secrets */
#define EZCFG_PPP_CHAP_SECRET_ENTRY         "chap_secret_entry"
#define EZCFG_PPP_CHAP_SECRET_ENTRY_NUM     "chap_secret_entry_num"

#endif
