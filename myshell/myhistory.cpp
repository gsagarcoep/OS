/*
Roll No.: 201305549
Name    : Gangasagar Patil

*/

#include "def.h"

/*************** history operation **************************/
/*Display its for history command
/***********************************************************/
void histo()
{
	FILE* fd=fopen("history.txt","r+");
	char *item=new char[1000];		
	int i=0;
	size_t j=0;		
	
	if(fd != NULL)
	{
	
		while(!feof(fd))
		{
			if(getline(&item,&j,fd)!= -1)
			{	
				cout<<" "<<++i<<"  "<<item;
			}
		}
	
		fclose(fd);		
	}
}

/**************************** Add to history **************************/
//It pushes commands in the history file 

void add_histo(string input)
{
	FILE* fd=fopen("history.txt","a+");
	
	if(fd != NULL)
	{
		fprintf(fd,"%s\n",input.c_str());
		fclose(fd);	
	}	
}

/*********************************************************************/
/* For commands like !ls 
/*********************************************************************/
void banghis(string input,string &args)
{
	FILE* fd=fopen("history.txt","a+");
	char *item=new char[1000];		
	size_t j=0;		
	int count=0;
	vector<string> history;
	
	if(fd != NULL)
	{
	
		while(!feof(fd))
		{
			if(getline(&item,&j,fd)!= -1)
			{
				string temp(item);
				string temp1=temp.substr(0,temp.length()-1);
				history.push_back(temp1);
			}
		}
	
		fclose(fd);
	}
	
	if(input.at(0)=='!')
	{
		int pipe_pos=input.find('|');
		string temp=input;	
		if(pipe_pos >0)
		{	
			while(input.at(pipe_pos)==' ')
				pipe_pos--;
			temp=input.substr(0,pipe_pos);
			cout<<temp<<endl;
		}
		
		string find=temp.substr(1);
		int h=atoi(find.c_str());
		int i=history.size()-1;
		
		if(h!=0 && i>=h)
		{
			args=history[h];
		
		}	
		else //if((input.at(1) > 'a' && input.at(1)<'z')||(input.at(1) > 'Z' && input.at(1)<'Z'))
		{
			
			for(;i>=0;i--)
			{
				if(history[i].length()>=find.length())
				{
					if(!(history[i].compare(0, find.length(), find)))
					{
						args=history[i];
						break;
					}
				}
			}
		}
	}
}
/***************** End of history Operation *************************/

/*********************** Not used ***********************************/

void hist(std::vector<string>& history)
{
	//create an iterator(static class iterator)
	vector<string>::iterator itr;
	
	for(itr=history.begin();itr!=history.end();itr++)
		cout<<*itr<<endl;
			
}

