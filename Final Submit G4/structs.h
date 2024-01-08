/*
 * structs.h
 *
 *  Created on: 16/12/2023
 *      Author: olivas
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_
//menu print values
#define MENU_TMP 1
#define MENU_UART 2
#define MENU_KEY 3
#define MENU_PACKET_FULL 4
#define MENU_PACKET_NOISE 6
#define MENU_PACKET_COUNT 5
#define MENU_CURRENT_TIME 7
#define MENU_PACKET_EMPTY 8
#define MENU_PACKET_TIME 9
#define MENU_STARTUP 10
#define MENU_COLD_START 11
#define MENU_WAIT_POSITION 12


#include "FreeRTOS.h"
#include "queue.h"

#include <stdbool.h>

bool stop_temp;





xQueueHandle Lcd_Write_Queue;
xQueueHandle Menu_Control_Queue;
xQueueHandle UART_Buffer_Queue;
xQueueHandle Menu_Buffer_Queue;

typedef struct lcd_packet_ // lcd info struct
{
    int identifier;
    float data;
    char  msg[70];
    char SNR[10];
    char key;
    int cont;
    bool start;
}lcd_packet;

typedef struct UART_packet_
{
    char *packet_number;
    char *info;
    char *RSSI;
    char *SNR;
    char *time;
    char *date;
}UART_packet;

typedef struct buffer_packet_
{
    int command;
    int packet_position;
    int time;

}buffer_packet;


#endif /* STRUCTS_H_ */
