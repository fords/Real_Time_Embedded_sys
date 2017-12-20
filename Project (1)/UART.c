#include "UART.h"


// UART Ports:
// ===================================================
// PA.0 = UART4_TX (AF8)   |  PA.1 = UART4_RX (AF8)      
// PB.6 = USART1_TX (AF7)  |  PB.7 = USART1_RX (AF7) 
// PD.5 = USART2_TX (AF7)  |  PD.6 = USART2_RX (AF7)

void UART2_Init(void) {
	// Enable the clock of USART 1 & 2
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;  // Enable USART 2 clock		
	
	// Select the USART1 clock source
	// 00: PCLK selected as USART2 clock
	// 01: System clock (SYSCLK) selected as USART2 clock
	// 10: HSI16 clock selected as USART2 clock
	// 11: LSE clock selected as USART2 clock
	RCC->CCIPR &= ~RCC_CCIPR_USART2SEL;
	RCC->CCIPR |=  RCC_CCIPR_USART2SEL_0;
	
	UART2_GPIO_Init();
	USART_Init(USART2);
	
	//NVIC_SetPriority(USART2_IRQn, 0);			// Set Priority to 1
	//NVIC_EnableIRQ(USART2_IRQn);					// Enable interrupt of USART1 peripheral
}

void UART2_GPIO_Init(void) {
	
	// Enable the peripheral clock of GPIO Port
	RCC->AHB2ENR |=   RCC_AHB2ENR_GPIODEN;
	
	// ********************** USART 2 ***************************
	// PD5 = USART2_TX (AF7)
	// PD6 = USART2_RX (AF7)
	// Alternate function, High Speed, Push pull, Pull up
	// **********************************************************
	// Input(00), Output(01), AlterFunc(10), Analog(11)
	GPIOD->MODER   &= ~(0xF << (2*5));	// Clear bits
	GPIOD->MODER   |=   0xA << (2*5);      		
	GPIOD->AFR[0]  |=   0x77<< (4*5);       	
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	GPIOD->OSPEEDR |=   0xF<<(2*5); 					 	
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOD->PUPDR   &= ~(0xF<<(2*5));
	GPIOD->PUPDR   |=   0x5<<(2*5);    				
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1) 
	GPIOD->OTYPER  &=  ~(0x3<<5) ;       	
}


void USART_Init (USART_TypeDef * USARTx) {
	// Default setting: 
	//     No hardware flow control, 8 data bits, no parity, 1 start bit and 1 stop bit		
	USARTx->CR1 &= ~USART_CR1_UE;  // Disable USART
	
	// Configure word length to 8 bit
	USARTx->CR1 &= ~USART_CR1_M;   // M: 00 = 8 data bits, 01 = 9 data bits, 10 = 7 data bits
	
	// Configure oversampling mode: Oversampling by 16 
	USARTx->CR1 &= ~USART_CR1_OVER8;  // 0 = oversampling by 16, 1 = oversampling by 8
	
	// Configure stop bits to 1 stop bit
	//   00: 1 Stop bit;      01: 0.5 Stop bit
	//   10: 2 Stop bits;     11: 1.5 Stop bit
	USARTx->CR2 &= ~USART_CR2_STOP;   
                                    
	// CSet Baudrate to 9600 using APB frequency (80,000,000 Hz)
	// If oversampling by 16, Tx/Rx baud = f_CK / USARTDIV,  
	// If oversampling by 8,  Tx/Rx baud = 2*f_CK / USARTDIV
  // When OVER8 = 0, BRR = USARTDIV
	// USARTDIV = 80MHz/9600 = 8333 = 0x208D
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
	// SR_RXNE (Read data register not empty) bit is set by hardware
	while (!(USARTx->ISR & USART_ISR_RXNE));  // Wait until RXNE (RX not empty) bit is set
	// USART resets the RXNE flag automatically after reading DR
	return ((uint8_t)(USARTx->RDR & 0xFF));
	// Reading USART_DR automatically clears the RXNE flag 
}

void USART_Write(USART_TypeDef * USARTx, uint8_t *buffer, uint32_t nBytes) {
	int i;
	// TXE is cleared by a write to the USART_DR register.
	// TXE is set by hardware when the content of the TDR 
	// register has been transferred into the shift register.
	for (i = 0; i < nBytes; i++) {
		while (!(USARTx->ISR & USART_ISR_TXE));   	// wait until TXE (TX empty) bit is set
		// Writing USART_DR automatically clears the TXE flag 	
		USARTx->TDR = buffer[i] & 0xFF;
		USART_Delay(300);
	}
	while (!(USARTx->ISR & USART_ISR_TC));   		  // wait until TC bit is set
	USARTx->ISR &= ~USART_ISR_TC;
}   
 

void USART_Delay(uint32_t us) {
	uint32_t time = 100*us/7;    
	while(--time);   
}
