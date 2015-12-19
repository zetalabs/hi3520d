#ifndef _UTILS_H_
#define _UTILS_H_

/* shell command */
/* core commands */
#define CMD_BRCTL		"/sbin/brctl"
#define CMD_CAT			"/bin/cat"
#define CMD_CHMOD		"/bin/chmod"
#define CMD_CHPASSWD		"/sbin/chpasswd"
#define CMD_CP			"/bin/cp"
#define CMD_DD			"/bin/dd"
#define CMD_DEPMOD		"/sbin/depmod"
#define CMD_EZCD		"/sbin/ezcd"
#define CMD_EZCM		"/sbin/ezcm"
#define CMD_FDISK		"/sbin/fdisk"
#define CMD_GRUB_INSTALL	"/usr/sbin/grub-install"
//#define CMD_IFCONFIG		"/sbin/ifconfig"
#define CMD_IFDOWN		"/sbin/ifdown"
#define CMD_IFUP		"/sbin/ifup"
#define CMD_IP			"/bin/ip"
#define CMD_IPTABLES		"/usr/sbin/iptables"
#define CMD_IPTABLES_RESTORE	"/usr/sbin/iptables-restore"
#define CMD_INSMOD		"/sbin/insmod"
#define CMD_KILLALL		"/bin/killall"
#define CMD_KLOGD  		"/sbin/klogd"
#define CMD_LDCONFIG  		"/sbin/ldconfig"
#define CMD_LOGIN  		"/bin/login"
#define CMD_MKFS_EXT4  		"/usr/sbin/mkfs.ext4"
#define CMD_MKDIR  		"/bin/mkdir"
#define CMD_MKSWAP  		"/sbin/mkswap"
#define CMD_MODPROBE  		"/sbin/modprobe"
#define CMD_MOUNT  		"/bin/mount"
#define CMD_MV  		"/bin/mv"
#define CMD_RC  		"/sbin/rc"
#define CMD_RM  		"/bin/rm"
#define CMD_RMMOD  		"/sbin/rmmod"
//#define CMD_ROUTE  		"/sbin/route"
#define CMD_SH  		"/bin/sh"
#define CMD_SHELLD  		"/sbin/shelld"
#define CMD_SWAPOFF  		"/sbin/swapoff"
#define CMD_SWAPON  		"/sbin/swapon"
#define CMD_SWITCH_ROOT		"/sbin/switch_root"
#define CMD_SYSLOGD  		"/sbin/syslogd"
#define CMD_TAR  		"/bin/tar"
#define CMD_TELNETD  		"/sbin/telnetd"
#define CMD_UDHCPC_SCRIPT	"/sbin/udhcpc.script"
#define CMD_UMOUNT		"/bin/umount"
#define CMD_UPNP_MONITOR	"/sbin/upnp_monitor"
#define CMD_XZCAT		"/bin/xzcat"

/* service commands */
#define CMD_CGCONFIGPARSER	"/usr/sbin/cgconfigparser"
#define CMD_CGRULESENGD	"/usr/sbin/cgrulesengd"
#define CMD_CRYPTSETUP	"/usr/sbin/cryptsetup"
#define CMD_DHID	"/usr/sbin/dhid"
#define CMD_DNSMASQ	"/usr/sbin/dnsmasq"
#define CMD_PPPD	"/usr/sbin/pppd"
#define CMD_PPPOE_SERVER	"/usr/sbin/pppoe-server"
#define CMD_RADIUSD	"/usr/sbin/radiusd"
#define CMD_RADVD	"/usr/sbin/radvd"


/* default boot config file path */
#define BOOT_CONFIG_DEFAULT_FILE_PATH   "/boot/ezbox_boot.cfg.dft"

/* boot config file path */
#define BOOT_CONFIG_FILE_PATH   "/boot/ezbox_boot.cfg"

/* upgrade config file path */
#define UPGRADE_CONFIG_FILE_PATH   "/boot/ezbox_upgrade.cfg"

/* rootfs cleanup config file path */
#define ROOTFS_CONFIG_FILE_PATH   "/boot/ezbox_rootfs.cfg"

/* kernel image file path */
#define KERNEL_IMAGE_FILE_PATH   "/boot/vmlinuz"

/* rootfs image file path */
#define ROOTFS_IMAGE_FILE_PATH   "/boot/rootfs.tar.xz"

/* root HOME path */
#define ROOT_HOME_PATH		"/root"

/* dm-crypt keys path */
#define DMCRYPT_DATA_PARTITION_KEY_FILE_PATH	"/etc/keys/data_partition_key"

/* backup directory path in /data */
#define DATA_BACKUP_DIR_PATH			"/data/backup"

/* rootfs directory path in /data */
#define DATA_ROOTFS_DIR_PATH			"/data/rootfs"
#define DATA_ROOTFS_BIN_DIR_PATH		DATA_ROOTFS_DIR_PATH "/bin"
#define DATA_ROOTFS_HOME_DIR_PATH		DATA_ROOTFS_DIR_PATH "/home"
#define DATA_ROOTFS_LIB_DIR_PATH		DATA_ROOTFS_DIR_PATH "/lib"
#define DATA_ROOTFS_ROOT_DIR_PATH		DATA_ROOTFS_DIR_PATH ROOT_HOME_PATH
#define DATA_ROOTFS_SBIN_DIR_PATH		DATA_ROOTFS_DIR_PATH "/sbin"
#define DATA_ROOTFS_USR_DIR_PATH		DATA_ROOTFS_DIR_PATH "/usr"
#define DATA_ROOTFS_VAR_DIR_PATH		DATA_ROOTFS_DIR_PATH "/var"

/* ezcd command config file path */
#define EZCD_CONFIG_FILE_PATH           "/etc/ezcd/ezcd.conf"
#define EZCD_CONFIG_FILE_PATH_NEW       "/etc/ezcd/ezcd.conf.new"

/* eznvc command config file path */
#define EZNVC_CONFIG_FILE_PATH          "/etc/eznvc/eznvc.conf"

/* ezcm command config file path */
//#define EZCM_CONFIG_FILE_PATH           "/etc/ezcm.conf"

/* nvram command config file path */
//#define NVRAM_CONFIG_FILE_PATH          "/etc/nvram.conf"

/* upnp_monitor task file path */
#define UPNP_MONITOR_TASK_FILE_PATH     "/etc/upnp_monitor/tasks"

/* udhcpc script path */
#define UDHCPC_SCRIPT_FILE_NAME	"default.script"
#define UDHCPC_SCRIPT_FILE_DIR	"/usr/share/udhcpc"
#define UDHCPC_SCRIPT_FILE_PATH	UDHCPC_SCRIPT_FILE_DIR "/" UDHCPC_SCRIPT_FILE_NAME

/* Linux kernel version */
#define KERNEL_VERSION(a,b,c)	(((a) << 16) + ((b) << 8) + (c))

/* Linux kernel char __initdata boot_command_line[COMMAND_LINE_SIZE]; */
#define KERNEL_COMMAND_LINE_SIZE	512

/* /proc/sys/kernel/hotplug */
#define PROC_HOTPLUG_FILE_PATH	"/proc/sys/kernel/hotplug"

/* partition mount timeout */
#define PARTITION_MOUNT_TIMEOUT		10

/* file line length */
#define FILE_LINE_BUFFER_SIZE		1024

#define RC_RUN_LEVEL(a,b,c)	(((a) << 16) + ((b) << 8) + (c))

/* removed line tails */
#define LINE_TAIL_STRING	" \t\r\n"

#define RC_MAX_ARGS	16
#define RC_COMMAND_LINE_SIZE	256
#define IS_BLANK(c)	((c) == ' ' || (c) == '\t')
#define IS_COLON(c)	((c) == ':')

typedef int (*rc_function_t)(int, char **);
typedef int (*action_func_t)(int, char **);

typedef struct rc_func_s {
	char *name;
	int flag;
	int (*func)(int flag);
	int start;
	int stop;
	char *deps;
} rc_func_t;

typedef struct proc_stat_s {
	int pid;
	char state;
} proc_stat_t;

/* rc action state */
enum {
	RC_ACT_UNKNOWN = 0,
	RC_ACT_BOOT,
	RC_ACT_START,
	RC_ACT_STOP,
	RC_ACT_RESTART,
	RC_ACT_RELOAD,
	RC_ACT_COMBINED,
	RC_ACT_USRDEF,
};

/* rc debug states */
enum {
	RC_DEBUG_UNKNOWN = 0,
	RC_DEBUG_DUMP,
};

/* init states */
enum {
	INIT_IDLE,
};

/* WAN type */
enum {
	WAN_TYPE_UNKNOWN = 0,
	WAN_TYPE_DHCP,
	WAN_TYPE_STATIC,
	WAN_TYPE_PPPOE,
};

#endif

