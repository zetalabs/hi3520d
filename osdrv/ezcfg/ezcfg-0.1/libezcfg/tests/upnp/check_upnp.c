#include <stdlib.h>
#include <check.h>
#include "ezcfg.h"
#include "ezcfg-private.h"

START_TEST(test_ezcfg_upnp_new)
{
	struct ezcfg *ezcfg;
	struct ezcfg_upnp *upnp;
	ezcfg = ezcfg_new(EZCFG_CONFIG_FILE_PATH);
	upnp = ezcfg_upnp_new(ezcfg);
	fail_if(upnp == NULL, 
		"ezcfg_upnp_new fail on creation");
	ezcfg_upnp_delete(upnp);
	ezcfg_delete(ezcfg);
}
END_TEST

START_TEST(test_ezcfg_upnp_parse_description)
{
	struct ezcfg *ezcfg;
	struct ezcfg_upnp *upnp;
	struct stat stat_buf;
	char path[] = "upnp/InternetGatewayDevice1.xml";

	fail_if(stat(path, &stat_buf) < 0,
		"stat %s", path);

	ezcfg = ezcfg_new(EZCFG_CONFIG_FILE_PATH);
	upnp = ezcfg_upnp_new(ezcfg);
	fail_if(upnp == NULL, 
		"ezcfg_upnp_new fail on creation");

	/* UPnP device role */
	ezcfg_upnp_set_role(upnp, EZCFG_UPNP_ROLE_DEVICE);

	fail_if(ezcfg_upnp_parse_description(upnp, path) == false,
		"ezcfg_upnp_parse_description %s fail", path);

	ezcfg_upnp_delete(upnp);
	ezcfg_delete(ezcfg);
}
END_TEST

static Suite * ezcfg_upnp_suite(void)
{
	Suite *s = suite_create("ezcfg_upnp");

	/* Core test case */
	TCase *tc_core = tcase_create("core");
	tcase_add_test(tc_core, test_ezcfg_upnp_new);
	tcase_add_test(tc_core, test_ezcfg_upnp_parse_description);
	suite_add_tcase(s, tc_core);

	return s;
}

int main (void)
{
	int number_failed;
	Suite *s = ezcfg_upnp_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
