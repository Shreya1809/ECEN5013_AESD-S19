/**
 * @file test.c
 * @author Shreya Chakraborty
 * @brief Cmoka Unit testing for TMP102 temperature sensor regster functions
 * @version 0.1
 * @date 2019-03-31
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "myI2C.h"
#include "tempSensor.h"
#include <unistd.h>
#include "includes.h"
#include "cmocka.h"
#include "temp.h"


i2c_struct_t i2c_handler;

static void tempSensor_InitTest(void **state)
{
    int ret = I2C_init(&i2c_handler);
    assert_int_equal(ret, 0);
}

static void tempSensor_GetTempTest(void **state)
{
    float temp_val;
    int ret = TMP102_getTemperature(&temp_val);
    assert_int_equal(ret, 0);
}

static void tempSensor_THighTest(void **state)
{
    float thighIn = 30.0, thighOut;
    int ret = TMP102_setThigh(thighIn);
    assert_int_equal(ret, 0);
    ret = TMP102_getThigh(&thighOut);
    assert_int_equal(ret, 0);
    assert_true(thighIn == thighOut);
}

static void tempSensor_TLowTest(void **state)
{
    float thighIn = 30.0, thighOut;
    int ret = TMP102_setTlow(thighIn);
    assert_int_equal(ret, 0);
    ret = TMP102_getTlow(&thighOut);
    assert_int_equal(ret, 0);
    assert_true(thighIn == thighOut);
}

static void tempSensor_WritePtrReg(void **state)
{
    int ret = TMP102_writePTRreg(TMP102_CONF_TM);
    assert_int_equal(ret, 0);
}

static void tempSensor_ShutDownTest(void **state)
{
    int ret = TMP102_setShutdownMode(0);
    ret |= TMP102_setShutdownMode(1);
    assert_int_equal(ret, 0);
}

static void tempSensor_SetResolutionTest(void **state)
{
    int ret = TMP102_setResolution(0);
    ret |= TMP102_setResolution(1);
    ret |= TMP102_setResolution(2);
    ret |= TMP102_setResolution(3);
    assert_int_equal(ret, 0);
}

static void tempSensor_SetFaultBitTest(void **state)
{
    int ret = TMP102_setFaultbits(0);
    ret |= TMP102_setFaultbits(1);
    ret |= TMP102_setFaultbits(2);
    ret |= TMP102_setFaultbits(3);
    assert_int_equal(ret, 0);
}

static void tempSensor_SetEMTest(void **state)
{
    int ret = TMP102_setEM(0);
    ret |= TMP102_setEM(1);
    assert_int_equal(ret, 0);
}

static void tempSensor_SetCRTest(void **state)
{
    int ret = TMP102_setCR(0);
    ret |= TMP102_setCR(1);
    ret |= TMP102_setCR(2);
    ret |= TMP102_setCR(3);
    assert_int_equal(ret, 0);
}

static void tempSensor_getALTest(void **state)
{
    uint8_t bit;
    int ret = TMP102_readAL(&bit);
    assert_int_equal(ret, 0);
}

static void tempSensor_tempConversionTest(void **state)
{
    float temp_val;
    float ret1 = getTemperature(CELCIUS);
    float ret2 = getTemperature(FARENHEIT);
    float ret3 = getTemperature(KELVIN);
    ret2 = (ret2 - 32) *(5/9);
    ret3 = (ret3 - 273.149994);
    assert_true(ret1 == ret2);
}

int main()
{
    const struct CMUnitTest temptests[] = {	
	
    cmocka_unit_test(tempSensor_InitTest),
    cmocka_unit_test(tempSensor_GetTempTest),
    cmocka_unit_test(tempSensor_THighTest),
    cmocka_unit_test(tempSensor_TLowTest),
    cmocka_unit_test(tempSensor_WritePtrReg),
    cmocka_unit_test(tempSensor_ShutDownTest),
    cmocka_unit_test(tempSensor_SetResolutionTest),
    cmocka_unit_test(tempSensor_SetFaultBitTest),
    cmocka_unit_test(tempSensor_SetEMTest),
    cmocka_unit_test(tempSensor_SetCRTest),
    cmocka_unit_test(tempSensor_tempConversionTest),

	};

	return cmocka_run_group_tests(temptests, NULL, NULL);
    printf("testing......\n");
    return 0;

}

