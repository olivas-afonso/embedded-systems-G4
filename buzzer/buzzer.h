/*
 * buzzer.h
 *
 *  Created on: 07/12/2023
 *      Author: pedro rodrigues
 */

#ifndef BUZZER_H_
#define BUZZER_H_


/*
 * File:   buzzer.h
 * Author: pedro rodrigues
 * Comments:
 * Revision history:
 */

/* Program Description: This program header provides routines
* for controlling the buzzer
*
* Hardware Description:
*
* PWNout ---> PB2
*
*/

/**************************************************************
*Includes and defines
**************************************************************/
// STD XC8 include

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "driverlib/pwm.h"
#include "driverlib/pwm.c"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"

#define ON 1
#define OFF 0

// function prototypes
 void initializePwm();
 void buzzerState(int state);


#endif /* BUZZER_H_ */
