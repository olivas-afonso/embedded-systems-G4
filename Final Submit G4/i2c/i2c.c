/*
 * i2c.c
 *
 *  Created on: 13/12/2023
 *      Author: olivas
 */


#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <math.h>

#define TMP_ADDR 0x48
#define REG_TEMP 0x00
#define REG_CONFIG 0x01




//attempt at i2c interrupts




void I2C_Setup(void)
{

    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2); // pin and peripheral configures

    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C2);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    GPIOPinConfigure(GPIO_PE4_I2C2SCL);
    GPIOPinConfigure(GPIO_PE5_I2C2SDA);

    GPIOPinTypeI2CSCL(GPIO_PORTE_BASE, GPIO_PIN_4);
    GPIOPinTypeI2C(GPIO_PORTE_BASE, GPIO_PIN_5);



    I2CMasterInitExpClk(I2C2_BASE, SysCtlClockGet(), false);

}


uint16_t I2C_Read(void)
{
    uint16_t aux;

    I2CMasterSlaveAddrSet(I2C2_BASE, TMP_ADDR, false); // write

    I2CMasterDataPut(I2C2_BASE, REG_TEMP);
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    while(I2CMasterBusy(I2C2_BASE));

    I2CMasterSlaveAddrSet(I2C2_BASE, TMP_ADDR, true); // read

    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
    while(I2CMasterBusy(I2C2_BASE));
    aux= I2CMasterDataGet(I2C2_BASE)<<8;

    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    while(I2CMasterBusy(I2C2_BASE));
    aux|= I2CMasterDataGet(I2C2_BASE);

    return aux;
}
