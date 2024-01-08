
#include "led_task.h"
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

#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#include "LCD/LCD.h"
#include "structs.h"
#include "aux_functions.h"

#include <string.h>
#include "switch_task.h"
#include "driverlib/hibernate.c"
#include "inc/hw_hibernate.h"

//*****************************************************************************
//
// The stack size for the buffer task.
//
//*****************************************************************************
#define BUFFERTASKSTACKSIZE        1024        // Stack size in words




#define LCD_Delay 150


//*****************************************************************************
//
// This task reads deals with the buffer reading and saving. Also deals with RTC timer setup
//
//*****************************************************************************

static void
Buff_Task(void *pvParameters)
{
    int buffer_line=0;
    int cont_total=0;
    char  UART_msg[50];

    char buffer[20][70]={"0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0"};
    lcd_packet packet_out;
    buffer_packet packet_in;
    packet_in.packet_position=0;
    int time;
    int cold_start_flag_local = true;
    int time_buffer[20];

    HibernateEnableExpClk(SysCtlClockGet());  //enable hybernate module
    HibernateRTCEnable();  //enable rtc

    while(cold_start_flag_local == true) //if we are in the cold start,
    {
        if(xQueueReceive (Menu_Buffer_Queue, &packet_in, 5/ portTICK_RATE_MS)==pdPASS)
                {
                time = packet_in.time;     //copies the time from the packet to the time
                HibernateRTCSet(time);     //sets the time in the RTC
                cold_start_flag_local = false;  //sets the flag to false
                }
        if(xQueueReceive (UART_Buffer_Queue, &UART_msg, 5/ portTICK_RATE_MS)==pdPASS);
    }

    while(cold_start_flag_local == false)
    {
        if(xQueueReceive (UART_Buffer_Queue, &UART_msg, 5/ portTICK_RATE_MS)==pdPASS)
        {
            cont_total++;  //increase the number of received messages

            strcpy(buffer[buffer_line], UART_msg);

            time_buffer[buffer_line] = HibernateRTCGet();  //gets time from the rtc and saves it to buffer

            buffer_line++; //increase the place where the buffer is

            if(buffer_line==20 ) buffer_line=0; //if buffer is bigger than 20, set to 0


        }


        if(xQueueReceive (Menu_Buffer_Queue, &packet_in, 10/ portTICK_RATE_MS) == pdPASS)
        {

            if(packet_in.command == 1)  // Display whole packet
            {

                strcpy(packet_out.msg, buffer[packet_in.packet_position]); //retrieves the requested packet from the bufer
                packet_out.identifier=MENU_PACKET_FULL;


                if(packet_in.packet_position <= cont_total-1) //check for if the packet exists
                {
                    xQueueSendToBack(Lcd_Write_Queue, &packet_out, portMAX_DELAY); // send info to lcd
                }
                else
                {
                    packet_out.identifier=MENU_PACKET_EMPTY;  //if packet doesnt exist, send different command to lcd
                    xQueueSendToBack(Lcd_Write_Queue, &packet_out, portMAX_DELAY);// send info to lcd
                }


            }
            if(packet_in.command == 2) // Display RSSI and SNR
            {
                packet_out.identifier=MENU_PACKET_NOISE;

                if(packet_in.packet_position <= cont_total-1) //check for if the packet exists
                {
                    char UART_end[15];
                    char *position_ptr = strchr(buffer[packet_in.packet_position], ';');  //finds the adress of the first semicolon
                    int position = position_ptr - buffer[packet_in.packet_position] ;   //calculates the position of the first semicolon
                    strncpy(UART_end, buffer[packet_in.packet_position] + position + 5 , 11);  //copies the stuff after semicolon in another string



                    char *position_ptr2 = strchr(UART_end, ',');  //finds the adress of the first colon
                    int position2 = position_ptr2 - UART_end ;   //calculates the position of the first semicolon
                    char RSSI[5];
                    strncpy(RSSI,UART_end, position2);
                    RSSI[position2]='\0';

                    char *position_ptr3 = strchr(UART_end, '\0');  //finds the adress of the first colon
                    int position3 = position_ptr3 - UART_end ;   //calculates the position of the first semicolon
                    char SNR[5];
                    strncpy(SNR,UART_end + position2 +1, position3 - position2);
                    SNR[position3-position2]='\0';

                    Lcd_Write_Integer(packet_in.packet_position);

                    strcpy(packet_out.msg, RSSI);  //returns the rrsi value for the requested packet
                    strcpy(packet_out.SNR, SNR);
                    xQueueSendToBack(Lcd_Write_Queue, &packet_out, portMAX_DELAY);// send info to lcd
                }
                else
                {

                    packet_out.identifier=MENU_PACKET_EMPTY; //if packet doesnt exist, send different command to lcd
                    xQueueSendToBack(Lcd_Write_Queue, &packet_out, portMAX_DELAY); // send info to lcd
                }

            }
            if(packet_in.command== 3) // number of packets
            {
                packet_out.identifier = MENU_PACKET_COUNT;
                packet_out.cont = cont_total;
                xQueueSendToBack(Lcd_Write_Queue, &packet_out, portMAX_DELAY);// send info to lcd
            }
            if(packet_in.command ==4) // time of packet
            {
                packet_out.identifier = MENU_PACKET_TIME;
                packet_out.cont=time_buffer[packet_in.packet_position];

                if(packet_in.packet_position <= cont_total-1)//check for if the packet exists
                {
                    xQueueSendToBack(Lcd_Write_Queue, &packet_out, portMAX_DELAY);// send info to lcd
                }
                else
                {
                    packet_out.identifier=MENU_PACKET_EMPTY; //if packet doesnt exist, send different command to lcd
                    xQueueSendToBack(Lcd_Write_Queue, &packet_out, portMAX_DELAY);// send info to lcd
                }


            }

        }

        vTaskDelay( LCD_Delay/ portTICK_RATE_MS);
    }

}

//*****************************************************************************
//
// Initializes the switch task.
//
//*****************************************************************************
uint32_t
Buff_TaskInit(void)
{




    if(xTaskCreate(Buff_Task, (const portCHAR *)"BUFFER", BUFFERTASKSTACKSIZE, NULL, tskIDLE_PRIORITY + PRIORITY_BUFFER_TASK, NULL) != pdTRUE)
    {
        //smth bad
        return 1;
    }



    return 0;
}
