#include <stdlib.h>
#include <check.h>
#include "ezcfg.h"
#include "ezcfg-private.h"

START_TEST(test_ezcfg_xml_new)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;
	ezcfg = ezcfg_new(EZCFG_CONFIG_FILE_PATH);
	xml = ezcfg_xml_new(ezcfg);
	fail_if(xml == NULL, 
		"ezcfg_xml_new fail on creation");
	ezcfg_xml_delete(xml);
	ezcfg_delete(ezcfg);
}
END_TEST

START_TEST(test_ezcfg_xml_parse)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;
	FILE *fp;
	struct stat stat_buf;
	char path[] = "xml/InternetGatewayDevice1.xml";
	char *buf;
	int buf_len;

	fail_if(stat(path, &stat_buf) < 0,
		"stat %s", path);

	buf_len = stat_buf.st_size + 1;
	buf = malloc(buf_len);
	fail_if(buf == NULL,
		"malloc %d bytes", buf_len);

	memset(buf, 0, buf_len);

	fp = fopen(path, "r");
	fail_if(fp == NULL,
		"fopen %s", path);

	fail_if(fread(buf, 1, stat_buf.st_size, fp) != (size_t)stat_buf.st_size,
		"fread %s", path);
	fclose(fp);
	buf[buf_len -1] = '\0';

	ezcfg = ezcfg_new(EZCFG_CONFIG_FILE_PATH);
	xml = ezcfg_xml_new(ezcfg);
	fail_if(xml == NULL, 
		"ezcfg_xml_new fail on creation");

	fail_if(ezcfg_xml_parse(xml, buf, buf_len) == false,
		"ezcfg_xml_parse %s fail", path);

	free(buf);

	ezcfg_xml_delete(xml);
	ezcfg_delete(ezcfg);
}
END_TEST

START_TEST(test_ezcfg_xml_parse_nil)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;
	char *buf;
	int buf_len;

	buf_len = 100;
	buf = malloc(buf_len);
	fail_if(buf == NULL,
		"malloc %d bytes", buf_len);

	memset(buf, 0, buf_len);

	ezcfg = ezcfg_new(EZCFG_CONFIG_FILE_PATH);
	xml = ezcfg_xml_new(ezcfg);
	fail_if(xml == NULL, 
		"ezcfg_xml_new fail on creation");

	fail_if(ezcfg_xml_parse(xml, buf, buf_len) == true,
		"ezcfg_xml_parse_nil fail");

	free(buf);

	ezcfg_xml_delete(xml);
	ezcfg_delete(ezcfg);
}
END_TEST

static Suite * ezcfg_xml_suite(void)
{
	Suite *s = suite_create("ezcfg_xml");

	/* Core test case */
	TCase *tc_core = tcase_create("core");
	tcase_add_test(tc_core, test_ezcfg_xml_new);
	tcase_add_test(tc_core, test_ezcfg_xml_parse);
	tcase_add_test(tc_core, test_ezcfg_xml_parse_nil);
	suite_add_tcase(s, tc_core);

	return s;
}

int main (void)
{
	int number_failed;
	Suite *s = ezcfg_xml_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
