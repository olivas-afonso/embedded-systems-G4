/*
 * uart_task.c
 *
 *  Created on: 22/12/2023
 *      Author: carlo
 */

#include "uart_task.h"

#include "LCD/LCD.h"

#include "structs.h"

xSemaphoreHandle semaphore_UART;

#define UARTSTACKSIZE 100
#define msg_size 70


void UARTIntHandler(void)
{
    uint32_t ui32Status;

    //
    // Get the interrrupt status.
    //

    ui32Status =  UARTIntStatus(UART1_BASE, false);
    //
    // Clear the asserted interrupts.
    //
     UARTIntClear(UART1_BASE, ui32Status);

     static portBASE_TYPE xHigherPriorityTaskWoken;

             xHigherPriorityTaskWoken = pdFALSE;
                    /* 'Give' the semaphore to unblock the task. */
                    xSemaphoreGiveFromISR( semaphore_UART, &xHigherPriorityTaskWoken );
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

void UART_Init(void)
{

    //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    //
    // Set GPIO B0 and B1 as UART pins.
    //
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);

    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Configure the UART for 9600, 8-N-1 operation.
    //
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));
    IntMasterEnable();
    IntEnable(INT_UART1);
    IntPrioritySet(INT_UART1, 0);
    UARTIntRegister(UART1_BASE, UARTIntHandler);
    UARTIntEnable(UART1_BASE, UART_INT_RX);

}

int32_t UART_receive(void){

    int32_t uart_received;
    uart_received=UARTCharGetNonBlocking(UART1_BASE);
    return uart_received;


}

static void
UartTaskReceive(void *pvParameters)
{
    uint8_t i = 0;
    char receivedChar;
    char full_msg[msg_size];
    int char_init = 0;


        while(1)
        {
            if(xSemaphoreTake(semaphore_UART, portMAX_DELAY)!=pdPASS) // waits for interrupt semaphore
            {
                while(1);
            }


            while (i < msg_size - 1)
            {

                while (!UARTCharsAvail(UART1_BASE)) // waits for available Char
                    vTaskDelay(pdMS_TO_TICKS(1));

                receivedChar = UARTCharGet(UART1_BASE);//Read

                if (receivedChar == 'P') // checks for first char
                {
                    char_init = 1;
                }

                if ((receivedChar == '\n' || receivedChar == '\r' ) && char_init != 0)//Checks for end of transmit Char and ignores empty msg
                {
                    char_init = 0;
                    full_msg[i] = '\0';


                    xQueueSendToBack(UART_Buffer_Queue, &full_msg, portMAX_DELAY);

                    i = 0;
                    break;
                }

                if (char_init == 1)
                {
                    full_msg[i++] = receivedChar;

                }
            }

            vTaskDelay(pdMS_TO_TICKS(10));
        }
}

uint32_t
UartTaskReceiveInit(void)
{

    // UART
    UART_Init();






    //
    // Create the Uart task.
    //
    if(xTaskCreate(UartTaskReceive, (const portCHAR *)"Uart receive", UARTSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_UART_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    //
    // Success.
    //
    vSemaphoreCreateBinary(semaphore_UART);
    return(0);
}
