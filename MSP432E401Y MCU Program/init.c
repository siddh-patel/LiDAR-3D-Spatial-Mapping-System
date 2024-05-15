#include "init.h"
#include <stdint.h>
#include "PLL.h"
#include "SysTick.h"
#include "uart.h"
#include "onboardLEDs.h"
#include "tm4c1294ncpdt.h"
#include "VL53L1X_api.h"

void I2C_Init(void)
{
    SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R0;   // activate I2C0
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1; // activate port B
    while ((SYSCTL_PRGPIO_R & 0x0002) == 0)
    {
    }; // ready?

    GPIO_PORTB_AFSEL_R |= 0x0C; // 3) enable alt funct on PB2,3       0b00001100
    GPIO_PORTB_ODR_R |= 0x08;   // 4) enable open drain on PB3 only

    GPIO_PORTB_DEN_R |= 0x0C; // 5) enable digital I/O on PB2,3
                              //    GPIO_PORTB_AMSEL_R &= ~0x0C; // 7) disable analog functionality on PB2,3

    // 6) configure PB2,3 as I2C
    //  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFF00FF)+0x00003300;
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFF00FF) + 0x00002200; // TED
    I2C0_MCR_R = I2C_MCR_MFE;                                          // 9) master function enable
    I2C0_MTPR_R = 0b0000000000000101000000000111011; // 8) configure for 100 kbps clock (added 8 clocks of glitch
                                                     // suppression ~50ns)
    //    I2C0_MTPR_R = 0x3B;                                        						// 8) configure for 100 kbps
    //    clock
}

// The VL53L1X needs to be reset using XSHUT.  We will use PG0
void PortG_Init(void)
{
    // Use PortG0
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R6; // activate clock for Port N
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R6) == 0)
    {
    };                           // allow time for clock to stabilize
    GPIO_PORTG_DIR_R &= 0x00;    // make PG0 in (HiZ)
    GPIO_PORTG_AFSEL_R &= ~0x01; // disable alt funct on PG0
    GPIO_PORTG_DEN_R |= 0x01;    // enable digital I/O on PG0
                                 // configure PG0 as GPIO
    // GPIO_PORTN_PCTL_R = (GPIO_PORTN_PCTL_R&0xFFFFFF00)+0x00000000;
    GPIO_PORTG_AMSEL_R &= ~0x01; // disable analog functionality on PN0

    return;
}

// XSHUT     This pin is an active-low shutdown input;
//					the board pulls it up to VDD to enable the sensor by default.
//					Driving this pin low puts the sensor into hardware standby. This input is not level-shifted.
void VL53L1X_XSHUT(void)
{
    GPIO_PORTG_DIR_R |= 0x01;        // make PG0 out
    GPIO_PORTG_DATA_R &= 0b11111110; // PG0 = 0
    FlashAllLEDs();
    SysTick_Wait10ms(10);
    GPIO_PORTG_DIR_R &= ~0x01; // make PG0 input (HiZ)
}