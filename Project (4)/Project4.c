#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


unsigned int max_transaction_time;
unsigned int max_transaction_time1;
unsigned int max_transaction_time2;
unsigned int max_transaction_time3;
unsigned int max_wait_time1;
unsigned int max_wait_time2;
unsigned int max_wait_time3;

unsigned int max_queue_time;
unsigned int average_queue_time;
unsigned int average_teller_time;
unsigned queue_time;
unsigned int average_transaction_time;
unsigned transaction_time;
unsigned int average_teller1_time;
unsigned teller1_waittime;
unsigned int average_teller2_time;
unsigned teller2_waittime;
unsigned int average_teller3_time;
unsigned teller3_waittime;
unsigned int teller1_time_break;
unsigned int teller3_time_break;
unsigned int teller2_time_break;
int token_number;
unsigned int time1;
int sleep_time;
int teller1_handelling;
int teller2_handelling;
int teller3_handelling;
int i=0;
int j=0;
int count = 0;
int teller1_done, teller2_done, teller3_done;
unsigned max_queue_time;
unsigned customer_max_queue;
int max_queue;
unsigned int random_customer;
unsigned int random_teller1;
unsigned int random_teller2;
unsigned int random_teller3;
int break_time1, break_time2, break_time3;
struct customer //Task Control Block for customer
{
	int token;
	int teller_number;
	unsigned int time_in;
	unsigned int time_out;
	unsigned int queue_in;
};

unsigned int time_value();



struct customer Customer[300];

/*Customer Thread*/

void* Customer_token_assign( void* arg )
{
	while(time_value()- time1< 42000)
	{
	//	printf("Time is :%u     ",time_value()- time1);

		printf("customer sleep %u", time_value()- time1);
		usleep ((((rand_r(&random_customer))%4)+1) * 100000);
		printf("       customer sleep %u\n", time_value()- time1);
		Customer[i].token = 1;
		Customer[i].queue_in = time_value()-time1;
		i++;

		if(max_queue< (i-token_number))
		{
			max_queue= i- token_number;

		}
	}

	return(0);
}

/*Teller1 Thread*/
void* Teller1( void* arg )
{
	while(Customer[token_number].token != 0 || time_value()-time1< 42000 )
	{

		if(Customer[token_number].token != 0 )
		{
			teller1_waittime= time_value()- time1- teller1_waittime;
			teller1_handelling= token_number;

			token_number++;
			Customer[teller1_handelling].time_in= time_value()-time1;

			Customer[teller1_handelling].token = 0;
			Customer[teller1_handelling].teller_number= 1;
			//printf("Customer: %d \n ",teller1_handelling);
			usleep(((rand_r(&random_teller1)%12)+1)*50000);
			Customer[teller1_handelling].time_out= time_value()-time1;

			average_teller1_time += teller1_waittime;
			if(max_wait_time1< teller1_waittime)
			{
				max_wait_time1= teller1_waittime;
			}
			teller1_waittime= time_value()-time1;
		}
		/*Break time for teller1*/
		if((time_value()- time1 - teller1_time_break) > (break_time1 =(((rand_r(&random_teller1)%30)+30)*100)))
			{
				usleep( break_time3*1000);
				teller1_time_break= time_value()-time1;
				teller1_waittime= time_value()-time1;
			}
	}
	teller1_done=1;
	return(0);
}

/*Teller2 Thread*/
void* Teller2( void* arg )
{
	while(Customer[token_number].token != 0 || (time_value()-time1< 42000) )
		{
			if(Customer[token_number].token != 0 )
			{
				teller2_waittime= time_value()- time1- teller2_waittime;
				teller2_handelling= token_number;
				token_number++;
				Customer[teller2_handelling].time_in= time_value()-time1;
				Customer[teller2_handelling].token = 0;
				Customer[teller2_handelling].teller_number= 2;

			//	printf("Customer: %d \n ",teller2_handelling);
				usleep(((rand_r(&random_teller2)%12)+1)*50000);
				Customer[teller2_handelling].time_out= time_value()-time1;

				average_teller2_time += teller2_waittime;
				if(max_wait_time2< teller2_waittime)
				{
					max_wait_time2= teller2_waittime;
				}
				teller2_waittime= time_value()-time1;

			}
			/*Break time for teller2*/
		if((time_value()- time1 - teller2_time_break) > ( break_time2= (((rand_r(&random_teller2)%30)+30)*100)))
							{
								usleep( break_time3*1000);
					teller2_time_break= time_value()-time1;
					teller2_waittime= time_value()-time1;
				}
		}
	teller2_done=1;
	return(0);
}

/*Teller3 Thread*/
void* Teller3( void* arg )
{

	while(Customer[token_number].token != 0 || (time_value()-time1 )< 42000 )
		{
			if(Customer[token_number].token != 0 )
			{
				teller3_waittime= time_value()- time1- teller3_waittime;
				teller3_handelling= token_number;
				token_number++;
				Customer[teller3_handelling].time_in= time_value()-time1;
				Customer[teller3_handelling].token = 0;
				Customer[teller3_handelling].teller_number= 3;
				sleep_time= (rand()%55)%10+0.5;
			//	printf("Customer: %d \n ",teller3_handelling);
				usleep(((rand_r(&random_teller3)%12)+1)*50000);
				Customer[teller3_handelling].time_out= time_value()-time1;
				average_teller3_time += teller3_waittime;
				if(max_wait_time3< teller3_waittime)
				{
					max_wait_time3= teller3_waittime;
				}
				teller3_waittime= time_value()-time1;

			}
			/*Break time for teller3*/
			if((time_value()- time1 - teller3_time_break) > (break_time3= (((rand_r(&random_teller3)%30)+30)*100)))
				{
					usleep( break_time3*1000);
					teller3_time_break= time_value()-time1;
					teller3_waittime= time_value()-time1;
				}
		}
	teller3_done=1;
	return(0);
}

int teller1_number_customer;
int teller2_number_customer;
int teller3_number_customer;

/*Main function entry*/
int main( void )
{
	time1= time_value();
	printf("Hello Welcome to BANK \n");
    pthread_create( NULL, NULL, &Customer_token_assign, NULL );
    pthread_create( NULL, NULL, &Teller1, NULL );
    pthread_create( NULL, NULL, &Teller2, NULL );
    pthread_create( NULL, NULL, &Teller3, NULL );
    /* Let the threads run for 60 seconds. */
    while((time_value()-time1 )< 42000 || teller1_done == 0 || teller2_done ==0|| teller3_done==0 )
    {
    }
    for(j=0;j<token_number; j++)
    {
		/*Calculation of final time for each teller*/
    	switch(Customer[j].teller_number)
    	{
			case 1:
				{
					queue_time= Customer[j].time_in - Customer[j].queue_in;
					transaction_time= Customer[j].time_out - Customer[j].time_in;
					average_teller1_time+= transaction_time;
					teller1_number_customer++;
					if(transaction_time > max_transaction_time1)
					{
						 max_transaction_time1= transaction_time;
					}
					if(max_queue_time< queue_time)
					{
						max_queue_time = queue_time;
					}

					average_queue_time +=  queue_time ;
					average_teller_time += transaction_time;
				}
			case 2:
				{
					queue_time= Customer[j].time_in - Customer[j].queue_in;
					transaction_time= Customer[j].time_out - Customer[j].time_in;
					average_teller2_time+= transaction_time;
					teller2_number_customer++;
					if(transaction_time > max_transaction_time2)
					{
						 max_transaction_time2= transaction_time;
					}
					if(max_queue_time< queue_time)
					{
						max_queue_time = queue_time;
					}

					average_queue_time +=  queue_time ;
					average_teller_time += transaction_time;
					break;

				}
			case 3:
				{
					queue_time= Customer[j].time_in - Customer[j].queue_in;
					transaction_time= Customer[j].time_out - Customer[j].time_in;
					average_teller3_time+= transaction_time;
					teller3_number_customer++;
					if(transaction_time  > max_transaction_time3)
					{
						 max_transaction_time3= transaction_time;
					}
					if(max_queue_time< queue_time)
						{
							max_queue_time = queue_time;
						}

						average_queue_time +=  queue_time ;
						average_teller_time += transaction_time;
					break;
				}
			default:
				break;

    	}
    }
	printf("Total number of customers visited bank is:%d \n", i);
	printf("Average time customer spends in queue:%f min \n", average_queue_time/(token_number*100.0));
	printf("Average time customer spends with teller:%f min\n", average_teller_time/(token_number*100.0));
	printf("Average time teller1 waiting for customer:%f min\n", average_teller1_time/(teller1_number_customer*100.0));
	printf("Average time teller2 waiting for customer:%f min \n", average_teller2_time/(teller2_number_customer*100.0));
	printf("Average time teller3 waiting for customer:%f min \n", average_teller3_time/(teller3_number_customer*100.0));
	printf("Max time customer spent in queue:%f min \n",(max_queue_time/100.0) );
	printf("Max time teller1 waiting for customer:%f min \n",(max_wait_time1/100.0) );
	printf("Max time teller2 waiting for customer:%f min \n",max_wait_time2/100.0 );
	printf("Max time teller3 waiting for customer:%f min \n",max_wait_time3/100.0 );
	printf("Max time teller1 spent in transaction:%f min \n",(max_transaction_time1/100.0) );
	printf("Max time teller2 spent in transaction:%f min \n",max_transaction_time2/100.0 );
	printf("Max time teller3 spent in transaction:%f min \n",max_transaction_time3/100.0 );
	printf("max number of people in queue :%u \n",max_queue );

	return(0);
}

/* Current time function*/
unsigned int time_value(void)
{
	struct timeval tv1;
	gettimeofday (&tv1, NULL);
	return (tv1.tv_sec * 1000 + tv1.tv_usec / 1000);
}

