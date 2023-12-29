

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

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#include "UART.h"
#include "aux_functions.h"

uint32_t g_ui32SysClock;
char charBuffer[128];
uint8_t *charHolder;


int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);


    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
        // wait till port A is ready.
    while(!ROM_SysCtlPeripheralReady){}

    // setting the pins
    ROM_GPIOPinConfigure(GPIO_PB0_U1RX);
    ROM_GPIOPinConfigure(GPIO_PB1_U1TX);
    ROM_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    // wait till the peripheral is ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART1)){}

    // configuring the UART with baud rate 9600
    ROM_UARTConfigSetExpClk(UART1_BASE,ROM_SysCtlClockGet(),9600,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    ROM_UARTEnable(UART1_BASE);


    //UART_Config();

    char test[] = "IM ALIVE";
    char *aux;

    Lcd_Clear();
    SysCtlDelay(16000*500);
    Lcd_Init();
    SysCtlDelay(16000*500);
    Lcd_Clear();
        SysCtlDelay(16000*500);

    while(1)
    {

        if(UARTCharsAvail(UART1_BASE)){
            strcpy(aux, ftoa());
            Lcd_Clear();
            SysCtlDelay(16000*500);
            Lcd_Write_String(aux);
        }



        //Lcd_Write_String(test);

        SysCtlDelay(20000000);


    }
    return 0;
}
