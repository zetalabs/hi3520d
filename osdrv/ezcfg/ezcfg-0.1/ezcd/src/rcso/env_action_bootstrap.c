/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : env_action_bootstrap.c
 *
 * Description  : ezbox env agent runs bootstrap service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2013-05-21   0.1       Write it from scratch
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

#define FDISK_COMMAND_FILE	"/tmp/fdisk-command"

#if 0
#define DBG(format, args...) do {\
	FILE *dbg_fp = fopen("/dev/kmsg", "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

static int generate_fdisk_command(char *name)
{
	int memsize;
	FILE *fp;
	int boot_n, root_n, ext_n, swap_n;
	//int data_n;
	int boot_size, root_size, swap_size;

	boot_n = 1;
	root_n = 2;
	ext_n = 3;
	swap_n = 5;
	//data_n = 6;

	fp = fopen(name, "w");
	if (fp == NULL)
		return (EXIT_FAILURE);

	memsize = utils_get_mem_size_mb();
	if (memsize < 1) {
		memsize = 512;
	}
	boot_size = 256;
	root_size = 512;
	swap_size = memsize;

	/* sda1 */
	/* add a new partition */
	fprintf(fp, "n\n");
	/* primary partition */
	fprintf(fp, "p\n");
	/* partition number is 1 */
	fprintf(fp, "%d\n", boot_n);
	/* first cylinder use default */
	fprintf(fp, "\n");
	/* last cylinder use 256M */
	fprintf(fp, "+%dM\n", boot_size);

	/* toggle a bootable flag */
	fprintf(fp, "a\n");
	/* partition number is 1 */
	fprintf(fp, "%d\n", boot_n);

	/* sda2 */
	/* add a new partition */
	fprintf(fp, "n\n");
	/* primary partition */
	fprintf(fp, "p\n");
	/* partition number is 2 */
	fprintf(fp, "%d\n", root_n);
	/* first cylinder use default */
	fprintf(fp, "\n");
	/* last cylinder use 512M */
	fprintf(fp, "+%dM\n", root_size);

	/* sda3 */
	/* add a new partition */
	fprintf(fp, "n\n");
	/* extended */
	fprintf(fp, "e\n");
	/* partition number is 3 */
	fprintf(fp, "%d\n", ext_n);
	/* first cylinder use default */
	fprintf(fp, "\n");
	/* last cylinder use default */
	fprintf(fp, "\n");

	/* sda5 */
	/* add a new partition */
	fprintf(fp, "n\n");
	/* logical */
	fprintf(fp, "l\n");
	/* first cylinder use default */
	fprintf(fp, "\n");
	/* last cylinder use 512M */
	fprintf(fp, "+%dM\n", swap_size);

	/* change a partition's system id */
	fprintf(fp, "t\n");
	/* partition number is 5 */
	fprintf(fp, "%d\n", swap_n);
	/* Linux swap */
	fprintf(fp, "82\n");

	/* sda6 */
	/* add a new partition */
	fprintf(fp, "n\n");
	/* logical */
	fprintf(fp, "l\n");
	/* first cylinder use default */
	fprintf(fp, "\n");
	/* last cylinder use default */
	fprintf(fp, "\n");

	/* write table to disk and exit */
	fprintf(fp, "w\n");

	fclose(fp);

	return (EXIT_SUCCESS);
}

static int generate_grub_cfg(char *name)
{
	char boot_args[1024];
	char distro_name[64];
	char sw_ver[16];
	FILE *fp;
	int ret;

	ret = utils_get_bootcfg_keyword_full(NVRAM_SERVICE_OPTION(SYS, BOOT_ARGS), boot_args, sizeof(boot_args));
	if (ret < 0) {
		DBG("%s(%d) utils_get_bootcfg_keyword(%s) error!\n", __func__, __LINE__, NVRAM_SERVICE_OPTION(SYS, BOOT_ARGS));
		return (EXIT_FAILURE);
	}

	ret = utils_get_bootcfg_keyword(NVRAM_SERVICE_OPTION(SYS, DISTRO_NAME), distro_name, sizeof(distro_name));
	if (ret < 0) {
		DBG("%s(%d) utils_get_bootcfg_keyword(%s) error!\n", __func__, __LINE__, NVRAM_SERVICE_OPTION(SYS, DISTRO_NAME));
		return (EXIT_FAILURE);
	}

	ret = utils_get_bootcfg_keyword(NVRAM_SERVICE_OPTION(SYS, SOFTWARE_VERSION), sw_ver, sizeof(sw_ver));
	if (ret < 0) {
		DBG("%s(%d) utils_get_bootcfg_keyword(%s) error!\n", __func__, __LINE__, NVRAM_SERVICE_OPTION(SYS, SOFTWARE_VERSION));
		return (EXIT_FAILURE);
	}

	fp = fopen(name, "w");
	if (fp == NULL) {
		DBG("%s(%d) fopen(%s) error!\n", __func__, __LINE__, name);
		return (EXIT_FAILURE);
	}

	fprintf(fp, "set default=\"%d\"\n", 0);
	fprintf(fp, "set timeout=%d\n", 0);
	fprintf(fp, "set root='(hd0,msdos1)'\n");

	fprintf(fp, "menuentry 'ezbox %s %s' --class gnu-linux --class gnu --class os {\n", distro_name, sw_ver);
	fprintf(fp, "\tlinux\t/vmlinuz %s\n", boot_args);
	fprintf(fp, "}\n");

	fclose(fp);

	return (EXIT_SUCCESS);
}

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int env_action_bootstrap(int argc, char **argv)
#endif
{
	int ret, flag;
	char buf[256];
	char name[64];
	char hdd_dev[64];
	char boot_dev[64];
	char root_dev[64];
	char data_dev[64];
	char swap_dev[64];
	char boot_mount_point[64];
	char root_mount_point[64];
	char data_mount_point[64];
	struct stat stat_buf;
	int boot_n, root_n, swap_n, data_n;
	char *fdisk_argv[] = { CMD_FDISK, buf, NULL };
	char *xzcat_argv[] = { CMD_XZCAT, ROOTFS_IMAGE_FILE_PATH, NULL };

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "bootstrap")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	/* get HDD device name */
	ret = utils_get_hdd_device_path(hdd_dev, sizeof(hdd_dev));
	if (ret < 1) {
		return (EXIT_FAILURE);
	}

	/* get boot partition device name */
	ret = utils_get_boot_device_path(boot_dev, sizeof(boot_dev));
	if (ret < 1) {
		return (EXIT_FAILURE);
	}

	/* get root partition device name */
	ret = utils_get_root_device_path(root_dev, sizeof(root_dev));
	if (ret < 1) {
		return (EXIT_FAILURE);
	}

	/* get data partition device name */
	ret = utils_get_data_device_path(data_dev, sizeof(data_dev));
	if (ret < 1) {
		return (EXIT_FAILURE);
	}

	/* get swap partition device name */
	ret = utils_get_swap_device_path(swap_dev, sizeof(swap_dev));
	if (ret < 1) {
		return (EXIT_FAILURE);
	}

	if (strncmp(boot_dev, hdd_dev, strlen(hdd_dev)) != 0) {
		return (EXIT_FAILURE);
	}
	if (strncmp(root_dev, hdd_dev, strlen(hdd_dev)) != 0) {
		return (EXIT_FAILURE);
	}
	if (strncmp(data_dev, hdd_dev, strlen(hdd_dev)) != 0) {
		return (EXIT_FAILURE);
	}
	if (strncmp(swap_dev, hdd_dev, strlen(hdd_dev)) != 0) {
		return (EXIT_FAILURE);
	}
	boot_n = atoi(boot_dev + strlen(hdd_dev));
	root_n = atoi(root_dev + strlen(hdd_dev));
	data_n = atoi(data_dev + strlen(hdd_dev));
	swap_n = atoi(swap_dev + strlen(hdd_dev));
	if ((boot_n != 1) ||
	    (root_n != 2) ||
	    (swap_n != 5) ||
	    (data_n != 6)) {
		return (EXIT_FAILURE);
	}

	/* check vmlinuz is OK */
	if ((stat(KERNEL_IMAGE_FILE_PATH, &stat_buf) != 0) ||
	    (!S_ISREG(stat_buf.st_mode))) {
		DBG("%s(%d) %s is not OK!\n", __func__, __LINE__, KERNEL_IMAGE_FILE_PATH);
		return (EXIT_FAILURE);
	}

	/* check rootfs is OK */
	if ((stat(ROOTFS_IMAGE_FILE_PATH, &stat_buf) != 0) ||
	    (!S_ISREG(stat_buf.st_mode))) {
		DBG("%s(%d) %s is not OK!\n", __func__, __LINE__, ROOTFS_IMAGE_FILE_PATH);
		return (EXIT_FAILURE);
	}

	snprintf(boot_mount_point, sizeof(boot_mount_point), "/mnt/boot");
	snprintf(root_mount_point, sizeof(root_mount_point), "/mnt/root");
	snprintf(data_mount_point, sizeof(data_mount_point), "/mnt/data");

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_START :
		/* cleanup HDD partition table */
		snprintf(buf, sizeof(buf), "%s if=/dev/zero of=/dev/%s bs=1024 count=1024", CMD_DD, hdd_dev);
		utils_system(buf);

		/* generate fdisk command */
		snprintf(name, sizeof(name), "%s-%d", FDISK_COMMAND_FILE, getpid());
		generate_fdisk_command(name);

		/* make partitions */
		snprintf(buf, sizeof(buf), "/dev/%s", hdd_dev);
		utils_execute(fdisk_argv, name, NULL, 0, NULL);

		unlink(name);

		/* populate /dev/sda1 ~ /dev/sda6 */
		utils_udev_pop_nodes();

		/* make boot device filesystem */
		snprintf(buf, sizeof(buf), "%s /dev/%s", CMD_MKFS_EXT4, boot_dev);
		utils_system(buf);

		/* mkdir -p boot_mount_point */
		snprintf(buf, sizeof(buf), "%s -p %s", CMD_MKDIR, boot_mount_point);
		utils_system(buf);

		/* mount boot partition */
		snprintf(buf, sizeof(buf), "%s -t ext4 /dev/%s %s", CMD_MOUNT, boot_dev, boot_mount_point);
		utils_system(buf);

		/* install bootloader */
		snprintf(buf, sizeof(buf), "%s --boot-directory=%s /dev/%s", CMD_GRUB_INSTALL, boot_mount_point, hdd_dev);
		utils_system(buf);

		/* generate grub.cfg */
		snprintf(name, sizeof(name), "%s/grub/grub.cfg", boot_mount_point);
		generate_grub_cfg(name);

		/* copy needed boot files */
		snprintf(buf, sizeof(buf), "%s -af %s %s/", CMD_CP, BOOT_CONFIG_FILE_PATH, boot_mount_point);
		utils_system(buf);
		snprintf(buf, sizeof(buf), "%s -af %s %s/", CMD_CP, BOOT_CONFIG_DEFAULT_FILE_PATH, boot_mount_point);
		utils_system(buf);
		snprintf(buf, sizeof(buf), "%s -af %s %s/", CMD_CP, KERNEL_IMAGE_FILE_PATH, boot_mount_point);
		utils_system(buf);
		snprintf(buf, sizeof(buf), "%s -af %s %s/", CMD_CP, ROOTFS_IMAGE_FILE_PATH, boot_mount_point);
		utils_system(buf);
		if ((stat(ROOTFS_CONFIG_FILE_PATH, &stat_buf) == 0) &&
		    (S_ISREG(stat_buf.st_mode))) {
			snprintf(buf, sizeof(buf), "%s -af %s %s/", CMD_CP, ROOTFS_CONFIG_FILE_PATH, boot_mount_point);
			utils_system(buf);
		}
		if ((stat(UPGRADE_CONFIG_FILE_PATH, &stat_buf) == 0) &&
		    (S_ISREG(stat_buf.st_mode))) {
			snprintf(buf, sizeof(buf), "%s -af %s %s/", CMD_CP, UPGRADE_CONFIG_FILE_PATH, boot_mount_point);
			utils_system(buf);
		}

		/* umount boot partition */
		snprintf(buf, sizeof(buf), "%s %s", CMD_UMOUNT, boot_mount_point);
		utils_system(buf);

		/* make root device filesystem */
		snprintf(buf, sizeof(buf), "%s /dev/%s", CMD_MKFS_EXT4, root_dev);
		utils_system(buf);

		/* mkdir -p root_mount_point */
		snprintf(buf, sizeof(buf), "%s -p %s", CMD_MKDIR, root_mount_point);
		utils_system(buf);

		/* mount root partition */
		snprintf(buf, sizeof(buf), "%s -t ext4 /dev/%s %s", CMD_MOUNT, root_dev, root_mount_point);
		utils_system(buf);

		/* uncompress rootfs image */
		snprintf(name, sizeof(name), ">%s/rootfs-%d.tar", root_mount_point, getpid());
		utils_execute(xzcat_argv, NULL, name, 0, NULL);
		snprintf(buf, sizeof(buf), "%s -C %s -vxf %s", CMD_TAR, root_mount_point, name+1);
		utils_system(buf);
		unlink(name+1);

		/* umount root partition */
		snprintf(buf, sizeof(buf), "%s %s", CMD_UMOUNT, root_mount_point);
		utils_system(buf);

		/* make swap partition */
		snprintf(buf, sizeof(buf), "%s /dev/%s", CMD_MKSWAP, swap_dev);
		utils_system(buf);

		/* make data device filesystem */
		snprintf(buf, sizeof(buf), "%s /dev/%s", CMD_MKFS_EXT4, data_dev);
		utils_system(buf);

		/* mkdir -p data_mount_point */
		snprintf(buf, sizeof(buf), "%s -p %s", CMD_MKDIR, data_mount_point);
		utils_system(buf);

		/* mount data partition */
		snprintf(buf, sizeof(buf), "%s -t ext4 /dev/%s %s", CMD_MOUNT, data_dev, data_mount_point);
		utils_system(buf);

		/* uncompress rootfs image */
		snprintf(name, sizeof(name), ">%s/rootfs-%d.tar", data_mount_point, getpid());
		utils_execute(xzcat_argv, NULL, name, 0, NULL);
		snprintf(buf, sizeof(buf), "%s/rootfs", data_mount_point);
		mkdir(buf, 0777);
		snprintf(buf, sizeof(buf), "%s -C %s/rootfs -vxf %s", CMD_TAR, data_mount_point, name+1);
		utils_system(buf);
		unlink(name+1);

		/* umount data partition */
		snprintf(buf, sizeof(buf), "%s %s", CMD_UMOUNT, data_mount_point);
		utils_system(buf);

		ret = EXIT_SUCCESS;
		break;

	default:
		ret = EXIT_FAILURE;
		break;
	}
	return (ret);
}
