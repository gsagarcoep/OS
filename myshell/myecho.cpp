/*
Roll No.: 201305549
Name    : Gangasagar Patil

*/

#include "def.h"

vector<string> local_var;


/*************** set local variable **************************/
/*	car1=sagar
 * 
/***********************************************************/
void set_var(string args)
{
	int equal_pos=args.find('=');
	
	//Create tokens separated by ' '
	char ** cstr=new char*[args.length()+1];
	char * tmpstr=new char[args.length()+1];
	std::strcpy (tmpstr, args.c_str());
	
	char * p = std::strtok (tmpstr," ");

 	int cnt=0;
 	while(p!=0)
 	{
 		cstr[cnt]=new char[strlen(p)+1];
 		strcpy(cstr[cnt],p);
 		p = strtok(NULL," ");
 		cnt++;
 	}
 	int fnd=0;
 	for (int j=0;j<cnt;j++)
 	{
 		string temp(cstr[j]);
 		int equal=temp.find('=');
 		string find=temp.substr(0,equal);
		int locasize=local_var.size();
		
		//Check local variable first
		for(int k=0;k<locasize;k++)
		{
			if(local_var[k].length()>=find.length())
			{
				if(!(local_var[k].compare(0,find.length(), find)))
				{
					//if present in local var,then update the value 
					local_var[k]=temp;
					fnd=1;
					break;
				}
			}
		}
		if(fnd==0)
			local_var.push_back(temp);
	}

}
/*************** export command **************************/
/*
 * 
/***********************************************************/
void myexport(char ** cstr)
{
	int fnd=0;
	int cnt=0;
 	while(cstr[cnt]!= NULL)
		cnt++;
	
 	for (int j=1;j<cnt;j++)
 	{
 		string find(cstr[j]);
 		int locasize=local_var.size();
		string value;
		
		//Check local variable first
		for(int k=0;k<locasize;k++)
		{
			if(local_var[k].length()>=find.length())
			{
				if(!(local_var[k].compare(0,find.length(), find)))
				{
					//if present in local var,then call setenv 
					value=local_var[k].substr(find.length()+1);
					
					if(setenv(find.c_str(),value.c_str(),1) != 0)
					{	
						cout<<strerror(errno)<<endl;
					}
					fnd=1;
					break;
				}
			}
		}
	}
}

/*************** echo command **************************/
/*
 * 
/***********************************************************/

void myecho(char** envp,char* const* args)
{

	vector<string> echosys;
	vector<string> arg;
	int i=0,count=0;
	while(envp[i]!= NULL)
	{	
		string temp(envp[i]);
		echosys.push_back(temp);
		count++;
		i++;
	}
	
	if( args[0] != NULL && !strcmp(args[0],"echo"))
	{
		int i=0;
		while(args[i]!= NULL)
		{	
			arg.push_back(args[i]);
			i++;
		}
		string found;
		int k=0;
		for(int j=1;j<i;j++)
		{
			int fnd=0;
			if(arg[j].at(0) == '$' && (arg[j].at(1)!=' ' && arg[j].at(1)!='|' && arg[j].at(1) != '$'))
			{
				int len=0;
				while((++len)< arg[j].length() && arg[j].at(len)!=' ' && (arg[j].at(len)!='|'));
				
				string find=arg[j].substr(1,len);
				int locasize=local_var.size();
				
				//Check local variable first
				for(k=0;k<locasize;k++)
				{
					if(local_var[k].length()>=find.length())
					{
						if(!(local_var[k].compare(0,find.length(), find)))
						{
							found=local_var[k].substr(find.length()+1);
							cout<<found<<"\t";
							fnd=1;
							break;
						}
					}
				}
				
				//if not present in local, chekc system variable
				if(fnd==0)
				for(k=0;k<count;k++)
				{
					if(echosys[k].length()>=find.length())
					{
						if(!(echosys[k].compare(0,find.length(), find)))
						{
							found=echosys[k].substr(find.length()+1);
							cout<<found<<"\t";
							fnd=1;
							break;
						}
					}
				}
			}
			// echo $$
			else if(arg[j].at(0) == '$' && arg[j].at(1) == '$')
			{
				cout<<getpid()<<"\t";
				fnd=1;
			}
			else
			{	
				cout<<arg[j]<<"\t";
				fnd=1;
			}
			
			if(fnd==0)
			{	
				cout<<"\t";
			}
		}
	}
	cout<<endl;
}

