#include "includes.h"
#include "cmocka.h"

static void logger_InitTest(void **state)
{
    int ret = logger_queue_init();
    assert_false(ret, -1);
}


int main()
{
    const struct CMUnitTest temptests[] = {	
	
    cmocka_unit_test(lightSensor_InitTest),
    cmocka_unit_test(lightSensor_GetLightTest),
    cmocka_unit_test(lightSensor_GetCH0Test),

	};

	//return cmocka_run_group_tests(temptests, NULL, NULL);
    printf("testing......\n");
    return 0;

}