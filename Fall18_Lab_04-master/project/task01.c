//task01: toggle of the GPIO at 2 Hz using Timer0 with 75% duty cycle
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"

int main(void)
{
    uint32_t ui32Period;

    SysCtlClockSet(
            SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ
                    | SYSCTL_OSC_MAIN); //40MHz clk
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1); //PWM match system clock

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIO peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1); //enable PWM peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); //enable timer0 peripheral

    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); //32-bit periodic mode timer0

    GPIOPinConfigure(GPIO_PF2_M1PWM6); //configure PF2(B) as PWM6
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2); //set PF2 pin type as PWM

    PWMGenConfigure(PWM1_BASE, PWM_GEN_3,
                    PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC); //countdown non-synchronous mode

    ui32Period = (SysCtlClockGet() / 2) / 2; //2Hz period
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1); //account for zero count

    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, 100); //set period to 100
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, 75); //75% duty cycle

    PWMGenEnable(PWM1_BASE, PWM_GEN_3); //enable PWM generator 3 for PWM6/7
    IntEnable(INT_TIMER0A); //enable vector associated w/ timer0a
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT); //enable timer interrupt
    IntMasterEnable(); //master interrupt enable
    TimerEnable(TIMER0_BASE, TIMER_A); //enable timer

    while (1)
    {
    }
}

void Timer0IntHandler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // Clear the timer interrupt
    // Read the current state of the GPIO pin and write back the opposite state
    if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2))
    {
        PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, false); //turn off LED with PWM
    }
    else
    {
        PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true); //turn on LED with PWM
    }
}

void PFSW2IntHandler(void)
{

}

