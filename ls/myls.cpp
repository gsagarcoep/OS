/*
Roll No.: 201305549
Name    : Gangasagar Patil

*/

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <deque>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
using namespace std;

//global variables

char temp[200],temp1[200],temp2[200],again[1000];
const char *month[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
const char *day[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
int a_flag=0,l_flag=0,r_flag=0, s_flag=0,dir_flag=0,A_flag=0,d_flag=0,h_flag=0,S_flag=0,t_flag=0;
char sc_flag;
int size,maxSize=0,nest,maxlen=0;
unsigned long mode;
char mode_str[10];
char tempstr[4];
void recursion(DIR * dirp, struct dirent * dp, struct stat st,char * cur_path);
/*****************************************Check mode********************/

void check_mode(unsigned long num)
{
	if(num%2 == 1)
		tempstr[2]='x';
	else
		tempstr[2]='-';
	if(num==2 || num==3 || num==6 || num==7)
		tempstr[1]='w';
	else
		tempstr[1]='-';
	if(num > 3)
		tempstr[0]='r';
	else
		tempstr[0]='-';
}

/*****************************************Permissions********************/
void permission(unsigned long no)
{
	
	unsigned long arr[3];	
	arr[0]=no & 448;
	arr[0]=arr[0] >> 6;
	arr[2]=no & 7 ;
	arr[1]=no & 56;
	arr[1]= arr[1] >> 3; 
	
	check_mode(arr[0]);
	strcpy(mode_str,tempstr);

	check_mode(arr[1]);
	strcat(mode_str,tempstr);

	check_mode(arr[2]);
	strcat(mode_str,tempstr);
	
}
/****************************** Print colors***************************/
void printColor(struct stat st,char* name)
{
	if ((st.st_mode & S_IFMT) == S_IFDIR)		
	{		
		cout<<"\e[1m\e[34m";
	}
	else if ((st.st_mode & S_IFMT) == S_IFREG)
	{ 
		if((st.st_mode & S_IEXEC) != 0)
			cout<<"\e[1m\e[32m";
	}
	
	else if ((st.st_mode & S_IFMT) == S_IFIFO)
	{ 
		cout<<"\e[40m\e[33m";
	}
	else if ((st.st_mode & S_IFMT) == S_IFLNK)
	{ 
		cout<<"\e[1m\e[36m";
	}
	else if ((st.st_mode & S_IFMT) == S_IFSOCK)
	{ 
		cout<<"\e[1m\e[56m";
	}
	else if ((st.st_mode & S_IFMT) == S_IFBLK)
	{ 
		cout<<"\e[40m\e[33m\e[1m";
	}
	
	else if ((st.st_mode & S_IFMT) == S_IFCHR)
	{ 
		cout<<"\e[40m\e[33m\e[1m";
	}
	
	if(strstr(name,".tar")!=NULL || strstr(name,".tgz")!=NULL || strstr(name,".arj")!=NULL || strstr(name,".zip")!=NULL || strstr(name,".gz")!=NULL || strstr(name,".deb")!=NULL) 
	{ 
		cout<<"\e[1m\e[31m";
	}
 
 	else if(strstr(name,".exe")!=NULL || strstr(name,".com")!=NULL || strstr(name,".btm")!=NULL || strstr(name,".bat")!=NULL) 
 	{ 
 		cout<<"\e[1m\e[32m";
 	} 
 	else if(strstr(name,".png")!=NULL || strstr(name,".bmp")!=NULL || strstr(name,".ppm")!=NULL || strstr(name,".tga")!=NULL || strstr(name,".mpg")!=NULL || strstr(name,".avi")!=NULL || strstr(name,".gl")!=NULL || strstr(name,".dl")!=NULL || strstr(name,".tif")!=NULL || strstr(name,".jpg")!=NULL ) 
 	{ 
 		cout<<"\e[1m\e[35m"; 
 	} 
 }
 
/********************************    -t     *********************************/

int timeCompare(const struct dirent** first, const struct dirent** second)
{

	struct stat st1,st2;
	lstat((*first)->d_name,&st1);
        lstat((*second)->d_name,&st2);
      	int compare = 0;
	if(st1.st_mtime > st2.st_mtime)
		compare = -1;
	else if(st1.st_mtime == st2.st_mtime)
		compare = 0;
	else 
		compare = 1;
	
	
	return compare;

}

/********************************    -S     *********************************/

int sizeCompare(const struct dirent** first, const struct dirent** second)
{

	struct stat st1,st2;
	lstat((*first)->d_name,&st1);
        lstat((*second)->d_name,&st2);
      	
	int compare = 0;
	if(st1.st_size > st2.st_size)
		compare = -1;
	else if(st1.st_size == st2.st_size)
		compare = 0;
	else 
		compare = 1;
	
	return compare;

}

/********************************    -h     ********************************/
void human_read()
{
	if(size<1024)
	{
		sc_flag ='B';
		return;		
	}
	else
	{
		size=size/1024;
		sc_flag='K';
		if(size>1024)
		{
			size=size/1024;
			sc_flag='M';
			if(size>1024)
			{
				size=size/1024;
				sc_flag='G';
				return;
			}
			else
				return;
		}
		else
			return;
	}
}
/********************************** size *********************/
void sflag(int size)
{
	if(s_flag)
	{
		int wid=1;
		
		if(maxSize >= 10000)
			wid=7;
		else if(maxSize >= 1000)
			wid=4;
		else if(maxSize >= 100)
			wid=3;
		else if(maxSize >= 10)
			wid=2;
		
		cout.width(wid);
		cout.setf(ios::right,ios::adjustfield);
		cout<<size;
		cout<<" ";		
	}
}
	
/********************************** -l   ********************/
void lflag(char* path, struct dirent * dp, struct stat st)
{
	sflag(st.st_blocks/2);
	//St mode
	if ((st.st_mode & S_IFMT) == S_IFDIR)		
	{		
		dir_flag=1;
		printf("d");
	}
	else if ((st.st_mode & S_IFMT) == S_IFREG)
	{ 
		printf("-");
	}
	
	else if ((st.st_mode & S_IFMT) == S_IFIFO)
	{ 
		printf("p");
	}
	else if ((st.st_mode & S_IFMT) == S_IFLNK)
	{ 
		printf("l");
	}
	else if ((st.st_mode & S_IFMT) == S_IFSOCK)
	{ 
		printf("s");
	}
	
	else if ((st.st_mode & S_IFMT) == S_IFBLK)
	{ 
		printf("b");
	}
	
	else if ((st.st_mode & S_IFMT) == S_IFCHR)
	{ 
		printf("c");
	}
	
	//permission 
	mode=st.st_mode;
	mode=mode & 511;
	
	permission(mode);
	printf("%s ",mode_str);
	
	// User and group name
	struct passwd *p1;
	struct group *g1;
	struct tm *tm1 = gmtime(&st.st_mtime);
		
	p1= getpwuid(st.st_uid);
	g1= getgrgid(st.st_gid);
	
	//No. of hard links
	cout.width(5);
	cout.setf(ios::right,ios::adjustfield);
	cout<<(st.st_nlink);
	
	//User name
	cout.width(10);
	cout.setf(ios::right,ios::adjustfield);
	cout<<(p1->pw_name);
	
	//Group name
	cout.width(10);
	cout.setf(ios::right,ios::adjustfield);
	cout<<(g1->gr_name);
	
	//size for -h option
	if(h_flag)
	{
		size = st.st_size;
		human_read();
		cout.width(10);
		cout.setf(ios::right,ios::adjustfield);
		cout<<size<<sc_flag;
	}
	else{
		cout.width(10);
		cout.setf(ios::right,ios::adjustfield);
		cout<<st.st_size;
	}
	cout<<" ";
	
	//Day month and time 
	cout<<month[tm1->tm_mon]<<" ";
	
	cout.width(2);
	cout.setf(ios::right,ios::adjustfield);
	cout<<tm1->tm_mday<<" ";
	
	if((tm1->tm_year) <113)
		cout<<" "<<1900+(tm1->tm_year)<< " ";
	else
	{
		if(tm1->tm_hour < 10)
			cout<<0;
		cout<<(tm1->tm_hour)<<":";
		if(tm1->tm_min < 10)
			cout<<0;
		cout<<(tm1->tm_min)<<" ";
	}
	
	printColor(st,dp->d_name);
	//File or directory name
	for(int i=0;i<strlen(dp->d_name);i++)
	{
		printColor(st,dp->d_name);
		cout<<(dp->d_name[i]);
	}
	cout<<"\e[0m";
	
	//symbolic link
	if ((st.st_mode & S_IFMT) == S_IFLNK)
	{ 
		char path1[512]= {'\0'};
		char temp[2000];
		strcpy(temp,path);
		strcat(temp,"/");
		strcat(temp,dp->d_name);
		int er = readlink(temp,path1,512);
		if(er != -1)

			cout<<"->"<<path1;
	}
	cout<<endl;
}

/************************ Parse command options *********************/
void parse(int opt)
{
	switch(opt)
	{
	case 'l':
		l_flag=1;
		break;
		
	case 'a':
		a_flag=1;
		break;
	case 'A':
		A_flag=1;
		break;
	case 'R':
		r_flag=1;
		break;
	case 'h':
		h_flag=1;
		break;
	case 'd':
		d_flag=1;
		break;
	case 's':
		s_flag=1;
		break;
	case 'S':
		if(!t_flag)
		S_flag=1;
		break;
	case 't':
		if(!S_flag)
		t_flag=1;
		break;
	default:/* ? */
		cout<<"ls: Invalid option.\nTry 'ls --help' for more information."<<endl;
	}
	
	if(d_flag)
	{
		a_flag=0;
		r_flag=0;
	}
	
}

/******************************* Display directory ********************/
void display_dir(char* curr)
{
	chdir(curr);	
	DIR * dirp1 = opendir(curr);
	struct stat st;
	long int total = 0;
	int i,n;
	struct dirent ** dirlist;
	
	if(r_flag)
		cout<<"\n"<<curr<<":\n";
		
	if(S_flag)
		n = scandir(curr, &dirlist, 0, sizeCompare);
	else if(t_flag)
		n = scandir(curr, &dirlist, 0, timeCompare);
	else
	n = scandir(curr, &dirlist, 0, alphasort);
		
	struct stat * starr=new struct 	stat[n];
	if(n<0)
	{
		i=errno;
		printf("%s\n",strerror(i));
	}
	
	//Create a array of stat
	for(int k=0;k<n;k++)
	{
		i=lstat(dirlist[k]->d_name, &st);

		if(maxlen < strlen(dirlist[k]->d_name))
			maxlen=strlen(dirlist[k]->d_name);
		if(i<0)
		{
			i=errno;
			continue;
		}
		
		if(i>=0)
		{
			starr[k]=st;
		}	
		if(s_flag && i>=0)
		{
			if(maxSize< st.st_blocks/2)
				maxSize=st.st_blocks/2;
			
		}
	
		if(!a_flag && dirlist[k]->d_name[0]=='.')
			continue;
		if(i>=0)
			total=total+st.st_blocks;
	}
	
	if(l_flag || s_flag)
		cout<<"total "<<total/2<<endl;
	int count= 0;
	
	//ls -l
	if(!a_flag && !d_flag)	
	{		
		for(int k=0;k<n;k++)
		{
			
			if(A_flag && dirlist[k]->d_name[0]=='.' && dirlist[k]->d_name[1]=='\0')
			{
				continue;
			}
			else if(A_flag && dirlist[k]->d_name[0] == '.' )//&& dp1->d_name[1]== '.')
			{
				continue;
			}
			else if(dirlist[k]->d_name[0]== '.' && A_flag==0)
			{
				continue;
			}
			else
			{
				if(l_flag)
				{
					lflag(curr,dirlist[k],starr[k]);
					dir_flag=0;
				}
				else
				{
					sflag(starr[k].st_blocks/2);
					count++;
					printColor(starr[k],dirlist[k]->d_name);
					cout.width(maxlen+1);
					cout.setf(ios::left,ios::adjustfield);
					cout<< dirlist[k]->d_name;
					cout<<"\e[0m";
					if(count % 3 == 0)
					cout<<endl;
				}
			}				
		
		}
	
	}
	
	//ls -d
	else if(d_flag)
	{
		for(int k=0;k<n;k++)
		{
			
			if(l_flag)
			{
				if(dirlist[k]->d_name[0]=='.' && dirlist[k]->d_name[1]=='\0')
					lflag(curr,dirlist[k],starr[k]);
					
				dir_flag=0;
			}
			else 
			{
				if(dirlist[k]->d_name[0]=='.' && dirlist[k]->d_name[1]=='\0')
				{
					
					sflag(starr[k].st_blocks/2);
					count++;
					printColor(starr[k],dirlist[k]->d_name);
					cout.width(maxlen+1);
					cout.setf(ios::left,ios::adjustfield);
					cout<< dirlist[k]->d_name;
					cout<<"\e[0m";
					if(count % 3 == 0)
						cout<<endl;
				}
			}
		}
	}
	//ls -a
	else
	{
		for(int k=0;k<n;k++)
		{
			if(i<0)
			{
				i=errno;
			//	printf("Error in stat() is   %s\n",strerror(i));
			}
			if(l_flag)
			{
				lflag(curr,dirlist[k],starr[k]);
				dir_flag=0;
			}			
			else		
			{	
				sflag(starr[k].st_blocks/2);
				count++;
				printColor(starr[k],dirlist[k]->d_name);
				cout.width(maxlen+1);
				cout.setf(ios::left,ios::adjustfield);
				cout<< dirlist[k]->d_name;
				cout<<"\e[0m";
				if(count % 3 == 0)
					cout<<endl;
			}
			
		}
	}
   	if(!l_flag && (count%3 != 0) )
		cout<<endl;

        /* Close the directory. */
	closedir (dirp1);
	
	//For ls -R
	if(r_flag)
	{
		for(int k=0;k<n;k++)
		{
			if ((starr[k].st_mode & S_IFMT) == S_IFDIR )
			{
			
				if(dirlist[k]->d_name[0]== '.'|| strcmp(dirlist[k]->d_name,again)==0)
				{	
					continue;
				}
				int length=strlen(curr);
				char pathn[length+512];
				strcpy(pathn,curr);
				strcat(pathn,"/");
				strcat(pathn,dirlist[k]->d_name);
				display_dir(pathn);
				strcpy(again,dirlist[k]->d_name);
			}
		}
	}
	
	delete[] starr;
}


/******************************* Main **************************/

int main(int argc, char *argv[])
{
	int result=-1;
	char* current_dir= (char*)calloc(strlen(get_current_dir_name())+1,sizeof(char));
	char* cur_dir=new char[1024];
	strcpy(cur_dir,".");
	int opt,k=0,disp=0,path_count=0;
	char* path;
	int root=0;
	strcpy(current_dir,get_current_dir_name()) ;
	if(argc >1)	
	{
		while((opt=getopt(argc,argv,"ladRsthASt"))!=-1)
			parse(opt);
		k=1;
		while(argv[k] != NULL)
		{
			int length=strlen(argv[k]);
			path = new char[length+1];
			strcpy(path,argv[k]);
			
			if((path[0] == '/')|| (path[0] == '.' && path[1] == '\0')||( path[0] == '.' && path[1] == '.')|| isalnum(path[0]))
			{
				if((path[0] == '.' && path[1] == '\0'))
					root=k;
				path_count++;
			}
			k++;
			if(k>= argc)
				break;
		}
		k=1;
		
		if(path_count>=2 && root !=0)
		{
			cout<<"."<<":"<<endl;
			
			display_dir(current_dir);
			chdir(current_dir);
			disp=1;
		}	
		
		while(argv[k] != NULL)
		{
			int length=strlen(argv[k]);
			path = new char[length+1];
			strcpy(path,argv[k]);
			
			if(path[0] == '/')
			{
				chdir(path);
				if(path_count>=2)
					cout<<path<<":"<<endl;
				display_dir(path);
				chdir(current_dir);
				disp=1;
			}
			else if( path[0] == '.' && path[1] == '\0')
			{
				k++;
				if(k>= argc)
					break;
				continue;
			}
			else if( path[0] == '.' && path[1] == '.')
			{
				char* path1= new char[strlen(current_dir)+length+4];
				strcpy(path1,current_dir);
				strcat(path1,"/");
				strcat(path1,path);
				if(path_count>=2)
					cout<<path1<<":"<<endl;				
				display_dir(path1);
				chdir(current_dir);
				disp=1;
			}
			if(!disp)
			{
				if(path[0]=='.'&& path[1]!='\0' &&  k != 0)// && (path[2]=='a' && path[3]!='.'))
				{
					char* path1= new char[strlen(current_dir)+length+3];
					strcpy(path1,current_dir);
					strcat(path1,"/");
					strcat(path1,path+2);
				
					display_dir(path1);
					chdir(current_dir);
					disp=1;
				}
				
				else if(isalnum(path[0]))
				{
					char* path1= new char[length+4];
					strcpy(path1,"./");
					strcat(path1,path);
					if(path_count>=2)
						cout<<path1<<":"<<endl;
					display_dir(path1);
					chdir(current_dir);				
					disp=1;
				}
				

			}
			k++;
			if(k>= argc)
				break;
		}	
		if( !disp) 
		{	
			display_dir(current_dir);	
		
		}
	}
	
	else 
		display_dir(current_dir);	
	
	free(cur_dir);
	
	return 0;
}
