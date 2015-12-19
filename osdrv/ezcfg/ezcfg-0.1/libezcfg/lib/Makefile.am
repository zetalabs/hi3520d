# ------------------------------------------------------------------------------
# Copyright (C) 2008-2014 ezbox project
# ------------------------------------------------------------------------------

LIBEZCFG_CURRENT = 1
LIBEZCFG_REVISION = 0
LIBEZCFG_AGE = 0

if CONFIG_EZBOX_EZCFG_TEST
EXPORTED_SYMBOLS = exported_symbols_test
else
EXPORTED_SYMBOLS = exported_symbols
endif

SUBDIRS = .

AM_CPPFLAGS = \
	-include ../config.h \
	-I../include \
	-I../api/include \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-DDATADIR=\""$(datadir)"\" \
	-DEZCFG_DEBUG \
	-Wall -Werror

LDADD = libezcfg.la

libezcfgincludedir = $(includedir)/libezcfg

libezcfginclude_HEADERS = \
	../api/include/ezcfg-api.h

lib_LTLIBRARIES = libezcfg.la

libezcfg_la_CPPFLAGS = $(AM_CPPFLAGS) $(EZBOX_DISTRO_DEFINE) -I../api/include

libezcfg_la_LDFLAGS = \
	-version-info $(LIBEZCFG_CURRENT):$(LIBEZCFG_REVISION):$(LIBEZCFG_AGE) \
	-export-symbols ../$(EXPORTED_SYMBOLS)

###################
# common part
###################
# ezcfg common
libezcfg_la_SOURCES = ../src/common/ezcfg.c
libezcfg_la_SOURCES += ../src/common/nv_pair.c
libezcfg_la_SOURCES += ../src/common/linked_list.c
libezcfg_la_SOURCES += ../src/common/stack_list.c
libezcfg_la_SOURCES += ../src/common/binary_tree.c
libezcfg_la_SOURCES += ../src/common/json.c
libezcfg_la_SOURCES += ../src/common/meta_nvram.c
libezcfg_la_SOURCES += ../src/common/nvram.c


###################
# basic object
###################
# ezcfg name-value pair
libezcfg_la_SOURCES += ../src/basic/nv_pair/nv_pair.c

# ezcfg linked list
libezcfg_la_SOURCES += ../src/basic/linked_list/linked_list.c

# ezcfg stack list
libezcfg_la_SOURCES += ../src/basic/stack_list/stack_list.c

# ezcfg binary tree
libezcfg_la_SOURCES += ../src/basic/binary_tree/binary_tree.c

# ezcfg nv linked list
libezcfg_la_SOURCES += ../src/basic/nv_linked_list/nv_linked_list.c

# ezcfg socket
libezcfg_la_SOURCES += ../src/basic/socket/socket.c
libezcfg_la_SOURCES += ../src/basic/socket/socket_http.c

# ezcfg authentication
libezcfg_la_SOURCES += ../src/basic/auth/auth.c

# ezcfg HTTP protocol
libezcfg_la_SOURCES += ../src/basic/http/http.c

# ezcfg JSON parser
libezcfg_la_SOURCES += ../src/basic/json/json.c
libezcfg_la_SOURCES += ../src/basic/json/json_nvram.c

# ezcfg thread
libezcfg_la_SOURCES += ../src/basic/thread/thread.c

# ezcfg process
libezcfg_la_SOURCES += ../src/basic/process/process.c


###################
# composite object
###################
# ezcfg NVRAM on JSON over HTTP representation through socket connection
libezcfg_la_SOURCES += ../src/composite/json_http/json_http.c

# ezcfg NVRAM on JSON over HTTP representation through socket connection
#libezcfg_la_SOURCES += ../src/composite/nv_json_http_socket/nv_json_http_socket.c

# ezcfg Multi-Agent System (MAS) model

# ezcfg socket-based MAS
#libezcfg_la_SOURCES += ../src/composite/socket_agent/socket_agent.c
#libezcfg_la_SOURCES += ../src/composite/socket_agent/socket_agent_core.c
#libezcfg_la_SOURCES += ../src/composite/socket_agent/socket_agent_master.c
#libezcfg_la_SOURCES += ../src/composite/socket_agent/socket_agent_worker.c
#libezcfg_la_SOURCES += ../src/composite/socket_agent/socket_agent_worker_nv_json_http_socket.c

###################
# aux utilities 
###################





###################
# FIXME: need to rearrange!!!
###################
# ezcfg data structures
#libezcfg_la_SOURCES += ../src/tree/avl_tree.c


# ezcfg Master/Worker thread model
#libezcfg_la_SOURCES += ../src/thread/thread.c
# libezcfg_la_SOURCES += ../src/thread/master.c
#libezcfg_la_SOURCES += ../src/thread/master_uevent.c
# libezcfg_la_SOURCES += ../src/thread/master_load_common_conf.c
#libezcfg_la_SOURCES += ../src/thread/master_load_socket_conf.c
#libezcfg_la_SOURCES += ../src/thread/master_load_auth_conf.c
if CONFIG_EZBOX_SERVICE_OPENSSL
#libezcfg_la_SOURCES += ../src/thread/master_load_ssl_conf.c
endif
if CONFIG_EZBOX_SERVICE_EZCFG_IGRSD
#libezcfg_la_SOURCES += ../src/thread/master_load_igrs_conf.c
endif
if CONFIG_EZBOX_SERVICE_EZCFG_UPNPD
#libezcfg_la_SOURCES += ../src/thread/master_upnp_ssdp.c
#libezcfg_la_SOURCES += ../src/thread/master_load_upnp_conf.c
endif
# libezcfg_la_SOURCES += ../src/thread/worker.c
# libezcfg_la_SOURCES += ../src/thread/worker_ctrl.c
if CONFIG_EZBOX_SERVICE_EZCFG_HTTPD
# libezcfg_la_SOURCES += ../src/thread/worker_http.c
endif
#libezcfg_la_SOURCES += ../src/thread/worker_soap_http.c
if CONFIG_EZBOX_SERVICE_EZCFG_IGRSD
#libezcfg_la_SOURCES += ../src/thread/worker_igrs.c
#libezcfg_la_SOURCES += ../src/thread/worker_igrs_isdp.c
endif
#libezcfg_la_SOURCES += ../src/thread/worker_uevent.c
if CONFIG_EZBOX_SERVICE_EZCFG_UPNPD
#libezcfg_la_SOURCES += ../src/thread/worker_upnp_ssdp.c
#libezcfg_la_SOURCES += ../src/thread/worker_upnp_http.c
#libezcfg_la_SOURCES += ../src/thread/worker_upnp_gena.c
endif

# ezcfg args
#libezcfg_la_SOURCES += ../src/args/arg_nvram_socket.c
if CONFIG_EZBOX_SERVICE_OPENSSL
#libezcfg_la_SOURCES += ../src/args/arg_nvram_ssl.c
endif

# ezcfg utils
libezcfg_la_SOURCES += ../src/util/util.c
libezcfg_la_SOURCES += ../src/util/util_adler32.c
libezcfg_la_SOURCES += ../src/util/util_base64.c
libezcfg_la_SOURCES += ../src/util/util_sha1.c
libezcfg_la_SOURCES += ../src/util/util_crc32.c
libezcfg_la_SOURCES += ../src/util/util_conf.c
libezcfg_la_SOURCES += ../src/util/util_file_get_line.c
libezcfg_la_SOURCES += ../src/util/util_file_extension.c
libezcfg_la_SOURCES += ../src/util/util_javascript_var_escaped.c
libezcfg_la_SOURCES += ../src/util/util_parse_args.c
libezcfg_la_SOURCES += ../src/util/util_socket_protocol.c
libezcfg_la_SOURCES += ../src/util/util_socket_domain.c
libezcfg_la_SOURCES += ../src/util/util_socket_type.c
libezcfg_la_SOURCES += ../src/util/util_socket_mcast.c
libezcfg_la_SOURCES += ../src/util/util_if_ipaddr.c
libezcfg_la_SOURCES += ../src/util/util_language.c
libezcfg_la_SOURCES += ../src/util/util_execute.c
#libezcfg_la_SOURCES += ../src/util/util_rc.c
libezcfg_la_SOURCES += ../src/util/util_service_binding.c
libezcfg_la_SOURCES += ../src/util/util_text.c
libezcfg_la_SOURCES += ../src/util/util_tzdata.c
libezcfg_la_SOURCES += ../src/util/util_url.c
libezcfg_la_SOURCES += ../src/util/util_wan.c
libezcfg_la_SOURCES += ../src/util/util_mkdir.c
libezcfg_la_SOURCES += ../src/util/util_snprintf_ns_name.c
if CONFIG_EZBOX_SERVICE_EZCFG_UPNPD
# libezcfg_la_SOURCES += ../src/util/util_upnp_role.c
# libezcfg_la_SOURCES += ../src/util/util_upnp_device_type.c
# libezcfg_la_SOURCES += ../src/util/util_upnp_monitor.c
endif

# ezcfg control
#libezcfg_la_SOURCES += ../src/ctrl/ctrl.c 
#libezcfg_la_SOURCES += ../src/ctrl/ctrl_ipc.c 
if CONFIG_EZBOX_SERVICE_EZCFG_UPNPD
# libezcfg_la_SOURCES += ../src/ctrl/ctrl_upnp.c
endif 
if CONFIG_EZBOX_SERVICE_EZCTP
# libezcfg_la_SOURCES += ../src/ctrl/ctrl_ezctp.c
endif 

# ezcfg nvram
#libezcfg_la_SOURCES += ../src/nvram/nvram.c
#libezcfg_la_SOURCES += ../src/nvram/nvram_defaults.c
#libezcfg_la_SOURCES += ../src/nvram/nvram_validators.c
#libezcfg_la_SOURCES += ../src/nvram/nvram_propagators.c

# ezctp support
if CONFIG_EZBOX_SERVICE_EZCTP
#libezcfg_la_SOURCES += ../src/ezctp/ezctp.c
endif 


# ezcfg uevent
#libezcfg_la_SOURCES += ../src/uevent/uevent.c

# ezcfg locale
#libezcfg_la_SOURCES += ../src/locale/locale.c

# ezcfg UUID
#libezcfg_la_SOURCES += ../src/uuid/uuid.c

# ezcfg HTTP/socket handler
#libezcfg_la_SOURCES += ../src/socket/socket_http.c
#libezcfg_la_SOURCES += ../src/http_socket/http_socket.c

if CONFIG_EZBOX_SERVICE_EZCFG_HTTPD
if CONFIG_EZBOX_SERVICE_EZCFG_HTTPD_SSI
# ezcfg HTTP SSI handling
#libezcfg_la_SOURCES += ../src/http/http_ssi.c
endif

if CONFIG_EZBOX_SERVICE_EZCFG_HTTPD_WEBSOCKET
# ezcfg HTTP WEBSOCKET handling
#libezcfg_la_SOURCES += ../src/websocket/websocket.c
#libezcfg_la_SOURCES += ../src/websocket/http_websocket.c
endif

if CONFIG_EZBOX_SERVICE_EZCFG_HTTPD_CGI_NVRAM
# ezcfg HTTP nvram handling
#libezcfg_la_SOURCES += ../src/http/http_nvram.c
endif

if CONFIG_EZBOX_SERVICE_EZCFG_HTTPD_CGI_INDEX
# ezcfg HTML/HTTP index handling
#libezcfg_la_SOURCES += ../src/http/http_html_index.c
endif

if CONFIG_EZBOX_SERVICE_EZCFG_HTTPD_CGI_ADMIN
# ezcfg HTML/HTTP admin handling
#libezcfg_la_SOURCES += ../src/http/http_html_admin.c

# ezcfg HTML/HTTP admin common head handling
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_common_head.c

# ezcfg HTML/HTTP admin head handling
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_head.c

# ezcfg HTML/HTTP admin foot handling
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_foot.c

# ezcfg HTML/HTTP admin button handling
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_button.c


# ezcfg HTML/HTTP admin menu handling
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_menu.c

# ezcfg HTML/HTTP admin menu status handling
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_menu_status.c

# ezcfg HTML/HTTP admin menu setup handling
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_menu_setup.c

# ezcfg HTML/HTTP admin menu management handling
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_menu_management.c


# ezcfg HTML/HTTP admin layout_css handling
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_layout_css.c

# ezcfg HTML/HTTP admin status_system handling
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_status_system.c

# ezcfg HTML/HTTP admin status_lan handling
if CONFIG_EZBOX_LAN_NIC
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_status_lan.c
endif

# ezcfg HTML/HTTP admin view_dhcp_client_table handling
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_view_dhcp_client_table.c

# ezcfg HTML/HTTP admin status_wan handling
if CONFIG_EZBOX_WAN_NIC
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_status_wan.c
endif

# ezcfg HTML/HTTP admin setup_system handling
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_setup_system.c

# ezcfg HTML/HTTP admin setup_lan handling
if CONFIG_EZBOX_LAN_NIC
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_setup_lan.c
endif

# ezcfg HTML/HTTP admin setup_wan handling
if CONFIG_EZBOX_WAN_NIC
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_setup_wan.c
endif

# ezcfg HTML/HTTP admin management_authz handling
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_management_authz.c

# ezcfg HTML/HTTP admin management_default handling
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_management_default.c

# ezcfg HTML/HTTP admin management_upgrade handling
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_management_upgrade.c

if CONFIG_EZBOX_SERVICE_EMC2
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_menu_cnc.c
# ezcfg HTML/HTTP admin cnc_setup handling
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_cnc_setup.c
# ezcfg HTML/HTTP admin cnc_default handling
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_cnc_default.c
# ezcfg HTML/HTTP admin cnc_latency handling
#libezcfg_la_SOURCES += ../src/http/admin/http_html_admin_cnc_latency.c
endif # end of CONFIG_EZBOX_SERVICE_EMC2
endif # end of CONFIG_EZBOX_SERVICE_EZCFG_HTTPD_CGI_ADMIN

if CONFIG_EZBOX_SERVICE_EZCFG_HTTPD_CGI_APPLY
# ezcfg HTML/HTTP apply handling
#libezcfg_la_SOURCES += ../src/http/http_html_apply.c

# ezcfg HTML/HTTP apply common head handling
#libezcfg_la_SOURCES += ../src/http/apply/http_html_apply_common_head.c

endif # end of CONFIG_EZBOX_SERVICE_EZCFG_HTTPD_CGI_APPLY
endif # end of CONFIG_EZBOX_SERVICE_EZCFG_HTTPD

# ezcfg XML doc
#libezcfg_la_SOURCES += ../src/xml/xml.c

# ezcfg HTML handling
#libezcfg_la_SOURCES += ../src/html/html.c

# ezcfg CSS handling
#libezcfg_la_SOURCES += ../src/css/css.c

# ezcfg SSI handling
#libezcfg_la_SOURCES += ../src/ssi/ssi.c

# ezcfg JSON over HTTP
#libezcfg_la_SOURCES += ../src/json_http/json_http.c

# ezcfg NVRAM on JSON over HTTP representation
#libezcfg_la_SOURCES += ../src/nv_json_http/nv_json_http.c

if CONFIG_EZBOX_SERVICE_OPENSSL
# ezcfg SSL handling
#libezcfg_la_SOURCES += ../src/ssl/ssl.c
endif

# ezcfg SOAP protocol
#libezcfg_la_SOURCES += ../src/soap/soap.c

# ezcfg SOAP-HTTP binding
#libezcfg_la_SOURCES += ../src/soap/soap_http.c

# ezcfg NVRAM/SOAP-HTTP binding
#libezcfg_la_SOURCES += ../src/soap/soap_http_nvram.c

# ezcfg IGRS protocol
#libezcfg_la_SOURCES += ../src/igrs/igrs.c
#libezcfg_la_SOURCES += ../src/igrs/igrs_create_session.c
#libezcfg_la_SOURCES += ../src/igrs/igrs_invoke_service.c

if CONFIG_EZBOX_SERVICE_EZCFG_IGRSD
# ezcfg ISDP protocol
#libezcfg_la_SOURCES += ../src/igrs/igrs_isdp.c
endif

if CONFIG_EZBOX_SERVICE_EZCFG_UPNPD
# ezcfg UPNP protocol
#libezcfg_la_SOURCES += ../src/upnp/upnp.c
# ezcfg SSDP protocol
#libezcfg_la_SOURCES += ../src/upnp/upnp_ssdp.c
#libezcfg_la_SOURCES += ../src/upnp/upnp_gena.c
endif


###################
# ezcfg API wrap
###################
libezcfg_la_SOURCES += ../api/src/api-common.c
#libezcfg_la_SOURCES += ../api/src/api-util.c
libezcfg_la_SOURCES += ../api/src/api-agent.c
#libezcfg_la_SOURCES += ../api/src/api-master.c
#libezcfg_la_SOURCES += ../api/src/api-ctrl.c
#libezcfg_la_SOURCES += ../api/src/api-ipc.c
#libezcfg_la_SOURCES += ../api/src/api-arg_nvram_socket.c
#if CONFIG_EZBOX_SERVICE_OPENSSL
#libezcfg_la_SOURCES += ../api/src/api-arg_nvram_ssl.c
#endif
libezcfg_la_SOURCES += ../api/src/api-nvram.c
#libezcfg_la_SOURCES += ../api/src/api-rc.c
#libezcfg_la_SOURCES += ../api/src/api-uuid.c
#if CONFIG_EZBOX_SERVICE_EZCFG_UPNPD
#libezcfg_la_SOURCES += ../api/src/api-upnp.c
#endif
#if CONFIG_EZBOX_SERVICE_EZCTP
#libezcfg_la_SOURCES += ../api/src/api-ezctp.c
#endif 
#libezcfg_la_SOURCES += ../api/src/api-ubootenv.c
#libezcfg_la_SOURCES += ../api/src/api-firmware.c
