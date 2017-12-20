/**************************************************************************************************************************************

Project6
STM32- PWM Output to drive the motor
						
Contributions- Deepak Siddharth, Vedant Karia, Zeyar Win
**************************************************************************************************************************************/
#include "stm32l476xx.h"


void Timer_INIT(void);
void system_clock(void);
void GPIO_INIT(int PORT, int PIN, int Input, int Function_number);
int shift1;
int shift2;

enum I_O
{
    I,
    O, 
    AF
};


enum PORT	
{
	PORTA,
	PORTB,
	PORTC
};


void GPIO_INIT(int PORT, int PIN, int Input, int Function_number)
{
	

	RCC->AHB2ENR |=   RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOEEN;
	
	shift1 = (0x00000001 << (2* PIN));
	shift2 = (0x00000003 << (2* PIN));
	
	switch(PORT)
	{
		case PORTA:
				if(Input == I)
				{
						GPIOA->MODER &=  ~shift2;
						GPIOA->OSPEEDR |= shift1;
				}
				else if (Input == O)
				{
						GPIOA->MODER |=  shift1;
						GPIOA->OSPEEDR |= shift1;
				}
				else 
				{
					GPIOA->MODER &=  ~shift1;
					GPIOA->MODER |=   (shift1<<1);
					if(PIN < 8)
					{
						GPIOA->AFR[0] |= (Function_number << PIN*4);
					}
					else
					{
						GPIOA->AFR[1] &= (0xFFFFFFF0 << (PIN-8)*4);
						GPIOA->AFR[1] |= (Function_number << (PIN-8)*4);
					}
					
				}
		break;
			
		case PORTB:
			if(Input == I)
				{
						GPIOB->MODER &=  ~shift2;
						GPIOB->OSPEEDR |= shift1;
				}
				else if(Input == O)
				{
						GPIOB->MODER |=  shift1;
						GPIOB->OSPEEDR |= shift1;
				}
				
				else 
				{
					GPIOB->MODER &=  ~shift1;
					GPIOB->MODER |=   (shift1<<1);
					if(PIN < 8)
					{
							GPIOB->AFR[0] &= (0xFFFFFFF0 << PIN*4);
						GPIOB->AFR[0] |= (Function_number << PIN*4);
					}
					else
					{
						GPIOB->AFR[1] &= (0xFFFFFFF0 << (PIN-8)*4);
						GPIOB->AFR[1] |= (Function_number << (PIN-8)*4);
					}
					
				}
		break;
		
		case PORTC:
		if(Input == I)
				{
						GPIOC->MODER &=  ~shift2;
						GPIOC->OSPEEDR |= shift1;
				}
				else if(Input == O)
				{
						GPIOC->MODER |=  shift1;
						GPIOC->OSPEEDR |= shift1;
				}
				else 
				{
					GPIOC->MODER &=  ~shift1;
					GPIOC->MODER |=   (shift1<<1);
					if(PIN < 8)
					{
						GPIOC->AFR[0] &= (0xFFFFFFF0 << PIN*4);
						GPIOC->AFR[0] |= (Function_number << PIN*4);
					}
					else
					{
						GPIOC->AFR[1] &= (0xFFFFFFF0 << (PIN-8)*4);
						GPIOC->AFR[1] |= (Function_number << (PIN-8)*4);
					}
					
				}
		break;
		
		default:
			break;
		
	}
}

void system_clock()
{
    uint32_t HSITrim;

	// To correctly read data from FLASH memory, the number of wait states (LATENCY)
  // must be correctly programmed according to the frequency of the CPU clock
  // (HCLK) and the supply voltage of the device.		
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |=  FLASH_ACR_LATENCY_2WS;
		
	// Enable the Internal High Speed oscillator (HSI
	RCC->CR |= RCC_CR_HSION;
	while((RCC->CR & RCC_CR_HSIRDY) == 0);
	// Adjusts the Internal High Speed oscillator (HSI) calibration value
	// RC oscillator frequencies are factory calibrated by ST for 1 % accuracy at 25oC
	// After reset, the factory calibration value is loaded in HSICAL[7:0] of RCC_ICSCR	
	HSITrim = 16; // user-programmable trimming value that is added to HSICAL[7:0] in ICSCR.
	RCC->ICSCR &= ~RCC_ICSCR_HSITRIM;
	RCC->ICSCR |= HSITrim << 24;
	
	RCC->CR    &= ~RCC_CR_PLLON; 
	while((RCC->CR & RCC_CR_PLLRDY) == RCC_CR_PLLRDY);
	
	// Select clock source to PLL
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI; // 00 = No clock, 01 = MSI, 10 = HSI, 11 = HSE
	
	// Make PLL as 80 MHz
	// f(VCO clock) = f(PLL clock input) * (PLLN / PLLM) = 16MHz * 20/2 = 160 MHz
	// f(PLL_R) = f(VCO clock) / PLLR = 160MHz/2 = 80MHz
	RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLN) | 20U << 8;
	RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLM) | 1U << 4; // 000: PLLM = 1, 001: PLLM = 2, 010: PLLM = 3, 011: PLLM = 4, 100: PLLM = 5, 101: PLLM = 6, 110: PLLM = 7, 111: PLLM = 8

	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLR;  // 00: PLLR = 2, 01: PLLR = 4, 10: PLLR = 6, 11: PLLR = 8	
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN; // Enable Main PLL PLLCLK output 

	RCC->CR   |= RCC_CR_PLLON; 
	while((RCC->CR & RCC_CR_PLLRDY) == 0);
	
	// Select PLL selected as system clock
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL; // 00: MSI, 01:HSI, 10: HSE, 11: PLL
	
	// Wait until System Clock has been selected
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
	
	// The maximum frequency of the AHB, the APB1 and the APB2 domains is 80 MHz.
	RCC->CFGR &= ~RCC_CFGR_HPRE;  // AHB prescaler = 1; SYSCLK not divided
	RCC->CFGR &= ~RCC_CFGR_PPRE1; // APB high-speed prescaler (APB1) = 1, HCLK not divided
	RCC->CFGR &= ~RCC_CFGR_PPRE2; // APB high-speed prescaler (APB2) = 1, HCLK not divided
	
	// RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
	// RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;
	// RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP; 
	// RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLQ;	
	// RCC->PLLCFGR |= RCC_PLLCFGR_PLLPEN; // Enable Main PLL PLLSAI3CLK output enable
	// RCC->PLLCFGR |= RCC_PLLCFGR_PLLQEN; // Enable Main PLL PLL48M1CLK output enable
	
	RCC->CR &= ~RCC_CR_PLLSAI1ON;  // SAI1 PLL enable
	while ( (RCC->CR & RCC_CR_PLLSAI1ON) == RCC_CR_PLLSAI1ON );
	
	// Configure and enable PLLSAI1 clock to generate 11.294MHz 
	// 8 MHz * 24 / 17 = 11.294MHz
	// f(VCOSAI1 clock) = f(PLL clock input) *  (PLLSAI1N / PLLM)
	// PLLSAI1CLK: f(PLLSAI1_P) = f(VCOSAI1 clock) / PLLSAI1P
	// PLLUSB2CLK: f(PLLSAI1_Q) = f(VCOSAI1 clock) / PLLSAI1Q
	// PLLADC1CLK: f(PLLSAI1_R) = f(VCOSAI1 clock) / PLLSAI1R
	RCC->PLLSAI1CFGR &= ~RCC_PLLSAI1CFGR_PLLSAI1N;
	RCC->PLLSAI1CFGR |= 24U<<8;
	
	// SAI1PLL division factor for PLLSAI1CLK
	// 0: PLLSAI1P = 7, 1: PLLSAI1P = 17
	RCC->PLLSAI1CFGR |= RCC_PLLSAI1CFGR_PLLSAI1P;
	RCC->PLLSAI1CFGR |= RCC_PLLSAI1CFGR_PLLSAI1PEN;
	
	// SAI1PLL division factor for PLL48M2CLK (48 MHz clock)
	// RCC->PLLSAI1CFGR &= ~RCC_PLLSAI1CFGR_PLLSAI1Q;
	// RCC->PLLSAI1CFGR |= U<<21;
	// RCC->PLLSAI1CFGR |= RCC_PLLSAI1CFGR_PLLSAI1QEN;
	
	// PLLSAI1 division factor for PLLADC1CLK (ADC clock)
	// 00: PLLSAI1R = 2, 01: PLLSAI1R = 4, 10: PLLSAI1R = 6, 11: PLLSAI1R = 8
	// RCC->PLLSAI1CFGR &= ~RCC_PLLSAI1CFGR_PLLSAI1R; 
	// RCC->PLLSAI1CFGR |= U<<25;
	// RCC->PLLSAI1CFGR |= RCC_PLLSAI1CFGR_PLLSAI1REN;
	
	RCC->CR |= RCC_CR_PLLSAI1ON;  // SAI1 PLL enable
	while ( (RCC->CR & RCC_CR_PLLSAI1ON) == 0);
	
	// SAI1 clock source selection
	// 00: PLLSAI1 "P" clock (PLLSAI1CLK) selected as SAI1 clock
	// 01: PLLSAI2 "P" clock (PLLSAI2CLK) selected as SAI1 clock
	// 10: PLL "P" clock (PLLSAI3CLK) selected as SAI1 clock
	// 11: External input SAI1_EXTCLK selected as SAI1 clock	
	RCC->CCIPR &= ~RCC_CCIPR_SAI1SEL;

	RCC->APB2ENR |= RCC_APB2ENR_SAI1EN;
}



void Timer_INIT()
{
	
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    GPIO_INIT(PORTB,3,AF,1);
   // GPIO_INIT(PORTA,0,AF,1);
    TIM2->CR1 &= ~TIM_CR1_CEN;
	TIM2->ARR= 20000;
	TIM2->PSC= 79;
    TIM2->EGR |= TIM_EGR_UG;
    TIM2->DIER |= TIM_DIER_UIE;
  
		
		
//	TIM2->CCMR1 &=~(TIM_CCMR1_CC1S);
//	TIM2->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2);
//	TIM2->CCMR1 |= (TIM_CCMR1_OC1PE) ;
//	TIM2->CR1 |= (TIM_CR1_ARPE);
//  TIM2->CCMR1 |= (TIM_CCMR1_CC1S_0);
//  TIM2->CCMR1 |= (TIM_CCMR1_IC1F_0|TIM_CCMR1_IC1F_1);
//  TIM2->CCMR1 &= ~(TIM_CCMR1_IC1F_2|TIM_CCMR1_IC1F_2);
//  TIM2->CCMR1 &= ~(TIM_CCMR1_IC1PSC); 
//	TIM2->CCER  &= (TIM_CCER_CC1NP | TIM_CCER_CC1P);
//	TIM2->CCER |= TIM_CCER_CC1E;
	TIM2->CR1 |= TIM_CR1_ARPE;
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_1| TIM_CCMR1_OC1M_2;
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M_0;
	TIM2->CCMR1 &= ~(TIM_CCMR1_CC1S);
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE;
	TIM2->CCER |= TIM_CCER_CC1E;
	TIM2->CCR1 = 388;
	
	TIM2->CCMR1 |= TIM_CCMR1_OC2M_1| TIM_CCMR1_OC2M_2;
	TIM2->CCMR1 &= ~TIM_CCMR1_OC2M_0;
	TIM2->CCMR1 &= ~(TIM_CCMR1_CC2S);
	TIM2->CCMR1 |= TIM_CCMR1_OC2PE;
	TIM2->CCER |= TIM_CCER_CC2E;
	TIM2->CCR2 = 739;
	
	TIM2->CR1 |=  TIM_CR1_CEN;	

}




uint32_t value;
float motion;
int i=0;

int main()
{
	system_clock();		//Init system clock
	Timer_INIT();		//Init timer 
	GPIO_INIT(PORTA,0, I, 0);		//Init Input ports	
	GPIO_INIT(PORTA,1, I, 0);
	GPIO_INIT(PORTA,2, I, 0);
	GPIO_INIT(PORTA,3, I, 0);
	GPIO_INIT(PORTA,5, I, 0);


	//while(((GPIOA->IDR)&0x20)==0);  
	
	while(1)
	{
		
			value= (GPIOA->IDR)& 0x0F;  //Read the Port A values
			motion= ((1750/15)*value)+388;
			TIM2->CCR2= motion;			//Map the values to PWM dutycycle
		
	}
	return(0);
}