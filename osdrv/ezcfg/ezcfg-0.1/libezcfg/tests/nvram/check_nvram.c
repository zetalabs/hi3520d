#include <stdlib.h>
#include <check.h>
#include "ezcfg.h"
#include "ezcfg-private.h"

START_TEST(test_ezcfg_nvram_new)
{
	struct ezcfg *ezcfg;
	struct ezcfg_nvram *nvram;
	ezcfg = ezcfg_new(EZCFG_CONFIG_FILE_PATH);
	nvram = ezcfg_nvram_new(ezcfg);
	fail_if(nvram == NULL, 
		"ezcfg_nvram_new fail on creation");
	ezcfg_nvram_delete(nvram);
	ezcfg_delete(ezcfg);
}
END_TEST

START_TEST(test_ezcfg_nvram_init)
{
	struct ezcfg *ezcfg;
	struct ezcfg_nvram *nvram;
	ezcfg = ezcfg_new(EZCFG_CONFIG_FILE_PATH);
	nvram = ezcfg_nvram_new(ezcfg);
	fail_if(nvram == NULL, 
		"ezcfg_nvram_new fail on creation");

	/* initialize nvram */
	ezcfg_nvram_fill_storage_info(nvram, NULL);
	fail_if(ezcfg_nvram_initialize(nvram) == false,
		"ezcfg_nvram_initialize fail");

	ezcfg_nvram_delete(nvram);
	ezcfg_delete(ezcfg);
}
END_TEST

static Suite * ezcfg_nvram_suite(void)
{
	Suite *s = suite_create("ezcfg_nvram");

	/* Core test case */
	TCase *tc_core = tcase_create("core");
	tcase_add_test(tc_core, test_ezcfg_nvram_new);
	tcase_add_test(tc_core, test_ezcfg_nvram_init);
	suite_add_tcase(s, tc_core);

	return s;
}

int main (void)
{
	int number_failed;
	Suite *s = ezcfg_nvram_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
