#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#define NUM 1000
#define RUNNING 2

//Counting semaphore
sem_t mysem,msem;

//Array of thread ids 
pthread_t threads[NUM];

//Array of mutex lock
pthread_mutex_t runningS[NUM];

//Array of states to keep track of states running
int state[NUM]={0};

//Array of indices of each thread
int t_id[NUM];
int flag=0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexH = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexO = PTHREAD_MUTEX_INITIALIZER;
int  oxygen,hydrogen,counter=0,numS;

//Thread routine
void *thread_function(void *dummyPtr)
{
	int value; 
	//printf("Thread number %ld\n", pthread_self());
	pthread_mutex_lock( &mutex1 );
      	sem_wait(&msem);
	
	sem_getvalue(&msem, &value); 
      	printf("The value of the semaphors is %d    %ld\n", value,pthread_self());
	printf("Thread number inn critical section %ld\n", pthread_self());
	printf("%d---\n",counter);
	counter++;
	//pthread_mutex_unlock( &mutex1 );
	sem_post(&msem);
	return NULL;
	
}

//Oxygen thread routine which will consume a O atom
void *thread_function_O(void *dummyPtr)
{
	if(oxygen<1)
	{
		flag=1;
		//return;
	}
	else{
		oxygen--;
		flag=0;
	}
		
	return NULL;
}

//Hydrogen thread routine which will consume 2 H atoms
void *thread_function_H(void *dummyPtr)
{
	if(hydrogen<1)
	{
		flag=1;
		//return;
	}
	else{
		hydrogen-=2;
		flag=0;
	}
	return NULL;
}


//Thread function for site
void *thread_function_site(void* index)
{
	int ind=*((int*)index);
	//printf(" index %d\n",ind);
	
	//Threads for Hydrogen and Oxygen
	pthread_t threadh1,threado; 
	
	while(1){

		//Get mutex lock to read/modify state array	
		pthread_mutex_lock(&mutex2);
		if(state[ind-1]==RUNNING || state[ind+1]==RUNNING || state[ind]!=0)
		{
			state[ind]=0;
			pthread_mutex_unlock(&mutex2);
			continue;
		}
		//printf("%d\n",ind);
		state[ind-1]=1;
		state[ind]=RUNNING;
		state[ind+1]=1;
		pthread_mutex_unlock(&mutex2);
	
		//Check for the number of sites working. 
		sem_wait(&mysem);
		
		//Acquire locks of neighbour so that they wont execute while current thread is working
		if(ind>=1 && ind<numS-1)
		{
		//	printf("Acquire both locks index %d\n",ind);	
			pthread_mutex_lock(&runningS[ind-1]);
			pthread_mutex_lock(&runningS[ind+1]);
		}
		else if(ind==0 && ind<numS-1)
		{
		//	printf("Acquire next locks index %d\n",ind);	
			pthread_mutex_lock(&runningS[ind+1]);
		}
		else if(ind==numS-1 && ind>0)
		{
		//	printf("Acquire previous locks index %d\n",ind);	
			pthread_mutex_lock(&runningS[ind-1]);
		}
	
		pthread_mutex_lock(&runningS[ind]);
	
	      	//ENter into critical section only H and O atoms are available
		if(hydrogen>=2 && oxygen>=1)
		{
			//printf("index %d\n",ind);
			pthread_mutex_lock(&mutex1);
	
			pthread_create( &threadh1, NULL, thread_function_H, NULL );
		 	//pthread_create( &threadh2, NULL, thread_function_H, NULL );	
			pthread_create( &threado, NULL, thread_function_O, NULL );
		
			pthread_join( threadh1, NULL);
			//pthread_join( threadh2, NULL);
			pthread_join( threado, NULL);
	
		
			pthread_mutex_unlock(&mutex1);
		
			if(flag!=1)
			{	
				printf("1 E Mj energy is created at site %d. \n", ind);
				printf("Number of Hydrogen atoms %d and Oxygen atoms %d are remaining.\n",hydrogen,oxygen);
			
				counter++;
			}
			//pthread_mutex_unlock(&mutexH );
			
			
			pthread_mutex_lock(&mutex2);
				state[ind-1]=0;
				state[ind]=0;
				state[ind+1]=0;
				sleep(1);
			pthread_mutex_unlock(&mutex2);
			sem_post(&mysem);
			
			pthread_mutex_unlock( &runningS[ind] );
			if(ind>=1 && ind<numS-1)
			{
				pthread_mutex_unlock(&runningS[ind+1]);
				pthread_mutex_unlock(&runningS[ind-1]);
			
			}
			else if(ind==0 && ind<numS-1)
				pthread_mutex_unlock(&runningS[ind+1]);
			else if(ind==numS-1 && ind>0)
				pthread_mutex_unlock(&runningS[ind-1]);

			sleep(2);
		}
		else
		{
			//pthread_mutex_unlock(&mutexH);
			//pthread_mutex_unlock(&mutexO);
			sem_post(&mysem);
			
			pthread_mutex_lock(&mutex2);
				state[ind-1]=0;
				state[ind]=0;
				state[ind+1]=0;
			pthread_mutex_unlock(&mutex2);
		
			pthread_mutex_unlock( &runningS[ind] );
			if(ind>=1 && ind<numS-1)
			{
				pthread_mutex_unlock(&runningS[ind+1]);
				pthread_mutex_unlock(&runningS[ind-1]);
			}
			else if(ind==0 && ind<numS-1)
				pthread_mutex_unlock(&runningS[ind+1]);
			else if(ind==numS-1 && ind>0)
				pthread_mutex_unlock(&runningS[ind-1]);
		
			break;
			//pthread_exit(NULL);
		}
				
	}
	return NULL;
}

int main( int argc, char *argv[])
{
    	
    	int i, j,semret;
	int numH,numO,numSites,thrEng;
	
	if(argc<5)
	{
		printf("./a.out <No of H atoms> <No of O atoms> <No of sites> <Threshold Energy(inUnits)>\n");
		return 0;
	}
	
	//Get commandline arguments and convert them to integers
	numH=atoi(argv[1]);
	numO=atoi(argv[2]);
	numSites=atoi(argv[3]);
	thrEng=atoi(argv[4]);
	oxygen=numO;
	hydrogen=numH;
	numS=numSites;
	
	semret=sem_init(&mysem, 0, thrEng);
	if(semret!=0)
	{
		printf("Semaphore failed to initialize. Exiting..\n");
		return 0;
	}
	//Create thread for each site
	for(i=1;i<=numSites;i++)
	{
		//runningS[i] = PTHREAD_MUTEX_INITIALIZER;
		t_id[i]=i;
		//Initialise mutex lock for each site
		pthread_mutex_init(&runningS[i], NULL);

		//thread creation	
		pthread_create( &threads[i], NULL, thread_function_site, (void*)&t_id[i] );
		
	}	
	 
	//Join all threads so that process waits for all threads 
	for(j=1; j < numSites; j++)
	{
	   	pthread_join( threads[j], NULL);
	}

	printf("\nTotal energy created: %d E Mj \n", counter);	
	return 0;
}
