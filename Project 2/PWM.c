/**************************************************************************************************************************************

PWM function 
 Version 8.1 -  	Final version with working Servo motor and recipie command
					
					
Resue of code -	 UART.c and LED.c from http://www.se.rit.edu/~swen-563/00/schedule.html
				 is used for initialising the UART, SYSTEM CLOCK and LED section of STM32 board.
**************************************************************************************************************************************/

#include "PWM.h"

/**PWM fucntion to setup the servo time periods and running**/
void timer_PWM()	
	{
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; 	//Enable clock for GPIOA
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN; 	//Enable clock for TIM2
	TIM2->PSC= 0x1F3F; 			//Prescalar is 7999+1 to make timer count as 100microseconds
	TIM2->EGR |=0x01;  			//Re-initialize and update of the registers. 
	GPIOA->MODER |= 0x0A;  	//Port A Pin0 and Pin 1 is enabled as Alternate function mode
	GPIOA->MODER &= ~(0x5); //Port A Pin0 and Pin 1 is enabled as Alternate function mode
	GPIOA->AFR[0] |= 0x011; 	//Select TMR2_CH1. Pin: PA0 an PA1 AF1-0001
	TIM2->CCMR1 |= 0x6868;		//OC1M Capture1 and Capture 2 in PWM Mode
	TIM2->CR1 |=0x80;			//Autoreload update 
	TIM2->CCER |=  TIM_CCER_CC1E | TIM_CCER_CC2E;
	TIM2->EGR |=0x01;			//Re-initialize and update of the registers. 
	TIM2->ARR = 0xC8;			//Reload counter is set to 200.
	TIM2->EGR |=0x01;			//Re-initialize and update of the registers.
	TIM2->CCR1 |= 0x14;			//Default position servoA set to 2microseconds
	TIM2->CCR2 |= 0x14;			//Default position servoB set to 2microseconds		
	TIM2->CR1 |=0x01;			//Enable timer2
	}

/**Initialise timer 5 as interrupt**/
void timer_wait()
{
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN; 	//	Enable clock for TIM5
	TIM5->PSC= 0x1F3F; 						//	Prescalar is 7999 to make timer count as 100microseconds
	TIM5->EGR |=0x01;  						//	Re-initialize and update of the registers.
	TIM5->ARR = 9000; 						//	One second delay timer-10000
	NVIC_EnableIRQ(TIM5_IRQn);				//	Enable NVIC for TIM5 IRQ
	TIM5->DIER = TIM_DIER_UIE; 				// 	Enable interrupt 
	TIM5->CR1 |= 0x01;   					//Enable timer5
}	


