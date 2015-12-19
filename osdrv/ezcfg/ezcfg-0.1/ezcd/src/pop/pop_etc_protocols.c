/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_protocols.c
 *
 * Description  : ezbox /etc/protocols file generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-02   0.1       Write it from scratch
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

int pop_etc_protocols(int flag)
{
        FILE *file = NULL;

	/* generate /etc/protocols */
	file = fopen("/etc/protocols", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	fprintf(file, "%s\t%d\t%s\n", "ip", 0, "IP");
	fprintf(file, "%s\t%d\t%s\n", "icmp", 1, "ICMP");
	fprintf(file, "%s\t%d\t%s\n", "igmp", 2, "IGMP");
	fprintf(file, "%s\t%d\t%s\n", "ggp", 3, "GGP");
	fprintf(file, "%s\t%d\t%s\n", "ipencap", 4, "IP-ENCAP");
	fprintf(file, "%s\t%d\t%s\n", "st", 5, "ST");
	fprintf(file, "%s\t%d\t%s\n", "tcp", 6, "TCP");
	fprintf(file, "%s\t%d\t%s\n", "egp", 8, "EGP");
	fprintf(file, "%s\t%d\t%s\n", "igp", 9, "IGP");
	fprintf(file, "%s\t%d\t%s\n", "pup", 12, "PUP");
	fprintf(file, "%s\t%d\t%s\n", "udp", 17, "UDP");
	fprintf(file, "%s\t%d\t%s\n", "hmp", 20, "HMP");
	fprintf(file, "%s\t%d\t%s\n", "xns-idp", 22, "XNS-IDP");
	fprintf(file, "%s\t%d\t%s\n", "iso-tp4", 29, "ISO-TP4");
	fprintf(file, "%s\t%d\t%s\n", "xtp", 36, "XTP");
	fprintf(file, "%s\t%d\t%s\n", "ddp", 37, "DDP");
	fprintf(file, "%s\t%d\t%s\n", "idpr-cmtp", 38, "IDPR-CMTP");
	fprintf(file, "%s\t%d\t%s\n", "ipv6", 41, "IPv6");
	fprintf(file, "%s\t%d\t%s\n", "ipv6-route", 43, "IPv6-Route");
	fprintf(file, "%s\t%d\t%s\n", "ipv6-frag", 44, "IPv6-Frag");
	fprintf(file, "%s\t%d\t%s\n", "idrp", 45, "IDRP");
	fprintf(file, "%s\t%d\t%s\n", "rsvp", 46, "RSVP");
	fprintf(file, "%s\t%d\t%s\n", "gre", 47, "GRE");
	fprintf(file, "%s\t%d\t%s\n", "esp", 50, "IPSEC-ESP");
	fprintf(file, "%s\t%d\t%s\n", "ah", 51, "IPSEC-AH");
	fprintf(file, "%s\t%d\t%s\n", "skip", 57, "SKIP");
	fprintf(file, "%s\t%d\t%s\n", "ipv6-icmp", 58, "IPv6-ICMP");
	fprintf(file, "%s\t%d\t%s\n", "ipv6-nonxt", 59, "IPv6-NoNxt");
	fprintf(file, "%s\t%d\t%s\n", "ipv6-opts", 60, "IPv6-Opts");
	fprintf(file, "%s\t%d\t%s\n", "rspf", 73, "RSPF CPHB");
	fprintf(file, "%s\t%d\t%s\n", "vmtp", 81, "VMTP");
	fprintf(file, "%s\t%d\t%s\n", "eigrp", 88, "EIGRP");
	fprintf(file, "%s\t%d\t%s\n", "ospf", 89, "OSPFIGP");
	fprintf(file, "%s\t%d\t%s\n", "ax.25", 93, "AX.25");
	fprintf(file, "%s\t%d\t%s\n", "ipip", 94, "IPIP");
	fprintf(file, "%s\t%d\t%s\n", "etherip", 97, "ETHERIP");
	fprintf(file, "%s\t%d\t%s\n", "encap", 98, "ENCAP");
	fprintf(file, "%s\t%d\t%s\n", "pim", 103, "PIM");
	fprintf(file, "%s\t%d\t%s\n", "ipcomp", 108, "IPCOMP");
	fprintf(file, "%s\t%d\t%s\n", "vrrp", 112, "VRRP");
	fprintf(file, "%s\t%d\t%s\n", "l2tp", 115, "L2TP");
	fprintf(file, "%s\t%d\t%s\n", "isis", 124, "ISIS");
	fprintf(file, "%s\t%d\t%s\n", "sctp", 132, "SCTP");
	fprintf(file, "%s\t%d\t%s\n", "fc", 133, "FC");

	fclose(file);
	return (EXIT_SUCCESS);
}
