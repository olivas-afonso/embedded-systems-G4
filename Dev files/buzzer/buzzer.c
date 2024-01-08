
/*
 * buzzer.c
 *
 *  Created on: 07/12/2023
 *      Author: pedro rodrigues
 */

#include "buzzer.h"


void initializePwm()
{
    // Enable the PWM1 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);

    // Wait for the PWM1 module to be ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM1))
    {
    }

    // PWM pin
    GPIOPinConfigure(GPIO_PF0_M1PWM4);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_0);

    // Configure the PWM generator for count down mode with immediate updates to the parameters.
    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, 20000);

    // Set the pulse width of PWM1 for a 50% duty cycle.
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_4, 10000);

    // Start the timers in generator 2.
    PWMGenEnable(PWM1_BASE, PWM_GEN_2);

    // Enable the outputs.
    //
    PWMOutputState(PWM1_BASE, PWM_OUT_4_BIT, true);
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
        PWMOutputState(PWM1_BASE, PWM_OUT_4_BIT, false);
    }

    if(state == ON)
    {
        PWMOutputState(PWM1_BASE, PWM_OUT_4_BIT, true);
    }
}