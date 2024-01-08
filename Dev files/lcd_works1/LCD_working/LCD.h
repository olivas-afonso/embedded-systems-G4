/*
 * LCD.h
 *
 *  Created on: 07/12/2023
 *      Author: olivas
 */

#ifndef LCD_H_
#define LCD_H_




/*
 * File:   LCD.h
 * Author: JP
 * Comments:
 * Revision history:
 */

/* Program Description: This program header provides routines
for controlling
* a STD HITACHI HD44780 and compatible LCDs
*
* Hardware Description:
*
* RS ---> RB0
* R/W ---> GND
* EN ---> Rb1
* D4 ---> RB5
* D5 ---> RB2
* D6 ---> RB4
* D7 ---> RB3
*
*/

/**************************************************************
*Includes and defines
**************************************************************/
// STD XC8 include

//#include "mcc_generated_files/mcc.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "driverlib/gpio.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"

// function prototypes
void Lcd_Port(char a);
void Lcd_Cmd(char a);
void Lcd_Clear(void);
void Lcd_Set_Cursor(char a, char b);
void Lcd_Init(void);
void Lcd_Write_Char(char a);
void Lcd_Write_String(const char *a);
void Lcd_Shift_Right(void);
void Lcd_Shift_Left(void);
void Lcd_Write_Integer(int v);
void Lcd_Write_Float(float f);


#endif /* LCD_H_ */
