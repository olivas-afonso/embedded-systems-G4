//*****************************************************************************
//
// switch_task.c - A simple switch task to process the buttons.
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

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "drivers/buttons.h"
#include "utils/uartstdio.h"

#include "switch_task.h"
#include "led_task.h"

#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#include "LCD/LCD.h"
#include "structs.h"
#include "aux_functions.h"

//*****************************************************************************
//
// The stack size for the display task.
//
//*****************************************************************************
#define LCDTASKSTACKSIZE        128        // Stack size in words


#define LCD_ITEM_SIZE   sizeof(uint8_t)
#define LCD_QUEUE_SIZE 10
#define LCD_Delay 150

//*****************************************************************************
//
// This task reads the buttons' state and passes this information to LEDTask.
//
//*****************************************************************************
static void
LCD_Task(void *pvParameters)
{
    portTickType xLastWake;
    lcd_packet data_in;
    char temperature[10];

    xLastWake = xTaskGetTickCount();

    Lcd_Clear();

    while(1)
    {
        if(xQueueReceive (Lcd_Write_Queue, &data_in, portMAX_DELAY)!=pdPASS)
        {
            while(1); // waiting vazio
        }

        if(data_in.identifier == MENU_TMP);  // TEMPERATURE DISPLAY
        {

            ftoa(data_in.data, temperature, 2);

            Lcd_Clear();
            vTaskDelay(50 / portTICK_RATE_MS);
            Lcd_Write_String(temperature);
            //Lcd_Write_Char(0xDF);
            //Lcd_Write_Char('C');


        } // resto dos menus

        vTaskDelay( LCD_Delay/ portTICK_RATE_MS);
    }

}

//*****************************************************************************
//
// Initializes the switch task.
//
//*****************************************************************************
uint32_t
LCD_TaskInit(void)
{
    Lcd_Clear();
    Lcd_Init();
    Lcd_Clear();

    if(xTaskCreate(LCD_Task, (const portCHAR *)"LCD", LCDTASKSTACKSIZE, NULL, tskIDLE_PRIORITY + PRIORITY_LCD_TASK, NULL) != pdTRUE)
    {
        //smth bad
        return 1;
    }


    Lcd_Write_String("OLA");


    //vTaskDelay( LCD_Delay/ portTICK_RATE_MS);

    return 0;
}
