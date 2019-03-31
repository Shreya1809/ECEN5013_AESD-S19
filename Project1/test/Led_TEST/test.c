/**
 * @file test.c
 * @author Shreya Chakraborty
 * @brief Test for gpio pin on BeagleBone Green
 * @version 0.1
 * @date 2019-03-31
 * 
 * @copyright Copyright (c) 2019
 * 
 */
/* standard headers */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* mraa header */
#include "mraa/gpio.h"

/* gpio declaration */
#define GPIO_PIN_19 19

int main(void)
{
    mraa_result_t status = MRAA_SUCCESS;
    mraa_gpio_context gpio_1;
    /* initialize mraa for the platform (not needed most of the times) */
    mraa_init();

    //! [Interesting]
    /* initialize GPIO pin */
    gpio_1 = mraa_gpio_init(GPIO_PIN_19);
    if (gpio_1 == NULL) {
        printf("Failed to initialize GPIO %d\n", GPIO_PIN_19);
        mraa_deinit();
        return EXIT_FAILURE;
    }
    /* set GPIO to input */
    status = mraa_gpio_dir(gpio_1, MRAA_GPIO_IN);
    if (status != MRAA_SUCCESS) {
        //goto err_exit;
        printf("Failed to set input direction GPIO %d\n", GPIO_PIN_19);
    }

    while(1)
    {
        status = mraa_gpio_read(gpio_1);
        if (status == -1) {
            printf("Failed to read GPIO %d\n", GPIO_PIN_19);
            continue;
            //goto err_exit;
        }
        printf("status is %d\n",status);
    
    }
}