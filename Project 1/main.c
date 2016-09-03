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
			if(mutex_lock==START)     //Wait to press 'n' to start a new bucket of measurement.
			{
				while(!(USART_Read(USART2)=='n'));
				mutex_lock=POST;
				Red_LED_On();
			}
			else if(mutex_lock==POST)
			{

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
			else if(mutex_lock==POST_SUCCESS)
			{
				Green_LED_On();
					if(flag==0)
						{
							if((TIM2->SR & TIM_SR_CC1IF)==TIM_SR_CC1IF)
								{
								t2= TIM2->CCR1;
								dt[counter]= t2-t1;
								t1=t2;
								counter++;
								if(counter>=1002)
								flag=HISTOGRAM;
								}
						}

				else if(flag==HISTOGRAM)
							{
							for(int i = 0; i < 1002; i++)
								{
									for(int j = 1; j < 1002-1; j++)
										{
											if(dt[j] < dt[i])
											{
												temp = dt[i];
												dt[i] = dt[j];
												dt[j] = temp;
											}
										}
								}


		  prev = dt[1];

		  for (int i = 2; i < 1002; i++)
				{
					if (dt[i] == prev)
					{
							count++;
					}
					else
					{
						num = sprintf((char *)dec_str, "\nSample = %d\t", prev);
						num += sprintf((char *)dec_str+num, "Count = %d\r\n", count);
						USART_Write(USART2, dec_str, num);
						prev = dt[i];
						count = 1;

					}
		  }
						num = sprintf((char *)dec_str, "\nSample = %d\t", prev);
						num += sprintf((char *)dec_str+num, "Count = %d\r\n", count);
						USART_Write(USART2, dec_str, num);
						prev=count=counter=0;
						count=1;
						mutex_lock=START;

		}
	Green_LED_Off();
		}

		}
}
