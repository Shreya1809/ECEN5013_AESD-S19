/**
 * @file test.c
 * @author your Shreya Chakraborty
 * @brief Cmoka Testing for Logger
 * @version 0.1
 * @date 2019-03-31
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "includes.h"
#include "cmocka.h"
#include <unistd.h>
#include "logger.h"

static void logger_InitTest(void **state)
{
    int ret = logger_queue_init();
    assert_false(ret == -1);
}

static void logger_EnqueueTest(void **state)
{
    int ret = LOG_ENQUEUE(1, 1, "testing");
    assert_int_equal(ret, 0);
}

static void logger_logInfoTest(void **state)
{
    int ret = LOG_INFO(1, "testing");
    assert_int_equal(ret, 0);
}

static void logger_logErrorTest(void **state)
{
    int ret = LOG_ERROR(1, "testing");
    assert_int_equal(ret, 0);
}

static void logger_logWarnTest(void **state)
{
    int ret = LOG_WARN(1, "testing");
    assert_int_equal(ret, 0);
}

static void logger_logDebugTest(void **state)
{
    int ret = LOG_DEBUG(1, "testing");
    assert_int_equal(ret, 0);
}
int main()
{
    const struct CMUnitTest logtests[] = {	
	
    cmocka_unit_test(logger_InitTest),
    cmocka_unit_test(logger_EnqueueTest),
    cmocka_unit_test(logger_logInfoTest),
    cmocka_unit_test(logger_logErrorTest),
    cmocka_unit_test(logger_logWarnTest),
    cmocka_unit_test(logger_logDebugTest)

	};

	return cmocka_run_group_tests(logtests, NULL, NULL);
    printf("testing......\n");
    return 0;

}