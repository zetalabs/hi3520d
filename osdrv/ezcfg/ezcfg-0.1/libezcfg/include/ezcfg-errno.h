#ifndef _EZCFG_ERRNO_H_
#define _EZCFG_ERRNO_H_

enum {
	EZCFG_E_ARGUMENT = 100, /* function arguments error */
	EZCFG_E_RESOURCE,       /* resource request error */
	EZCFG_E_CONNECTION,     /* connection error */
	EZCFG_E_READ,           /* read error */
	EZCFG_E_WRITE,          /* write error */
	EZCFG_E_PARSE,          /* parse error */
	EZCFG_E_SPACE,          /* space not enough error */
	EZCFG_E_RESULT,         /* result is error */
	EZCFG_E_CRC,            /* CRC check error */
};

#endif
