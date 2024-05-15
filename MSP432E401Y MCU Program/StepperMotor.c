
#include "tm4c1294ncpdt.h"
#include <stdint.h>
#include "SysTick.h"
#include "StepperMotor.h"
#include "init.h"
#include "Variables.h"

//Initialize Port H as output
void PortH_Init(void)
{
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R7; // Activate the clock for Port H
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R7) == 0)
    {
    }; // Allow time for clock to stabilize

    GPIO_PORTH_DIR_R |= 0x0F;    // Enable PH0 and PH1 as inputs
    GPIO_PORTH_AFSEL_R &= ~0x0F; // disable alt funct on Port M pins (PM0-PM3)
    GPIO_PORTH_DEN_R |= 0x0F;    // Enable PH0, PH1, PH2, and PH3 as digital pins
    GPIO_PORTH_AMSEL_R &= ~0x0F; // disable analog functionality on Port M	pins (PM0-PM3)

    return;
}

// 4 motor steps to make the motor spin
char motor_steps[] = {0b00000011, 0b00000110, 0b00001100, 0b00001001};
int delay = 300000;

// Steps Parameter is for numeber of step or the angel at which the sensor will collect data.
// direction parameters is to make the motor spin in both direction using same function.
void spin(int steps, int direction)
{
    for (int i = 0; i < steps; i++)
    {
        if (direction == CLOCKWISE)
        {
            for (int j = 0; j < 4; j++)
            {
                GPIO_PORTH_DATA_R = motor_steps[j];
                SysTick_Wait(delay);
            }
        }
        else if (direction == COUNTER_CLOCKWISE)
        {
            for (int j = 3; j >= 0; j--)
            {
                GPIO_PORTH_DATA_R = motor_steps[j];
                SysTick_Wait(delay);
            }
        }
    }
}