#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "variables.h"
#include "vl53l1x_api.h"
#include "StepperMotor.h"
#include "onboardLEDs.h"
#include "uart.h"

// Enable interrupts
void EnableInt(void)
{
    __asm("    cpsie   i\n");
}

// Disable interrupts
void DisableInt(void)
{
    __asm("    cpsid   i\n");
}

// Low power wait
void WaitForInt(void)
{
    __asm("    wfi\n");
}

// Global variable visible in Watch window of debugger
// Increments at least once per button press
volatile unsigned long FallingEdges = 0;

// Give clock to Port J and initalize PJ1 as Digital Input GPIO
void PortJ_Init(void)
{
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R8; // Activate clock for Port J
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R8) == 0)
    {
    };                         // Allow time for clock to stabilize
    GPIO_PORTJ_DIR_R &= ~0x02; // Make PJ1 input
    GPIO_PORTJ_DEN_R |= 0x02;  // Enable digital I/O on PJ1

    GPIO_PORTJ_PCTL_R &= ~0x000000F0; //  Configure PJ1 as GPIO
    GPIO_PORTJ_AMSEL_R &= ~0x02;      //  Disable analog functionality on PJ1
    GPIO_PORTJ_PUR_R |= 0x02;         //	Enable weak pull up resistor on PJ1
}

// Interrupt initialization for GPIO Port J IRQ# 51
void PortJ_Interrupt_Init(void)
{

    FallingEdges = 0; // Initialize counter

    GPIO_PORTJ_IS_R = 0;     // (Step 1) PJ1 is Edge-sensitive
    GPIO_PORTJ_IBE_R = 0;    //     			PJ1 is not triggered by both edges
    GPIO_PORTJ_IEV_R = 0;    //     			PJ1 is falling edge event
    GPIO_PORTJ_ICR_R = 0x02; // 					Clear interrupt flag by setting proper bit in ICR register
    GPIO_PORTJ_IM_R = 0x02;  // 					Arm interrupt on PJ1 by setting proper bit in IM register

    NVIC_EN1_R = 0x00080000; // (Step 2) Enable interrupt 51 in NVIC (which is in Register EN1)

    NVIC_PRI12_R = 0xA0000000; // (Step 4) Set interrupt priority to 5

    EnableInt(); // (Step 3) Enable Global Interrupt. lets go!
}