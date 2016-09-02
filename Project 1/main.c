/*******************************************************

main function
program displays the upper and lower limits and allows the user to either accept those values or to change the lower limit to a new value.
• The lower limit can be from 50 microseconds to 9950 microseconds.
• The upper limit will always be 100 microseconds longer than the current lower limit.
*******************************************************/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32l476xx.h"

/****** User defined Header files *******/
#include "SysClock.h"
#include "LED.h"
#include "UART.h"
#include "main.h"



/** main function program entry **/
int main(void){

	System_Clock_Init();
	LED_Init();
	UART2_Init();
	timer_capture();
	USART_Write(USART2, (uint8_t *)start, strlen(start));
	while (1)
{
	mutex_lock = START
	if(mutex_lock == START)     //Wait to press 'n' to start a new bucket of measurement.
	{
		while(!(USART_Read(USART2)=='n'));
		mutex_lock = POST;					//Go to Post function block
		Red_LED_On();


		if(POST_function()==1)
			{
			getlimits(&low_bound,&up_bound);	//Print the lower bound and upper bound
			if (mutex_lock==POST_FAIL)		//Post function check for buckets in range failed.
				{
					USART_Write(USART2, (uint8_t *)result_fail, strlen(result_fail));
					USART_Write(USART2, (uint8_t *)freq, strlen(freq));
					mutex_lock=START;			//Go back to start
				}
				else
				{
					Red_LED_Off();
					mutex_lock=POST_SUCCESS;			//Proceed to calculate.
					flag=0;
				}
			}
		}
}
}
