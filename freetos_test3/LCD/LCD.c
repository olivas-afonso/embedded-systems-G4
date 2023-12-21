
#include "LCD.h"

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"

#define D4 GPIO_PIN_0
#define D5 GPIO_PIN_1
#define D6 GPIO_PIN_2
#define D7 GPIO_PIN_3

#define RS GPIO_PIN_6
#define EN GPIO_PIN_7

void delay_ms(uint16_t time)
{
    SysCtlDelay(16000*time);
}

void delay_us(uint16_t time)
{
    SysCtlDelay(16*time);
}

/**************************************************************
* Function: void Lcd_Port (char a)
*
* Returns: Nothing
*
* Description: LCD Setup Routines
**************************************************************/
void Lcd_Port(char a)
{
if(a & 1)
GPIOPinWrite(GPIO_PORTE_BASE, D4, D4);
else
GPIOPinWrite(GPIO_PORTE_BASE, D4, 0x0);
if(a & 2)
    GPIOPinWrite(GPIO_PORTE_BASE, D5, D5);
else
    GPIOPinWrite(GPIO_PORTE_BASE, D5, 0x0);
if(a & 4)
    GPIOPinWrite(GPIO_PORTE_BASE, D6, D6);
else
    GPIOPinWrite(GPIO_PORTE_BASE, D6, 0x0);
if(a & 8)
    GPIOPinWrite(GPIO_PORTE_BASE, D7, D7);
else
    GPIOPinWrite(GPIO_PORTE_BASE, D7, 0x0);
}
/**************************************************************
* Function: void Lcd_Cmd (char a)
*
* Returns: Nothing
*
* Description: Sets LCD command
**************************************************************/
void Lcd_Cmd(char a)
{
GPIOPinWrite(GPIO_PORTC_BASE, RS, 0x0);
Lcd_Port(a);
GPIOPinWrite(GPIO_PORTC_BASE, EN, EN);
delay_ms(1);
GPIOPinWrite(GPIO_PORTC_BASE, EN, 0x0);
delay_ms(1);// adding delay
}
/**************************************************************
* Function: void Lcd_Clear()
*
* Returns: Nothing
*
* Description: Clears the LCD
**************************************************************/
void Lcd_Clear(void)
{
Lcd_Cmd(0);
Lcd_Cmd(1);
}
/**************************************************************
* Function: void Lcd_Set_Cursor(char a, char b)
*
* Returns: Nothing
*
* Description: Sets the LCD cursor position
**************************************************************/
void Lcd_Set_Cursor(char a, char b)
{
char temp,z,y;
if(a == 1)
{
temp = 0x80 + b - 1;
z = temp>>4;
y = temp & 0x0F;
Lcd_Cmd(z);
Lcd_Cmd(y);
}

else if(a == 2)
{
temp = 0xC0 + b - 1;
z = temp>>4;
y = temp & 0x0F;
Lcd_Cmd(z);
Lcd_Cmd(y);
}
}
/**************************************************************
* Function: void Lcd_Init()
*
* Returns: Nothing
*
* Description: Initializes the LCD
**************************************************************/
void Lcd_Init(void)
{
Lcd_Port(0x00);
delay_ms(40);
Lcd_Cmd(0x03);
delay_ms(10);
Lcd_Cmd(0x03);
delay_ms(10);
Lcd_Cmd(0x03);
/////////////////////////////////////////////////////
Lcd_Cmd(0x02);
Lcd_Cmd(0x02);//Function set 1, 0-4bits
Lcd_Cmd(0x00);// nº linhas  font 5x8 Nº de linhas 1

Lcd_Cmd(0x00);// display on/off
Lcd_Cmd(0x0F);// 1, Display-on, Cursor - 1, Blink -0


Lcd_Cmd(0x00);// entry mode set
Lcd_Cmd(0x06);// increment the address by 1, shift the cursor to right

}
/**************************************************************
* Function: void Lcd_Write_Char (char a)
*
* Returns: Nothing
*
* Description: Writes a character to the LCD
**************************************************************/
void Lcd_Write_Char(char a)
{
char temp,y;
temp = a&0x0F;
y = a&0xF0;
GPIOPinWrite(GPIO_PORTC_BASE, RS, RS);
Lcd_Port(y>>4); //Data transfer
GPIOPinWrite(GPIO_PORTC_BASE, EN, EN);
delay_us(20);
GPIOPinWrite(GPIO_PORTC_BASE, EN, 0x0);
Lcd_Port(temp);
GPIOPinWrite(GPIO_PORTC_BASE, EN, EN);
delay_us(20);
GPIOPinWrite(GPIO_PORTC_BASE, EN, 0x0);
}

/**************************************************************
* Function: void Lcd_Write_String (const char *a)
*
* Returns: Nothing
*
* Description: Writes a string to the LCD
**************************************************************/
void Lcd_Write_String(const char *a)
{
int i;
for(i=0;a[i]!='\0';i++)
Lcd_Write_Char(a[i]);
}
/**************************************************************
* Function: void Lcd_Shift_Right()
*
* Returns: Nothing
*
* Description: Shifts text on the LCD right
**************************************************************/
void Lcd_Shift_Right(void)
{
Lcd_Cmd(0x01);
Lcd_Cmd(0x0C);
}

/**************************************************************
* Function: void Lcd_Shift_Left()
*
* Returns: Nothing
*
* Description: Shifts text on the LCD left
**************************************************************/
void Lcd_Shift_Left(void)
{
Lcd_Cmd(0x01);
Lcd_Cmd(0x08);
}
/**************************************************************
* Function: void Lcd_Write_Integer(int v)
*
* Returns: Nothing
*
* Description: Converts a string to an integer
**************************************************************/
void Lcd_Write_Integer(int v)
{
char buf[8];
Lcd_Write_String(itoa(buf, v, 10));
}

/**************************************************************
* Function: void Lcd_Write_Float(float f)
*
* Returns: Nothing
*
* Description: Converts a string to a float
**************************************************************/
void Lcd_Write_Float(float f)
{
char* buf11;
int status;
buf11 = ftoa(f, &status);
Lcd_Write_String(buf11);
}
