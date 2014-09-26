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
pthread_t t_geeks[NUM];
pthread_t t_nongeeks[NUM];
pthread_t t_singers[NUM];

//Array of mutex lock
pthread_mutex_t runningS[NUM];
sem_t m_geeks[NUM];
sem_t m_nongeeks[NUM];
sem_t m_singers[NUM];

//Array of states to keep track of states running
int state[NUM]={0};

//Array of indices of each thread
int t_id[NUM];
int t_idG[NUM];
int t_idN[NUM];
int t_idS[NUM];

int flag=0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexbridge = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexO = PTHREAD_MUTEX_INITIALIZER;
int  geeks,nongeeks,onbridge=0,singers,pass=0;



//Thread routine
void *GeekArrives(void *dummyptr)
{
	int ind=*((int*)dummyptr);
	sem_wait( &m_geeks[ind]);
      	
      	pthread_mutex_lock( &mutexbridge );
      	if(onbridge<4)
      	{
      		//BoardBridge()
      		onbridge++;
	}
	geeks--;
      	printf("Geek-1 ");	
	
	if(onbridge==4)
	{
		//GoBridge()
		onbridge-=4;
	}
	
	pthread_mutex_unlock( &mutexbridge );
	return NULL;
}

void *NonGeekArrives(void *dummyptr)
{
	int ind=*((int*)dummyptr);
	sem_wait( &m_nongeeks[ind]);
      	
      	pthread_mutex_lock( &mutexbridge );
      	if(onbridge<4)
      	{
      		//BoardBridge()
      		onbridge++;
      	}
      	nongeeks--;
      	printf("Non-geek-1 ");	
      	
      	if(onbridge==4)
	{
		//GoBridge
		onbridge-=4;
	}
      	
      	pthread_mutex_unlock( &mutexbridge );
      	
      	return NULL;
}

void *SingerArrives(void *dummyptr)
{
	int ind=*((int*)dummyptr);
	sem_wait( &m_singers[ind]);
	
	pthread_mutex_lock( &mutexbridge );
      	if(onbridge<4)
      	{
      		//BoardBridge()
      		onbridge++;
      	}
      	singers--;
	printf("Singer-1 ");	
	if(onbridge==4)
	{
		//GoBridge
		onbridge-=4;
	}

	pthread_mutex_unlock( &mutexbridge );

	return NULL;
}

//Thread function for site
void allocateBridge()
{
	int i,j,k;
	while(1)
	{
		if((geeks+nongeeks+singers) <4)
		{	
			//printf("Googbye 1\n");
			break;	
		}
		if(singers>0)
		{
			if((geeks+nongeeks)<3)
			{
				//printf("Googbye 2\n");
				break;
			}
			i=singers;	
			
		
			if(nongeeks>0 && geeks>1)
			{
				j=geeks;
				k=nongeeks;
				printf("\n\n Pass %d : People on the bridge are ->\n",++pass);			
				
				sem_post(&m_singers[i]);
				sem_post(&m_geeks[j--]);
				sem_post(&m_geeks[j]);
				sem_post(&m_nongeeks[k]);
				
				pthread_join(t_singers[i],NULL);
				pthread_join(t_geeks[j++],NULL);
				pthread_join(t_geeks[j],NULL);
				pthread_join(t_nongeeks[k],NULL);
				//onbridge-=4;
			}
			else if(nongeeks>1 && geeks>0)
			{
				j=geeks;
				k=nongeeks;
				printf("\n\n Pass %d : People on the bridge are ->\n",++pass);			
							
				sem_post(&m_singers[i]);
				sem_post(&m_geeks[j]);
				sem_post(&m_nongeeks[k--]);
				sem_post(&m_nongeeks[k]);
				
				pthread_join(t_singers[i],NULL);
				pthread_join(t_geeks[j],NULL);
				pthread_join(t_nongeeks[k++],NULL);
				pthread_join(t_nongeeks[k],NULL);
				//onbridge-=4;
			}
			else if(nongeeks>2)
			{
			
				k=nongeeks;
				printf("\n\n Pass %d : People on the bridge are ->\n",++pass);			
							
				sem_post(&m_singers[i]);
				sem_post(&m_nongeeks[k--]);
				sem_post(&m_nongeeks[k--]);
				sem_post(&m_nongeeks[k]);
				
				pthread_join(t_singers[i],NULL);
				pthread_join(t_nongeeks[k++],NULL);
				pthread_join(t_nongeeks[k++],NULL);
				pthread_join(t_nongeeks[k],NULL);
				//onbridge-=4;
			}
			else if(nongeeks>2)
			{
				j=geeks;
				printf("\n\n Pass %d : People on the bridge are ->\n",++pass);			
							
				sem_post(&m_singers[i]);
				sem_post(&m_geeks[j--]);
				sem_post(&m_geeks[j--]);
				sem_post(&m_geeks[j]);
				
				pthread_join(t_singers[i],NULL);
				pthread_join(t_geeks[j++],NULL);
				pthread_join(t_geeks[j++],NULL);
				pthread_join(t_geeks[j],NULL);
			//	onbridge-=4;
			}
		}
		else
		{
			
			if((geeks+nongeeks)<4)
			{
				//printf("Googbye 2\n");
				break;
			}
			
			if((geeks>1) && (nongeeks>1))
			{
				k=nongeeks;
				j=geeks;
				printf("\n\n Pass %d : People on the bridge are -> \n",++pass);			
				
				sem_post(&m_geeks[j--]);
				sem_post(&m_geeks[j]);
				sem_post(&m_nongeeks[k--]);
				sem_post(&m_nongeeks[k]);
				
				pthread_join(t_geeks[j++],NULL);
				pthread_join(t_geeks[j],NULL);
				pthread_join(t_nongeeks[k++],NULL);
				pthread_join(t_nongeeks[k],NULL);
				//onbridge-=4;
			}
			
			else if(nongeeks>4)
			{
				k=nongeeks;
				printf("\n\n Pass %d : People on the bridge are -> \n",++pass);			
				
				sem_post(&m_nongeeks[k--]);
				sem_post(&m_nongeeks[k--]);
				sem_post(&m_nongeeks[k--]);
				sem_post(&m_nongeeks[k]);
				
				pthread_join(t_nongeeks[k++],NULL);
				pthread_join(t_nongeeks[k++],NULL);
				pthread_join(t_nongeeks[k++],NULL);
				pthread_join(t_nongeeks[k],NULL);
				onbridge-=4;
			}
			else if(geeks>4)
			{
				k=geeks;
				printf("\n\n Pass %d : People on the bridge are -> \n",++pass);			
				
				sem_post(&m_geeks[k--]);
				sem_post(&m_geeks[k--]);
				sem_post(&m_geeks[k--]);
				sem_post(&m_geeks[k]);
				
				pthread_join(t_geeks[k++],NULL);
				pthread_join(t_geeks[k++],NULL);
				pthread_join(t_geeks[k++],NULL);
				pthread_join(t_geeks[k],NULL);
				//onbridge-=4;
			}
			else
			{
			 	//printf("Goodbye 3\n");
			 	break;
			}
		
		}	
	
	}
	
}

int main( int argc, char *argv[])
{
    	
    	int i, j,semret;
	int numG,numN,numS,numThreads;
	
	if(argc<4)
	{
		printf("./a.out <#of geeks> <#of non-geeks> <#of singers>\n");
		return 0;
	}
	
	//Get commandline arguments and convert them to integers
	numG=atoi(argv[1]);
	numN=atoi(argv[2]);
	numS=atoi(argv[3]);
	geeks=numG;
	nongeeks=numN;
	singers=numS;
	
	///semret=sem_init(&mysem, 0, 4);

	if(semret!=0)
	{
		printf("Semaphore failed to initialize. Exiting..\n");
		return 0;
	}
	
	numThreads=numG+numN+numS;
	
	//Create thread for each nongeek
	for(i=1;i<=numN;i++)
	{
		t_idN[i]=i;
		//Initialise mutex lock for each site
		sem_init(&m_nongeeks[i], 0,0);

		//thread creation	
		pthread_create( &t_nongeeks[i], NULL, NonGeekArrives, (void*)&t_idN[i] );
	}	
	
	//Create thread for each geek
	for(i=1;i<=numG;i++)
	{
		t_idG[i]=i;
		//Initialise mutex lock for each site
		sem_init(&m_geeks[i], 0, 0);

		//thread creation	
		pthread_create( &t_geeks[i], NULL, GeekArrives, (void*)&t_idG[i] );
	}
	
	//Create thread for each geeks
	for(i=1;i<=numS;i++)
	{
		t_idS[i]=i;
		//Initialise mutex lock for each site
		sem_init(&m_singers[i], 0,0);

		//thread creation	
		pthread_create( &t_singers[i], NULL, SingerArrives, (void*)&t_idS[i] );
	} 
	
	allocateBridge();
	
	printf("\n\nTotal number of passes: %d\nRemaining Geeks: %d\nRemaining Non-Geeks: %d\nRemaining Singers: %d\n",pass,geeks,nongeeks,singers);	
	return 0;
}
