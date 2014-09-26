#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#define SPEC1 1
#define SPEC2 2
#define SPEC3 3
#define SPEC4 4

int students_no,courses_no;

int max_stud_per_course[5]={0,12,24,12,12};

//Array of structures 
struct student* students_arr;
struct course* courses_arr;

//Array of threads
pthread_t* thread_s;

//Array of indices
int* t_id;

//Array of mutex
sem_t* m_course;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

//Structure for each student  
struct student
{
	int id;
	int branch;
	int allocated_no;
	int choice[9];
	int course[5];
};

//structure for each course
struct course
{
	int spec;
	int total;
	int branch_s[5];
};

//Initialise course structure 
struct course init_course(int spec)
{
	struct course c;
	int i;
	
	c.spec=spec+1;
	c.total=0;
	for(i=1;i<=4;i++)
	{
		c.branch_s[i]=0;
	}
	return c;
} 

//Initialise student structure
struct student init_student(int branch,int id)
{
	struct student s;
	int i,countsp1=2,countsp2=2,countsp3=2,countsp4=2;
	int ran,ind=1;
	s.branch=branch+1;
	s.id=id;
	s.allocated_no=0;
	for(i=1;i<=4;i++)
	{
		s.course[i]=0;
	}
	for(i=1;i<=8;i++)
	{
		s.choice[i]=0;
	}
	
	//Fill 8 choices randomly
	while(1)
	{
		if(ind==9 || (countsp1==0 && countsp2==0 && countsp3==0 && countsp4==0))
			break;
		
		if(courses_no<=3 && ind==3)
			break;	
		
		ran=rand()%courses_no+1;
		
		/*for(i=1;i<ind;i++)
		{
			if(s.choice[i]==ran)
			{	
				ran=(ran+4)%courses_no;
				break;
			}		
			if(courses_arr[s.choice[i]].spec == courses_arr[ran].spec)
			{
				ran=(ran+4)%courses_no;
				break;
			}	
		}
		*/
		//printf("I am stuck here %d %d %d %d %d %d\n",ind,countsp1,countsp2,countsp3,countsp4,ran);	
		
		if( courses_arr[ran].spec == SPEC1 && countsp1>0)
		{
			s.choice[ind++]=ran;
			countsp1--;
		}
		else if( courses_arr[ran].spec == SPEC2 && countsp2>0)
		{
			s.choice[ind++]=ran;
			countsp2--;
		}
		else if( courses_arr[ran].spec == SPEC3 && countsp3>0)
		{
			s.choice[ind++]=ran;
			countsp3--;
		}
		else if( courses_arr[ran].spec == SPEC4 && countsp4>0)
		{
			s.choice[ind++]=ran;
			countsp4--;
		}
	}
	
	return s;
}

//Thread function for each student. It will allocate the courses to student.

void* allocate(void* dummyptr)
{
	struct student stud= *((struct student*)dummyptr);
	struct course co;
	int i,flag=0;
	
	/*printf("\nI am %d student with Branch %d and choices \n",stud.id,stud.branch);
	for(i=1;i<=8;i++)
	{
		printf(" %d ",stud.choice[i]);
	}
	printf("\n");
	*/
	//pthread_mutex_lock(&mutex1);
	//pthread_mutex_unlock(&mutex1);

	//Iterate each choice
	for(i=1;i<=8;i++)
	{
		//Get lock for the course of the given choice
		sem_wait( &m_course[stud.choice[i]]);
		co=courses_arr[stud.choice[i]];
		
		//If a course from this spec is already allocated to student
		if(stud.course[co.spec]!=0 )
		{
			sem_post( &m_course[stud.choice[i]]);			
			continue;
		}
		//If maximum allowed students from that branch are already allocated the course  	
		if(co.branch_s[stud.branch] >= max_stud_per_course[stud.branch])
		{
			sem_post( &m_course[stud.choice[i]]);			
			continue;
		}
		//If maximum students per course achieved
		if(co.total>=60)
		{
			sem_post( &m_course[stud.choice[i]]);					
			continue;
		}
		//printf("Student %d allocated course %d\n",stud.id,stud.choice[i]);
		courses_arr[stud.choice[i]].total++;
		courses_arr[stud.choice[i]].branch_s[stud.branch]++;
		stud.course[co.spec]=stud.choice[i];
		stud.allocated_no++;
		sem_post( &m_course[stud.choice[i]]);			
	}
	
	//If a student is not allocated 4 courses of different spectrum
	if(stud.allocated_no<4)
	{
		printf("Student of id %d has not been allocated %d courses \n",stud.id,4-stud.allocated_no);
	}
}


int main( int argc, char *argv[])
{
    	int i, j,spec;
	int numC,numS;
	FILE *f = fopen("allocated.txt", "w");
	
	if(argc<3)
	{
		printf("./a.out <#of students> <#of courses>\n");
		return 0;
	}
	
	//Get commandline arguments and convert them to integers
	numS=atoi(argv[1]);
	numC=atoi(argv[2]);
	students_no=numS;
	courses_no=numC;
	
	//Allocate memory
	courses_arr=(struct course*)malloc(sizeof(struct course)*(numC+1));
	students_arr=(struct student*)malloc(sizeof(struct student)*(numS+1));
	thread_s=(pthread_t*)malloc(sizeof(pthread_t)*(numS+1));
	t_id=(int*)calloc((numS+1),sizeof(int));
	m_course=(sem_t*)malloc(sizeof(sem_t)*(numC+1));
	
	//Initialise courses structure
	for(j=1;j<=numC;j++)
	{
		courses_arr[j]=init_course(j%4);
		//Initialise mutex lock for each site
		sem_init(&m_course[j], 0, 1);
	}
		
	//Create thread for each student
	for(i=1;i<=numS;i++)
	{
		t_id[i]=i;
		students_arr[i]=init_student(i%4,i);
	
		//thread creation	
		pthread_create( &thread_s[i], NULL, allocate, (void*)&students_arr[i] );
	}	
	
	//Join all student threads
	for(i=1;i<=numS;i++)
	{
		pthread_join( thread_s[i], NULL);
	}
	
	if (f == NULL)
	{
    		printf("Error opening file!\n");
    		exit(1);
	}
	
	//Print courses info to allocated.txt		
	for(i=1;i<=numC;i++)
	{
		fprintf(f,"Course %d has no. of enrolled students %d\n",i,courses_arr[i].total);
	
	}
	
	return 0;
}
