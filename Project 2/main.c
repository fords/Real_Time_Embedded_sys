/**************************************************************************************************************************************

main function
 Version 8.1 -
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
			/***Servo1 control program***/
			if(input1 != 0x58 && input1!= 0x78) //Check ASCII Hex value of X and x to terminate
				{
				/**Switch case for servo A. p=pause, b=break, c=contnue, r=rotate right, l=rotate left, n=nop**/
				switch ( input1)
						{
						case 'c':					//CASE continue
						case 'C':
							flagsyncA=0;
							statusA= run;				//Continue State Green Led ON
							Green_LED_On();
							Red_LED_Off();
							break;
						case 'p':					//CASE pause so update status and halt the servo A
						case 'P':
							flagsyncA=1;
							statusA= pause;
							Green_LED_Off();
							Red_LED_On();				//Pause State Red Led ON
							break;
						case 'l':					//CASE Rotate Left
						case 'L':
							if(flagsyncA==1)
							TIM2->CCR1=TIM2->CCR1-4;
							break;
						case 'r':					//CASE Rotate Right
						case 'R':
							if(flagsyncA==1)
							TIM2->CCR1=TIM2->CCR1+4;
							break;
						case 'n':					//CASE No-Operation
						case 'N':
							break;
						case 'b':					//CASE Break
						case 'B':
							flagsyncA=0;
								servoA.cmdparam = &buf_servoA[0]; 						//Re-Start the recipe again for servo A
								statusA=run;
								Green_LED_On();
								Red_LED_Off();
							break;
						}
				}
			else
				{
				USART_Write(USART2, (uint8_t *)command4, strlen(command4));	 //Invalid command entry for servo A
				}

			}
		else
			USART_Write(USART2, (uint8_t *)command6, strlen(command6));	  //Invalid termination Enter not pressed.
	}


/*user defined function to realise the recipie command*/
void runsnipp(void)
{
	static uint8_t opcodeA,paramA,opcodeB,paramB,loopcntA,loopcntB,flagA,flagB,*looptrA,*looptrB,runFA=0,runFB=0,waitA,waitB,paramA_L,paramB_L;

	// opcodeX:Bit5-7 in the Mnemonics, paramX: Bit0-4 in the Mnemonics, loopcntX: No of count times looping,
	// flagX: to sync between start and end loop, runX: Flag for no of Wait states, WaitX: is the wait counter

	if(statusA!=pause)
		{
     opcodeA=decodeOpcode(*(servoA.cmdparam));		//decode the Mnemonic
     paramA=decodeRange(*(servoA.cmdparam));			//decode the Range
			switch(opcodeA)
			{
				case 0:																		//Opcode Instruction=Recipie End
					TIM2->CCR1 =0;
					break;

        case 1:																		//Opcode Instruction=Move
					if(paramA<=6)
						TIM2->CCR1 = paramA*4;
					else
					{
						statusA= error;
						Green_LED_On();												//Green and Red LED ON to show illegal operation
						Red_LED_On();
					}
					break;

        case 2:																		//Opcode Instruction=Wait
						if(paramA<=32 && paramA>0)
						{
						if(runFA==0)
						{
							waitA=paramA;
							runFA=1;
						}
						else
						{
						waitA-=1;
						if(waitA==0)
						runFA=0;
						}
					}
				else
					{
						statusA= error;												//Green and Red LED ON to show illegal operation
						Green_LED_On();
						Red_LED_On();
					}
				break;

				case 3:  																	//Opcode Instruction=Skip
					servoA.cmdparam++;
         break;

				case 4:																		//Opcode Instruction=Loop Start
					if(paramA<=31)
            {
            if(flagA==0)
            {
							loopcntA=paramA;
							looptrA=servoA.cmdparam;
							flagA=1;
						}
						}
					else
					{
						statusA= error;
						Green_LED_On();													//Green and Red LED ON to show illegal operation
						Red_LED_On();
					}
					break;


			case 5:																				//Opcode Instruction=Loop End
				if(flagA==1)
					{
					if(loopcntA!=0)
						{
						loopcntA-=1;
						servoA.cmdparam=looptrA;
						}
					else
						flagA=0;
				}
				break;
            case 7:																	//Opcode Instruction=Break Loop
						loopcntA=0;
				break;
            }

		if(opcodeA!=RECIPE_END)
      {
				if(runFA==0)
				servoA.cmdparam++;						//Next Instruction increment
			}
			else
			{
				//do nothing
			}
        paramA_L=paramA;

	}
		else
		{
			//do nothing
		}


		}
     paramB_L=paramB;

			}
else
		{
			//do nothing
		}

}


/**user defined program to set different recipie commands for the motors to execute**/

void testsnipp()
{

	servoA.cmdparam= &buf_servoA[0];
	servoB.cmdparam= &buf_servoB[0];



//6 Positions A-PA0
	  *servoA.cmdparam= MOV+0;
    *(servoA.cmdparam++) =  MOV+1;
    *(servoA.cmdparam++) =  MOV+2;
    *(servoA.cmdparam++) =  MOV+3;
    *(servoA.cmdparam++) =  MOV+4;
    *(servoA.cmdparam++) =  MOV+5;
		*(servoA.cmdparam++) =  MOV+6;
    *(servoA.cmdparam++) =  RECIPE_END;

//6 Positions B-PA1
	  *servoB.cmdparam= MOV+0;
    *(servoB.cmdparam++) =  MOV+1;
    *(servoB.cmdparam++) =  MOV+2;
    *(servoB.cmdparam++) =  MOV+3;
    *(servoB.cmdparam++) =  MOV+4;
    *(servoB.cmdparam++) =  MOV+5;
		*(servoB.cmdparam++) =  MOV+6;
    *(servoB.cmdparam++) =  RECIPE_END;


/*


//Skip A-PA0- Position 3
	  *servoA.cmdparam= MOV+0;
    *(servoA.cmdparam++) =  MOV+1;
    *(servoA.cmdparam++) =  MOV+2;
		*(servoA.cmdparam++) =  SKIP;
    *(servoA.cmdparam++) =  MOV+3;
    *(servoA.cmdparam++) =  MOV+4;
    *(servoA.cmdparam++) =  MOV+5;
		*(servoA.cmdparam++) =  MOV+6;
    *(servoA.cmdparam++) =  RECIPE_END;

//Skip B-PA1- Position 3
	  *servoB.cmdparam= MOV+0;
    *(servoB.cmdparam++) =  MOV+1;
    *(servoB.cmdparam++) =  MOV+2;
		*(servoB.cmdparam++) =  SKIP;
    *(servoB.cmdparam++) =  MOV+3;
    *(servoB.cmdparam++) =  MOV+4;
    *(servoB.cmdparam++) =  MOV+5;
		*(servoB.cmdparam++) =  MOV+6;
    *(servoB.cmdparam++) =  RECIPE_END;

*/

/*
//Loop test A-PA0-  4times all positions

	   *servoA.cmdparam= MOV+0;
    *(servoA.cmdparam++) =  LOOP_START+4;
    *(servoA.cmdparam++) =  MOV+1;
    *(servoA.cmdparam++) =  MOV+2;
    *(servoA.cmdparam++) =  MOV+3;
    *(servoA.cmdparam++) =  MOV+4;
    *(servoA.cmdparam++) =  MOV+5;
		*(servoA.cmdparam++) =  MOV+6;
    *(servoA.cmdparam++) =  LOOP_END;
    *(servoA.cmdparam++) =  RECIPE_END;

//Loop test B-PA1-  4times all positions
	   *servoB.cmdparam= MOV+0;
    *(servoB.cmdparam++) =  LOOP_START+4;
    *(servoB.cmdparam++) =  MOV+1;
    *(servoB.cmdparam++) =  MOV+2;
    *(servoB.cmdparam++) =  MOV+3;
    *(servoB.cmdparam++) =  MOV+4;
    *(servoB.cmdparam++) =  MOV+5;
		*(servoB.cmdparam++) =  MOV+6;
    *(servoB.cmdparam++) =  LOOP_END;
    *(servoB.cmdparam++) =  RECIPE_END;

*/
/*
//ROR test A-PA0-  2 positions
    *servoA.cmdparam= MOV+0;
		*(servoA.cmdparam++) =  LOOP_START+4;
    *(servoA.cmdparam++) =  MOV+1;
    *(servoA.cmdparam++) =  MOV+3;
		*(servoA.cmdparam++) =  BREAK;
		*(servoA.cmdparam++) =  LOOP_END;
    *(servoA.cmdparam++) =  RECIPE_END;

//ROR test B-PA1-  2 positions
    *servoB.cmdparam= MOV+0;
		*(servoB.cmdparam++) =  LOOP_START+4;
    *(servoB.cmdparam++) =  MOV+1;
    *(servoB.cmdparam++) =  MOV+3;
		*(servoB.cmdparam++) =  BREAK;
		*(servoB.cmdparam++) =  LOOP_END;
    *(servoB.cmdparam++) =  RECIPE_END;
*/

/*
//Test Snippt A-PA0
*(servoA.cmdparam) = MOV+0; //There must NOT be intervening instructions in this group to allow
*(servoA.cmdparam++) = MOV+5 ; //verification of default time delay.
*(servoA.cmdparam++) = MOV+1;
*(servoA.cmdparam++) = MOV+3;
*(servoA.cmdparam++) = LOOP_START+2 ;//Test the default loop behavior.
*(servoA.cmdparam++) = MOV+1;
*(servoA.cmdparam++) = MOV+4;
*(servoA.cmdparam++) = LOOP_END;
*(servoA.cmdparam++) = MOV+0;
*(servoA.cmdparam++) = MOV+2;
*(servoA.cmdparam++) = WAIT_TIME+0;
*(servoA.cmdparam++) = MOV+3 ; //Move to an adjacent position to verify
*(servoA.cmdparam++) = WAIT_TIME+0;
*(servoA.cmdparam++) = MOV+2;
*(servoA.cmdparam++) = MOV+3 ; //Measure the timing precision of the 9.3 second delay with an external
*(servoA.cmdparam++) = WAIT_TIME+3; //timer.
*(servoA.cmdparam++) = WAIT_TIME+3;
*(servoA.cmdparam++) = WAIT_TIME+3;
*(servoA.cmdparam++) = MOV+4;

*/

/*
// //Test Snippt B-PA1

*(servoB.cmdparam) = MOV+0; //There must NOT be intervening instructions in this group to allow
*(servoB.cmdparam++) = MOV+5 ; //verification of default time delay.
*(servoB.cmdparam++) = MOV+1;
*(servoB.cmdparam++) = MOV+3;
*(servoB.cmdparam++) = LOOP_START+2 ;//Test the default loop behavior.
*(servoB.cmdparam++) = MOV+1;
*(servoB.cmdparam++) = MOV+4;
*(servoB.cmdparam++) = LOOP_END;
*(servoB.cmdparam++) = MOV+0;
*(servoB.cmdparam++) = MOV+2;
*(servoB.cmdparam++) = WAIT_TIME+0;
*(servoB.cmdparam++) = MOV+3 ; //Move to an adjacent position to verify
*(servoB.cmdparam++) = WAIT_TIME+0;
*(servoB.cmdparam++) = MOV+2;
*(servoB.cmdparam++) = MOV+3 ; //Measure the timing precision of the 9.3 second delay with an external
*(servoB.cmdparam++) = WAIT_TIME+3; //timer.
*(servoB.cmdparam++) = WAIT_TIME+3;
*(servoB.cmdparam++) = WAIT_TIME+3;
*(servoB.cmdparam++) = MOV+4;
*/

  servoA.cmdparam= &buf_servoA[0];
	servoB.cmdparam= &buf_servoB[0];

}
