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
#include <bbgled.h>
#include <unistd.h>
#include <string.h>

int BBGinit(void)
{
    FILE *export_file = NULL; //declare pointers
    FILE *IO_direction = NULL;
    FILE *IO_value = NULL;

    char low[] = "0"; // value low led off
    char high[] = "1"; //value high led on
    char dir[] = "out"; //direction
    char ledok[] = "44"; //p8_12 led green
    char lederr[] = "26";//p8_14 led red
    char ledval[2];
    //turn off the red led if set
    IO_value = fopen ("/sys/class/gpio/gpio26/value", "w+");
    if(IO_value == NULL)
    {
        return -1;
    }
    //fread(ledval,sizeof(ledval),1,IO_value);
    fwrite (low, 1, sizeof(low), IO_value);
    /*if(strcmp(ledval,"1") == 0) 
    {
        fwrite (low, 1, sizeof(low), IO_value); //set the pin to LOW   
    }*/
    fclose (IO_value);
    //this part here exports gpio44 green led to remain on unless there is an error
    export_file = fopen ("/sys/class/gpio/export", "w");
    if(export_file == NULL)
    {
        return -1;
    }
    fwrite (ledok, 1, sizeof(ledok), export_file);
    fclose (export_file);
    //this part here sets the direction of the pin
    IO_direction = fopen("/sys/class/gpio/gpio44/direction", "w");
    if(IO_direction == NULL)
    {
        return -1;
    }
    fwrite(dir, 1, sizeof(dir), IO_direction); //set the pin to HIGH
    fclose(IO_direction);
    // green led on, writing value as 1
    IO_value = fopen ("/sys/class/gpio/gpio44/value", "w");
    if(IO_value == NULL)
    {
        return -1;
    }
    fwrite (high, 1, sizeof(high), IO_value); //set the pin to HIGH
    fclose (IO_value);
    
    return 0;
}
int BBGled_error(void)
{
    FILE *export_file = NULL; //declare pointers
    FILE *IO_direction = NULL;
    FILE *IO_value = NULL;

    char low[] = "0";
    char high[] = "1";
    char dir[] = "out";
    char ledok[] = "44"; //p8_12 led green
    char lederr[] = "26";//p8_14 led red
    //this part here exports gpio44 green led to remain on unless there is an error
    export_file = fopen ("/sys/class/gpio/export", "w");
    if(export_file == NULL)
    {
        return -1;
    }
    fwrite (lederr, 1, sizeof(lederr), export_file);
    fclose (export_file);
    //this part here sets the direction of the pin
    IO_direction = fopen("/sys/class/gpio/gpio26/direction", "w");
    if(IO_direction == NULL)
    {
        return -1;
    }
    fwrite(dir, 1, sizeof(dir), IO_direction); //set the pin to HIGH
    fclose(IO_direction);
    // turning off led green, gpio 44
    IO_value = fopen ("/sys/class/gpio/gpio44/value", "w");
    if(IO_value == NULL)
    {
        return -1;
    }
    fwrite (low, 1, sizeof(low), IO_value); //set the pin to LOW
    fclose (IO_value);
    //turning on red led gpio 26, writing value 1
    IO_value = fopen ("/sys/class/gpio/gpio26/value", "w");
    if(IO_value == NULL)
    {
        return -1;
    }
    fwrite (high, 1, sizeof(high), IO_value); //set the pin to HIGH
    fclose (IO_value);

    return 0;
}
