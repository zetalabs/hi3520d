/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-http.h
 *
 * Description  : implement HTTP/1.1 protocol (RFC 2616)
 *                and it's extension (RFC 2774)
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-30   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_HTTP_H_
#define _EZCFG_HTTP_H_

#include "ezcfg.h"

typedef enum {
	HTTP_REQUEST = 0,
	HTTP_RESPONSE
} http_state_t;

/* ezcfg http define strings */
#define EZCFG_HTTP_SP_STRING           " "
#define EZCFG_HTTP_CRLF_STRING         "\r\n"

/* HTTP/1.1 (RFC2616) defined methods */
#define EZCFG_HTTP_METHOD_OPTIONS  "OPTIONS"
#define EZCFG_HTTP_METHOD_GET      "GET"
#define EZCFG_HTTP_METHOD_HEAD     "HEAD"
#define EZCFG_HTTP_METHOD_POST     "POST"
#define EZCFG_HTTP_METHOD_PUT      "PUT"
#define EZCFG_HTTP_METHOD_DELETE   "DELETE"
#define EZCFG_HTTP_METHOD_TRACE    "TRACE"
#define EZCFG_HTTP_METHOD_CONNECT  "CONNECT"
/* HTTP/1.1 extension (RFC2774) defined methods */
#define EZCFG_HTTP_METHOD_OPTIONS_EXT  "M-OPTIONS"
#define EZCFG_HTTP_METHOD_GET_EXT      "M-GET"
#define EZCFG_HTTP_METHOD_HEAD_EXT     "M-HEAD"
#define EZCFG_HTTP_METHOD_POST_EXT     "M-POST"
#define EZCFG_HTTP_METHOD_PUT_EXT      "M-PUT"
#define EZCFG_HTTP_METHOD_DELETE_EXT   "M-DELETE"
#define EZCFG_HTTP_METHOD_TRACE_EXT    "M-TRACE"
#define EZCFG_HTTP_METHOD_CONNECT_EXT  "M-CONNECT"

/* HTTP/1.1 (RFC2616) General headers */
#define EZCFG_HTTP_HEADER_CACHE_CONTROL       "Cache-Control"
#define EZCFG_HTTP_HEADER_CONNECTION          "Connection"
#define EZCFG_HTTP_HEADER_DATE                "Date"
#define EZCFG_HTTP_HEADER_PRAGMA              "Pragma"
#define EZCFG_HTTP_HEADER_TRAILER             "Trailer"
#define EZCFG_HTTP_HEADER_TRANSFER_ENCODING   "Transfer-Encoding"
#define EZCFG_HTTP_HEADER_UPGRADE             "Upgrade"
#define EZCFG_HTTP_HEADER_VIA                 "Via"
#define EZCFG_HTTP_HEADER_WARNING             "Warning"
/* HTTP/1.1 (RFC2616) request headers */
#define EZCFG_HTTP_HEADER_ACCEPT              "Accept"
#define EZCFG_HTTP_HEADER_ACCEPT_CHARSET      "Accept-Charset"
#define EZCFG_HTTP_HEADER_ACCEPT_ENCODING     "Accept-Encoding"
#define EZCFG_HTTP_HEADER_ACCEPT_LANGUAGE     "Accept-Language"
#define EZCFG_HTTP_HEADER_AUTHORIZATION       "Authorization"
#define EZCFG_HTTP_HEADER_EXPECT              "Expect"
#define EZCFG_HTTP_HEADER_FROM                "From"
#define EZCFG_HTTP_HEADER_HOST                "Host"
#define EZCFG_HTTP_HEADER_IF_MATCH            "If-Match"
#define EZCFG_HTTP_HEADER_IF_MODIFIED_SINCE   "If-Modified-Since"
#define EZCFG_HTTP_HEADER_IF_NONE_MATCH       "If-None-Match"
#define EZCFG_HTTP_HEADER_IF_RANGE            "If-Range"
#define EZCFG_HTTP_HEADER_IF_UNMODIFIED_SINCE "If-Unmodified-Since"
#define EZCFG_HTTP_HEADER_MAX_FORWARDS        "Max-Forwards"
#define EZCFG_HTTP_HEADER_PROXY_AUTHORIZATION "Proxy-Authorization"
#define EZCFG_HTTP_HEADER_RANGE               "Range"
#define EZCFG_HTTP_HEADER_REFERER             "Referer"
#define EZCFG_HTTP_HEADER_TE                  "TE"
#define EZCFG_HTTP_HEADER_USER_AGENT          "User-Agent"
/* HTTP/1.1 (RFC2616) response headers */
#define EZCFG_HTTP_HEADER_ACCEPT_RANGES       "Accept-Ranges"
#define EZCFG_HTTP_HEADER_AGE                 "Age"
#define EZCFG_HTTP_HEADER_ETAG                "ETag"
#define EZCFG_HTTP_HEADER_LOCATION            "Location"
#define EZCFG_HTTP_HEADER_PROXY_AUTHENTICATE  "Proxy-Authenticate"
#define EZCFG_HTTP_HEADER_RETRY_AFTER         "Retry-After"
#define EZCFG_HTTP_HEADER_SERVER              "Server"
#define EZCFG_HTTP_HEADER_VARY                "Vary"
#define EZCFG_HTTP_HEADER_WWW_AUTHENTICATE    "WWW-Authenticate"
/* HTTP/1.1 (RFC2616) entity headers */
#define EZCFG_HTTP_HEADER_ALLOW               "Allow"
#define EZCFG_HTTP_HEADER_CONTENT_ENCODING    "Content-Encoding"
#define EZCFG_HTTP_HEADER_CONTENT_LANGUAGE    "Content-Language"
#define EZCFG_HTTP_HEADER_CONTENT_LENGTH      "Content-Length"
#define EZCFG_HTTP_HEADER_CONTENT_LOCATION    "Content-Location"
#define EZCFG_HTTP_HEADER_CONTENT_MD5         "Content-MD5"
#define EZCFG_HTTP_HEADER_CONTENT_RANGE       "Content-Range"
#define EZCFG_HTTP_HEADER_CONTENT_TYPE        "Content-Type"
#define EZCFG_HTTP_HEADER_EXPIRES             "Expires"
#define EZCFG_HTTP_HEADER_LAST_MODIFIED       "Last-Modified"
/* HTTP/1.1 extension (RFC2774) headers */
#define EZCFG_HTTP_HEADER_MAN                 "Man"
#define EZCFG_HTTP_HEADER_OPT                 "Opt"
#define EZCFG_HTTP_HEADER_C_MAN               "C-Man"
#define EZCFG_HTTP_HEADER_C_OPT               "C-Opt"
/* HTTP/1.1 extension (RFC2774) responese headers */
#define EZCFG_HTTP_HEADER_EXT                 "Ext"
#define EZCFG_HTTP_HEADER_C_EXT               "C-Ext"
/* HTTP/1.1 (RFC2616) status code reason phrase section 10 */
#define EZCFG_HTTP_REASON_PHRASE_1XX          "Informational"
#define EZCFG_HTTP_REASON_PHRASE_100          "Continue"
#define EZCFG_HTTP_REASON_PHRASE_101          "Switching Protocols"
/* HTTP Extensions for Distributed Authoring -- WEBDAV (RFC2518) status code 102 */
#define EZCFG_HTTP_REASON_PHRASE_102          "Processing"
#define EZCFG_HTTP_REASON_PHRASE_2XX          "Successful"
#define EZCFG_HTTP_REASON_PHRASE_200          "OK"
#define EZCFG_HTTP_REASON_PHRASE_201          "Created"
#define EZCFG_HTTP_REASON_PHRASE_202          "Accepted"
#define EZCFG_HTTP_REASON_PHRASE_203          "Non-Authoritative Information"
#define EZCFG_HTTP_REASON_PHRASE_204          "No Content"
#define EZCFG_HTTP_REASON_PHRASE_205          "Reset Content"
#define EZCFG_HTTP_REASON_PHRASE_206          "Partial Content"
/* HTTP Extensions for Distributed Authoring -- WEBDAV (RFC2518) status code 207 */
#define EZCFG_HTTP_REASON_PHRASE_207          "Multi-Status"
#define EZCFG_HTTP_REASON_PHRASE_3XX          "Redirection"
#define EZCFG_HTTP_REASON_PHRASE_300          "Multiple Choices"
#define EZCFG_HTTP_REASON_PHRASE_301          "Moved Permanently"
#define EZCFG_HTTP_REASON_PHRASE_302          "Found"
#define EZCFG_HTTP_REASON_PHRASE_303          "See Other"
#define EZCFG_HTTP_REASON_PHRASE_304          "Not Modified"
#define EZCFG_HTTP_REASON_PHRASE_305          "Use Proxy"
#define EZCFG_HTTP_REASON_PHRASE_306          "(Unused)"
#define EZCFG_HTTP_REASON_PHRASE_307          "Temporary Redirect"
#define EZCFG_HTTP_REASON_PHRASE_4XX          "Client Error"
#define EZCFG_HTTP_REASON_PHRASE_400          "Bad Request"
#define EZCFG_HTTP_REASON_PHRASE_401          "Unauthorized"
#define EZCFG_HTTP_REASON_PHRASE_402          "Payment Required"
#define EZCFG_HTTP_REASON_PHRASE_403          "Forbidden"
#define EZCFG_HTTP_REASON_PHRASE_404          "Not Found"
#define EZCFG_HTTP_REASON_PHRASE_405          "Method Not Allowed"
#define EZCFG_HTTP_REASON_PHRASE_406          "Not Acceptable"
#define EZCFG_HTTP_REASON_PHRASE_407          "Proxy Authentication Required"
#define EZCFG_HTTP_REASON_PHRASE_408          "Request Timeout"
#define EZCFG_HTTP_REASON_PHRASE_409          "Conflict"
#define EZCFG_HTTP_REASON_PHRASE_410          "Gone"
#define EZCFG_HTTP_REASON_PHRASE_411          "Length Required"
#define EZCFG_HTTP_REASON_PHRASE_412          "Precondition Failed"
#define EZCFG_HTTP_REASON_PHRASE_413          "Request Entity Too Large"
#define EZCFG_HTTP_REASON_PHRASE_414          "Request-URI Too Long"
#define EZCFG_HTTP_REASON_PHRASE_415          "Unsupported Media Type"
#define EZCFG_HTTP_REASON_PHRASE_416          "Requested Range Not Satisfiable"
#define EZCFG_HTTP_REASON_PHRASE_417          "Expectation Failed"
/* HTTP Extensions for Distributed Authoring -- WEBDAV (RFC2518) for status code 422-424 */
#define EZCFG_HTTP_REASON_PHRASE_422          "Unprocessable Entity"
#define EZCFG_HTTP_REASON_PHRASE_423          "Locked"
#define EZCFG_HTTP_REASON_PHRASE_424          "Failed Dependency"
/* Upgrading to TLS Within HTTP/1.1 (RFC2817) for status code 426 */
#define EZCFG_HTTP_REASON_PHRASE_426          "Upgrade Required"
#define EZCFG_HTTP_REASON_PHRASE_5XX          "Server Error"
#define EZCFG_HTTP_REASON_PHRASE_500          "Internal Server Error"
#define EZCFG_HTTP_REASON_PHRASE_501          "Not Implemented"
#define EZCFG_HTTP_REASON_PHRASE_502          "Bad Gateway"
#define EZCFG_HTTP_REASON_PHRASE_503          "Service Unavailable"
#define EZCFG_HTTP_REASON_PHRASE_504          "Gateway Timeout"
#define EZCFG_HTTP_REASON_PHRASE_505          "HTTP Version Not Supported"
/* HTTP Extensions for Distributed Authoring -- WEBDAV (RFC2518) status code 507 */
#define EZCFG_HTTP_REASON_PHRASE_507          "Insufficient Storage"

/* ezcfg HTTP charset strings */
#define EZCFG_HTTP_CHARSET_NAME               "charset"
#define EZCFG_HTTP_CHARSET_UTF8               "UTF-8"

/* ezcfg HTTP content-coding strings */
#define EZCFG_HTTP_CONTENT_CODING_NAME        "content-coding"
#define EZCFG_HTTP_CONTENT_CODING_GZIP        "gzip"
#define EZCFG_HTTP_CONTENT_CODING_COMPRESS    "compress"
#define EZCFG_HTTP_CONTENT_CODING_DEFLATE     "deflate"
#define EZCFG_HTTP_CONTENT_CODING_IDENTITY    "identity"

/* ezcfg HTTP transfer-coding strings */
#define EZCFG_HTTP_TRANSFER_CODING_NAME       "transfer-coding"
#define EZCFG_HTTP_TRANSFER_CODING_CHUNKED    "chunked"

/* ezcfg HTTP MIME type strings */
#define EZCFG_HTTP_MIME_APPLICATION_JSON      "application/json"
#define EZCFG_HTTP_MIME_APPLICATION_SOAP_XML  "application/soap+xml"
#define EZCFG_HTTP_MIME_APPLICATION_X_JAVASCRIPT        "application/x-javascript"
#define EZCFG_HTTP_MIME_TEXT_HTML             "text/html"
#define EZCFG_HTTP_MIME_TEXT_XML              "text/xml"
#define EZCFG_HTTP_MIME_TEXT_CSS              "text/css"

/* ezcfg HTTP Cache directives strings */
#define EZCFG_HTTP_CACHE_REQUEST_NO_CACHE           "no-cache"
#define EZCFG_HTTP_CACHE_REQUEST_NO_STORE           "no-store"
#define EZCFG_HTTP_CACHE_REQUEST_MAX_AGE            "max-age"
#define EZCFG_HTTP_CACHE_REQUEST_MAX_STALE          "max-stale"
#define EZCFG_HTTP_CACHE_REQUEST_MIN_FRESH          "min-fresh"
#define EZCFG_HTTP_CACHE_REQUEST_NO_TRANSFORM       "no-transform"
#define EZCFG_HTTP_CACHE_REQUEST_ONLY_IF_CACHED     "only-if-cached"
#define EZCFG_HTTP_CACHE_RESPONSE_PUBLIC            "public"
#define EZCFG_HTTP_CACHE_RESPONSE_PRIVATE           "private"
#define EZCFG_HTTP_CACHE_RESPONSE_NO_CACHE          "no-cache"
#define EZCFG_HTTP_CACHE_RESPONSE_NO_STORE          "no-store"
#define EZCFG_HTTP_CACHE_RESPONSE_NO_TRANSFORM      "no-transform"
#define EZCFG_HTTP_CACHE_RESPONSE_MUST_REVALIDATE   "must-revalidate"
#define EZCFG_HTTP_CACHE_RESPONSE_PROXY_REVALIDATE  "proxy-revalidate"
#define EZCFG_HTTP_CACHE_RESPONSE_MAX_AGE           "max-age"
#define EZCFG_HTTP_CACHE_RESPONSE_S_MAXAGE          "s-maxage"

/* ezcfg HTTP Pragma directives strings */
#define EZCFG_HTTP_PRAGMA_NO_CACHE                  "no-cache"

#endif
