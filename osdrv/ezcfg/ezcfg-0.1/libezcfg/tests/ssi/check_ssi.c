#include <stdlib.h>
#include <check.h>
#include "ezcfg.h"
#include "ezcfg-private.h"

START_TEST(test_ezcfg_ssi_new)
{
	struct ezcfg *ezcfg;
	struct ezcfg_nvram *nvram;
	struct ezcfg_ssi *ssi;
	ezcfg = ezcfg_new(EZCFG_CONFIG_FILE_PATH);
	nvram = ezcfg_nvram_new(ezcfg);
	ssi = ezcfg_ssi_new(ezcfg, nvram);
	fail_if(ssi == NULL, 
		"ezcfg_ssi_new fail on creation");
	ezcfg_ssi_delete(ssi);
	ezcfg_nvram_delete(nvram);
	ezcfg_delete(ezcfg);
}
END_TEST

START_TEST(test_ezcfg_ssi_parse_include)
{
	struct ezcfg *ezcfg;
	struct ezcfg_nvram *nvram;
	struct ezcfg_ssi *ssi;
	struct stat stat_buf;
	char buf[1024];
	int msg_len;
	char path[] = "ssi/mytest_include.shtm";

	fail_if(stat(path, &stat_buf) < 0,
		"stat %s", path);

	ezcfg = ezcfg_new(EZCFG_CONFIG_FILE_PATH);
	nvram = ezcfg_nvram_new(ezcfg);

	/* initialize nvram */
	ezcfg_nvram_fill_storage_info(nvram, NULL);
	fail_if(ezcfg_nvram_initialize(nvram) == false,
		"ezcfg_nvram_initialize fail");

	ssi = ezcfg_ssi_new(ezcfg, nvram);
	fail_if(ssi == NULL, 
		"ezcfg_ssi_new fail on creation");

	/* set default document root */
	fail_if(ezcfg_ssi_set_document_root(ssi, "./") == false,
		"ezcfg_ssi_set_document_root fail for [%s]", "./");

	/* set SSI file path */
	fail_if(ezcfg_ssi_set_path(ssi, path) == false,
		"ezcfg_ssi_set_path fail for [%s]", path);

	/* open SSI file */
	fail_if(ezcfg_ssi_open_file(ssi, "r") == NULL,
		"ezcfg_ssi_open_file fail for [%s]", path);

	/* process SSI file */
	msg_len = ezcfg_ssi_file_get_line(ssi, buf, sizeof(buf));
	fail_if(msg_len < 0,
		"ezcfg_ssi_file_get_line fail");

	while(msg_len >= 0) {
		if (msg_len > 0) {
			printf("buf=[%s]\n", buf);
		}
		msg_len = ezcfg_ssi_file_get_line(ssi, buf, sizeof(buf));
	}

	ezcfg_ssi_delete(ssi);
	ezcfg_nvram_delete(nvram);
	ezcfg_delete(ezcfg);
}
END_TEST

START_TEST(test_ezcfg_ssi_parse_echo)
{
	struct ezcfg *ezcfg;
	struct ezcfg_nvram *nvram;
	struct ezcfg_ssi *ssi;
	struct stat stat_buf;
	char buf[1024];
	int msg_len;
	char path[] = "ssi/mytest_echo.shtm";

	fail_if(stat(path, &stat_buf) < 0,
		"stat %s", path);

	ezcfg = ezcfg_new(EZCFG_CONFIG_FILE_PATH);
	nvram = ezcfg_nvram_new(ezcfg);

	/* initialize nvram */
	ezcfg_nvram_fill_storage_info(nvram, NULL);
	fail_if(ezcfg_nvram_initialize(nvram) == false,
		"ezcfg_nvram_initialize fail");

	ssi = ezcfg_ssi_new(ezcfg, nvram);
	fail_if(ssi == NULL, 
		"ezcfg_ssi_new fail on creation");

	/* set default document root */
	fail_if(ezcfg_ssi_set_document_root(ssi, "./") == false,
		"ezcfg_ssi_set_document_root fail for [%s]", "./");

	/* set SSI file path */
	fail_if(ezcfg_ssi_set_path(ssi, path) == false,
		"ezcfg_ssi_set_path fail for [%s]", path);

	/* open SSI file */
	fail_if(ezcfg_ssi_open_file(ssi, "r") == NULL,
		"ezcfg_ssi_open_file fail for [%s]", path);

	/* process SSI file */
	msg_len = ezcfg_ssi_file_get_line(ssi, buf, sizeof(buf));
	fail_if(msg_len < 0,
		"ezcfg_ssi_file_get_line fail");

	while(msg_len >= 0) {
		if (msg_len > 0) {
			printf("buf=[%s]\n", buf);
		}
		msg_len = ezcfg_ssi_file_get_line(ssi, buf, sizeof(buf));
	}

	ezcfg_ssi_delete(ssi);
	ezcfg_nvram_delete(nvram);
	ezcfg_delete(ezcfg);
}
END_TEST

static Suite * ezcfg_ssi_suite(void)
{
	Suite *s = suite_create("ezcfg_ssi");

	/* Core test case */
	TCase *tc_core = tcase_create("core");
	tcase_add_test(tc_core, test_ezcfg_ssi_new);
	tcase_add_test(tc_core, test_ezcfg_ssi_parse_include);
	tcase_add_test(tc_core, test_ezcfg_ssi_parse_echo);
	suite_add_tcase(s, tc_core);

	return s;
}

int main (void)
{
	int number_failed;
	Suite *s = ezcfg_ssi_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
