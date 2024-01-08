#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"

#include "LCD.h"
#include "i2c.h"

#define tmp_addr 0x48
void float_to_string( char * out, float in)
{
    int inteira= 0, decimal = 0, i, contas[3];

    inteira = (int) in;
    decimal = (int) ( (in - inteira) * 100 );

    contas[0] = inteira / 100;
    contas[1] = (inteira - contas[0]*100) /10;
    contas[2] = inteira - contas[0]*100 - contas[1]*10;

    for(i = 0; i < 3; i++)
    {
        out[i] = contas[i] + '0';
    }
    out[3] = '.';

    contas[0] = decimal / 10;
    contas[1] = (decimal - contas[0]*10);

    for(i = 0; i < 2; i++)
    {
        out[i+4] = contas[i] + '0';
    }

    out[6] = '\0';


    return;

}
// C program for implementation of ftoa()
#include <math.h>
#include <stdio.h>

// Reverses a string 'str' of length 'len'
void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

// Converts a given integer x to string str[].
// d is the number of digits required in the output.
// If d is more than the number of digits in x,
// then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

// Converts a floating-point/double number to a string.
void ftoa(float n, char* res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0) {
        res[i] = '.'; // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter
        // is needed to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}

// Driver program to test above function

int main(void)
{

        char test[] = "passei";
        char test1[] = "here1";

        SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);


        I2C_Setup();


        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

        GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
        GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);

        I2C_Config();

        Lcd_Clear();
        Lcd_Init();




        //Lcd_Write_String(test1);


        Lcd_Clear();

        while (1)
        {
            uint16_t data;
            uint8_t part1, part2;
            float aux=10;
            char  test2[20];

           data=I2C_Read();


           data= data >> 4;
           aux=data*0.0625;

           ftoa(aux, test2, 3);


            Lcd_Write_String(test2);

            while(1);



        }




        return 0;
}

//sends an I2C command to the specified slave

