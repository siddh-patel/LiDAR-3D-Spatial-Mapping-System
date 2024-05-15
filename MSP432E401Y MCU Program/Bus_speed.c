#include <stdint.h>
#include "PLL.h"
#include "SysTick.h"
#include "uart.h"
#include "onboardLEDs.h"
#include "tm4c1294ncpdt.h"
#include "VL53L1X_api.h"
#include "init.h"
#include "Variables.h"
#include "StepperMotor.h"


void PortM_Init(void){
	//Use PortN onboard LED	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R11;				// activate clock for Port M
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R11) == 0){};	// allow time for clock to stabilize
	GPIO_PORTM_DIR_R |= 0x1;												// make PM0 out
	GPIO_PORTM_AFSEL_R &= ~0x1;		 								// disable alt funct on PM0
	GPIO_PORTM_DEN_R |= 0x1;												// enable digital I/O on PM0
																									// configure PN0 as GPIO
	//GPIO_PORTN_PCTL_R = (GPIO_PORTN_PCTL_R&0xFFFFFF0F)+0x00000000;
	GPIO_PORTM_AMSEL_R &= ~0x1;		 								// disable analog functionality on PM0		
	
	return;
}

void bus_demo(void)
{
    GPIO_PORTM_DATA_R ^= 0x01;
    SysTick_Wait(960000);
    GPIO_PORTM_DATA_R ^= 0x01;
    SysTick_Wait(960000);
}
