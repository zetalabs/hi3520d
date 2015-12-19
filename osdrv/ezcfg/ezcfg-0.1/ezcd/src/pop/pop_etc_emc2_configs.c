/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_emc2_configs.c
 *
 * Description  : ezbox /root/emc2/ config files generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-07-06   0.1       Write it from scratch
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

static int gen_etc_emc2_configs_ezcnc_ini(int flag)
{
        FILE *file = NULL;
	char name[64];
	char buf[64];
	int rc;
	int i, axes=0, num;

	switch (flag) {
	case RC_ACT_START :
		/* generate /root/emc2/ezcnc.ini */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, INIFILE), buf, sizeof(buf));
		if (rc <= 0) {
			return (EXIT_FAILURE);
		}

		//file = fopen("/etc/emc2/configs/ezcnc.ini", "w");
		file = fopen(buf, "w");
		if (file == NULL)
			return (EXIT_FAILURE);

		/* General section */
		fprintf(file, "[%s]\n", "EMC");
		/* Version of this INI file */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMC_VERSION), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "VERSION = %s\n", buf);
		}
		/* Name of machine, for use with display, etc. */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMC_MACHINE), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "MACHINE = %s\n", buf);
		}

		/* Debug level, 0 means no messages. */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMC_DEBUG), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "DEBUG = %s\n", buf);
		}

		/* Sections for display options */
		fprintf(file, "[%s]\n", "DISPLAY");
		/* Name of display program, e.g., xemc */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_DISPLAY_DISPLAY), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "DISPLAY = %s\n", buf);
		}
		/* Initial display setting for position, RELATIVE or MACHINE */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_DISPLAY_POSITION_OFFSET), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "POSITION_OFFSET = %s\n", buf);
		}
		/* Initial display setting for position, COMMANDED or ACTUAL */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_DISPLAY_POSITION_FEEDBACK), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "POSITION_FEEDBACK = %s\n", buf);
		}
		/* Highest value that will be allowed for feed override, 1.0 = 100% */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_DISPLAY_MAX_FEED_OVERRIDE), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "MAX_FEED_OVERRIDE = %s\n", buf);
		}
		/* The minimum spindle override the user may select 0.5 = 50% */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_DISPLAY_MIN_SPINDLE_OVERRIDE), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "MIN_SPINDLE_OVERRIDE = %s\n", buf);
		}
		/* The maximum spindle override the user may select 0.5 = 50% */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_DISPLAY_MAX_SPINDLE_OVERRIDE), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "MAX_SPINDLE_OVERRIDE = %s\n", buf);
		}
		/* Prefix to be used */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_DISPLAY_PROGRAM_PREFIX), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "PROGRAM_PREFIX = %s\n", buf);
		}
		/* Introductory graphic */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_DISPLAY_INTRO_GRAPHIC), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "INTRO_GRAPHIC = %s\n", buf);
		}
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_DISPLAY_INTRO_TIME), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "INTRO_TIME = %s\n", buf);
		}
		/* Cycle time, in seconds, that display will sleep between polls */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_DISPLAY_CYCLE_TIME), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "CYCLE_TIME = %s\n", buf);
		}
		/* Path to help file, not used in AXIS */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_DISPLAY_HELP_FILE), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "HELP_FILE = %s\n", buf);
		}

		/* Task controller section */
		fprintf(file, "[%s]\n", "TASK");
		/* Name of task controller program, e.g., milltask */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_TASK_TASK), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "TASK = %s\n", buf);
		}
		/* Cycle time, in seconds, that task controller will sleep between polls */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_TASK_CYCLE_TIME), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "CYCLE_TIME = %s\n", buf);
		}

		/* Part program interpreter section */
		fprintf(file, "[%s]\n", "RS274NGC");
		/* File containing interpreter variables */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_RS274NGC_PARAMETER_FILE), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "PARAMETER_FILE = %s\n", buf);
		}
		/* File containing debug log */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_RS274NGC_LOG_FILE), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "LOG_FILE = %s\n", buf);
		}


		/* Motion control section */
		fprintf(file, "[%s]\n", "EMCMOT");
		/* Name of the motion controller to use (only one exists for nontrivkins) */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMCMOT_EMCMOT), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "EMCMOT = %s\n", buf);
		}
		/* Timeout for comm to emcmot, in seconds */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMCMOT_COMM_TIMEOUT), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "COMM_TIMEOUT = %s\n", buf);
		}
		/* Interval between tries to emcmot, in seconds */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMCMOT_COMM_WAIT), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "COMM_WAIT = %s\n", buf);
		}
		/* Base task period, in nanosecs - this is the fastest thread in the machine */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMCMOT_BASE_PERIOD), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "BASE_PERIOD = %s\n", buf);
		}
		/* Servo task period, in nanosecs - will be rounded to an int multiple of BASE_PERIOD */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMCMOT_SERVO_PERIOD), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "SERVO_PERIOD = %s\n", buf);
		}

		/* Hardware Abstraction Layer section */
		fprintf(file, "[%s]\n", "HAL");
		/* Use two pass processing for loading HAL comps */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_TWOPASS), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "TWOPASS = %s\n", buf);
		}
		/* list of hal config files to run through halcmd
		 * files are executed in the order in which they appear
		 */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE_NUM), buf, sizeof(buf));
		if (rc > 0) {
			num = atoi(buf);
			for (i = 0; i < num; i++) {
				snprintf(name, sizeof(name), "%s_%d", NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE), i);
				rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
				if (rc > 0) {
					fprintf(file, "HALFILE = %s\n", buf);
				}
			}
		}

		/* list of halcmd commands to execute */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALCMD_NUM), buf, sizeof(buf));
		if (rc > 0) {
			num = atoi(buf);
			for (i = 0; i < num; i++) {
				snprintf(name, sizeof(name), "%s_%d", NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALCMD), i);
				rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
				if (rc > 0) {
					fprintf(file, "HALCMD = %s\n", buf);
				}
			}
		}
		/* Execute the file shutdown.hal when EMC is exiting */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_SHUTDOWN), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "SHUTDOWN = %s\n", buf);
		}

		/* Hardware Abstraction Layer UI section */
		fprintf(file, "[%s]\n", "HALUI");
		/* Use two pass processing for loading HAL comps */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_HALUI_MDI_COMMAND), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "MDI_COMMAND = %s\n", buf);
		}

		/* Trajectory planner section */
		fprintf(file, "[%s]\n", "TRAJ");
		/* machine specific settings */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_AXES), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "AXES = %s\n", buf);
			axes = atoi(buf);
		}
		/* COORDINATES = X Y Z A B C */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_COORDINATES), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "COORDINATES = %s\n", buf);
		}
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_HOME), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "HOME = %s\n", buf);
		}
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_LINEAR_UNITS), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "LINEAR_UNITS = %s\n", buf);
		}
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_ANGULAR_UNITS), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "ANGULAR_UNITS = %s\n", buf);
		}
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_CYCLE_TIME), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "CYCLE_TIME = %s\n", buf);
		}
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_DEFAULT_VELOCITY), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "DEFAULT_VELOCITY = %s\n", buf);
		}
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_MAX_VELOCITY), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "MAX_VELOCITY = %s\n", buf);
		}
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_DEFAULT_ACCELERATION), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "DEFAULT_ACCELERATION = %s\n", buf);
		}
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_MAX_ACCELERATION), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "MAX_ACCELERATION = %s\n", buf);
		}
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_NO_FORCE_HOMING), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "NO_FORCE_HOMING = %s\n", buf);
		}

		/* Axes sections */
		for (i = 0; i < axes; i++) {
			fprintf(file, "[%s_%d]\n", "AXIS", i);
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_TYPE);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "TYPE = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_WRAPPED_ROTARY);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "WRAPPED_ROTARY = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_UNITS);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "UNITS = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_MAX_VELOCITY);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "MAX_VELOCITY = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_MAX_ACCELERATION);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "MAX_ACCELERATION = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_BACKLASH);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "BACKLASH = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_COMP_FILE);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "COMP_FILE = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_COMP_FILE_TYPE);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "COMP_FILE_TYPE = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_MIN_LIMIT);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "MIN_LIMIT = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_MAX_LIMIT);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "MAX_LIMIT = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_MIN_FERROR);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "MIN_FERROR = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_FERROR);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "FERROR = %s\n", buf);
			}
			/* homing */
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_HOME);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "HOME = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_HOME_OFFSET);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "HOME_OFFSET = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_HOME_SEARCH_VEL);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "HOME_SEARCH_VEL = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_HOME_LATCH_VEL);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "HOME_LATCH_VEL = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_HOME_FINAL_VEL);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "HOME_FINAL_VEL = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_HOME_USE_INDEX);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "HOME_USE_INDEX = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_HOME_IGNORE_LIMITS);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "HOME_IGNORE_LIMITS = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_HOME_IS_SHARED);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "HOME_IS_SHARED = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_HOME_SEQUENCE);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "HOME_SEQUENCE = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_VOLATILE_HOME);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "VOLATILE_HOME = %s\n", buf);
			}
			/* servo */
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_DEADBAND);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "DEADBAND = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_BIAS);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "BIAS = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_P);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "P = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_I);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "I = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_D);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "D = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_FF0);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "FF0 = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_FF1);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "FF1 = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_FF2);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "FF2 = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_OUTPUT_SCALE);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "OUTPUT_SCALE = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_OUTPUT_OFFSET);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "OUTPUT_OFFSET = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_MAX_OUTPUT);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "MAX_OUTPUT = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_INPUT_SCALE);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "INPUT_SCALE = %s\n", buf);
			}
			/* stepper */
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_SCALE);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "SCALE = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_STEPGEN_MAXACCEL);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "STEPGEN_MAXACCEL = %s\n", buf);
			}
			snprintf(name, sizeof(name), "%s_%d_%s",
				NVRAM_SERVICE_OPTION(EMC2, CONF_SECTION_AXIS),
				i, EZCFG_EMC2_CONF_KEYWORD_STEPGEN_MAXVEL);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "STEPGEN_MAXVEL = %s\n", buf);
			}
		}

		/* section for main IO controller parameters */
		fprintf(file, "[%s]\n", "EMCIO");
		/* Name of IO controller program, e.g., io */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMCIO_EMCIO), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "EMCIO = %s\n", buf);
		}
		/* cycle time, in seconds */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMCIO_CYCLE_TIME), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "CYCLE_TIME = %s\n", buf);
		}
		/* tool table file */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMCIO_TOOL_TABLE), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "TOOL_TABLE = %s\n", buf);
		}
		/* Specifies the location to move to when performing a tool change if three digits are used */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMCIO_TOOL_CHANGE_POSITION), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "TOOL_CHANGE_POSITION = %s\n", buf);
		}
		/* The spindle will be left on during the tool change when the value is 1 */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMCIO_TOOL_CHANGE_WITH_SPINDLE_ON), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "TOOL_CHANGE_WITH_SPINDLE_ON = %s\n", buf);
		}
		/* The Z axis will be moved to machine zero prior to the tool change when the value is 1 */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMCIO_TOOL_CHANGE_QUILL_UP), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "TOOL_CHANGE_QUILL_UP = %s\n", buf);
		}
		/* The machine is moved to reference point defined by parameters 5181-5186 for G30 if the value is 1 */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMCIO_TOOL_CHANGE_AT_G30), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "TOOL_CHANGE_AT_G30 = %s\n", buf);
		}
		/* This is for machines that cannot place the tool back into the pocket it came from */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMCIO_RANDOM_TOOLCHANGER), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "RANDOM_TOOLCHANGER = %s\n", buf);
		}

		fclose(file);

		break;

	case RC_ACT_STOP :

		break;
	}

	return (EXIT_SUCCESS);
}

static int gen_etc_emc2_configs_ezcnc_hal(int flag)
{
        FILE *file = NULL;
	char *p;
	char buf[128];
	char name[64];
	char ini_dir[64];
	int rc, i, j, num;

	switch (flag) {
	case RC_ACT_START :
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, INIFILE), ini_dir, sizeof(ini_dir));
		if (rc <= 0) {
			return (EXIT_FAILURE);
		}

		p = strrchr(ini_dir, '/');
		if (p == NULL) {
			return (EXIT_FAILURE);
		}
		*p = '\0';

		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE_NUM), buf, sizeof(buf));
		if (rc <= 0) {
			return (EXIT_FAILURE);
		}
		num = atoi(buf);

		for (i = 0; i < num; i++) {
			snprintf(name, sizeof(name), "%s_%d", NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE), i);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			snprintf(name, sizeof(name), "%s/%s", ini_dir, buf);
			/* generate HALFILE */
			file = fopen(name, "w");
			if (file == NULL)
				return (EXIT_FAILURE);

			j = 1;
			snprintf(name, sizeof(name), "%s_%d.%d",
				NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE), i, j);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			while(rc > 0) {
				fprintf(file, "%s\n", buf);
				j++;
				snprintf(name, sizeof(name), "%s_%d.%d",
					NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE), i, j);
				rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			}
			fclose(file);
		}

#if 0
		/* generate HALFILE */
		snprintf(buf, sizeof(buf), "%s/ezcnc.hal", ini_dir);
		//file = fopen("/etc/emc2/configs/ezcnc.hal", "w");
		file = fopen(buf, "w");
		if (file == NULL)
			return (EXIT_FAILURE);

		/* core HAL config file for steppers */
		/* first load the core RT modules that will be needed
		 * kinematics
		 */
		fprintf(file, "%s\n", "loadrt trivkins");

		/* motion controller, get name and thread periods from ini file */
		fprintf(file, "%s\n", "loadrt [EMCMOT]EMCMOT base_period_nsec=[EMCMOT]BASE_PERIOD servo_period_nsec=[EMCMOT]SERVO_PERIOD num_joints=[TRAJ]AXES");

		/* stepper module, three step generators, all three using step/dir */
		fprintf(file, "%s\n", "loadrt stepgen step_type=0,0,0");

		/* hook functions to base thread (high speed thread for step generation) */
		fprintf(file, "%s\n", "addf stepgen.make-pulses base-thread");

		/* hook functions to servo thread */
		fprintf(file, "%s\n", "addf stepgen.capture-position servo-thread");
		fprintf(file, "%s\n", "addf motion-command-handler servo-thread");
		fprintf(file, "%s\n", "addf motion-controller servo-thread");
		fprintf(file, "%s\n", "addf stepgen.update-freq servo-thread");

		/* connect position commands from motion module to step generator */
		fprintf(file, "%s\n", "net Xpos-cmd axis.0.motor-pos-cmd => stepgen.0.position-cmd");
		fprintf(file, "%s\n", "net Ypos-cmd axis.1.motor-pos-cmd => stepgen.1.position-cmd");
		fprintf(file, "%s\n", "net Zpos-cmd axis.2.motor-pos-cmd => stepgen.2.position-cmd");

		/* connect position feedback from step generators
		 * to motion module
		 */
		fprintf(file, "%s\n", "net Xpos-fb stepgen.0.position-fb => axis.0.motor-pos-fb");
		fprintf(file, "%s\n", "net Ypos-fb stepgen.1.position-fb => axis.1.motor-pos-fb");
		fprintf(file, "%s\n", "net Zpos-fb stepgen.2.position-fb => axis.2.motor-pos-fb");

		/* connect enable signals for step generators */
		fprintf(file, "%s\n", "net Xen axis.0.amp-enable-out => stepgen.0.enable");
		fprintf(file, "%s\n", "net Yen axis.1.amp-enable-out => stepgen.1.enable");
		fprintf(file, "%s\n", "net Zen axis.2.amp-enable-out => stepgen.2.enable");

		/* connect signals to step pulse generator outputs */
		fprintf(file, "%s\n", "net Xstep <= stepgen.0.step");
		fprintf(file, "%s\n", "net Xdir  <= stepgen.0.dir");
		fprintf(file, "%s\n", "net Ystep <= stepgen.1.step");
		fprintf(file, "%s\n", "net Ydir  <= stepgen.1.dir");
		fprintf(file, "%s\n", "net Zstep <= stepgen.2.step");
		fprintf(file, "%s\n", "net Zdir  <= stepgen.2.dir");

		/* set stepgen module scaling - get values from ini file */
		fprintf(file, "%s\n", "setp stepgen.0.position-scale [AXIS_0]SCALE");
		fprintf(file, "%s\n", "setp stepgen.1.position-scale [AXIS_1]SCALE");
		fprintf(file, "%s\n", "setp stepgen.2.position-scale [AXIS_2]SCALE");

		/* set stepgen module accel limits - get values from ini file */
		fprintf(file, "%s\n", "setp stepgen.0.maxaccel [AXIS_0]STEPGEN_MAXACCEL");
		fprintf(file, "%s\n", "setp stepgen.1.maxaccel [AXIS_1]STEPGEN_MAXACCEL");
		fprintf(file, "%s\n", "setp stepgen.2.maxaccel [AXIS_2]STEPGEN_MAXACCEL");

		/* standard pinout config file for 3-axis steppers
		 * using a parport for I/O
		 */
		/* first load the parport driver */
		fprintf(file, "%s\n", "loadrt hal_parport cfg=\"0x0378\"");

		/* next connect the parport functions to threads
		 * read inputs first
		 */
		fprintf(file, "%s\n", "addf parport.0.read base-thread 1");

		/* write outputs last */
		fprintf(file, "%s\n", "addf parport.0.write base-thread -1");

		/* finally connect physical pins to the signals */
		fprintf(file, "%s\n", "net Xstep => parport.0.pin-03-out");
		fprintf(file, "%s\n", "net Xdir  => parport.0.pin-02-out");
		fprintf(file, "%s\n", "net Ystep => parport.0.pin-05-out");
		fprintf(file, "%s\n", "net Ydir  => parport.0.pin-04-out");
		fprintf(file, "%s\n", "net Zstep => parport.0.pin-07-out");
		fprintf(file, "%s\n", "net Zdir  => parport.0.pin-06-out");

		/* create a signal for the estop loopback */
		fprintf(file, "%s\n", "net estop-loop iocontrol.0.user-enable-out iocontrol.0.emc-enable-in");

		/* create signals for tool loading loopback */
		fprintf(file, "%s\n", "net tool-prep-loop iocontrol.0.tool-prepare iocontrol.0.tool-prepared");
		fprintf(file, "%s\n", "net tool-change-loop iocontrol.0.tool-change iocontrol.0.tool-changed");

		/* connect "spindle on" motion controller pin to a physical pin */
		fprintf(file, "%s\n", "net spindle-on motion.spindle-on => parport.0.pin-09-out");

		fclose(file);
#endif
		break;

	case RC_ACT_STOP :
		break;
	}

	return (EXIT_SUCCESS);
}

static int gen_etc_emc2_configs_ezcnc_tbl(int flag)
{
        FILE *file = NULL;
	char name[64];
	char buf[64];
	char ini_dir[64];
	char *p;
	int rc;

	switch (flag) {
	case RC_ACT_START :
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, INIFILE), ini_dir, sizeof(ini_dir));
		if (rc <= 0) {
			return (EXIT_FAILURE);
		}

		p = strrchr(ini_dir, '/');
		if (p == NULL) {
			return (EXIT_FAILURE);
		}
		*p = '\0';

		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, CONF_EMCIO_TOOL_TABLE), name, sizeof(name));
		if (rc <= 0) {
			return (EXIT_FAILURE);
		}

		/* generate /etc/emc2/configs/ezcnc.tbl */
		snprintf(buf, sizeof(buf), "%s/%s", ini_dir, name);
		file = fopen(buf, "w");
		if (file == NULL)
			return (EXIT_FAILURE);

		fprintf(file, "%s\n", "T1 P1 D0.125000 Z+0.511000 ;1/8 end mill");
		fprintf(file, "%s\n", "T2 P2 D0.062500 Z+0.100000 ;1/16 end mill");
		fprintf(file, "%s\n", "T3 P3 D0.201000 Z+1.273000 ;#7 tap drill");
		fprintf(file, "%s\n", "T99999 P99999 Z+0.100000 ;big tool number");

		fclose(file);
		break;

	case RC_ACT_STOP :
		break;
	}

	return (EXIT_SUCCESS);
}

int pop_etc_emc2_configs(int flag)
{
	int rc = EXIT_SUCCESS;

	switch (flag) {
	case RC_ACT_START :
		rc = gen_etc_emc2_configs_ezcnc_ini(RC_ACT_START);
		if (rc == EXIT_FAILURE)
			return (rc);

		rc = gen_etc_emc2_configs_ezcnc_hal(RC_ACT_START);
		if (rc == EXIT_FAILURE)
			return (rc);

		rc = gen_etc_emc2_configs_ezcnc_tbl(RC_ACT_START);
		if (rc == EXIT_FAILURE)
			return (rc);

		break;

	case RC_ACT_STOP :
		gen_etc_emc2_configs_ezcnc_tbl(RC_ACT_STOP);
		gen_etc_emc2_configs_ezcnc_hal(RC_ACT_STOP);
		gen_etc_emc2_configs_ezcnc_ini(RC_ACT_STOP);

		break;

	default :
		rc = EXIT_FAILURE;
		break;
	}

	return (rc);
}
