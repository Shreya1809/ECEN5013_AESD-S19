/*
 * lcd.h
 *
 *  Created on: Apr 16, 2019
 *      Author: Shreya
 */

#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

void LCD_init(void);

void LCD_printString(int r, int c, const char *data);

void LCD_update();

void LCD_clear(void);

#endif /* LCD_DRIVER_H_ */
