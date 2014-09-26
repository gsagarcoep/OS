/*
My shell


*/

#include "def.h"

/***********************************************/

void fileenv(char** envp)
{

	FILE* fd=fopen(".myshellrc","a+");
	int i =0;
	
	if(fd!=NULL)
	{
		while(envp[i]!=NULL)
		{
	//	cout<<envp[i]<<endl;
			fprintf(fd,"%s\n",envp[i]);
			i++;
		
		}
		fclose(fd);
	}
}


/***********************************************/
/*
 *  main() 
 */
/***********************************************/
int main (int argc, char* argv[],char** envp)
{
	vector<string> history;
	string ex("exit");
	string his("history");	
	signal(SIGINT,sigproc);
	int i=0;
	
	while(1)
	{
		
		string input,args;
		int is_pipe_main=0,pipe_position=-1;
		//Print the prompt
		//cout<<"process id "<<getpid()<<endl;
		printPrompt();
		getline(std::cin,input);
		args=input;
		
		if(input.compare("\0")==0)
			continue;
			
		if(input.at(0)==' ')
		{
			int l=0;
			int temp=0;
			while(l<input.length())
			{
				if(input.at(l)!=' ' && input.at(l)!='\0')
				{
					temp=1;
					break;
				}
				l++;
			}	
			if(temp!=1)	
				continue;
		
		}
		
		/*********************** If exit ******************/
		if(input.compare(ex)== 0)
		{	
			exit(0);
		}
		
 		/************************ History ******************/
		if(input.at(0)=='!')
		{
			banghis(input,args);
			int pipe_pos=input.find('|');
			
			if(pipe_pos >0)
			{	
				args.append(input.substr(pipe_pos));		
			}
			input=args;
		}

 		/****** History *********/
 		add_histo(input);

		
		/******************* export = ************************/
		int equal_pos=input.find('=');
		if(equal_pos >0)
		{
			if(input.at(equal_pos-1)!=' ' && (input.at(equal_pos-1)!=' '))
			{
				set_var(input);
				continue;
			}
			
		}
		/********************* Redirection *******************/
		//For output redirection
		int redirect_pos=input.find('>');
		if(redirect_pos >0)
		{
			if(input.at(redirect_pos+1)=='>')
			{
			 	string ip= input.substr(0,redirect_pos);
				args=ip;
			}
			
			if(input.at(redirect_pos-1)==' ')
			{
				string ip= input.substr(0,redirect_pos);
				args=ip;
			}
						
		}
		//For input redirection
		redirect_pos=input.find('<');
		if(redirect_pos >0)
		{
			if(input.at(redirect_pos+1)=='<')
			{
			 	string ip= input.substr(0,redirect_pos);
				args=ip;
			}
			
			else if(input.at(redirect_pos-1)==' ')
			{
				string ip= input.substr(0,redirect_pos);
				args=ip;
			}
		}
		
		/************ End of Redirection *******************/
		
		
		
				
		/************* Create char* array for execvp *************/
		char ** cstr=new char*[args.length()+1];
		char * tmpstr=new char[args.length()+1];
		std::strcpy (tmpstr, args.c_str());
	
 		char * p = std::strtok (tmpstr," ");

 		int cnt=0;
 		while(p!=0)
 		{
 			if(strcmp(p,"&")==0)
 			{
 				p = strtok(NULL," ");
 				continue;
 			}
 			cstr[cnt]=new char[strlen(p)+1];
 			strcpy(cstr[cnt],p);
 			p = strtok(NULL," ");
 			cnt++;
 		}
		cstr[cnt]=new char[3];
 		cstr[cnt]='\0';
 		// Done with creation of char* array
 		
 		int pipe= input.find('|');	
 		
		if(strcmp(cstr[0], "export")== 0)
		{
			myexport(cstr);
		//	cout<<getenv(cstr[1]);
			continue;
		
		}
		else if(strcmp(cstr[0], "echo") == 0 && pipe<=0)
 		{
 			myecho(envp,cstr);
 			continue;
 		}		
 		else if(strcmp(cstr[0], "cd") == 0)
		{
			string temp;
			if(cnt==1 || strcmp(cstr[1],"~")==0)
			{
				string t("/home/");
				t.append(getlogin());
				temp=t;
				chdir(temp.c_str());
			}
			else
				chdir(cstr[1]);
		}
		else
 			executeComm(*cstr, cstr,args,input,envp);
 	}
	return 0;
}
