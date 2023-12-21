/*
 * structs.h
 *
 *  Created on: 16/12/2023
 *      Author: olivas
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_

#define MENU_TMP 1
#define MENU_UART 2

xQueueHandle Lcd_Write_Queue;

typedef struct lcd_packet_
{
    uint8_t identifier;
    float data;
    char msg[20];
}lcd_packet;

#endif /* STRUCTS_H_ */
