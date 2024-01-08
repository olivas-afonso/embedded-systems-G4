/*
 * aux_functions.h
 *
 *  Created on: 16/12/2023
 *      Author: olivas
 */

#ifndef AUX_FUNCTIONS_H_
#define AUX_FUNCTIONS_H_

#include <math.h>
#include <stdio.h>

void ftoa(float n, char* res, int afterpoint);

int myAtoi(char* str);

char* myItoa(int num, char* str, int base);

uint8_t getHours(uint32_t posixTime);
uint8_t getMinutes(uint32_t posixTime);
uint8_t getSeconds(uint32_t posixTime);




#endif /* AUX_FUNCTIONS_H_ */
