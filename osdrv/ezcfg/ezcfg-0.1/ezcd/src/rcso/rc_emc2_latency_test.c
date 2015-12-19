/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_emc2.c
 *
 * Description  : ezbox run EMC2 Enhanced Machine Controller service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-07-05   0.1       Write it from scratch
 * 2011-10-28   0.2       Modify it to use rcso framework
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

#include "ezcd.h"
#include "pop_func.h"
#include "rc_func.h"

#if 0
#define DBG(format, args...) do {\
	FILE *fp = fopen("/tmp/emc2.debug", "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

#define LATEXIT_SH_FLAG_FILE	"/tmp/.latexit.sh"

static int generate_latexit_sh(const char *path)
{
	FILE * file;
	file = fopen(path, "w");
	if (file == NULL) {
		return EXIT_FAILURE;
	}

	fprintf(file, "echo 1 > %s\n", LATEXIT_SH_FLAG_FILE);
	fprintf(file, "while [ -f %s ]\n", LATEXIT_SH_FLAG_FILE);
	fprintf(file, "do\n");
	fprintf(file, "  sl=`halcmd gets sl`\n");
	fprintf(file, "  nvram set %s $sl 2>&1 |>/dev/null\n", NVRAM_SERVICE_OPTION(EMC2, LAT_SERVO_MAX_INTERVAL));
	fprintf(file, "  sj=`halcmd gets sj`\n");
	fprintf(file, "  nvram set %s $sj 2>&1 |>/dev/null\n", NVRAM_SERVICE_OPTION(EMC2, LAT_SERVO_MAX_JITTER));
	fprintf(file, "  st=`halcmd gets st`\n");
	fprintf(file, "  nvram set %s $st 2>&1 |>/dev/null\n", NVRAM_SERVICE_OPTION(EMC2, LAT_SERVO_LAST_INTERVAL));
	fprintf(file, "  bl=`halcmd gets bl`\n");
	fprintf(file, "  nvram set %s $bl 2>&1 |>/dev/null\n", NVRAM_SERVICE_OPTION(EMC2, LAT_BASE_MAX_INTERVAL));
	fprintf(file, "  bj=`halcmd gets bj`\n");
	fprintf(file, "  nvram set %s $bj 2>&1 |>/dev/null\n", NVRAM_SERVICE_OPTION(EMC2, LAT_BASE_MAX_JITTER));
	fprintf(file, "  bt=`halcmd gets bt`\n");
	fprintf(file, "  nvram set %s $bt 2>&1 |>/dev/null\n", NVRAM_SERVICE_OPTION(EMC2, LAT_BASE_LAST_INTERVAL));
	fprintf(file, "done\n");

	fclose(file);
	return EXIT_SUCCESS;

}

static int generate_test_hal_file(const char *path, long base_period, long servo_period)
{
	FILE * file;
	file = fopen(path, "w");
	if (file == NULL) {
		return EXIT_FAILURE;
	}

	fprintf(file, "loadrt threads name1=fast period1=%ld name2=slow period2=%ld\n", base_period, servo_period);
	fprintf(file, "loadrt timedelta count=2\n");
	fprintf(file, "addf timedelta.0 fast\n");
	fprintf(file, "addf timedelta.1 slow\n");
	fprintf(file, "start\n");
	fprintf(file, "net sl timedelta.1.max\n");
	fprintf(file, "net sj timedelta.1.jitter\n");
	fprintf(file, "net st timedelta.1.out\n");
	fprintf(file, "net bl timedelta.0.max\n");
	fprintf(file, "net bj timedelta.0.jitter\n");
	fprintf(file, "net bt timedelta.0.out\n");
	fprintf(file, "net reset timedelta.0.reset timedelta.1.reset\n");
	fprintf(file, "loadusr -w sh latexit.sh\n");

	fclose(file);
	return EXIT_SUCCESS;
}

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_emc2_latency_test(int argc, char **argv)
#endif
{
	int rc;
	char *p;
	char buf[64];
	char cmd[128];
	char ini_file[64];
	char ini_dir[64];
	char old_dir[64];
	char test_file[64];
	long base_period, servo_period;
	int flag, ret;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "emc2_latency_test")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	rc = utils_nvram_cmp(NVRAM_SERVICE_OPTION(EMC2, LAT_TEST_ENABLE), "1");
	if (rc < 0) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_STOP :
		/* Stop latency test in background */
		snprintf(cmd, sizeof(cmd), "%s -rf %s", CMD_RM, LATEXIT_SH_FLAG_FILE);
		utils_system(cmd);

		/* $HALCMD stop */
		snprintf(cmd, sizeof(cmd), "/usr/bin/halcmd stop");
		utils_system(cmd);

		/* $HALCMD unload all */
		snprintf(cmd, sizeof(cmd), "/usr/bin/halcmd unload all");
		utils_system(cmd);

		/* Stop REALTIME */
		rc_realtime(RC_ACT_STOP);
		if (flag == RC_ACT_STOP) {
			ret = EXIT_SUCCESS;
			break;
		}

		/* RC_ACT_RESTART fall through */
		sleep(1);
	case RC_ACT_START :
		/* create configs dir */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, INIFILE), ini_file, sizeof(ini_file));
		if (rc <= 0) {
			return (EXIT_FAILURE);
		}

		if (getcwd(old_dir, sizeof(old_dir)) == NULL) {
			snprintf(old_dir, sizeof(old_dir), "%s", ROOT_HOME_PATH);
		}
		strcpy(ini_dir, ini_file);
		p = strrchr(ini_dir, '/');
		if (p != NULL) {
			*p = '\0';
		}
		else {
			strcpy(ini_dir, old_dir);
		}

		snprintf(cmd, sizeof(cmd), "%s -p %s", CMD_MKDIR, ini_dir);
		utils_system(cmd);

		pop_etc_emc2_rtapi_conf(RC_ACT_START);

		pop_etc_emc2_configs(RC_ACT_START);

		/* generate latexit.sh */
		snprintf(buf, sizeof(buf), "%s/latexit.sh", ini_dir);
		generate_latexit_sh(buf);

		/* get base period */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, LAT_BASE_PERIOD), buf, sizeof(buf));
		if (rc <= 0) {
			return (EXIT_FAILURE);
		}
		base_period = atol(buf);

		/* get servo period */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, LAT_SERVO_PERIOD), buf, sizeof(buf));
		if (rc <= 0) {
			return (EXIT_FAILURE);
		}
		servo_period = atol(buf);

		/* generate lat.hal */
		snprintf(test_file, sizeof(test_file), "%s/lat.hal", ini_dir);
		generate_test_hal_file(test_file, base_period, servo_period);

		/* export NML_FILE */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMC_NML_FILE), buf, sizeof(buf));
		if (rc <= 0) {
			snprintf(buf, sizeof(buf), "/usr/share/emc/emc.nml");
		}
		setenv("NMLFILE", buf, 1);

		/* start EMC2 Enhanced Machine Controller latency test */
		/* change dir to ini_dir */
		chdir(ini_dir);

		/* Start REALTIME */
		rc_realtime(RC_ACT_START);

		/* sleep a second */
		sleep(1);

		/* Run latency test in background */
		/* $HALCMD -i "$INIFILE" -f $CFGFILE */
		snprintf(cmd, sizeof(cmd), "/usr/bin/halcmd -i %s -f %s &", ini_file, test_file);
		utils_system(cmd);

		/* restore to original dir */
		chdir(old_dir);

		ret = EXIT_SUCCESS;
		break;

	case RC_ACT_RELOAD :
		/* $HALCMD sets reset 1 */
		snprintf(cmd, sizeof(cmd), "/usr/bin/halcmd sets %s %d", "reset", 1);
		utils_system(cmd);

		/* $HALCMD sets reset 0 */
		snprintf(cmd, sizeof(cmd), "/usr/bin/halcmd sets %s %d", "reset", 0);
		utils_system(cmd);
		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}
	return (EXIT_SUCCESS);
}
