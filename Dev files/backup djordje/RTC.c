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
#include "driverlib/hibernate.c"
#include "inc/hw_hibernate.h"


#define SEC_MINUTE 60
#define SEC_HOUR 3600

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

void setTimeOfDay(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    uint32_t total = minutes*SEC_MINUTE+hours*SEC_HOUR+seconds;
    HibernateRTCSet(total);
}

uint8_t getHours(uint32_t posixTime)
{
    uint32_t hours = posixTime/SEC_HOUR;
    return hours % 24;
}

uint8_t getMinutes(uint32_t posixTime)
{
uint32_t minutes = posixTime/SEC_MINUTE;
return minutes % 60;
}

uint8_t getSeconds(uint32_t posixTime) {
    return posixTime % 60;
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
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    // RTC Configuration
    HibernateEnableExpClk(SysCtlClockGet());
    HibernateRTCEnable();

    setTimeOfDay(12, 11, 11);   //sets time of day, #todo dinamical


    while(1)
    {
        int Hours = getHours(HibernateRTCGet());
        int Minutes = getMinutes(HibernateRTCGet());
        int Seconds = getSeconds(HibernateRTCGet());
        char test2[] = "00";
        //char* ptr = &test;
        citoa (Hours,&test2[0],10 );


        citoa (Minutes,&test2[0],10 );


        citoa (Seconds,&test2[0],10 );



    }
}

