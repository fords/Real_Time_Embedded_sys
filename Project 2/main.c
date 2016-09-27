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
		/*Red_LED_Toggle();		//Test purpose
		Green_LED_Toggle();		//Test purpose*/
		test=1;
		}
	else if(test==1)
		{
		/*Green_LED_Toggle(); //Test purpose
		Red_LED_Toggle();		//Test purpose*/
		test=0;
		}
    TIM5->SR &= ~(1<<0);        // Clear Interrupt Flag
	runsnipp();
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

	while (1)
		{
			/***Enter Command1 for controlling Servo 1***/
			USART_Write(USART2, (uint8_t *)command1, strlen(command1));
			rx_cmd1= USART_Read(USART2);
			m = sprintf((char *)buffer, "%c", rx_cmd1);
			USART_Write(USART2, buffer, m);

			/***Enter Command2 for controlling Servo 2***/
			USART_Write(USART2, (uint8_t *)command2, strlen(command2));
			rx_cmd2= USART_Read(USART2);
			n = sprintf((char *)buffer, "%c", rx_cmd2);
			USART_Write(USART2, buffer, n);

			/***Press Enter to Start/Alter servo operation***/
			USART_Write(USART2, (uint8_t *)command3, strlen(command3));
			rx_cmd3= USART_Read(USART2);

			user_command(rx_cmd1,rx_cmd2,rx_cmd3);
		}

}

	/***User defined function called to control servo1 and servo2***/
	void user_command(uint8_t input1, uint8_t input2, uint8_t input3)
	{

		if ( input3 == 0xd) 									//Check if Enter is presssed
			{
        sprintf("enter");
			}
  }
