/*
 * i2c.h
 *
 *  Created on: 13/12/2023
 *      Author: olivas
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/pin_map.h"
#include "inc/hw_i2c.h"
#include "string.h"

void I2C_Setup(void);
void I2C_Config(void);
uint16_t I2C_Read(void);

#endif /* I2C_H_ */
