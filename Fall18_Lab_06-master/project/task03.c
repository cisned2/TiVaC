//task03: Change PWM duty cycle from 90% to 10% to control the brightness of
//the all three LED at PF1, PF2, and PF3 using three nested “for loops”

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
    //start at 90% duty cycle
    int i = 90 ;
    int j = 90 ;
    int k = 90 ;

    SysCtlClockSet(
            SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ
                    | SYSCTL_OSC_MAIN); //40MHz clk
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1); //PWM match system clock

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIO peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1); //enable PWM peripheral

    GPIOPinConfigure(GPIO_PF1_M1PWM5); //configure PF1(R) as PWM5
    GPIOPinConfigure(GPIO_PF2_M1PWM6); //configure PF2(B) as PWM6
    GPIOPinConfigure(GPIO_PF3_M1PWM7); //configure PF2(G) as PWM7

    //define as PWM pins
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);

    //countdown non-synchronous mode
    PWMGenConfigure(PWM1_BASE, PWM_GEN_2,PWM_GEN_MODE_DOWN
                    | PWM_GEN_MODE_NO_SYNC);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN
                    | PWM_GEN_MODE_NO_SYNC);

    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, 100); //set period to 100
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, 100); //set period to 100

    PWMGenEnable(PWM1_BASE, PWM_GEN_3); //enable PWM generator 3 for PWM6/7
    PWMGenEnable(PWM1_BASE, PWM_GEN_2); //enable PWM generator 2 for PWM5

    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true); //turn on LED with PWM
    PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true); //turn on LED with PWM
    PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true); //turn on LED with PWM

    while(1)
    {
        //cycle through colors and end with dim white
        for(; i>=10; --i) {
            for(; j>=10; --j) {
                for(; k>=10; --k) {
                    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, k);
                    SysCtlDelay(2000000);//delay
                }
                PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, j);
                SysCtlDelay(2000000);//delay
            }
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, i);
            SysCtlDelay(2000000);//delay
        }
    }
}
