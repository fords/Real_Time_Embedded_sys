#include "UART.h"


// UART Ports:
// ===================================================
// PA.0 = UART4_TX (AF8)   |  PA.1 = UART4_RX (AF8)
// PB.6 = USART1_TX (AF7)  |  PB.7 = USART1_RX (AF7)
// PD.5 = USART2_TX (AF7)  |  PD.6 = USART2_RX (AF7)

void UART2_Init(void) {

	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
	RCC->CCIPR &= ~RCC_CCIPR_USART2SEL;
	RCC->CCIPR |=  RCC_CCIPR_USART2SEL_0;

	UART2_GPIO_Init();
	USART_Init(USART2);

	//NVIC_SetPriority(USART2_IRQn, 0);			// Set Priority to 1
	//NVIC_EnableIRQ(USART2_IRQn);					// Enable interrupt of USART1 peripheral
}

void UART2_GPIO_Init(void) {

	RCC->AHB2ENR |=   RCC_AHB2ENR_GPIODEN;


	GPIOD->MODER   &= ~(0xF << (2*5));	// Clear bits
	GPIOD->MODER   |=   0xA << (2*5);
	GPIOD->AFR[0]  |=   0x77<< (4*5);
	GPIOD->OSPEEDR |=   0xF<<(2*5);
	GPIOD->PUPDR   &= ~(0xF<<(2*5));
	GPIOD->PUPDR   |=   0x5<<(2*5);
	GPIOD->OTYPER  &=  ~(0x3<<5) ;
}


void USART_Init (USART_TypeDef * USARTx) {
	USARTx->CR1 &= ~USART_CR1_UE;  // Disable USART

	USARTx->CR1 &= ~USART_CR1_M;   // M: 00 = 8 data bits, 01 = 9 data bits, 10 = 7 data bits

	USARTx->CR1 &= ~USART_CR1_OVER8;  // 0 = oversampling by 16, 1 = oversampling by 8


	USARTx->CR2 &= ~USART_CR2_STOP;


	USARTx->BRR  = 0x208D; // Limited to 16 bits

	USARTx->CR1  |= (USART_CR1_RE | USART_CR1_TE);  	// Transmitter and Receiver enable

  if (USARTx == UART4){
		USARTx->CR1 |= USART_CR1_RXNEIE;  			// Received Data Ready to be Read Interrupt
		USARTx->CR1 &= ~USART_CR1_TCIE;    			// Transmission Complete Interrupt
		USARTx->CR1 &= ~USART_CR1_IDLEIE;  			// Idle Line Detected Interrupt
		USARTx->CR1 &= ~USART_CR1_TXEIE;   			// Transmit Data Register Empty Interrupt
		USARTx->CR1 &= ~USART_CR1_PEIE;    			// Parity Error Interrupt
		USARTx->CR1 &= ~USART_CR2_LBDIE;				// LIN Break Detection Interrupt Enable
		USARTx->CR1 &= ~USART_CR3_EIE;					// Error Interrupt Enable (Frame error, noise error, overrun error)
		//USARTx->CR3 &= ~USART_CR3_CTSIE;				// CTS Interrupt
	}

	if (USARTx == USART2){
		USARTx->ICR |= USART_ICR_TCCF;
		USART1->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;
	}

	USARTx->CR1  |= USART_CR1_UE; // USART enable

	while ( (USARTx->ISR & USART_ISR_TEACK) == 0); // Verify that the USART is ready for reception
	while ( (USARTx->ISR & USART_ISR_REACK) == 0); // Verify that the USART is ready for transmission
}


uint8_t USART_Read (USART_TypeDef * USARTx) {

	while (!(USARTx->ISR & USART_ISR_RXNE));
	return ((uint8_t)(USARTx->RDR & 0xFF));

}

void USART_Write(USART_TypeDef * USARTx, uint8_t *buffer, uint32_t nBytes) {
	int i;
	for (i = 0; i < nBytes; i++) {
		while (!(USARTx->ISR & USART_ISR_TXE));

		USARTx->TDR = buffer[i] & 0xFF;
		USART_Delay(300);
	}
	while (!(USARTx->ISR & USART_ISR_TC));
	USARTx->ISR &= ~USART_ISR_TC;
}


void USART_Delay(uint32_t us) {
	uint32_t time = 100*us/7;
	while(--time);
}
