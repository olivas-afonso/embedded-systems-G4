/*
 * UART_task.c
 *
 *  Created on: 20/12/2023
 *      Author: olivas
 */

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "inc/hw_i2c.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#include "LCD.h"






void
UARTIntHandler(void)
{
    uint32_t ui32Status;
    char* aux;

    //
    // Get the interrrupt status.
    //
    ui32Status = ROM_UARTIntStatus(UART1_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    ROM_UARTIntClear(UART1_BASE, ui32Status);



    while(ROM_UARTCharsAvail(UART1_BASE))
    {

        strcpy(aux, (char *)ROM_UARTCharGetNonBlocking(UART1_BASE));
        Lcd_Clear();
        SysCtlDelay(2000000);
        Lcd_Write_String(aux);



    }

}

void UART_Config(void)
{
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    ROM_IntMasterEnable();

    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    ROM_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    ROM_UARTConfigSetExpClk(UART1_BASE, ROM_SysCtlClockGet(), 9600,
                                (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                                 UART_CONFIG_PAR_NONE));

    ROM_UARTEnable(INT_UART1);
    ROM_UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);
}

const char* UART_Read(void)
{

    char *msg_received;


    while(ROM_UARTCharsAvail(UART1_BASE))
        {
            strcpy(msg_received, (char *)ROM_UARTCharGetNonBlocking(UART1_BASE));
            //msg_received = (char *)ROM_UARTCharGetNonBlocking(UART1_BASE);
        }

    return msg_received;
}
