

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
#include <stdlib.h>
#include <temp_task.h>
#include "lcd_task.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"

#include "inc/tm4c123gh6pm.h"


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
// The stack size for the Menu task.
//
//*****************************************************************************
#define MENUTASKSTACKSIZE        128        // Stack size in words


#define MENU_DELAY 10
extern xSemaphoreHandle semaphore_cold_start;
bool cold_start_flag;
bool stop_temp;
bool stop_keyboard;

//*****************************************************************************
//
// This task reads the keypad information and decides what do to next. Also handles cold start
//
//*****************************************************************************
static void
Menu_Task(void *pvParameters)
{

    char key_pressed;
    lcd_packet packet_out;
    buffer_packet buffer_info;
    buffer_info.packet_position=0;
    int flag_day=0, flag_time=0;
    char day_format[10];
    char time_format[10];
    int time_int=0;
    int aux=0, state=0;
    int flag=0;


    while(1)
    {
        if(flag==0) // only happens one on startup
        {

            packet_out.identifier=MENU_COLD_START;
            xQueueSendToBack(Lcd_Write_Queue, &packet_out, portMAX_DELAY);
            flag=1;
        }

        if(xQueueReceive (Menu_Control_Queue, &key_pressed, portMAX_DELAY)!=pdPASS) //wait for keypad queue message

        {
            while(1); // waiting empty
        }



        if(cold_start_flag == true && key_pressed != ' ') // if cold start and also ignore key that are spaces
        {

            if(flag_day== 1 && flag_time==0)//time setup
                        {

                            time_format[aux]=key_pressed;
                            switch (aux) // save each individual char for time, checks for possible time values
                            {
                            case 0:
                                if(strchr("3456789ABCDEF", key_pressed)!= NULL) break;
                                Lcd_Set_Cursor(1,1);
                                Lcd_Write_Char(key_pressed);
                                time_int=(key_pressed - '0')*36000;
                                aux++;

                                break;

                            case 1:
                                if(time_format[0]=='2')
                                    if(strchr("56789ABCDEF", key_pressed)!= NULL) break;
                                if(strchr("ABCDEF", key_pressed)!= NULL) break;
                                Lcd_Set_Cursor(1,2);
                                Lcd_Write_Char(key_pressed);
                                Lcd_Set_Cursor(1,4);
                                time_int+=(key_pressed - '0')*3600;
                                aux++;

                                break;
                            case 2:
                                if(strchr("6789ABCDEF", key_pressed)!= NULL) break;
                                Lcd_Set_Cursor(1,4);
                                Lcd_Write_Char(key_pressed);
                                time_int+=(key_pressed - '0')*600;
                                aux++;

                                break;

                            case 3:
                                if(strchr("ABCDEF", key_pressed)!= NULL) break;
                                Lcd_Set_Cursor(1,5);
                                Lcd_Write_Char(key_pressed);
                                Lcd_Set_Cursor(1,7);
                                time_int+=(key_pressed - '0')*60;
                                aux++;

                                break;

                            case 4:
                                if(strchr("6789ABCDEF", key_pressed)!= NULL) break;
                                Lcd_Set_Cursor(1,7);
                                Lcd_Write_Char(key_pressed);
                                time_int+=(key_pressed - '0')*10;
                                aux++;
                                break;


                            case 5:
                                if(strchr("ABCDEF", key_pressed)!= NULL) break;
                                Lcd_Set_Cursor(1,8);
                                Lcd_Write_Char(key_pressed);
                                time_int+=(key_pressed - '0');
                                flag_time=1;
                                time_format[aux+1]='\0';
                                aux=0;
                                buffer_info.time=time_int;
                                xQueueSendToBack(Menu_Buffer_Queue, &buffer_info, portMAX_DELAY);
                                break;
                            default: break;
                            }

                        }

            if(flag_day==0) // day setup
            {
                day_format[aux]=key_pressed;

                switch (aux) // save each individual char for time, checks for possible time values
                {
                case 0:
                    if(strchr("456789ABCDEF", key_pressed)!= NULL) break;
                    Lcd_Set_Cursor(1,1);
                    Lcd_Write_Char(key_pressed);
                    aux++;

                    break;

                case 1:
                    if(day_format[0]=='3')
                        if(strchr("23456789ABCDEF", key_pressed)!= NULL) break;
                    if(day_format[0]=='0')
                        if(strchr("0ABCDEF", key_pressed)!= NULL) break;
                    if(strchr("ABCDEF", key_pressed)!= NULL) break;
                    Lcd_Set_Cursor(1,2);
                    Lcd_Write_Char(key_pressed);
                    Lcd_Set_Cursor(1,4);
                    aux++;
                    day_format[aux]='/';
                    aux++;

                    break;
                case 3:
                    if(strchr("23456789ABCDEF", key_pressed)!= NULL) break;
                    Lcd_Set_Cursor(1,4);
                    Lcd_Write_Char(key_pressed);
                    aux++;

                    break;

                case 4:
                    if(day_format[3]=='1')
                        if(strchr("3456789ABCDEF", key_pressed)!= NULL) break;
                    if(day_format[3]=='0')
                        if(strchr("0ABCDEF", key_pressed)!= NULL) break;
                    if(strchr("ABCDEF", key_pressed)!= NULL) break;
                    Lcd_Set_Cursor(1,5);
                    Lcd_Write_Char(key_pressed);
                    Lcd_Set_Cursor(1,7);
                    aux++;
                    day_format[aux]='/';
                    aux++;

                    break;

                case 6:
                    if(strchr("ABCDEF", key_pressed)!= NULL) break;
                    Lcd_Set_Cursor(1,7);
                    Lcd_Write_Char(key_pressed);
                    aux++;

                    break;

                case 7:
                    if(strchr("ABCDEF", key_pressed)!= NULL) break;
                    Lcd_Set_Cursor(1,8);
                    Lcd_Write_Char(key_pressed);
                    aux++;

                    break;

                case 8:
                    if(strchr("ABCDEF", key_pressed)!= NULL) break;
                    Lcd_Set_Cursor(1,9);
                    Lcd_Write_Char(key_pressed);
                    aux++;

                    break;

                case 9:
                    if(strchr("ABCDEF", key_pressed)!= NULL) break;
                    Lcd_Set_Cursor(1,10);
                    Lcd_Write_Char(key_pressed);
                    flag_day=1;
                    day_format[aux+1]='\0';
                    aux=0;
                    Lcd_Clear();
                    vTaskDelay(1/portTICK_RATE_MS);
                    Lcd_Write_String("HH:MM:SS");
                    break;
                default: break;

                }

            }

            if(flag_day==1 && flag_time ==1 ) //  End of cold start
            {
                packet_out.identifier=MENU_STARTUP;
                xQueueSendToBack(Lcd_Write_Queue, &packet_out, portMAX_DELAY);
                while(1) // waiting for the start key
                {
                    xQueueReceive (Menu_Control_Queue, &key_pressed, portMAX_DELAY);
                    if(key_pressed == 'C') break;
                }
                cold_start_flag=false; //ends cold start, starts temp reading
            }

            vTaskDelay(1/portTICK_RATE_MS);
            vTaskDelay(1/portTICK_RATE_MS);

        }
        else if(stop_keyboard == false)
        {

        switch (key_pressed)
        {
        case ' ': break;

        case 'A':
            buffer_info.command = 3; // cont command
            vTaskDelay(1/portTICK_RATE_MS);
            xQueueSendToBack(Menu_Buffer_Queue, &buffer_info, portMAX_DELAY);
            break;

        case 'B':
            buffer_info.command=1; // full packet command
            stop_temp=true;

            packet_out.identifier=MENU_WAIT_POSITION;
            xQueueSendToBack(Lcd_Write_Queue, &packet_out, portMAX_DELAY);

            while(1) // waits for position to print
            {
                if(xQueueReceive (Menu_Control_Queue, &key_pressed, portMAX_DELAY)==pdPASS) break;
            }

            buffer_info.packet_position=key_pressed - '0'; // turns char into int


            xQueueSendToBack(Menu_Buffer_Queue, &buffer_info, portMAX_DELAY);
            break;

        case 'C':
            buffer_info.command=2; // noise command
            stop_temp=true;

            packet_out.identifier=MENU_WAIT_POSITION;
            xQueueSendToBack(Lcd_Write_Queue, &packet_out, portMAX_DELAY);

            while(1)// waits for position to print
            {
                if(xQueueReceive (Menu_Control_Queue, &key_pressed, portMAX_DELAY)==pdPASS) break;
            }

            buffer_info.packet_position=key_pressed - '0';// turns char into int
            xQueueSendToBack(Menu_Buffer_Queue, &buffer_info, portMAX_DELAY);
            break;

        case 'D': // time command
            packet_out.identifier=MENU_CURRENT_TIME;
            strcpy(packet_out.msg, day_format);
            vTaskDelay(20 / portTICK_RATE_MS);
            xQueueSendToBack(Lcd_Write_Queue, &packet_out, portMAX_DELAY);
            break;

        case 'E': // buzzer command
            if(state==0)
            {
                PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true); //turns buzzer on
                state = 1;
            }

            else if(state==1)
            {
                PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, false);//turns buzzer off
                state = 0;
            }
            break;

        case 'F':
            buffer_info.command=4; // noise command
            stop_temp=true;

            packet_out.identifier=MENU_WAIT_POSITION;
            xQueueSendToBack(Lcd_Write_Queue, &packet_out, portMAX_DELAY);

            while(1)// waits for position to print
            {
                if(xQueueReceive (Menu_Control_Queue, &key_pressed, portMAX_DELAY)==pdPASS) break;
            }

            buffer_info.packet_position=key_pressed - '0';// turns char into int
            xQueueSendToBack(Menu_Buffer_Queue, &buffer_info, portMAX_DELAY);
            break;

        }

        }

            vTaskDelay( MENU_DELAY/ portTICK_RATE_MS);
        }
    }



//*****************************************************************************
//
// Initializes the Menu task.
//
//*****************************************************************************
uint32_t
Menu_TaskInit(void)
{

    // Enable the PWM1 peripheral
    //PWM setup
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    GPIOPinConfigure(GPIO_PF2_M1PWM6);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);

    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, 64000);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, PWMGenPeriodGet(PWM1_BASE, PWM_GEN_3) / 2);

    PWMGenEnable(PWM1_BASE, PWM_GEN_3);


    if(xTaskCreate(Menu_Task, (const portCHAR *)"MENU", MENUTASKSTACKSIZE, NULL, tskIDLE_PRIORITY + PRIORITY_MENU_TASK, NULL) != pdTRUE)
    {
        //smth bad
        return 1;
    }

    cold_start_flag=true; // starts cold start, stops temp



    return 0;
}
