//task 01: Continuously display the temperature of the device (internal temperature sensor) on the
//a) hyperterminal, and b) GUI Composer (Temp Sensor) using a timer interrupt every 0.5 secs.


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
//#define TARGET_IS_BLIZZARD_RB1
#include "driverlib/rom.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"

#include <stdio.h>

char temp[4];

#ifdef DEBUG
void__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

uint32_t ui32ADC0Value[4];
volatile uint32_t ui32TempAvg;
volatile uint32_t ui32TempValueC;
volatile uint32_t ui32TempValueF;

int main(void)
{
    uint32_t ui32Period;


    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN
                   |SYSCTL_XTAL_16MHZ);



    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0); //enable ADC0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIO peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);



    ADCHardwareOversampleConfigure(ADC0_BASE, 32); //hardware averaging

    //configure timer type and period
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    ui32Period = (SysCtlClockGet() / 2);
    TimerLoadSet(TIMER1_BASE, TIMER_A, ui32Period -1);

    //uart initialization
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));


    //configure ADC0 with temperature sensor
    ADCSequenceConfigure(ADC0_BASE, 2, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 2, 0, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 2, 1, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 2, 2, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE,2,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);

    ADCSequenceEnable(ADC0_BASE, 2); //enable sequence

    IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();
    TimerEnable(TIMER1_BASE, TIMER_A);

    while(1){
    }
}

void Timer1IntHandler(void)
{
    int i =0;
    // Clear the timer interrupt
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    ADCIntClear(ADC0_BASE, 2); //clear the ADC buffer
     ADCProcessorTrigger(ADC0_BASE, 2); //set off trigger to start convert

     //wait for ADC to finish converting
     while(!ADCIntStatus(ADC0_BASE, 2, false))
     {
     }

     ADCSequenceDataGet(ADC0_BASE, 2, ui32ADC0Value); //grab ADC value
     ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2]
                    + ui32ADC0Value[3] + 2)/4;
     ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
     ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;

     sprintf(temp, "%d", ui32TempValueF);


     while(temp[i] != '\0'){
         UARTCharPut(UART0_BASE, temp[i]);
         i++;
     }

     UARTCharPut(UART0_BASE, 'F');
     UARTCharPut(UART0_BASE, '\r\n');

}
