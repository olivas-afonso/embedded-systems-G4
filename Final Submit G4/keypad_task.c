/*
 * keypad_task.c
 *
 *  Created on: 03/01/2024
 *      Author: olivas
 */


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
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

#include "keypad_task.h"

#include "structs.h"

#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "keypad/keypad.h"


//*****************************************************************************
//
// The stack size for the Keypad task.
//
//*****************************************************************************
#define KEYPADTASKSTACKSIZE        128         // Stack size in words

//*******************************************************************************************
//
// This task reads the Keypad information and writes it into the Menu_Control_Queue Task
//
//*******************************************************************************************


extern xSemaphoreHandle semaphore_keypad;


static void
Keypad_Task(void *pvParameters)
{

    char key;

   while(1)
   {

       if(xSemaphoreTake(semaphore_keypad, portMAX_DELAY)!=pdPASS) // wait for keypad semaphore (after debounce)
       {
           while(1);
       }


       key=keyboard(); //save keypad that was pressed as char



       if(key != ' ')
       {
           xQueueSendToFront(Menu_Control_Queue, &key, portMAX_DELAY);// send to Menu_task FRONT
       }







   }


}

//*****************************************************************************
//
// Initializes the Keypad task.
//
//*****************************************************************************
uint32_t
Keypad_TaskInit(void)
{
    vSemaphoreCreateBinary(semaphore_keypad);

    keypad_init();

    if(xTaskCreate(Keypad_Task, (const portCHAR *)"KEYPAD", KEYPADTASKSTACKSIZE, NULL, tskIDLE_PRIORITY + PRIORITY_KEYPAD_TASK, NULL)!= pdTRUE)
    {
        //smth bad
        return 1;
    }

    return 0;

}
