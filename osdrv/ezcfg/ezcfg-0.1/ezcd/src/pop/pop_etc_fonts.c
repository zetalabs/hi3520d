/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_fonts.c
 *
 * Description  : ezbox /etc/fonts/fonts.dtd &
                  /etc/fonts/fonts.conf file generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-09-02   0.1       Write it from scratch
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

static int generate_fonts_dtd(FILE *file)
{
	fprintf(file, "%s\n", "<!ELEMENT fontconfig (dir | cache | cachedir | include | config | selectfont | match | alias)* >");

	fprintf(file, "%s\n", "<!ELEMENT dir (#PCDATA)>");
	fprintf(file, "%s\n", "<!ATTLIST dir xml:space (default|preserve) 'preserve'>");

	fprintf(file, "%s\n", "<!ELEMENT cache (#PCDATA)>");
	fprintf(file, "%s\n", "<!ATTLIST cache xml:space (default|preserve) 'preserve'>");

	fprintf(file, "%s\n", "<!ELEMENT cachedir (#PCDATA)>");
	fprintf(file, "%s\n", "<!ATTLIST cachedir xml:space (default|preserve) 'preserve'>");

	fprintf(file, "%s\n", "<!ELEMENT include (#PCDATA)>");
	fprintf(file, "%s\n", "<!ATTLIST include");
	fprintf(file, "\t%s\n", "ignore_missing (no|yes) \"no\"");
	fprintf(file, "\t%s\n", "xml:space (default|preserve) \"preserve\">");

	fprintf(file, "%s\n", "<!ELEMENT config (blank|rescan)*>");

	fprintf(file, "%s\n", "<!ELEMENT blank (int)*>");

	fprintf(file, "%s\n", "<!ELEMENT rescan (int)>");

	fprintf(file, "%s\n", "<!ELEMENT selectfont (rejectfont | acceptfont)* >");

	fprintf(file, "%s\n", "<!ELEMENT rejectfont (glob | pattern)*>");

	fprintf(file, "%s\n", "<!ELEMENT acceptfont (glob | pattern)*>");

	fprintf(file, "%s\n", "<!ELEMENT glob (#PCDATA)>");

	fprintf(file, "%s\n", "<!ELEMENT pattern (patelt)*>");

	fprintf(file, "%s\n", "<!ENTITY % constant 'int|double|string|matrix|bool|charset|const'>");

	fprintf(file, "%s\n", "<!ELEMENT patelt (%constant;)*>");
	fprintf(file, "%s\n", "<!ATTLIST patelt name CDATA #REQUIRED>");

	fprintf(file, "%s\n", "<!ELEMENT alias (family*, prefer?, accept?, default?)>");
	fprintf(file, "%s\n", "<!ATTLIST alias binding (weak|strong|same) \"weak\">");
	fprintf(file, "%s\n", "<!ELEMENT prefer (family)*>");
	fprintf(file, "%s\n", "<!ELEMENT accept (family)*>");
	fprintf(file, "%s\n", "<!ELEMENT default (family)*>");
	fprintf(file, "%s\n", "<!ELEMENT family (#PCDATA)>");
	fprintf(file, "%s\n", "<!ATTLIST family xml:space (default|preserve) 'preserve'>");

	fprintf(file, "%s\n", "<!ENTITY % expr 'int|double|string|matrix|bool|charset");
	fprintf(file, "\t%s\n", "|name|const");
	fprintf(file, "\t%s\n", "|or|and|eq|not_eq|less|less_eq|more|more_eq|contains|not_contains");
	fprintf(file, "\t%s\n", "|plus|minus|times|divide|not|if|floor|ceil|round|trunc'>");

	fprintf(file, "%s\n", "<!ELEMENT match (test*, edit*)>");
	fprintf(file, "%s\n", "<!ATTLIST match target (pattern|font|scan) \"pattern\">");

	fprintf(file, "%s\n", "<!ELEMENT test (%expr;)*>");
	fprintf(file, "%s\n", "<!ATTLIST test");
	fprintf(file, "\t%s\n", "qual (any|all|first|not_first) \"any\"");
	fprintf(file, "\t%s\n", "name CDATA #REQUIRED");
	fprintf(file, "\t%s\n", "target (pattern|font|default) \"default\"");
	fprintf(file, "\t%s\n", "compare (eq|not_eq|less|less_eq|more|more_eq|contains|not_contains) \"eq\">");

	fprintf(file, "%s\n", "<!ELEMENT edit (%expr;)*>");
	fprintf(file, "%s\n", "<!ATTLIST edit");
	fprintf(file, "\t%s\n", "name CDATA #REQUIRED");
	fprintf(file, "\t%s\n", "mode (assign|assign_replace|prepend|append|prepend_first|append_last) \"assign\"");
	fprintf(file, "\t%s\n", "binding (weak|strong|same) \"weak\">");

	fprintf(file, "%s\n", "<!ELEMENT int (#PCDATA)>");
	fprintf(file, "%s\n", "<!ATTLIST int xml:space (default|preserve) 'preserve'>");
	fprintf(file, "%s\n", "<!ELEMENT double (#PCDATA)>");
	fprintf(file, "%s\n", "<!ATTLIST double xml:space (default|preserve) 'preserve'>");
	fprintf(file, "%s\n", "<!ELEMENT string (#PCDATA)>");
	fprintf(file, "%s\n", "<!ATTLIST string xml:space (default|preserve) 'preserve'>");
	fprintf(file, "%s\n", "<!ELEMENT matrix (double,double,double,double)>");
	fprintf(file, "%s\n", "<!ELEMENT bool (#PCDATA)>");
	fprintf(file, "%s\n", "<!ELEMENT charset (#PCDATA)>");
	fprintf(file, "%s\n", "<!ATTLIST charset xml:space (default|preserve) 'preserve'>");
	fprintf(file, "%s\n", "<!ELEMENT name (#PCDATA)>");
	fprintf(file, "%s\n", "<!ATTLIST name xml:space (default|preserve) 'preserve'>");
	fprintf(file, "%s\n", "<!ELEMENT const (#PCDATA)>");
	fprintf(file, "%s\n", "<!ATTLIST const xml:space (default|preserve) 'preserve'>");
	fprintf(file, "%s\n", "<!ELEMENT or (%expr;)*>");
	fprintf(file, "%s\n", "<!ELEMENT and (%expr;)*>");
	fprintf(file, "%s\n", "<!ELEMENT eq ((%expr;), (%expr;))>");
	fprintf(file, "%s\n", "<!ELEMENT not_eq ((%expr;), (%expr;))>");
	fprintf(file, "%s\n", "<!ELEMENT less ((%expr;), (%expr;))>");
	fprintf(file, "%s\n", "<!ELEMENT less_eq ((%expr;), (%expr;))>");
	fprintf(file, "%s\n", "<!ELEMENT more ((%expr;), (%expr;))>");
	fprintf(file, "%s\n", "<!ELEMENT more_eq ((%expr;), (%expr;))>");
	fprintf(file, "%s\n", "<!ELEMENT contains ((%expr;), (%expr;))>");
	fprintf(file, "%s\n", "<!ELEMENT not_contains ((%expr;), (%expr;))>");
	fprintf(file, "%s\n", "<!ELEMENT plus (%expr;)*>");
	fprintf(file, "%s\n", "<!ELEMENT minus (%expr;)*>");
	fprintf(file, "%s\n", "<!ELEMENT times (%expr;)*>");
	fprintf(file, "%s\n", "<!ELEMENT divide (%expr;)*>");
	fprintf(file, "%s\n", "<!ELEMENT not (%expr;)>");
	fprintf(file, "%s\n", "<!ELEMENT if ((%expr;), (%expr;), (%expr;))>");
	fprintf(file, "%s\n", "<!ELEMENT floor (%expr;)>");
	fprintf(file, "%s\n", "<!ELEMENT ceil (%expr;)>");
	fprintf(file, "%s\n", "<!ELEMENT round (%expr;)>");
	fprintf(file, "%s\n", "<!ELEMENT trunc (%expr;)>");

	return (EXIT_SUCCESS);
}

static int generate_fonts_conf(FILE *file)
{
	int i;
	fprintf(file, "%s\n", "<?xml version=\"1.0\"?>");
	fprintf(file, "%s\n", "<!DOCTYPE fontconfig SYSTEM \"fonts.dtd\">");
	fprintf(file, "%s\n", "<fontconfig>");

	fprintf(file, "\t%s\n", "<dir>/usr/share/fonts</dir>");
	fprintf(file, "\t%s\n", "<dir>/usr/local/share/fonts</dir>");

	/* Accept deprecated 'mono' alias, replacing it with 'monospace' */
	fprintf(file, "\t%s\n", "<match target=\"pattern\">");
	fprintf(file, "\t\t%s\n", "<test qual=\"any\" name=\"family\">");
	fprintf(file, "\t\t\t%s\n", "<string>mono</string>");
	fprintf(file, "\t\t%s\n", "</test>");
	fprintf(file, "\t\t%s\n", "<edit name=\"family\" mode=\"assign\">");
	fprintf(file, "\t\t\t%s\n", "<string>monospace</string>");
	fprintf(file, "\t\t%s\n", "</edit>");
	fprintf(file, "\t%s\n", "</match>");

	/* Accept alternate 'sans serif' spelling, replacing it with 'sans-serif' */
	fprintf(file, "\t%s\n", "<match target=\"pattern\">");
	fprintf(file, "\t\t%s\n", "<test qual=\"any\" name=\"family\">");
	fprintf(file, "\t\t\t%s\n", "<string>sans serif</string>");
	fprintf(file, "\t\t%s\n", "</test>");
	fprintf(file, "\t\t%s\n", "<edit name=\"family\" mode=\"assign\">");
	fprintf(file, "\t\t\t%s\n", "<string>sans-serif</string>");
	fprintf(file, "\t\t%s\n", "</edit>");
	fprintf(file, "\t%s\n", "</match>");

	/* Accept deprecated 'sans' alias, replacing it with 'sans-serif' */
	fprintf(file, "\t%s\n", "<match target=\"pattern\">");
	fprintf(file, "\t\t%s\n", "<test qual=\"any\" name=\"family\">");
	fprintf(file, "\t\t\t%s\n", "<string>sans</string>");
	fprintf(file, "\t\t%s\n", "</test>");
	fprintf(file, "\t\t%s\n", "<edit name=\"family\" mode=\"assign\">");
	fprintf(file, "\t\t\t%s\n", "<string>sans-serif</string>");
	fprintf(file, "\t\t%s\n", "</edit>");
	fprintf(file, "\t%s\n", "</match>");

	/* Load local system customization file */
	fprintf(file, "\t%s\n", "<include ignore_missing=\"yes\">conf.d</include>");

	/* Font cache directory list */
	fprintf(file, "\t%s\n", "<cachedir>/var/cache/fontconfig</cachedir>");

	fprintf(file, "\t%s\n", "<config>");

	/* These are the default Unicode chars that are expected to be blank
	 * in fonts.
	 */
	fprintf(file, "\t\t%s\n", "<blank>");
	i = 0x0020;
	fprintf(file, "\t\t\t<int>0x00%X</int>\n", i);
	i = 0x00A0;
	fprintf(file, "\t\t\t<int>0x00%X</int>\n", i);
	i = 0x00AD;
	fprintf(file, "\t\t\t<int>0x00%X</int>\n", i);
	i = 0x034F;
	fprintf(file, "\t\t\t<int>0x0%X</int>\n", i);
	for (i=0x0600; i<=0x0603; i++) {
		fprintf(file, "\t\t\t<int>0x0%X</int>\n", i);
	}
	i = 0x06DD;
	fprintf(file, "\t\t\t<int>0x0%X</int>\n", i);
	i = 0x070F;
	fprintf(file, "\t\t\t<int>0x0%X</int>\n", i);
	for (i=0x115F; i<=0x1160; i++) {
		fprintf(file, "\t\t\t<int>0x%X</int>\n", i);
	}
	i = 0x1680;
	fprintf(file, "\t\t\t<int>0x%X</int>\n", i);
	for (i=0x17B4; i<=0x17B5; i++) {
		fprintf(file, "\t\t\t<int>0x%X</int>\n", i);
	}
	i = 0x180E;
	fprintf(file, "\t\t\t<int>0x%X</int>\n", i);
	for (i=0x2000; i<=0x200F; i++) {
		fprintf(file, "\t\t\t<int>0x%X</int>\n", i);
	}
	for (i=0x2028; i<=0x202F; i++) {
		fprintf(file, "\t\t\t<int>0x%X</int>\n", i);
	}
	for (i=0x205F; i<=0x206F; i++) {
		fprintf(file, "\t\t\t<int>0x%X</int>\n", i);
	}
	i = 0x2800;
	fprintf(file, "\t\t\t<int>0x%X</int>\n", i);
	i = 0x3000;
	fprintf(file, "\t\t\t<int>0x%X</int>\n", i);
	i = 0x3164;
	fprintf(file, "\t\t\t<int>0x%X</int>\n", i);
	i = 0xFEFF;
	fprintf(file, "\t\t\t<int>0x%X</int>\n", i);
	i = 0xFFA0;
	fprintf(file, "\t\t\t<int>0x%X</int>\n", i);
	for (i=0xFFF9; i<=0xFFFB; i++) {
		fprintf(file, "\t\t\t<int>0x%X</int>\n", i);
	}
	fprintf(file, "\t\t%s\n", "</blank>");

	/* Rescan configuration every 30 seconds when FcFontSetList is called */
	fprintf(file, "\t\t%s\n", "<rescan>");
	i = 30;
	fprintf(file, "\t\t\t<int>%d</int>\n", i);
	fprintf(file, "\t\t%s\n", "</rescan>");

	fprintf(file, "\t%s\n", "</config>");

	fprintf(file, "%s\n", "</fontconfig>");

	return (EXIT_SUCCESS);
}

int pop_etc_fonts_fonts_dtd(int flag)
{
        FILE *file = NULL;

	/* generate /etc/fonts/fonts.dtd */
	file = fopen("/etc/fonts/fonts.dtd", "w");
	if (file == NULL)
		return (EXIT_FAILURE);


	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_START :
		generate_fonts_dtd(file);
		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}

int pop_etc_fonts_fonts_conf(int flag)
{
        FILE *file = NULL;

	/* generate /etc/fonts/fonts.conf */
	file = fopen("/etc/fonts/fonts.conf", "w");
	if (file == NULL)
		return (EXIT_FAILURE);


	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_START :
		generate_fonts_conf(file);
		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
