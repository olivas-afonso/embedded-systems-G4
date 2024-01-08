
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
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
extern bool stop_temp;
extern bool stop_keyboard;
//*******************************************************************************************
//
// This task reads the information from Lcd_Write_Queue and Displays the correct information
//
//*******************************************************************************************
static void
LCD_Task(void *pvParameters)
{

    lcd_packet data_in;

    int position3;
    char temperature[10];
    int i;





    while(1)
    {
        if(xQueueReceive (Lcd_Write_Queue, &data_in, portMAX_DELAY)!=pdPASS)//wait for print info
        {
            while(1); // waiting
        }
        stop_keyboard=true;
        switch(data_in.identifier)
        {
            case MENU_TMP: // print temperature

                ftoa(data_in.data, temperature, 2);

                Lcd_Clear();
                vTaskDelay(10 / portTICK_RATE_MS);
                Lcd_Write_String(temperature);
                vTaskDelay(10 / portTICK_RATE_MS);

                Lcd_Write_Char((char)223);
                vTaskDelay(10 / portTICK_RATE_MS);

                Lcd_Write_Char('C');
                vTaskDelay(10 / portTICK_RATE_MS);
                stop_keyboard=false;
                break;

            case MENU_KEY:

                Lcd_Clear();
                vTaskDelay(10 / portTICK_RATE_MS);
                Lcd_Write_Char(data_in.key);
                vTaskDelay(10 / portTICK_RATE_MS);
                stop_keyboard=false;
                break;

            case MENU_PACKET_FULL:// print Packet Full

                Lcd_Clear();
                vTaskDelay(10 / portTICK_RATE_MS);
                Lcd_Write_String(data_in.msg);
                if(position3 < 4)break;
                for( i=0; i<23; i++)
                {
                    Lcd_Shift_Left();
                    vTaskDelay(333/portTICK_RATE_MS);
                }
                stop_temp=false;
                stop_keyboard=false;
                break;

            case MENU_PACKET_EMPTY:// print Packet Empty
                Lcd_Clear();
                vTaskDelay(10 / portTICK_RATE_MS);
                Lcd_Write_String("Position Empty");
                vTaskDelay(1000/portTICK_RATE_MS);
                stop_temp=false;
                stop_keyboard=false;
                break;

            case MENU_PACKET_NOISE:// print Packet noise
                Lcd_Clear();
                vTaskDelay(10/ portTICK_RATE_MS);
                Lcd_Write_String("RSSI ");
                vTaskDelay(10/ portTICK_RATE_MS);
                Lcd_Write_String(data_in.msg);
                vTaskDelay(10 / portTICK_RATE_MS);
                Lcd_Write_String(" SNR ");
                vTaskDelay(10 / portTICK_RATE_MS);
                Lcd_Write_String(data_in.SNR);
                vTaskDelay(10/portTICK_RATE_MS);
                stop_temp=false;
                stop_keyboard=false;

                break;

            case MENU_PACKET_TIME:// print packet save time
                Lcd_Clear();
                vTaskDelay(10/portTICK_RATE_MS);
                Lcd_Write_String("Saved at: ");
                vTaskDelay(10/portTICK_RATE_MS);
                if(getHours(data_in.cont) < 10) Lcd_Write_Integer(0);
                Lcd_Write_Integer(getHours(data_in.cont));
                Lcd_Write_Char(':');
                if(getMinutes(data_in.cont) < 10) Lcd_Write_Integer(0);
                Lcd_Write_Integer(getMinutes(data_in.cont));
                Lcd_Write_Char(':');
                if(getSeconds(data_in.cont) < 10) Lcd_Write_Integer(0);
                Lcd_Write_Integer(getSeconds(data_in.cont));
                vTaskDelay(2000/portTICK_RATE_MS);
                stop_temp=false;
                stop_keyboard=false;
                break;


            case MENU_PACKET_COUNT:// print Packet count
                Lcd_Clear();
                vTaskDelay(10 / portTICK_RATE_MS);
                Lcd_Write_String("Packet Count: ");
                vTaskDelay(10 / portTICK_RATE_MS);
                Lcd_Write_Integer(data_in.cont);
                vTaskDelay(2000 / portTICK_RATE_MS);
                stop_keyboard=false;
                break;

            case MENU_CURRENT_TIME:// print current time

                Lcd_Clear();
                vTaskDelay(10/portTICK_RATE_MS);
                Lcd_Write_String(data_in.msg);
                vTaskDelay(10 / portTICK_RATE_MS);
                Lcd_Write_Char(' ');
                if(getHours(ROM_HibernateRTCGet()) < 10) Lcd_Write_Integer(0);
                Lcd_Write_Integer(getHours(ROM_HibernateRTCGet()));
                Lcd_Write_Char(':');
                if(getMinutes(ROM_HibernateRTCGet()) < 10) Lcd_Write_Integer(0);
                Lcd_Write_Integer(getMinutes(ROM_HibernateRTCGet()));
                Lcd_Write_Char(':');
                if(getSeconds(ROM_HibernateRTCGet()) < 10) Lcd_Write_Integer(0);
                Lcd_Write_Integer(getSeconds(ROM_HibernateRTCGet()));
                vTaskDelay(2000/portTICK_RATE_MS);
                stop_keyboard=false;
                break;

            }


        vTaskDelay( LCD_Delay/ portTICK_RATE_MS);
    }

}

//*****************************************************************************
//
// Initializes the LCD task.
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



    return 0;
}
