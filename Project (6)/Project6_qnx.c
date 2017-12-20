/**************************************************************************************************************************************

Project6
QNX ADC- Read and Map the ADC inputs from 0-15 values.
						
Contributions- Deepak Siddharth, Vedant Karia, Zeyar Win
**************************************************************************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/timeb.h>
#include <sched.h>
#include <sys/neutrino.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/select.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>
#include <hw/inout.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>

#define BASE_ADDRESS 0x280
#define ADC_CHANNEL 0x282
#define ADC_RANGE 0x283
#define CTRL_ADDRESS 0x28B
#define INPUT_ADDRESS 0x289  
#define OUTPUT_ADDRESS 0x288

#define PORT_LENGTH 1

float measured_volt;
static signed short measured_val;

uintptr_t ctrl_handle;
uintptr_t data_handleA;
uintptr_t ADC_channel_handle;
uintptr_t ADC_input_handle;
uintptr_t ADC_start_handle;
uintptr_t ADC_upperByte_handle;
uintptr_t ADC_lowerByte_handle;


void *adc_data_in(void* arg1)
{
static signed char UpperByte;
static signed char LowerByte;

	  while(1)
		{
			out8(ADC_start_handle,0x80);  				//Start the ADC conversion
			while(in8(ADC_input_handle)& 0x80 !=0); 	// wait for ADBUSY to go low, base+3 bit 7
			LowerByte = in8(ADC_lowerByte_handle);
			UpperByte = in8(ADC_upperByte_handle);
			measured_raw = (UpperByte*256) + LSB;		// combine the 2 bytes into a 16-bit value	
			measured_volt = ((measured_val/32768)*10);	//convert to voltage range
		}

}

int main(int argc, char *argv[])
{
	if ( ThreadCtl(_NTO_TCTL_IO, NULL) == -1)   // root permission
		{
			perror("Failed to get I/O access permission");
			return 1;
		}
	if(ctrl_handle == MAP_DEVICE_FAILED)  		// test for root permission
		{
			perror("Failed to map control register");
			return 2;
		}

	if(data_handleA == MAP_DEVICE_FAILED) 		// test for root permission
		{
			perror("Failed to map control register");
			return 2;
		}

	/* Get a handle for parallel port's control register */
	ctrl_handle =mmap_device_io(PORT_LENGTH,CTRL_ADDRESS); 		

	/* Get a handle to the parallel port's Data Register B*/	
	data_handleA=mmap_device_io(PORT_LENGTH,BASE_ADDRESS +OUTPUT_ADDRESS);  
	
	/* Get a handle to configure ADC Input channel*/
	ADC_channel_handle=mmap_device_io(PORT_LENGTH,ADC_CHANNEL);  
	
	/* Get a handle to configure ADC Range*/
	ADC_input_handle=mmap_device_io(PORT_LENGTH,ADC_RANGE);  
	
	/* Get a handle to start ADC*/
	ADC_start_handle=mmap_device_io(PORT_LENGTH,ADC_start_handle);  
	
	/* Get a handle to capture ADC MSB*/
	ADC_upperByte_handle=mmap_device_io(PORT_LENGTH,BASE_ADDRESS +0x1); 

	/* Get a handle to capture ADC LSB*/
	ADC_lowerByte_handle=mmap_device_io(PORT_LENGTH,BASE_ADDRESS +0x0);  

	pthread_t ADC_receive;
	
		/* Set PortA as output */
		out8(ctrl_handle,0x00);  
		
		/* Set the pin 43 to receive ADC values*/
		out8(ADC_channel_handle,0x44); 
		
		usleep(200);
		
		/* Set the range of input voltage from -5 to +5v*/
		out8(ADC_input_handle,0x00); 
		

		pthread_create(&ADC_receive, NULL, &adc_data_in, NULL );
		

		while (1)
		{
			/*Convert the voltage range from 0x1-0xE with each count from 10v/14. While 0x0 and 0xF 
			represents voltage greater or lesser than +/-5*/
			 if(measured_volt < -5) 
			 out8(data_handleA,0x00);

			 else if(measured_volt >= -5 && measured_volt <-4.28)
			 out8(data_handleA,0x01);

			 else if(measured_volt >= -4.28 && measured_volt <-3.57)
			 out8(data_handleA,0x02);
		 
			 else if(measured_volt >= -3.57 && measured_volt <-2.85)
			 out8(data_handleA,0x03);
						
			 else if(measured_volt >= -2.85 && measured_volt <-2.14)
			 out8(data_handleA,0x04);
							 
			 else if(measured_volt >=-2.14 && measured_volt <-1.42)
			 out8(data_handleA,0x05);
								
			 else if(measured_volt >= -1.42 && measured_volt <-.714)
			 out8(data_handleA,0x06);
									
			 else if(measured_volt >= -.714 && measured_volt <0)
			 out8(data_handleA,0x07);
										
			 else if(measured_volt >= 0 && measured_volt <.714)
			 out8(data_handleA,0x08);
									
										 
			 else if(measured_volt >= .714 && measured_volt <1.42)
			 out8(data_handleA,0x09);
										
										
			 else if(measured_volt >= 1.42 && measured_volt <=2.14)
			 out8(data_handleA,0x0A);

			 else if(measured_volt >= 2.14 && measured_volt <=2.85)
			 out8(data_handleA,0x0B);

			 else if(measured_volt >= 2.85 && measured_volt <=3.57)
			 out8(data_handleA,0x0C);
		 
			 else if(measured_volt >= 3.57 && measured_volt <=4.28)
			 out8(data_handleA,0x0D);
												

			 else if(measured_volt >= 4.28 && measured_volt <= 5)
			 out8(data_handleA,0x0E);

			 else(measured_volt >5)
			 out8(data_handleA,0x0F);
		}

}