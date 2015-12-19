/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_mdev_conf.c
 *
 * Description  : ezbox /etc/mdev.conf file generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-08-12   0.1       Write it from scratch
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

static int generate_mdev_common_rules(FILE *file)
{
#if 0
	/* null            0:0 666 */
	fprintf(file, "%s %d:%d %o\n", "null", 0, 0, 0666);
	/* zero            0:0 666 */
	fprintf(file, "%s %d:%d %o\n", "zero", 0, 0, 0666);
	/* urandom         0:0 444 */
	fprintf(file, "%s %d:%d %o\n", "urandom", 0, 0, 0444);

	/* kmem            0:9 000 */
	fprintf(file, "%s %d:%d %o\n", "kmem", 0, 9, 0000);
	/* mem             0:9 640 */
	fprintf(file, "%s %d:%d %o\n", "mem", 0, 9, 0640);
	/* port            0:9 640 */
	fprintf(file, "%s %d:%d %o\n", "port", 0, 9, 0640);

	/* console         0:5 600 */
	fprintf(file, "%s %d:%d %o\n", "port", 0, 5, 0600);
	/* ptmx            0:5 660 */
	fprintf(file, "%s %d:%d %o\n", "ptmx", 0, 5, 0660);
	/* tty[0-9]*       0:5 660 */
	fprintf(file, "%s %d:%d %o\n", "tty[0-9]*", 0, 5, 0660);

	/* ttyS[0-9]*      0:20 640 */
	fprintf(file, "%s %d:%d %o\n", "ttyS[0-9]*", 0, 20, 0640);

	/* fd[0-9]*        0:11 660 */
	fprintf(file, "%s %d:%d %o\n", "fd[0-9]*", 0, 11, 0660);

	/* sd[a-z]*        0:6 660 */
	fprintf(file, "%s %d:%d %o\n", "sd[a-z]*", 0, 6, 0660);
	/* hd[a-z]*        0:6 660 */
	fprintf(file, "%s %d:%d %o\n", "hd[a-z]*", 0, 6, 0660);
#else
	/* support module loading on hotplug */
	/* $MODALIAS=.*    root:root 660 @modprobe "$MODALIAS" */
	fprintf(file, "%s %s:%s %o %s\n",
		"$MODALIAS=.*",
		"root",
		"root",
		0660,
		"@modprobe \"$MODALIAS\"");

	/* null may already exist; therefore ownership has to be changed with command */
	/* null            root:root 666 @chmod 666 $MDEV */
	fprintf(file, "%s %s:%s %o %s\n",
		"null",
		"root",
		"root",
		0666,
		"@chmod 666 \"$MDEV\"");
	/* zero            root:root 666 */
	fprintf(file, "%s %s:%s %o\n",
		"zero",
		"root",
		"root",
		0666);
	/* full            root:root 666 */
	fprintf(file, "%s %s:%s %o\n",
		"full",
		"root",
		"root",
		0666);
	/* random          root:root 444 */
	fprintf(file, "%s %s:%s %o\n",
		"random",
		"root",
		"root",
		0444);
	/* urandom         root:root 444 */
	fprintf(file, "%s %s:%s %o\n",
		"urandom",
		"root",
		"root",
		0444);
	/* hwrandom        root:root 444 */
	fprintf(file, "%s %s:%s %o\n",
		"hwrandom",
		"root",
		"root",
		0444);
	/* grsec           root:root 660 */
	fprintf(file, "%s %s:%s %o\n",
		"hwrandom",
		"root",
		"root",
		0660);

	/* kmem            root:root 640 */
	fprintf(file, "%s %s:%s %o\n",
		"kmem",
		"root",
		"root",
		0640);
	/* mem             root:root 640 */
	fprintf(file, "%s %s:%s %o\n",
		"mem",
		"root",
		"root",
		0640);
	/* port            root:root 640 */
	fprintf(file, "%s %s:%s %o\n",
		"port",
		"root",
		"root",
		0640);
	/* console may already exist; therefore ownership has to be changed with command */
	/* console         root:tty 600 @chmod 600 $MDEV */
	fprintf(file, "%s %s:%s %o %s\n",
		"console",
		"root",
		"tty",
		0600,
		"@chmod 600 \"$MDEV\"");
	/* ptmx            root:tty 666 */
	fprintf(file, "%s %s:%s %o\n",
		"ptmx",
		"root",
		"tty",
		0666);
	/* pty.*           root:tty 660 */
	fprintf(file, "%s %s:%s %o\n",
		"pty.*",
		"root",
		"tty",
		0660);

	/* Typical devices */
	/* tty             root:tty 666 */
	fprintf(file, "%s %s:%s %o\n",
		"tty",
		"root",
		"tty",
		0666);
	/* tty[0-9]*       root:tty 660 */
	fprintf(file, "%s %s:%s %o\n",
		"tty[0-9]*",
		"root",
		"tty",
		0660);
	/* vcsa*[0-9]*     root:tty 660 */
	fprintf(file, "%s %s:%s %o\n",
		"vcsa*[0-9]*",
		"root",
		"tty",
		0660);
	/* ttyS[0-9]*      root:uucp 660 */
	fprintf(file, "%s %s:%s %o\n",
		"ttyS[0-9]*",
		"root",
		"uucp",
		0660);

	/* block devices */
	/* ram([0-9]*)     root:disk 660 >rd/%1 */
	fprintf(file, "%s %s:%s %o %s\n",
		"ram([0-9]*)",
		"root",
		"disk",
		0660,
		">rd/%1");
	/* loop([0-9]+)    root:disk 660 >loop/%1 */
	fprintf(file, "%s %s:%s %o %s\n",
		"loop([0-9]+)",
		"root",
		"disk",
		0660,
		">loop/%1");
	// sd[a-z].*       root:disk 660 */lib/mdev/usbdisk_link
	fprintf(file, "%s %s:%s %o %s\n",
		"sd[a-z].*",
		"root",
		"disk",
		0660,
		"*/lib/mdev/usbdisk_link");
	// hd[a-z][0-9]*   root:disk 660 */lib/mdev/ide_links
	fprintf(file, "%s %s:%s %o %s\n",
		"hd[a-z][0-9]*",
		"root",
		"disk",
		0660,
		"*/lib/mdev/ide_link");
	/* md[0-9]*        root:disk 660 */
	fprintf(file, "%s %s:%s %o\n",
		"md[0-9]*",
		"root",
		"disk",
		0660);
	/* sr[0-9]*        root:cdrom 660 @ln -sf $MDEV cdrom */
	fprintf(file, "%s %s:%s %o %s\n",
		"sr[0-9]*",
		"root",
		"cdrom",
		0660,
		"@ln -sf $MDEV cdrom");
	/* fd[0-9]*        root:floppy 660 */
	fprintf(file, "%s %s:%s %o\n",
		"fd[0-9]*",
		"root",
		"floppy",
		0660);

	/* net devices */
	/* -net/.*         root:root 600 @nameif */
	fprintf(file, "%s %s:%s %o %s\n",
		"-net/.*",
		"root",
		"root",
		0600,
		"@nameif");
	/* tun[0-9]*       root:root 600 =net/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"tun[0-9]*",
		"root",
		"root",
		0600,
		"=net/");
	/* tap[0-9]*       root:root 600 =net/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"tap[0-9]*",
		"root",
		"root",
		0600,
		"=net/");

	/* alsa sound devices and audio stuff */
	/* pcm.*           root:audio 660 =snd/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"pcm.*",
		"root",
		"audio",
		0660,
		"=snd/");
	/* control.*       root:audio 660 =snd/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"control.*",
		"root",
		"audio",
		0660,
		"=snd/");
	/* midi.*          root:audio 660 =snd/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"midi.*",
		"root",
		"audio",
		0660,
		"=snd/");
	/* seq             root:audio 660 =snd/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"seq",
		"root",
		"audio",
		0660,
		"=snd/");
	/* timer           root:audio 660 =snd/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"timer",
		"root",
		"audio",
		0660,
		"=snd/");

	/* adsp            root:audio 660 >sound/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"adsp",
		"root",
		"audio",
		0660,
		">sound/");
	/* audio           root:audio 660 >sound/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"audio",
		"root",
		"audio",
		0660,
		">sound/");
	/* dsp             root:audio 660 >sound/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"dsp",
		"root",
		"audio",
		0660,
		">sound/");
	/* mixer           root:audio 660 >sound/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"mixer",
		"root",
		"audio",
		0660,
		">sound/");
	/* sequencer.*     root:audio 660 >sound/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"sequencer.*",
		"root",
		"audio",
		0660,
		">sound/");

	/* Less typical devices */
	/* raid controllers */
	/* cciss!(.*)      root:disk 660 =cciss/%1 */
	fprintf(file, "%s %s:%s %o %s\n",
		"cciss!(.*)",
		"root",
		"disk",
		0660,
		"=cciss/%1");
	/* ida!(.*)        root:disk 660 =ida/%1 */
	fprintf(file, "%s %s:%s %o %s\n",
		"ida!(.*)",
		"root",
		"disk",
		0660,
		"=ida/%1");
	/* rd!(.*)         root:disk 660 =rd/%1 */
	fprintf(file, "%s %s:%s %o %s\n",
		"rd!(.*)",
		"root",
		"disk",
		0660,
		"=rd/%1");

	/* ttyLTM[0-9]     root:dialout 660 @ln -sf $MDEV modem */
	fprintf(file, "%s %s:%s %o %s\n",
		"ttyLTM[0-9]",
		"root",
		"dialout",
		0660,
		"@ln -sf $MDEV modem");
	/* ttySHSF[0-9]    root:dialout 660 @ln -sf $MDEV modem */
	fprintf(file, "%s %s:%s %o %s\n",
		"ttySHSF[0-9]",
		"root",
		"dialout",
		0660,
		"@ln -sf $MDEV modem");
	/* slamr           root:dialout 660 @ln -sf $MDEV slamr0 */
	fprintf(file, "%s %s:%s %o %s\n",
		"slamr",
		"root",
		"dialout",
		0660,
		"@ln -sf $MDEV slamr0");
	/* slusb           root:dialout 660 @ln -sf $MDEV slusb0 */
	fprintf(file, "%s %s:%s %o %s\n",
		"slusb",
		"root",
		"dialout",
		0660,
		"@ln -sf $MDEV slusb0");

	/* fuse            root:root 666 */
	fprintf(file, "%s %s:%s %o\n",
		"fuse",
		"root",
		"root",
		0666);

	/* dri device */
	/* card[0-9]       root:video 660 =dri/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"card[0-9]",
		"root",
		"video",
		0660,
		"=dri/");

	/* misc stuff */
	/* agpgart         root:root 660 >misc/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"agpgart",
		"root",
		"root",
		0660,
		">misc/");
	/* psaux           root:root 660 >misc/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"psaux",
		"root",
		"root",
		0660,
		">misc/");
	/* rtc             root:root 664 >misc/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"rtc",
		"root",
		"root",
		0664,
		">misc/");

	/* input stuff */
	/* event[0-9]+     root:root 640 =input/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"event[0-9]+",
		"root",
		"root",
		0640,
		"=input/");
	/* mice            root:root 640 =input/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"mice",
		"root",
		"root",
		0640,
		"=input/");
	/* mouse[0-9]      root:root 640 =input/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"mouse[0-9]",
		"root",
		"root",
		0640,
		"=input/");
	/* ts[0-9]         root:root 600 =input/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"ts[0-9]",
		"root",
		"root",
		0600,
		"=input/");

	/* v4l stuff */
	/* vbi[0-9]        root:video 660 >v4l/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"vbi[0-9]",
		"root",
		"video",
		0660,
		">v4l/");
	/* video[0-9]      root:video 660 >v4l/ */
	fprintf(file, "%s %s:%s %o %s\n",
		"video[0-9]",
		"root",
		"video",
		0660,
		">v4l/");

	/* dvb stuff */
	// dvb.*           root:video 660 */lib/mdev/dvbdev
	fprintf(file, "%s %s:%s %o %s\n",
		"dvb.*",
		"root",
		"video",
		0660,
		"*/lib/mdev/dvbdev");

	/* load drivers for usb devices */
	// usbdev[0-9].[0-9]       root:root 660 */lib/mdev/usbdev
	fprintf(file, "%s %s:%s %o %s\n",
		"usbdev[0-9].[0-9]",
		"root",
		"root",
		0660,
		"*/lib/mdev/usbdev");
	/* usbdev[0-9].[0-9]_.*    root:root 660 */
	fprintf(file, "%s %s:%s %o\n",
		"usbdev[0-9].[0-9]_.*",
		"root",
		"root",
		0660);

	/* zaptel devices */
	/* zap(.*)         root:dialout 660 =zap/%1 */
	fprintf(file, "%s %s:%s %o %s\n",
		"zap(.*)",
		"root",
		"dialout",
		0660,
		"=zap/%1");
	/* dahdi!(.*)      root:dialout 660 =dahdi/%1 */
	fprintf(file, "%s %s:%s %o %s\n",
		"dahdi!(.*)",
		"root",
		"dialout",
		0660,
		"=dahdi/%1");
#endif
	return (EXIT_SUCCESS);
}

int pop_etc_mdev_conf(int flag)
{
        FILE *file = NULL;

	/* generate /etc/mdev.conf */
	file = fopen("/etc/mdev.conf", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	generate_mdev_common_rules(file);

	switch (flag) {
	case RC_ACT_BOOT :
		break;

	case RC_ACT_RESTART :
	case RC_ACT_START :
#if (HAVE_EZBOX_SERVICE_EMC2 == 1)
		fprintf(file, "%s %d:%d %o\n", "rtai_shm*", 0, 0, 0666);
		fprintf(file, "%s %d:%d %o\n", "rtf[0-9]*", 0, 0, 0666);
#endif
		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
