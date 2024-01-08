//*****************************************************************************
//
// led_task.c - A simple flashing LED task.
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
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "drivers/rgb.h"
#include "drivers/buttons.h"
#include "utils/uartstdio.h"

#include "temp_task.h"

#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#include "i2c/i2c.h"
#include "LCD/LCD.h"
#include "structs.h"
#include "aux_functions.h"


//*****************************************************************************
//
// The stack size for the TMP task.
//
//*****************************************************************************
#define TMPTASKSTACKSIZE        128         // Stack size in words


#define TMP_ITEM_SIZE           sizeof(uint8_t)
#define TMP_QUEUE_SIZE          5

#define TMP_DELAY        1500 // datasheet typical conversion time is 320ms


#define TMP_ADDR 0x48
#define REG_TEMP 0x00
#define REG_CONFIG 0x01




void I2C_Config(void)
{
    I2CMasterSlaveAddrSet(I2C2_BASE, TMP_ADDR, false);

    I2CMasterDataPut(I2C2_BASE, 0x01); // config reg

    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    while(I2CMasterBusy(I2C2_BASE));

    I2CMasterDataPut(I2C2_BASE, 0x60); //12-Bit resolution
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    while(I2CMasterBusy(I2C2_BASE));



}


float read_temp(void)
{
    uint16_t data;

    data=I2C_Read();
    data= data >> 4; //12 bit resolution, need to take last 4 zeros of 16bit word
    return data*0.0625;
}






extern bool cold_start_flag;
extern bool stop_temp;


static void
TMP_Task(void *pvParameters)
{

   portTickType xLastWake;
   float temperature;
   char aux[20];

   lcd_packet packet_out;
   packet_out.identifier = MENU_TMP;

   xLastWake = xTaskGetTickCount();



   while(1)
   {
      if(cold_start_flag==false && stop_temp==false) //stops temp from sending messages to lcd
      {

       temperature=read_temp(); // gets temp reading
       packet_out.data=temperature;

       ftoa(temperature, aux, 2); // turns into string for printing

       xQueueSendToBack(Lcd_Write_Queue, &packet_out, portMAX_DELAY);//send to lcd
      }

       vTaskDelayUntil(&xLastWake, TMP_DELAY/ portTICK_RATE_MS);

   }


}

//*****************************************************************************
//
// Initializes the TMP task.
//
//*****************************************************************************
uint32_t
TMP_TaskInit(void)
{


    if(xTaskCreate(TMP_Task, (const portCHAR *)"TMP", TMPTASKSTACKSIZE, NULL, tskIDLE_PRIORITY + PRIORITY_TMP_TASK, NULL)!= pdTRUE)
    {
        //smth bad
        return 1;
    }

    I2C_Setup();
    I2C_Config();



    return 0;

}
