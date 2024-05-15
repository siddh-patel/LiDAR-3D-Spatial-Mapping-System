/*
 * COMPENG 2DX3 Final Project Code
 * Written by Siddh Patel
 * Last Updated: March 31th, 2024
 */

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
#include "OnBoardButton.h"


// Global Variable
#define SAMPLES 64
#define TOTAL_Z_STEPS 3


uint16_t dev = 0x29; // address of the ToF sensor as an I2C slave peripheral
int status = 0;
int spinMotor = 0;
int distances[SAMPLES]; // array to store measurements

void GPIOJ_IRQHandler(void)
{
		// If button is pressed then start or stop motor
    spinMotor = ~spinMotor;
    // Acknowledge flag by setting proper bit in ICR register
    GPIO_PORTJ_ICR_R = 0x02;
}

int main(void)
{
    // initialize
    PLL_Init();
    PortH_Init();
    SysTick_Init();
    onboardLEDs_Init();
    I2C_Init();
    UART_Init();
    PortJ_Init();
    PortJ_Interrupt_Init();
		
		uint16_t Distance;
    uint8_t RangeStatus;
    uint8_t dataReady;
    uint8_t sensorState = 0;
		uint16_t SignalRate;
		uint16_t AmbientRate;
		uint16_t SpadNum; 	
		
		while (sensorState == 0)
    {
				// check if the sensor has been booted
        status = VL53L1X_BootState(dev, &sensorState);
        SysTick_Wait10ms(10);
    }
		//Flash all LEDs when program loaded
    FlashAllLEDs();

    status = VL53L1X_ClearInterrupt(dev); /* clear interrupt has to be called to enable next interrupt*/
    status = VL53L1X_SensorInit(dev); // Initialize the sensor
    Status_Check("SensorInit", status);// Check the status of the sensor
    status = VL53L1X_StartRanging(dev); /* This function has to be called to enable the ranging */
    Status_Check("StartRanging", status); // Check if sensor has started ranging.
		
    while (1)
    { 
        if (spinMotor)
        {
            // Loop for the total amount of z steps
						for (int t = 0; t < TOTAL_Z_STEPS; t++)
            {
								// Spin the motor for the desired angle to complete 1 revolution
                for (int i = 0; i < SAMPLES; i++)
                {
										// Turn on LED 1 when the motor is spinning to collect data
										GPIO_PORTN_DATA_R ^= 0b00000010;
                    while (dataReady == 0)
                    {
                        // Check if the MCU is read to collect data
												status = VL53L1X_CheckForDataReady(dev, &dataReady);
                        VL53L1_WaitMs(dev, 2);
                    }
										
                    dataReady = 0;
                    status = VL53L1X_GetRangeStatus(dev, &RangeStatus); // make sure status ok
                    status = VL53L1X_GetDistance(dev, &Distance);       // Measure the distance
                    FlashLED2(1); // Flash Led 2 to indicate that data has been taken.
                    distances[i] = Distance;              // Place distance in array
                    status = VL53L1X_ClearInterrupt(dev); /* clear interrupt has to be called to enable next interrupt*/

                    // spins 5.625 deg -> 64 measurements
                    spin(512 / SAMPLES, CLOCKWISE);
										GPIO_PORTN_DATA_R ^= 0b00000010; // Turn off LED 1 when data has been done collecting and is spinning to return to home
										
                }
								
                static char signal;
                for (int i = 0; i < SAMPLES; i++)
                { // print values to uart one by one from array
                    sprintf(printf_buffer, "%u,", distances[i]);
                    signal = UART_InChar(); // wait for acknowledgement from PC
                    if (signal == 0x31)
                    {
                        UART_printf(printf_buffer); // send the measurement
                    }
                }
                // spin back to starting position
                spin(512, COUNTER_CLOCKWISE);
                spinMotor = 0;
            }
        }
    }
}