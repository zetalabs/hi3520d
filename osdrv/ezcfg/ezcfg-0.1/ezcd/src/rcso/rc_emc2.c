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
 * 2011-10-05   0.2       Modify it to use rcso framework
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

static int clean_emc2_nvram(void)
{
	ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(RC, EMC2_ENABLE));
	ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(EMC2, MODULES));
	ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(EMC2, MODPATH));
	ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(EMC2, RTLIB_DIR));
	ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(EMC2, RTAPI_DEBUG));
	ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(EMC2, INIFILE));
	ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(EMC2, RESTORE_DEFAULTS));
        /* EMC2 Latency test */
	ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(EMC2, LAT_TEST_ENABLE));
	ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(EMC2, LAT_BASE_PERIOD));
	ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(EMC2, LAT_SERVO_PERIOD));

	return (EXIT_SUCCESS);
}


#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_emc2(int argc, char **argv)
#endif
{
	int rc;
	char *p;
	char name[64];
	char buf[64];
	char cmd[256];
	char ini_file[64];
	char ini_dir[64];
	char old_dir[64];
	int num, i;
	int flag, ret;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "emc2")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_RELOAD :
	case RC_ACT_STOP :
		/* make sure the emc2 service has been started */
		if (utils_has_process_by_name("emcsvr") == false) {
			return (EXIT_FAILURE);
		}

		/* Stop remote access in background */
		/* emcrsh -- -ini "$INIFILE" */
		snprintf(cmd, sizeof(cmd), "start-stop-daemon -K -n emcrsh");
		utils_system(cmd);

		/* Stop display in foreground */
		/* killall keystick */

		/* Stop emctask in background */
		/* killall $EMCTASK */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_TASK_TASK), buf, sizeof(buf));
		if (rc > 0) {
			snprintf(cmd, sizeof(cmd), "start-stop-daemon -K -s %d -n %s", SIGKILL, buf);
			utils_system(cmd);
			sleep(1);
		}

		/* stop the realtime stuff ticking */
		/* $HALCMD stop */
		/* force an unlock of the HAL mutex */
#if 0
		snprintf(cmd, sizeof(cmd), "/usr/bin/halcmd -R");
		utils_system(cmd);
#endif
		/* stop realtime threads */
		snprintf(cmd, sizeof(cmd), "/usr/bin/halcmd %s", "stop");
		utils_system(cmd);
		/* unload all realtime stuff */
		snprintf(cmd, sizeof(cmd), "/usr/bin/halcmd unload all");
		utils_system(cmd);

		/* Stop halui in background, if necessary */
		/* killall $HALUI */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALUI), buf, sizeof(buf));
		if (rc > 0) {
			snprintf(cmd, sizeof(cmd), "start-stop-daemon -K -s %d -n %s", SIGKILL, buf);
			utils_system(cmd);
		}

		/* Stop emcio in background */
		/* killall $EMCIO */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMCIO_EMCIO), buf, sizeof(buf));
		if (rc > 0) {
			snprintf(cmd, sizeof(cmd), "start-stop-daemon -K -s %d -n %s", SIGKILL, buf);
			utils_system(cmd);
		}

		/* Stop REALTIME */
		rc_realtime(RC_ACT_STOP);

		/* Stop emcserver in background, always (it owns/creates the NML buffers) */
		snprintf(cmd, sizeof(cmd), "start-stop-daemon -K -s %d -n %s", SIGKILL, "emcsvr");
		utils_system(cmd);

		if (flag == RC_ACT_STOP) {
			ret = EXIT_SUCCESS;
			break;
		}
		else if (flag == RC_ACT_RELOAD) {
			/* then restore the default emc2 settings */
			clean_emc2_nvram();
			ret = EXIT_SUCCESS;
			break;
		}

		/* RC_ACT_RESTART fall through */
		sleep(1);
	case RC_ACT_START :
		rc = utils_nvram_cmp(NVRAM_SERVICE_OPTION(RC, EMC2_ENABLE), "1");
		if (rc < 0) {
			return (EXIT_FAILURE);
		}

		/* make sure the emc2 service has not been started */
		if (utils_has_process_by_name("emcsvr") == true) {
			return (EXIT_FAILURE);
		}

		mkdir("/etc/emc2", 0777);
		mkdir("/etc/emc2/configs", 0777);
		mkdir("/var/emc2", 0777);
		mkdir("/var/emc2/nc_files", 0777);

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

		/* export NML_FILE */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMC_NML_FILE), buf, sizeof(buf));
		if (rc <= 0) {
			snprintf(buf, sizeof(buf), "/usr/share/emc/emc.nml");
		}
		setenv("NMLFILE", buf, 1);

		/* start EMC2 Enhanced Machine Controller service */
		/* change dir to ini_dir */
		chdir(ini_dir);

		/* Run emcserver in background, always (it owns/creates the NML buffers) */
		setenv("INI_FILE_NAME", ini_file, 1);
		snprintf(cmd, sizeof(cmd), "start-stop-daemon -S -b -n emcsvr -a /usr/bin/emcsvr -- -ini %s", ini_file);
		utils_system(cmd);

		/* sleep a second */
		sleep(1);

		/* Start REALTIME */
		rc_realtime(RC_ACT_START);

		/* export the location of the HAL realtime modules so that
		 * "halcmd loadrt" can find them
		 */
		/* export HAL_RTMOD_DIR=$EMC2_RTLIB_DIR */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, RTLIB_DIR), buf, sizeof(buf));
		if (rc > 0) {
			setenv("HAL_RTMOD_DIR", buf, 1);
		}

		/* Run emcio in background */
		/* $HALCMD loadusr -Wn iocontrol $EMCIO -ini "$INIFILE" */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMCIO_EMCIO), buf, sizeof(buf));
		if (rc > 0) {
			snprintf(cmd, sizeof(cmd), "/usr/bin/halcmd loadusr -Wn iocontrol %s -ini %s", buf, ini_file);
			utils_system(cmd);
		}

		/* Run halui in background, if necessary */
		/* $HALCMD loadusr -Wn halui $HALUI -ini "$INIFILE" */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALUI), buf, sizeof(buf));
		if (rc > 0) {
			snprintf(cmd, sizeof(cmd), "/usr/bin/halcmd loadusr -Wn halui %s -ini %s", buf, ini_file);
			utils_system(cmd);
		}

		/* execute HALCMD config files (if any)
		 * get first config file name from ini file
		 */
		/* $HALCMD -i "$INIFILE" -f $CFGFILE */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE_NUM), buf, sizeof(buf));
		if (rc > 0) {
			num = atoi(buf);
			for (i = 0; i < num; i++) {
				snprintf(name, sizeof(name), "%s_%d", NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE), i);
				rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
				if (rc > 0) {
					snprintf(cmd, sizeof(cmd), "/usr/bin/halcmd -i %s -f %s", ini_file, buf);
					utils_system(cmd);
					sleep(1);
				}
			}
		}

		/* execute discrete HAL commands from ini file (if any)
		 * get first command from ini file
		 */
		/* $HALCMD $HALCOMMAND */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALCMD_NUM), buf, sizeof(buf));
		if (rc > 0) {
			num = atoi(buf);
			for (i = 0; i < num; i++) {
				snprintf(name, sizeof(name), "%s_%d", NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALCMD), i);
				rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
				if (rc > 0) {
					snprintf(cmd, sizeof(cmd), "/usr/bin/halcmd %s", buf);
					utils_system(cmd);
				}
			}
		}

		/* start the realtime stuff ticking */
		/* $HALCMD start */
		snprintf(cmd, sizeof(cmd), "/usr/bin/halcmd %s", "start");
		utils_system(cmd);

		/* Run emctask in background */
		/* $EMCTASK -ini "$INIFILE" & */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_TASK_TASK), buf, sizeof(buf));
		if (rc > 0) {
			snprintf(cmd, sizeof(cmd), "start-stop-daemon -S -b -n %s -a /usr/bin/%s -- -ini %s", buf, buf, ini_file);
			utils_system(cmd);
		}

		/* Run display in foreground */
		/* keystick -ini "$INIFILE" */

		/* Run remote access in background */
		/* emcrsh -- -ini "$INIFILE" */
		snprintf(cmd, sizeof(cmd), "start-stop-daemon -S -b -n emcrsh -a /usr/bin/emcrsh -- -- -ini %s", ini_file);

		i = strlen(cmd);
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, EMCRSH_PORT), buf, sizeof(buf));
		if (rc > 0) {
			snprintf(cmd+i, sizeof(cmd)-i, " --port %s", buf);
		}

		i = strlen(cmd);
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, EMCRSH_NAME), buf, sizeof(buf));
		if (rc > 0) {
			snprintf(cmd+i, sizeof(cmd)-i, " --name %s", buf);
		}

		i = strlen(cmd);
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, EMCRSH_CONNECTPW), buf, sizeof(buf));
		if (rc > 0) {
			snprintf(cmd+i, sizeof(cmd)-i, " --connectpw %s", buf);
		}

		i = strlen(cmd);
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, EMCRSH_ENABLEPW), buf, sizeof(buf));
		if (rc > 0) {
			snprintf(cmd+i, sizeof(cmd)-i, " --enablepw %s", buf);
		}

		i = strlen(cmd);
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, EMCRSH_SESSIONS), buf, sizeof(buf));
		if (rc > 0) {
			snprintf(cmd+i, sizeof(cmd)-i, " --sessions %s", buf);
		}

		i = strlen(cmd);
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, EMCRSH_PATH), buf, sizeof(buf));
		if (rc > 0) {
			snprintf(cmd+i, sizeof(cmd)-i, " --path %s", buf);
		}

		utils_system(cmd);

		/* restore to original dir */
		chdir(old_dir);

		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}
	return (ret);
}
