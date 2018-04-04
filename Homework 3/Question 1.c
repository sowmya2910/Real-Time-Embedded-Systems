//*****************************************************************************
//
// freertos_demo.c - Simple FreeRTOS example.
//
// Copyright (c) 2012-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 2.1.4.178 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define PERIOD1 (1000000)  
#define PERIOD2 (4000000) 
//*****************************************************************************
//
// The mutex that protects concurrent access of UART from multiple tasks.
//
//*****************************************************************************
xSemaphoreHandle xSemaphore;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}

#endif

//*****************************************************************************
//
// This hook is called by FreeRTOS when an stack overflow error is detected.
//
//*****************************************************************************
void
vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}

void InterruptHandler(void)
{
    // Clear the timer interrupt.
    ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    UARTprintf("Semaphore Received from Interrupt\n");
    xSemaphoreGive(xSemaphore);
}

static void Task1(void *pvParameters){

	uint32_t timer1val = 0;
    uint32_t count = 0;
    int32_t difference = 0;
    while(1){

        UARTprintf("Waiting for Semaphore\n");
        timer1val = TimerValueGet(TIMER1_BASE, TIMER_A);
        xSemaphoreTake(xSemaphore, portMAX_DELAY);
        difference = count - timer1val ;
        if(difference < 0 )
            difference = - difference - 100000000;
        UARTprintf("Task got Semaphore after %d Clock Cycle \n", difference);
        count = timer1val;

    }
    vTaskDelete(NULL);
}

uint32_t Task1Initialize(void){
    if(xTaskCreate(Task1,(const portCHAR *)"Task1",128,NULL,1,NULL)!=pdTRUE){
        return (1);
    }
    return (0);
}

//*****************************************************************************
//
// Initialize FreeRTOS and start the initial set of tasks.
//
//*****************************************************************************
int
main(void)
{
    //
    // Set the clocking to run at 50 MHz from the PLL.
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);

    //
    // Initialize the UART and configure it for 115,200, 8-N-1 operation.
    //
    ConfigureUART();

   ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
   ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

   // Enable processor interrupts
   ROM_IntMasterEnable();

   // Timer Configuration

   ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
   ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

   ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, PERIOD2);
   ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, ROM_SysCtlClockGet()*4);

   // Setup the interrupts for the timer timeouts.
   ROM_IntEnable(INT_TIMER0A); //Timer 0 Enable

   ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT); //Timer 0 in Periodic mode

   // Timer Enable
   ROM_TimerEnable(TIMER0_BASE, TIMER_A);
   ROM_TimerEnable(TIMER1_BASE, TIMER_A);

    // Semaphore creation
    xSemaphore = xSemaphoreCreateBinary();

    if(Task1Initialize()!=0)
    {
        while(1)
        {
        }
    }

    vTaskStartScheduler();

    // If task scheduler returns error. loop forever.
    // Should never come here.
    while(1){

    }
}
