/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_kdrive.c
 *
 * Description  : ezbox run TinyX/Kdrive service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-06-26   0.1       Write it from scratch
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

int rc_kdrive(int flag)
{
	int rc;
	char buf[256];
	char mouse_driver[16];
	char mouse_device[64];
	char mouse_protocol[16];
	char keybd_driver[16];
	char keybd_device[64];
	char kdrive_args[256];

	switch (flag) {
	case RC_START :
		rc = nvram_match(NVRAM_SERVICE_OPTION(RC, KDRIVE_ENABLE), "1");
		if (rc < 0) {
			return (EXIT_FAILURE);
		}

		/* TinyX/kdrive MOUSE_DRIVER */
		mouse_driver[0]= '\0';
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(KDRIVE, MOUSE_DRIVER), mouse_driver, sizeof(mouse_driver));

		/* TinyX/kdrive MOUSE_DEVICE */
		mouse_device[0]= '\0';
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(KDRIVE, MOUSE_DEVICE), mouse_device, sizeof(mouse_device));

		/* TinyX/kdrive MOUSE_PROTOCOL */
		mouse_protocol[0]= '\0';
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(KDRIVE, MOUSE_PROTOCOL), mouse_protocol, sizeof(mouse_protocol));

		/*  TinyX/kdrive KEYBOARD */
		keybd_driver[0] = '\0';
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(KDRIVE, KEYBD_DRIVER), keybd_driver, sizeof(keybd_driver));

		/*  TinyX/kdrive KEYBOARD */
		keybd_device[0] = '\0';
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(KDRIVE, KEYBD_DEVICE), keybd_device, sizeof(keybd_device));

		kdrive_args[0] = '\0';
		if (keybd_driver[0] != '\0') {
			snprintf(kdrive_args + strlen(kdrive_args), sizeof(kdrive_args) - strlen(kdrive_args), "-keybd %s", keybd_driver);
			if (keybd_device[0] != '\0') {
				snprintf(kdrive_args + strlen(kdrive_args), sizeof(kdrive_args) - strlen(kdrive_args), ",device=%s", keybd_device);
			}
			snprintf(kdrive_args + strlen(kdrive_args), sizeof(kdrive_args) - strlen(kdrive_args), " ");
		}

		if (mouse_driver[0] != '\0') {
			snprintf(kdrive_args + strlen(kdrive_args), sizeof(kdrive_args) - strlen(kdrive_args), "-mouse %s", mouse_driver);
			if (mouse_device[0] != '\0') {
				snprintf(kdrive_args + strlen(kdrive_args), sizeof(kdrive_args) - strlen(kdrive_args), ",device=%s", mouse_device);
			}
			if (mouse_protocol[0] != '\0') {
				snprintf(kdrive_args + strlen(kdrive_args), sizeof(kdrive_args) - strlen(kdrive_args), ",protocol=%s", mouse_protocol);
			}
			snprintf(kdrive_args + strlen(kdrive_args), sizeof(kdrive_args) - strlen(kdrive_args), " ");
		}

		/* setup environment */
		setenv("DISPLAY", ":0", 1);

		/* start TinyX/kdrive server */
		snprintf(buf, sizeof(buf), "start-stop-daemon -S -b -n Xfbdev -a /usr/bin/Xfbdev -- %s", kdrive_args);
		system(buf);

		/* wait a second for X startup */
		sleep(1);

		/* start matchbox-window-manager */
		snprintf(buf, sizeof(buf), "start-stop-daemon -S -b -n matchbox-window-manager -a /usr/bin/matchbox-window-manager");
		system(buf);

		/* start unicode-rxvt daemon */
		snprintf(buf, sizeof(buf), "start-stop-daemon -S -b -n urxvtd -a /usr/bin/urxvtd");
		system(buf);

		/* wait a second for urxvtd startup */
		sleep(1);

		/* start unicode-rxvt client */
		snprintf(buf, sizeof(buf), "start-stop-daemon -S -b -n urxvtc -a /usr/bin/urxvtc");
		system(buf);

		break;

	case RC_STOP :
		system("start-stop-daemon -K -s KILL -n urxvtc");
		system("start-stop-daemon -K -s KILL -n urxvtd");
		system("start-stop-daemon -K -s KILL -n matchbox-window-manager");
		system("start-stop-daemon -K -s KILL -n Xfbdev");
		break;

	case RC_RESTART :
		rc = rc_kdrive(RC_STOP);
		sleep(1);
		rc = rc_kdrive(RC_START);
		break;
	}
	return (EXIT_SUCCESS);
}
