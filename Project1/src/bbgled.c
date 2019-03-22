/**
 * @file bbgled.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-03-20
 * @Reference https://www.teachmemicro.com/beaglebone-black-blink-led-using-c/ 
 * @copyright Copyright (c) 2019
 * 
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "logger.h"
#include "bbgled.h"
#include "includes.h"

int redLed_OnOff(bool option)
{
    char low[] = "0"; // value low led off
    char high[] = "1"; //value high led on
    char dir[] = "out"; //direction 
    char lederr[] = "26";//p8_14 led red 
    //exports gpio 26 to be confugued for red led
    FILE *export_file = fopen ("/sys/class/gpio/export", "w");
    if(export_file == NULL)
    {
        return -1;
    }
    fwrite (lederr, 1, sizeof(lederr), export_file);
    fclose (export_file); 
    //this part here sets the direction of the pin
    FILE *IO_direction = fopen("/sys/class/gpio/gpio26/direction", "w");
    if(IO_direction == NULL)
    {
        return -1;
    }
    fwrite(dir, 1, sizeof(dir), IO_direction); //set the pin to HIGH
    fclose(IO_direction);
    //turning on red led gpio 26, writing value 1
    FILE *IO_value = fopen ("/sys/class/gpio/gpio26/value", "w");
    if(IO_value == NULL)
    {
        return -1;
    }
    if(option == 1)
    {
        fwrite (high, 1, sizeof(high), IO_value); //set the pin to HIGH
    }
    else{
        fwrite (low, 1, sizeof(low), IO_value); //set the pin to HIGH
    }
    fclose (IO_value);

    return 0;
}

int greenLed_OnOff(bool option)
{
    char low[] = "0"; // value low led off
    char high[] = "1"; //value high led on
    char dir[] = "out"; //direction 
    char ledok[] = "44";//p8_12 led green
    //exports gpio 44 to be confugued for red led
    FILE *export_file = fopen ("/sys/class/gpio/export", "w");
    if(export_file == NULL)
    {
        return -1;
    }
    fwrite (ledok, 1, sizeof(ledok), export_file);
    fclose (export_file); 
    //this part here sets the direction of the pin
    FILE *IO_direction = fopen("/sys/class/gpio/gpio44/direction", "w");
    if(IO_direction == NULL)
    {
        return -1;
    }
    fwrite(dir, 1, sizeof(dir), IO_direction); //set the pin to HIGH
    fclose(IO_direction);
    //turning on red led gpio 26, writing value 1
    FILE *IO_value = fopen ("/sys/class/gpio/gpio44/value", "w");
    if(IO_value == NULL)
    {
        return -1;
    }
    if(option == 1)
    {
        fwrite (high, 1, sizeof(high), IO_value); //set the pin to HIGH
    }
    else{
        fwrite (low, 1, sizeof(low), IO_value); //set the pin to HIGH
    }
    fclose (IO_value);

    return 0;
}

