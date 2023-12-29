
/*
 * buzzer.c
 *
 *  Created on: 07/12/2023
 *      Author: pedro rodrigues
 */

#include "buzzer.h"


/**************************************************************
* Function: void initializePwm()
*
* Returns: Nothing
*
* Description: Configure PWM pins
**************************************************************/
void initializePwm()
{
    // Enable the PWM0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    // Wait for the PWM0 module to be ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0))
    {
    }

    // Configure the PWM generator for count down mode with immediate updates to the parameters.
    PWMGenConfigure(PWM_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    // Set the period. For a 50 KHz frequency, the period = 1/50,000, or 20
    // microseconds. For a 20 MHz clock, this translates to 400 clock ticks.
    // Use this value to set the period.
    PWMGenPeriodSet(PWM_BASE, PWM_GEN_0, 400);

    // Set the pulse width of PWM0 for a 50% duty cycle.
    PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, 200);

    // Start the timers in generator 0.
    PWMGenEnable(PWM_BASE, PWM_GEN_0);

    // Enable the outputs.
    //
    PWMOutputState(PWM_BASE, PWM_OUT_0_BIT, true);
}



/**************************************************************
* Function: void buzzerState(int state)
*
* Returns: Nothing
*
* Description: Turns the buzzer ON or OFF
**************************************************************/
void buzzerState(int state)
{
    if(state == OFF)
    {
        PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, false);
    }

    if(state == ON)
    {
        PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, true);
    }
}


}
