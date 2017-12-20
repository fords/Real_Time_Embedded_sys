#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <sys/neutrino.h>
#include <stdint.h>
#include <termios.h>
#include <sys/mman.h>	/* for mmap_device_io() */
#include <unistd.h>		/* for sleep() */
#include <hw/inout.h>	/* for in*() and out*() functions */

unsigned int time_value_micro();

#define LOW 0x00
#define HIGH 0xFF

int backup;
int delay_time;
#define BASE_ADDRESS 0x280
/* The Neutrino IO port used here corresponds to a single register which is one byte long */
#define PORT_LENGTH 1
/* The first parallel port usually starts at 0x378. Each parallel port is three bytes wide. The first byte
   is the Data register, the second byte is the status register and the third byte is the control register */

#define BASE_ADDRESS 0x280
#define OUTPUT_ADDRESS 0x288
#define INPUT_ADDRESS 0x289
#define CTRL_ADDRESS 0x28B
char echo_byte;
unsigned int time_prev,time_aft,privity_err ;
uintptr_t ctrl_handle;
uintptr_t data_handleA,data_handleB;

int execute[100];
int execute2[100];
int input1;
int input2;
char  buffer[2];

uint16_t servo1_state=0;
uint16_t servo2_state=0;
int i=0, j=0;
int value;
unsigned int time1;
unsigned int period= 20000;

unsigned long position1= 388;
unsigned long position2= 1439;
int end1=0, end0=0;

int loop_number=0;
int loop_start=0;
int loop_number2=0;
int loop_start2=0;

char  motor1;
char motor2;
uint8_t position_motor1=0;
uint8_t position_motor2=0;
uint8_t Motor1_uart_pause=0;
uint8_t Motor2_uart_pause=0;
volatile uint8_t Pause_motor1=0;
volatile uint8_t Pause_motor2=0;


int opcode= 0xE0;

#define Mov (0x20)
#define LOOP   (0x80)
#define  END_LOOP (0xA0)
#define WAIT (0x40)
#define JUMP (0x60)
#define  LABEL (0xB0)
#define RECIPE_END (0x00)
#define  RESET (0xE0)


FILE *filename;


uint8_t  Recipe[100] = {Mov+2, Mov+5, Mov+0, Mov+3, Mov+5, WAIT+ 31, WAIT+ 31, WAIT+ 31, Mov+3, Mov+2, Mov+5, Mov+1, RECIPE_END};

int position_values[6]= {430,690,955,1220,1485,1750};

void Move_motor1( uint8_t value);
void Move_motor2( uint8_t value);
void Command_entered();
void delay_motor1( uint8_t value);
void delay_motor2( uint8_t value);

/*Update the pulse width for servo 1*/
void *Servo_PWM1(void *arg )
{
	while(1)
	{
		servo1_state = in8(data_handleA);
		servo1_state = servo1_state|0x01;
		out8(data_handleA, servo1_state);
		//time1= time_value_micro();
		nanospin_ns(position1*1000);		
		servo1_state = servo1_state & 0xFE;
		out8(data_handleA, servo1_state);
		//time1= time_value_micro();
		usleep((period - position1));
	}

}

/*Update the pulse width for servo 2*/
void *Servo_PWM2(void *arg )
{
	while(1)
	{
		servo2_state = in8(data_handleA);
		servo2_state = servo2_state|0x02;
		out8(data_handleA, servo2_state);
		//time1= time_value_micro();
		nanospin_ns(position2*1000);
		servo2_state = servo2_state & 0xFD;
		out8(data_handleA, servo2_state);
		//time1= time_value_micro();
		usleep((period - position2));
	}

}

/*Recipie 1 execution thread*/
void *Servo1_execution(void *arg)
{
	end1=1;
		if(execute[i] !=0)
		{
			//printf("i value is %i", i);
						if(Pause_motor1 == 0 && Motor1_uart_pause ==0)
						{
							input1= (execute[i] & opcode);
							switch(input1)
							{
								case Mov:
								{
									if((execute[i] - Mov) >=0 && (execute[i] - Mov)<6)
											{
												Move_motor1(execute[i] - Mov);
												position_motor1= execute[i] - Mov;
											}

										else
										{
											printf("Motor1 cannot Move more than 5 positions \r \n");
										}
										i++;
									}
								break;

									case WAIT:
								{
									delay_motor1(execute[i] - WAIT);
										i++;
									}
								break;
									case RECIPE_END:
										break;

									case LOOP:
									{
										loop_number= (execute[i]  - LOOP);
										loop_start= i+1;
										i++;
									}
									break;


									case END_LOOP:
									{
										if(loop_number != 0)
										{
											i= loop_start;
											loop_number--;
										}
										i++;
									}
									break;
									case JUMP:
									{
											backup=i;
											while( (execute[i]  | 0xE0) == LABEL)
											{
												i++;
											}
											i++;
									}
									case RESET:
									{
											i=0;
									}
									break;

									default:
										break;
							}

						}

					}
end0=0;
return(0);
}

/*Recipie 2 execution thread*/
void *Servo2_execution(void *arg)
{
end1=1;
		if(execute2[j]!=0)
		{
						if(Pause_motor2 == 0 && Motor2_uart_pause ==0)
						{
							input2= (execute[j] & opcode);


									switch(input2)
									{
										case Mov:
										{
											if((execute2[j] - 0x20) >=0 && (execute2[j] - 0x20)<6)
											{
												Move_motor2(execute2[j] - Mov);
												position_motor2= execute2[j] - Mov;
											}
											else
											{
												printf("Motor cannot Move more than 5 positions \r \n");
											}
											j++;
										}
										break;
										case WAIT:
										{
											delay_motor2(execute2[j] - WAIT);
											j++;
										}
										break;
										case RECIPE_END:
										{
											delay_motor2(execute2[j] - WAIT);
											j++;
										}break;

										case LOOP:
										{
											loop_number2= (execute2[j]  - LOOP);
											loop_start2= j+1;
											j++;

										}
										break;

										case END_LOOP:
										{
											if(loop_number2 != 0)
											{
												i= loop_start2;
												loop_number2--;
											}
											j++;
										}
										break;

										case RESET:
										{

											j=0;
										}
										break;

										case JUMP:
										{
											backup=j;
											while( (execute2[j]  & 0xE0) == LABEL)
											{
												j++;
											}
											j++;
										}
										break;

										default:
											break;
							}
						}
			}

end1=0;
return(0);
}

int value1;
pthread_t* thread_ID1;
pthread_t* thread_ID2;
int k;

/*Main code */
int main()
{

	privity_err = ThreadCtl( _NTO_TCTL_IO, NULL );

		/* Get a handle to the parallel port's Control Register */
		ctrl_handle = mmap_device_io( PORT_LENGTH, CTRL_ADDRESS );


		/* Get a handle to the parallel port's Data Register B*/
		data_handleA = mmap_device_io( PORT_LENGTH, OUTPUT_ADDRESS );

		/* Get a handle to the parallel port's Data Register B*/
		data_handleB = mmap_device_io( PORT_LENGTH, INPUT_ADDRESS );


					echo_byte = in8(ctrl_handle);
					echo_byte= echo_byte|0x02;
					echo_byte= echo_byte&0xEF;
					out8( ctrl_handle, 0x02);
					out8(data_handleA, 0x00);
					Move_motor1(0);
					Move_motor2(0);


					pthread_create( NULL, NULL, &Servo_PWM1, NULL );
					pthread_create( NULL, NULL, &Servo_PWM2, NULL );
					//pthread_create(NULL, NULL, Command_entered, NULL);
					//pthread_create( NULL, NULL, &Command_entered, NULL );
					while(1)
					{
						if(end1==0 && end0==0)
						{
							pthread_create( NULL, NULL, &Servo1_execution, NULL );
							pthread_create( NULL, NULL, &Servo2_execution, NULL );
						}
						Command_entered();

						//Servo1_execution();

					}
					return(0);
}

unsigned int time_value_micro(void)
{
	struct timeval tv1;
	gettimeofday (&tv1, NULL);
	return (tv1.tv_usec);
}


void Move_motor1( uint8_t value)
{
	position1= position_values[value];
	delay_time = value-position_motor2;
	if(delay_time >0)
	{
		delay_motor1(2*delay_time);
	}
	else if(delay_time !=0)
	{
		delay_motor1(2*(-1)*delay_time);
	}
}

void Move_motor2( uint8_t value)
{
		position2= position_values[value];
		delay_time = value-position_motor2;
		if(delay_time >0)
		{
			delay_motor2(2*delay_time);
		}
		else if(delay_time !=0)
		{
			delay_motor2(2*(-1)*delay_time);
		}
}

/*Serial port input execution*/
void Command_entered()
{

	if(tcischars(1)> 0)
	{


		char motor1= fgetchar();
		char motor2= fgetchar();
		char c= fgetchar();


					if(motor1 == 'p' || motor1 == 'P' )
					{
						Motor1_uart_pause=1;
					}
					else if(motor1 == 'c' || motor1 == 'C' )
					{
						Motor1_uart_pause=0;
					}
					else if(motor1 == 'r' || motor1 == 'R' )
					{
						if(position_motor1>0)
						{
							Move_motor1 (position_motor1 - 1);
							position_motor1--;
						}
						else
						{
							printf("Warning! Motor1 is at the extreme position \r\n");
						}
					}
					else if(motor1 == 'l' || motor1 == 'L' )
					{
						if(position_motor1<5)
						{
							Move_motor1(position_motor1 + 1);
							position_motor1++;
						}
						else
						{
							printf("Warning! Motor1 is at the extreme position \r\n");
						}
					}
					else if(motor1 == 'n' || motor1 == 'N' )
					{
					}
					else if(motor1 == 'b' || motor1 == 'B' )
					{
							for(k=0; k<100; k++)
							{
								if(Recipe[k] != 0)
								{
								execute[k]=	Recipe[k];
								}
								else
								{
									execute[k]= 0;
								}
								i=0;
								Motor1_uart_pause=0;
							}
					}





					if(motor2 == 'p' || motor2 == 'P' )
					{
						Motor2_uart_pause=1;
					}
					else if(motor2 == 'c' || motor2 == 'C' )
					{
						Motor2_uart_pause=0;
					}
					else if(motor2 == 'r' || motor2 == 'R' )
					{
						if(position_motor2>0)
						{
							Move_motor2 (position_motor2 - 1);
							position_motor2--;
						}
						else
						{
							printf("Warning! Motor2 is at the extreme position \r\n");
						}
					}
					else if(motor2 == 'l' || motor2 == 'L' )
					{
						if(position_motor2<5)
						{
							Move_motor2 (position_motor2 + 1);
							position_motor2++;
						}
						else
						{
							printf("Warning! Motor2 is at the extreme position \r\n");
						}
					}
					else if(motor2 == 'n' || motor2 == 'N' )
					{
					}
					else if(motor2 == 'b' || motor2 == 'B' )
					{
							for(k=0; k<100; k++)
							{
								if(Recipe[k] != 0)
								{
								execute2[k]= Recipe[k]	;
								}
								else
								{
									execute2[k]= 0;
								}
								j=0;
								Motor2_uart_pause=0;
							}
					}

	}

}

void delay_motor1( uint8_t value)
{
	Pause_motor1= 1;
	usleep(1000*100*value);
	Pause_motor1= 0;
}

void delay_motor2( uint8_t value)
{
	Pause_motor2= 1;
	usleep(1000*100*value);
	Pause_motor2= 0;
}



