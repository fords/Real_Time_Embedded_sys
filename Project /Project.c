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


unsigned int time_prev,time_aft,privity_err ;
uintptr_t ctrl_handle;
uintptr_t data_handleA,data_handleB;
//uintptr_t status_handle;

char echo_byte,mutex_lock=0;
unsigned int time_value(void)
{
	struct timeval tv1;
	gettimeofday (&tv1, NULL);
	return (tv1.tv_sec * 1000 + tv1.tv_usec / 1000);
}

int main( void )
{

	printf("Welcome to the Momentics IDE\n");
	/* Give this thread root permission to access the hardware */

	privity_err = ThreadCtl( _NTO_TCTL_IO, NULL );
	if ( privity_err == -1)
	{
		fprintf( stderr, "can't get root permissions\n");
		return -1;
	}

	/* Get a handle to the parallel port's Control Register */
	ctrl_handle = mmap_device_io( PORT_LENGTH, CTRL_ADDRESS );
	if ( ctrl_handle == MAP_DEVICE_FAILED ) {
		perror( "control map failed" );
		return EXIT_FAILURE;
	}


	/* Get a handle to the parallel port's Data Register B*/
	data_handleA = mmap_device_io( PORT_LENGTH, OUTPUT_ADDRESS );
	if ( data_handleA == MAP_DEVICE_FAILED ) {
		perror( "data map failed" );
		return EXIT_FAILURE;
	}

	/* Get a handle to the parallel port's Data Register B*/
	data_handleB = mmap_device_io( PORT_LENGTH, INPUT_ADDRESS );
	if ( data_handleB == MAP_DEVICE_FAILED ) {
		perror( "data map failed" );
		return EXIT_FAILURE;
	}

		while(1)
		{
			echo_byte = in8(ctrl_handle);
			echo_byte= echo_byte|0x02;
			echo_byte= echo_byte&0xEF;
			out8( ctrl_handle, echo_byte );

			if(mutex_lock==0)
			{
				out8(data_handleA, HIGH);
				mutex_lock=1;
				usleep(1000*1000);
				printf("High \n");
			}

			else
			{
				out8(data_handleA, LOW);
				mutex_lock=0;
				usleep(1000*1000);
				printf("Low \n");
			}

			/*echo_byte = in8(echo_handle);
			if(echo_byte & 0x01)
			time_prev=time_value;
			while(in8(echo_handle)&0x01);
			time_aft=time_value;
			printf("Time is :%u     ",time_aft- time_prev);
			usleep(1000*1000);*/
		}

	return(0);
}
