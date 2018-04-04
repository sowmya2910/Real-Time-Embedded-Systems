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
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define ITERATIONS10 49000
#define ITERATIONS40 210000

unsigned int seqIterations = 47;
unsigned int idx = 0, jdx = 1;
unsigned int fib = 0, fib0 = 0, fib1 = 1;
unsigned int reqIterations = 1, Iterations = ITERATIONS10;

//*****************************************************************************
//
// Fibonacci Series
// Reference: Sam Siewart
#define FIB_TEST(seqCnt, iterCnt)      \
   for(idx=0; idx < iterCnt; idx++)    \
   {                                   \
      fib = fib0 + fib1;               \
      while(jdx < seqCnt)              \
      {                                \
         fib0 = fib1;                  \
         fib1 = fib;                   \
         fib = fib0 + fib1;            \
         jdx++;                        \
      }                                \
   }                                   \
//*****************************************************************************


//*****************************************************************************
//
// The mutex that protects concurrent access of UART from multiple tasks.
//
//*****************************************************************************
xSemaphoreHandle xSemaphore1;
xSemaphoreHandle xSemaphore2;

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

static void Task1(void *pvParameters){
    uint32_t start = 0;
    uint32_t stop = 0;
    while(1){
        xSemaphoreTake(xSemaphore1,portMAX_DELAY);
        UARTprintf("Task1 Running\n");
        start = TimerValueGet(TIMER0_BASE, TIMER_A);
        FIB_TEST(seqIterations, ITERATIONS10);
        stop = TimerValueGet(TIMER0_BASE, TIMER_A);
        UARTprintf("Load Time for Task1: %d ms\n",(start-stop)/50000);
        xSemaphoreGive(xSemaphore2);
    }
    vTaskDelete(NULL);
}

static void Task2(void *pvParameters){
    uint32_t start = 0;
    uint32_t stop = 0;
    while(1){
        xSemaphoreTake(xSemaphore2,portMAX_DELAY);
        start = TimerValueGet(TIMER0_BASE, TIMER_A);
        UARTprintf("Task2 Running\n");
        FIB_TEST(seqIterations, ITERATIONS40);
        stop = TimerValueGet(TIMER0_BASE, TIMER_A);
        UARTprintf("Load Time for Task2: %d ms\n",(start-stop)/50000);
        xSemaphoreGive(xSemaphore1);
        }
    vTaskDelete(NULL);
}

uint32_t Task1Initialize(void){
    if(xTaskCreate(Task1,(const portCHAR *)"Task1",1280,NULL,2,NULL)!=pdTRUE){
        return (1);
    }
    return (0);
}

uint32_t Task2Initialize(void){
    if(xTaskCreate(Task2,(const portCHAR *)"Task2",1280,NULL,1,NULL)!=pdTRUE){
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

    ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

    ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, ROM_SysCtlClockGet()*4);
    ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, ROM_SysCtlClockGet()*4);

    ROM_TimerEnable(TIMER0_BASE, TIMER_A);
    ROM_TimerEnable(TIMER1_BASE, TIMER_A);

    xSemaphore1       = xSemaphoreCreateBinary();
    xSemaphore2       = xSemaphoreCreateBinary();

    if(Task1Initialize()!=0)
    {
        while(1)
        {
        }
    }

    if(Task2Initialize()!=0)
    {
        while(1)
        {
        }
    }
    //Give the Semaphore for Task 1
    xSemaphoreGive(xSemaphore1);
    vTaskStartScheduler();

    // If task scheduler returns error. loop forever.
    // Should never come here.
    while(1){

    }
}
