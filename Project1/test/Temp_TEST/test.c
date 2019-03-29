#include "myI2C.h"
#include "tempSensor.h"
#include <unistd.h>
#include "cmocka.h"

i2c_struct_t i2c_handler;

static void tempSensor_Test(void **state)
{
    int ret = I2C_init(&i2c_handler);
    assert_int_equal(ret, 0);
}

int main()
{
    const struct CMUnitTest temptests[] = {	
	
    cmocka_unit_test(tempSensor_Test)
    
	};

	return cmocka_run_group_tests(temptests, NULL, NULL);

}