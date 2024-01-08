//*****************************************************************************
//
// project0.c - Example to demonstrate minimal TivaWare setup
//
// Copyright (c) 2012-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include "ccs/LCD.h"
#include <stdint.h>
#include <stdbool.h>
//#include "inc/hw_ints.h"   //hw ints and lcd library have a collision and dont work together
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"  //
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


//*****************************************************************************
//
// Define pin to LED color mapping.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Project Zero (project0)</h1>
//!
//! This example demonstrates the use of TivaWare to setup the clocks and
//! toggle GPIO pins to make the LED's blink. This is a good place to start
//! understanding your launchpad and the tools that can be used to program it.
//
//*****************************************************************************

#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

int var;
uint32_t g_ui32SysClock;
char charBuffer[128];
uint8_t *charHolder;
char test[] = "IM ALIVE";
char test2[] = "G";

char keyboard (void)

{

    char key[4][4] =
    {
        {'1', '2', '3', 'F'},
        {'4', '5', '6', 'E'},
        {'7', '8', '9', 'D'},
        {'A', '0', 'B', 'C'}
    };                           //define the keyboard layout
    int j,i;
    char letter;
    char row[] = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6};    //define the pins for the rows
    char line[] = {GPIO_PIN_6, GPIO_PIN_3, GPIO_PIN_2, GPIO_PIN_1};     //define the pins for the lines
    letter = ' ';
    for (i = 0; i < 4; i++)
    {
        GPIOPinWrite(GPIO_PORTB_BASE, row[i], row[i]); //set pin in the rows to high and check all combinations
        for (j = 0; j < 4; j++) 
        {
            if (GPIOPinRead(GPIO_PORTD_BASE , line[j]) == line[j])  //check pins in the line, if the pin is high, return value of the key
                {
                letter = key[i][j];
                }
        }    
        GPIOPinWrite(GPIO_PORTB_BASE, row[i], ~row[i]); //set pin to low

    }
    return letter;
}


void IntGPIOd(void)
{
    GPIOIntDisable(GPIO_PORTD_BASE, GPIO_INT_PIN_6 | GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_3);
    GPIOIntClear(GPIO_PORTD_BASE, GPIO_INT_PIN_6 | GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_3 );

    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6, ~(GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6)); //set B pins to low
    SysCtlDelay(5);


    GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, GREEN_LED); // Turn on the LED

    test2[0] = keyboard();
    Lcd_Write_String(test2);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6, (GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6)); //set B pins to low
    SysCtlDelay(10);

    GPIOIntEnable(GPIO_PORTD_BASE, GPIO_INT_PIN_6 | GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_3);

}

//*****************************************************************************
//
// Main 'C' Language entry point.  Toggle an LED using TivaWare.
//
//screen pins :input  X1:PD6   X2:PD3  X3:PD2   X4:PD1 output  Y1:PB3   Y2:PB4   Y3:PB5   Y4:PB6
//
//*****************************************************************************
int
main(void)
{

       SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);  //set the clock to 50 mhz
       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
       while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))  // Enable and wait for the port to be ready for access
       {
       }

       GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);       //sets pins as outputs for the screen todo interleaving for bit setting

       GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);    //sets pins as outputs for the screen

       GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);    //sets pins as outputs for the keyboard
       GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 ); //set B pins to high

       GPIOPinTypeGPIOInput(GPIO_PORTD_BASE , GPIO_PIN_6 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);   //sets pins D6, D3, D2, D1 as inputs for the keyboard
       GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED);   // Configure the GPIO port for the LED operation.

       GPIOIntRegister(GPIO_PORTD_BASE, IntGPIOd);
       GPIOIntEnable(GPIO_PORTD_BASE, GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_3 | GPIO_INT_PIN_6 );      /*    Configures the Red LED, GPIO_PIN_1, to output  */
       GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_RISING_EDGE); //set the interrupt on pins 1 2 3 6 for rising edge

       IntMasterEnable();





       Lcd_Init();
       SysCtlDelay(2000000);
       Lcd_Clear();
       SysCtlDelay(2000000);
       Lcd_Write_String(test);

    SysCtlDelay(200000);
    GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, RED_LED); // Turn on the LED



    while(1)
    {


/*
        //test2[0] = keyboard();
        //Lcd_Write_String(test2);
        //GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, BLUE_LED); // Turn on the LED

        SysCtlDelay(10000000);
        Lcd_Clear();
        SysCtlDelay(4000000);
*/
    }
}


