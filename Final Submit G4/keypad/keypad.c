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

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"   //hw ints and lcd library have a collision and dont work together
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

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "keypad.h"

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif
int volatile flag = 0;

//volatile uint8_t check_keyboard=0; //its 1 if there is a pending read to be done of the keyboard

xSemaphoreHandle semaphore_keypad; // semaphore for keypad interrupts

char keyboard (void)   //goes trough all the lines and rows and returns the last key that was high, also sets the interrupts for the keyboard

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
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6, ~(GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6)); //set B pins to low
    SysCtlDelay(5);
    for (i = 0; i < 4; i++)
    {
        GPIOPinWrite(GPIO_PORTB_BASE, row[i], row[i]); //set pin in the rows to high and check all combinations
        SysCtlDelay(5);
        for (j = 0; j < 4; j++)
        {
            if (GPIOPinRead(GPIO_PORTD_BASE , line[j]) == line[j])  //check pins in the line, if the pin is high, return value of the key
                {
                letter = key[i][j];
                }
        }
        GPIOPinWrite(GPIO_PORTB_BASE, row[i], ~row[i]); //set pin to low
    }
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6, (GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6)); //set B pins to high
    SysCtlDelay(5);
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/6);  //sets timer to 1/3 of a second
    TimerEnable(TIMER0_BASE, TIMER_A);
    flag = 1;
    return letter;

}


void IntGPIOd(void)  //Disables GPIO interrupts, clears the flag, and sets the timer to be run, after which the key is read
{
    GPIOIntDisable(GPIO_PORTD_BASE, GPIO_INT_PIN_6 | GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_3);  //disable interrupt

    GPIOIntClear(GPIO_PORTD_BASE, GPIO_INT_PIN_6 | GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_3 );   //clear interrupt
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/200);  //sets timer to 1/20 of a second
    TimerEnable(TIMER0_BASE, TIMER_A);

}


void
Timer0IntHandler(void)    //Timer intterupt, clears the flag and sets check keyboard to 1 to be checked in the main function
{
    if (flag == 0)
    {
        TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

        static portBASE_TYPE xHigherPriorityTaskWoken;

        xHigherPriorityTaskWoken = pdFALSE;
               /* 'Give' the semaphore to unblock the task. */
               xSemaphoreGiveFromISR( semaphore_keypad, &xHigherPriorityTaskWoken );
               if( xHigherPriorityTaskWoken == pdTRUE ) {
               /* Giving the semaphore unblocked a task, and the priority of the unblocked task
               is higher than the currently running task - force a context switch to ensure that
               the interrupt returns directly to the unblocked (higher priority) task.
               NOTE: The actual macro to use (context switch) from an ISR is dependent on the
               port. This is the correct macro for the Open Watcom DOS port. Other ports may
               require different syntax */
               portEND_SWITCHING_ISR (xHigherPriorityTaskWoken);
               }
    }
    else
    {
        TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

        GPIOIntClear(GPIO_PORTD_BASE, GPIO_INT_PIN_6 | GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_3 );  //clears interrupts that are caused by the keyboard function
        GPIOIntEnable(GPIO_PORTD_BASE, GPIO_INT_PIN_6 | GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_3);  //enable interupts on the gpio port d
        flag = 0;
    }
}

//*****************************************************************************
//
// Main 'C' Language entry point.  Toggle an LED using TivaWare.
//
//screen pins :input  X1:PD6   X2:PD3  X3:PD2   X4:PD1 output  Y1:PB3   Y2:PB4   Y3:PB5   Y4:PB6
//
//*****************************************************************************
void keypad_init(void)
{

       //SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);  //set the clock to 50 mhz
       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

       GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);    //sets pins as outputs for the keyboard
       GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 ); //set B pins to high
       GPIOPinTypeGPIOInput(GPIO_PORTD_BASE , GPIO_PIN_6 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);   //sets pins D6, D3, D2, D1 as inputs for the keyboard
       GPIOIntRegister(GPIO_PORTD_BASE, IntGPIOd);  // binds portd interrupts to IntGPIOd functuion
       //IntPrioritySet(GPIO_PORTD_BASE, 1);
       GPIOIntEnable(GPIO_PORTD_BASE, GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_3 | GPIO_INT_PIN_6 );      /*    sets pins 1 2 3 5 of portd for interrupts  */
       GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_RISING_EDGE);    //set the interrupt on pins 1 2 3 6 for rising edge

       SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);    //timer setup
       TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT);  //timer runs only once
       TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/200);  //sets timer to 1/20 of a second
       IntPrioritySet(INT_TIMER0A, 1);

       TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0IntHandler);  // binds timer interrupts to Timer0IntHandler function
       IntEnable(INT_TIMER0A);
       TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

       IntMasterEnable();


}
