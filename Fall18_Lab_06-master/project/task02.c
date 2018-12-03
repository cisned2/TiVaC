//task02: Change PWM duty cycle from 10% to 90% to control the brightness of the LED at PF1.

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"

int main(void)
{
    bool fadeOut;
    int dutyCycle = 0;

    SysCtlClockSet(
            SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ
                    | SYSCTL_OSC_MAIN); //40MHz clk
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1); //PWM match system clock

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIO peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1); //enable PWM peripheral

    GPIOPinConfigure(GPIO_PF1_M1PWM5); //configure PF1(R) as PWM5
//    GPIOPinConfigure(GPIO_PF2_M1PWM6); //configure PF2(B) as PWM6
//    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2); //set PF2 pin type as PWM
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1); //set PF1 pin type as PWM


    PWMGenConfigure(PWM1_BASE, PWM_GEN_2,
                    PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC); //countdown non-synchronous mode
//    PWMGenConfigure(PWM1_BASE, PWM_GEN_3,
  //                  PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC); //countdown non-synchronous mode

    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, 100); //set period to 100
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, 10); //50% duty cycle

//    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, 100); //set period to 100
//    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, 100); //75% duty cycle

 //   PWMGenEnable(PWM1_BASE, PWM_GEN_3); //enable PWM generator 3 for PWM6/7
    PWMGenEnable(PWM1_BASE, PWM_GEN_2); //enable PWM generator 2 for PWM5

    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true); //turn on LED with PWM

 //   PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true); //turn on LED with PWM
    while(1)
    {
        if (!fadeOut){
            if(dutyCycle <= 90){
                dutyCycle = dutyCycle + 5;
                if (dutyCycle > 90)
                    fadeOut = true;
            }
        }
        else{
            dutyCycle = dutyCycle - 5;
            if(dutyCycle <= 10)
                fadeOut = false;
        }

       PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, dutyCycle); //change duty cycle
       SysCtlDelay(2000000);//delay
    }
}
