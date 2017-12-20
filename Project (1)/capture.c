/*******************************************************
capture function  
 Version 12.1 -  	Final version with working Histogram program.
									PWM capture initialisation, POST function and getlimits
									Updated Comments

Resue of code -	 UART.c and LED.c from http://www.se.rit.edu/~swen-563/00/schedule.html
								 is used for initialising the UART and LED section of STM32 board.
*******************************************************/

#include "capture.h"

/** Input Capture Function entry **/	
void timer_capture()	
{
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; 	//enable clock for GPIOA
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN; //enable clock for TIM2
	TIM2->PSC= 0x4F; 											//Set Prescalar is 79
	TIM2->EGR |=0x01;											//Reinitalise the register
	GPIOA->MODER |= 0x02; 								//Port A Pin0 is enabled as Alternate function mode
	GPIOA->MODER &= ~(0x1);								//Port A Pin0 is enabled as Alternate function mode
	GPIOA->AFR[0] |= 0x01; 							  //Select TMR2_CH1. Pin: PA0. AF1-0001
	TIM2->CCMR1 |= 0x01; 									//CC1S is 01 on T1
	TIM2->CCER |= 0x0000; 								//CC2NP:CC2P= 00 because rising edge.
  TIM2->CCER |=  TIM_CCER_CC1E;					//CC2NP:CC2P= 00 because rising edge.
	TIM2->CR1 |=0x01;											//Capture enable
}



/** Post Function entry **/	
int POST_function(void)
{
	uint32_t t1=0, t2=0,delta=0;
	uint16_t test=0; 
	
		while (test!=2)		//Compute delta for 1000 samples
		{
			if((TIM2->SR & TIM_SR_CC1IF)==TIM_SR_CC1IF)
				{
						t2= TIM2->CCR1;
						delta= t2-t1;
						t1=t2;
						test++;
				}
		}
		if(delta<100000)	//Samples in Range
			{
				USART_Write(USART2, (uint8_t *)result_pass, strlen(result_pass)); 
				flag=0;
				mutex_lock= 1;
				test=0;
			}
			else						//Samples out of Range
			{	
				mutex_lock= 2;
				//test=0;
				flag=0;
			} 
		return 1;
}

/** Get limits function entry **/	
void getlimits(uint16_t* x,uint16_t* y)
{
	char val1[]= "\r\nEnter lower limits for the from 50ms all the way to 9950ms: ";
	char val2[]= "\r\nYour upper limit is 100ms plus lower limit";
	char val3[]= "\r\nBucket lower bound is: ";
	char val4[]= "\r\nBucket upper bound is: ";
	
	uint8_t rec_val[5]; 
	char * rec_val2;
	unsigned short out=0;
	USART_Write(USART2, (uint8_t *) val1, strlen(val1)); 

/*Input the lower bound*/
	for(int i=0;i<5;i++)
	{
		rec_val[i] = USART_Read(USART2);
		*rec_val2=rec_val[i];
		USART_Write(USART2,rec_val2, strlen(rec_val2)); 
		if((rec_val[i]== 0xd)|| i>=5)
		break;
	}	
	USART_Write(USART2, (uint8_t *) val3, strlen(val3)); 
	USART_Write(USART2,rec_val, strlen(rec_val)); 
	USART_Write(USART2, (uint8_t *) val2, strlen(val2)); 
	out=atoi(rec_val);
	*x= out;
	*y= out+100;
	tempval=out+100;
	/*Display the output*/
	USART_Write(USART2, (uint8_t *) val4, strlen(val4)); 
	
	num2 += sprintf((char *)dec_str2, "%d", tempval);
	USART_Write(USART2, dec_str2, num2);	

}


