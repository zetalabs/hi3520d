#include <stdlib.h>
#include <check.h>
#include "ezcfg.h"
#include "ezcfg-private.h"

START_TEST(test_ezcfg_json_new)
{
	struct ezcfg *ezcfg;
	struct ezcfg_json *json;
	ezcfg = ezcfg_new(EZCFG_CONFIG_FILE_PATH);

	json = ezcfg_json_new(ezcfg);
	fail_if(json == NULL, 
		"ezcfg_json_new fail on object");
	ezcfg_json_delete(json);

	json = ezcfg_json_new(ezcfg);
	fail_if(json == NULL, 
		"ezcfg_json_new fail on array");
	ezcfg_json_delete(json);

	ezcfg_delete(ezcfg);
}
END_TEST

START_TEST(test_ezcfg_json_parse_text_object)
{
	struct ezcfg *ezcfg;
	struct ezcfg_json *json;
	FILE *fp;
	struct stat stat_buf;
	char path[] = "json/json_object.txt";
	char *buf;
	int buf_len;

	printf("%s(%d) =============\n", __func__, __LINE__);
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

	printf("buf=[%s]\n", buf);

	ezcfg = ezcfg_new(EZCFG_CONFIG_FILE_PATH);
	json = ezcfg_json_new(ezcfg);
	fail_if(json == NULL, 
		"ezcfg_json_new fail on object");

	printf("%s(%d) +++++++++++++\n\n", __func__, __LINE__);
	fail_if(ezcfg_json_parse_text(json, buf, buf_len) == EZCFG_RET_FAIL,
		"ezcfg_json_parse %s fail", path);
	printf("%s(%d) +++++++++++++\n\n", __func__, __LINE__);

	memset(buf, '\0', buf_len);
	printf("%s(%d) +++++++++++++\n\n", __func__, __LINE__);
	fail_if(ezcfg_json_write_message(json, buf, buf_len) < 0,
		"ezcfg_json_write_message %s fail", path);

	printf("%s(%d) +++++++++++++\n\n", __func__, __LINE__);
	printf("%s(%d) buf=[%s]\n\n", __func__, __LINE__, buf);
	printf("%s(%d) +++++++++++++\n\n", __func__, __LINE__);
	free(buf);

	printf("%s(%d) +++++++++++++\n\n", __func__, __LINE__);
	ezcfg_json_delete(json);
	printf("%s(%d) +++++++++++++\n\n", __func__, __LINE__);
	ezcfg_delete(ezcfg);
	printf("%s(%d) +++++++++++++\n\n", __func__, __LINE__);
}
END_TEST

START_TEST(test_ezcfg_json_parse_text_array)
{
	struct ezcfg *ezcfg;
	struct ezcfg_json *json;
	FILE *fp;
	struct stat stat_buf;
	char path[] = "json/json_array.txt";
	char *buf;
	int buf_len;

	printf("%s(%d) =============\n", __func__, __LINE__);
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
	json = ezcfg_json_new(ezcfg);
	fail_if(json == NULL, 
		"ezcfg_json_new fail on array");

	printf("%s(%d) +++++++++++++\n\n", __func__, __LINE__);
	fail_if(ezcfg_json_parse_text(json, buf, buf_len) == EZCFG_RET_FAIL,
		"ezcfg_json_parse %s fail", path);
	printf("%s(%d) +++++++++++++\n\n", __func__, __LINE__);

	memset(buf, '\0', buf_len);
	printf("%s(%d) +++++++++++++\n\n", __func__, __LINE__);
	fail_if(ezcfg_json_write_message(json, buf, buf_len) < 0,
		"ezcfg_json_write_message %s fail", path);
	printf("%s(%d) +++++++++++++\n\n", __func__, __LINE__);

	printf("%s(%d) buf=[%s]\n\n", __func__, __LINE__, buf);
	printf("%s(%d) +++++++++++++\n\n", __func__, __LINE__);
	free(buf);
	printf("%s(%d) +++++++++++++\n\n", __func__, __LINE__);

	ezcfg_json_delete(json);
	printf("%s(%d) +++++++++++++\n\n", __func__, __LINE__);
	ezcfg_delete(ezcfg);
	printf("%s(%d) +++++++++++++\n\n", __func__, __LINE__);
}
END_TEST

START_TEST(test_ezcfg_json_parse_nil)
{
	struct ezcfg *ezcfg;
	struct ezcfg_json *json;
	char *buf;
	int buf_len;

	buf_len = 100;
	buf = malloc(buf_len);
	fail_if(buf == NULL,
		"malloc %d bytes", buf_len);

	memset(buf, 0, buf_len);

	ezcfg = ezcfg_new(EZCFG_CONFIG_FILE_PATH);
	snprintf(buf, buf_len, "{}");
	json = ezcfg_json_new(ezcfg);
	fail_if(json == NULL, 
		"ezcfg_json_parse_nil fail on new object");

	fail_if(ezcfg_json_parse_text(json, buf, buf_len) == EZCFG_RET_FAIL,
		"ezcfg_json_parse_nil fail on object");
	ezcfg_json_delete(json);

	snprintf(buf, buf_len, "[]");
	json = ezcfg_json_new(ezcfg);
	fail_if(json == NULL, 
		"ezcfg_json_parse_nil fail on new array");
	fail_if(ezcfg_json_parse_text(json, buf, buf_len) == EZCFG_RET_FAIL,
		"ezcfg_json_parse_nil fail on array");
	ezcfg_json_delete(json);

	free(buf);
	ezcfg_delete(ezcfg);
}
END_TEST

static Suite * ezcfg_json_suite(void)
{
	Suite *s = suite_create("ezcfg_json");

	/* Core test case */
	TCase *tc_core = tcase_create("core");
	tcase_add_test(tc_core, test_ezcfg_json_new);
	tcase_add_test(tc_core, test_ezcfg_json_parse_text_object);
	tcase_add_test(tc_core, test_ezcfg_json_parse_text_array);
	tcase_add_test(tc_core, test_ezcfg_json_parse_nil);
	suite_add_tcase(s, tc_core);

	return s;
}

int main (void)
{
	int number_failed;
	Suite *s = ezcfg_json_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
