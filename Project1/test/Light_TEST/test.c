/**
 * @file test.c
 * @author Shreya Chakraborty
 * @brief Cmoka unit testing for light sensor register functions
 * @version 0.1
 * @date 2019-03-31
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "myI2C.h"
#include "lightSensor.h"
#include <unistd.h>
#include "includes.h"
#include "cmocka.h"
#include "light.h"
#include "logger.h"

// i2c_struct_t i2c_handler;

static void lightSensor_InitTest(void **state)
{
    int ret = I2C_init(&i2c_handler);
    ret += APDS9301_powerup();
    assert_int_equal(ret, 0);
    //sleep(1);
}

static void lightSensor_GetLightTest(void **state)
{
    float lux;
    int ret = APDS9301_getlight(&lux);
    assert_int_equal(ret, 0);
}

static void lightSensor_GetCH0Test(void **state)
{
    uint16_t data;
    int ret = APDS9301_getCh0(&data);
    assert_int_equal(ret, 0);
}

static void lightSensor_GetCH1Test(void **state)
{
    uint16_t data;
    int ret = APDS9301_getCh1(&data);
    assert_int_equal(ret, 0);
}

static void lightSensor_GetIDregTest(void **state)
{
    uint8_t data;
    int ret = APDS9301_readIDreg(&data);
    assert_int_equal(ret, 0);
}

static void lightSensor_GetCTRLTest(void **state)
{
    uint8_t data;
    int ret = APDS9301_readCTRLreg(&data);
    assert_int_equal(ret, 0);
}

static void lightSensor_setgainTest(void **state)
{
    int ret = APDS9301_setTiming_gain(0);
    ret |= APDS9301_setTiming_gain(1);
    assert_int_equal(ret, 0);
}

static void lightSensor_setTimingTest(void **state)
{
    int ret = APDS9301_setTiming_integ(0);
    ret |= APDS9301_setTiming_gain(1);
    assert_int_equal(ret, 0);
}

static void lightSensor_interruptCTRLTest(void **state)
{
    int ret = APDS9301_interruptCTRLreg(0);
    ret |= APDS9301_interruptCTRLreg(1);
    assert_int_equal(ret, 0);
}

static void lightSensor_setTHlowlowTest(void **state)
{
    int ret = APDS9301_writeTHRESH_lowlow(1);
    assert_int_equal(ret, 0);
}

static void lightSensor_setTHhilowLTest(void **state)
{
    int ret = APDS9301_writeTHRESH_highlow(1);
    assert_int_equal(ret, 0);
}

static void lightSensor_getTHlowlowTest(void **state)
{
    uint8_t th_lowlow;
    int ret = APDS9301_readTHRESH_lowlow(&th_lowlow);
    assert_int_equal(ret, 0);
}

static void lightSensor_getTHlowhiTest(void **state)
{
    uint8_t th_lowhi;
    int ret = APDS9301_readTHRESH_lowhigh(&th_lowhi);
    assert_int_equal(ret, 0);
}

static void lightSensor_getTHhighlowTest(void **state)
{
    uint8_t th_hilow;
    int ret = APDS9301_readTHRESH_highlow(&th_hilow);
    assert_int_equal(ret, 0);
}

static void lightSensor_getTHhihiTest(void **state)
{
    uint8_t th_hihi;
    int ret = APDS9301_readTHRESH_lowlow(&th_hihi);
    assert_int_equal(ret, 0);
}

int main()
{
    const struct CMUnitTest temptests[] = {	
	
    cmocka_unit_test(lightSensor_InitTest),
    cmocka_unit_test(lightSensor_GetLightTest),
    cmocka_unit_test(lightSensor_GetCH0Test),
    cmocka_unit_test(lightSensor_GetCH1Test),
    cmocka_unit_test(lightSensor_GetIDregTest),
    cmocka_unit_test(lightSensor_GetCTRLTest),
    cmocka_unit_test(lightSensor_setgainTest),
    cmocka_unit_test(lightSensor_setTimingTest),
    cmocka_unit_test(lightSensor_interruptCTRLTest),
    cmocka_unit_test(lightSensor_setTHlowlowTest),
    cmocka_unit_test(lightSensor_setTHhilowLTest),
    cmocka_unit_test(lightSensor_getTHlowlowTest),
    cmocka_unit_test(lightSensor_getTHlowhiTest),
    cmocka_unit_test(lightSensor_getTHhighlowTest),
    cmocka_unit_test(lightSensor_getTHhihiTest),

	};

	return cmocka_run_group_tests(temptests, NULL, NULL);
    printf("testing......\n");
    return 0;

}