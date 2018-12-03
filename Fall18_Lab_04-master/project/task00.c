#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

int main(void){
    uint32_t ui32Period;

    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); //40MHz clk

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIO peripherals
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); //set RGB as output

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); //enable timer0 peripheral
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); //32-bit periodic mode

    ui32Period = (SysCtlClockGet() / 10) / 2; //1/2 desired period calculation
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1); //account for zero count

    IntEnable(INT_TIMER0A); //enable vector associated w/ timer0a
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT); //enable timer interrupt
    IntMasterEnable(); //master interrupt enable

    TimerEnable(TIMER0_BASE, TIMER_A); //enable timer

    while(1){

    }

}

    void Timer0IntHandler(void)
    {
        TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // Clear the timer interrupt
        // Read the current state of the GPIO pin and write back the opposite state
        if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2)){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
        }
        else{
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
        }
    }
