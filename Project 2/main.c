/**************************************************************************************************************************************

main function
 Servo motor and recipie command

**************************************************************************************************************************************/


#include "stm32l476xx.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/****** User defined Header files *******/
#include "SysClock.h"
#include "LED.h"
#include "UART.h"
#include "main.h"


/***TIM5_IRQHandler***/
void TIM5_IRQHandler (void)
{
  if ((TIM5->SR & 0x0001) != 0) // Check if Interrupt is set?
	{
	if(test==0)
		{
		Red_LED_Toggle();		//Test purpose
		Green_LED_Toggle();		//Test purpose
		test=1;
		}
	else if(test==1)
		{
		Green_LED_Toggle(); //Test purpose
		Red_LED_Toggle();		//Test purpose
		test=0;
		}
    TIM5->SR &= ~(1<<0);        // Clear Interrupt Flag

 }
}


int main(void){

	System_Clock_Init(); 			// Set system clock = 80 MHz
  testsnipp();							// Initialize the test snippet code
	LED_Init();								// Initialize LED
	UART2_Init();							// Initialize UART
	timer_PWM();							// Initialize PWM
	timer_wait();							// Initialize Timer Interrupt
	Green_LED_Off();					// Reset LED before begin
	Red_LED_Off();						// Reset LED before begin
  TIM5_IRQHandler();


}
