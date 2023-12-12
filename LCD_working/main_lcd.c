

/**
 * main.c
 */
/*
#define D4 GPIO_PIN_0
#define D5 GPIO_PIN_1
#define D6 GPIO_PIN_2
#define D7 GPIO_PIN_3

#define RS GPIO_PIN_6
#define EN GPIO_PIN_7
*/

#include "LCD.h"

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"

#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"


int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);


    char test[] = "IM ALIVE";

    Lcd_Clear();
    Lcd_Init();

    while(1)
    {

        Lcd_Write_String(test);
        while(1);

    }
    return 0;
}
