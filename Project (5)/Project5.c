/**************************************************************************************************************************************

Project5  
Ultra-Sonic sensor -  	Final version with working ultra sonic sensor with updated comments
						
Contributions- Vedant Karia, Deepak Siddharth, Zeyar Win
**************************************************************************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include <pthread.h>
#include <string.h>
#include <sys/time.h>

#include <sys/mman.h>	/* for mmap_device_io() */


#include <unistd.h>		/* for sleep() */
#include <stdint.h>
#include <hw/inout.h>	/* for in*() and out*() functions */
#include <sys/neutrino.h>	/* for ThreadCtl() */

#define LOW 0x00
#define HIGH 0xFF

#define BASE_ADDRESS 0x280
/* The Neutrino IO port used here corresponds to a single register which is one byte long */
#define PORT_LENGTH 1
/* The first parallel port usually starts at 0x378. Each parallel port is three bytes wide. The first byte
   is the Data register, the second byte is the status register and the third byte is the control register */

#define BASE_ADDRESS 0x280
#define OUTPUT_ADDRESS 0x288
#define INPUT_ADDRESS 0x289
#define CTRL_ADDRESS 0x28B

uint8_t recived;
char echo_byte;
float distance;

clock_t time_start;
clock_t time_end;

unsigned int time_value_micro(void);
unsigned int privity_err ;

uintptr_t ctrl_handle;
uintptr_t data_handleA,data_handleB;


unsigned int time_value(void)
{
	struct timeval tv1;
	gettimeofday (&tv1, NULL);
	return (tv1.tv_sec * 1000 + tv1.tv_usec / 1000);
}

unsigned int time_value_micro(void)
{
	struct timeval tv1;
	gettimeofday (&tv1, NULL);
	return (tv1.tv_usec);
}

void *sensor(void* arg )
{
	while(1)
			{

				out8(data_handleA, 0x01); //Pulse High
				usleep(20);
				out8(data_handleA, 0x00); //Pulse Low


				while(recived == 0x00)    //Wait until echo is not received.
				{
					recived = in8(data_handleB)& 0x01;
				}

				if(recived == 0x01)			// Echo detected
				{
					time_start= clock();	//Start time
					while( recived == 0x01 ) //Wait until echo is still high.
					{
						recived = in8(data_handleB)&0x01;
					}
					time_end = clock();		//End time
					if(time_end- time_start> 18000 )
					{
						printf("Distance is more than the range \n");
					}
					else if(time_end- time_start<300 )
					{
						printf("Distance is less than the range \n");
					}
					else
					{
						distance = (time_end- time_start)/149.4;
						printf("Distance is %f inches\n", distance);
					}

				}
				else
				{
					puts("Not received the ECHO");
				}

			usleep(1000000);		//Sleep before Pulse high
			}
}


int main( void )
{

	printf("Welcome to the Momentics IDE\n");
	/* Give this thread root permission to access the hardware */
	privity_err = ThreadCtl( _NTO_TCTL_IO, NULL );

	/* Get a handle to the parallel port's Control Register */
	ctrl_handle = mmap_device_io( PORT_LENGTH, CTRL_ADDRESS );


	/* Get a handle to the parallel port's Data Register B*/
	data_handleA = mmap_device_io( PORT_LENGTH, OUTPUT_ADDRESS );

	/* Get a handle to the parallel port's Data Register B*/
	data_handleB = mmap_device_io( PORT_LENGTH, INPUT_ADDRESS );

	/*Register configuration for IO port- Control Handle*/
	echo_byte = in8(ctrl_handle);
	echo_byte= echo_byte|0x02;
	echo_byte= echo_byte&0xEF;
	out8( ctrl_handle, 0x02);

	pthread_create( NULL, NULL, &sensor, NULL );
	
	while(1);
	
	return(0);
}


