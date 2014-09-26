/*
Roll No.: 201305549
Name    : Gangasagar Patil

*/

#include "def.h"
int is_pipe=0;
int curr_pid=0;
vector<int> procid;
int bg_cnt=0;
/***********************************************/
/*
 * siqproc() handles ctrl+c 
 * sig_t inp= input signal to be handled 
 */
/***********************************************/
void sigproc(int inp)
{
	
	//printf("%d",inp);
	//signal(SIGINT, sigproc);
    //	printf("\n Cannot be terminated using Ctrl+C \n");
	//fflush(stdout);
//	printPrompt();
//	return;
}

/***********************************************/
//Print the prompt to take input
/***********************************************/
void printPrompt()
{

	string curr,dirname;	
	
	curr=get_current_dir_name();
	cout<<"\e[1m\e[32mRunning@"<<getlogin()<<":~";
	
	for(int i=curr.length()-1;i>=0;i--)
	{
		
		dirname.push_back(curr.at(i));
		if(curr.at(i) == '/')
			break;
	
	}
	
	for(int j=dirname.length()-1;j>=0;j--)
	cout<<dirname.at(j);
	
	cout<<"$ "<<"\e[0m";
		
}

void parseArgs(string input,string &command, string &args)
{
	//string command,args;
	
	for(int i=0;i<input.length();)	
	{
		while(i<input.length() && input.at(i) != ' ')
		{	
			command.push_back(input.at(i));
			i++;
		}
	
		if(i<input.length())
			args=input.substr(++i);
		break;
	}
	
}

/***********************************************/
/* For <<
/***********************************************/

void inin_redirect(string fileName)
{
	FILE* fd=fopen("temp.txt","w");
	string ip;

	while(1)
	{
		getline(std::cin,ip);
		if(ip.compare(fileName)==0)		
		{

			break;
		}
		
		//Write the input string to temp file
		fprintf(fd,"%s\n",ip.c_str());
		
	}

	fclose(fd);	
}

/********************** Execute your command ************************/
/* It executes every command, creates child, handles pipe.
 *
 * input: command string exluding meta chracters
 * args : entire input string including every meta character
 * command: command name
 * cstr: tokenised input string input
 *
/********************************************************************/
void executeComm(const char* command, char* const* cstr,string input,string args,char** envp)
{
	pid_t pid, pid2	;
	int *status,*status1;
	int np = std::count(input.begin(), input.end(), '|');
	int out_redi=0,in_redi=0,outout_redi=0,inin_redi=0;
	string ip,fileName;
	int refd;
	int bg_pos=input.find('&');
	
	/************************ Foreground ***************************/
	if(strcmp(command,"fg")==0)
	{
		if(procid.size()>0)
		{
			bg_cnt--;
			
			kill(procid[bg_cnt], SIGCONT);
			waitpid(procid[bg_cnt],status,0);
			procid.pop_back();
		}
	
	}
	/********************* For redirection **************************/
	/****************** input ****************/
	int redirect_pos=args.find('<');
	if(redirect_pos >0)
	{
		in_redi=1;
		
		if(redirect_pos<=args.length()-1)
		{	
			if(args.at(++redirect_pos)=='<')
			{
			 	inin_redi=1;
				while(redirect_pos < args.length()-1 && args.at(++redirect_pos)==' ');
							
			}
		
			if(!inin_redi)
				while(redirect_pos < args.length()-1 && args.at(redirect_pos)==' ')
					redirect_pos++;
				
			fileName=args.substr(redirect_pos);
		}
	}
	
	/**************** Output ****************/
	redirect_pos=args.find('>');
	if(redirect_pos >0)
	{
		out_redi=1;
		
		if(redirect_pos<args.length()-1)
		{	
			if(args.at(++redirect_pos)=='>')
			{
			 	outout_redi=1;
			 	while(redirect_pos < args.length()-1 && args.at(++redirect_pos)==' ');
			}
			
			if(!outout_redi)
				while(redirect_pos < args.length()-1 && args.at(redirect_pos)==' ')
					redirect_pos++;
			

			fileName=args.substr(redirect_pos);
		}
	}
	/************** If Pipe is absent ************************/	
	
	
	if(np==0)
	{
		//Create a child process for executing the command
		pid = fork();
 		curr_pid=pid;
 	      	//wait for the child process to terminate
 	      	if(pid)
 	      	{
 	      		if(bg_pos<=0)
 	      		{
 	      		  	waitpid(pid,status,0);
 	      		}
 	      		else
 	      		{
 	      			procid.push_back(pid);
 	      			cout<<"["<<++bg_cnt<<"]\t"<<pid<<endl;	
 	      			kill(pid, SIGSTOP);
 	      		  }
 	      		curr_pid=0;
 	     	//	cout<<"jigar"<<endl;	
 	     	} 
 	      	
 	      	if(pid==0)
 	      	{
 	      		if(out_redi)
 	      		{
 	      			if(outout_redi)// >>
 	      				refd=open(fileName.c_str(),O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);

 	      			else // >
 	      				refd=open(fileName.c_str(),O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU);

				if(refd!= -1)
				{
					dup2(refd,fileno(stdout));
				}
			}
			else if(in_redi)
 	      		{
 	      			if(!inin_redi) // <
 	      			{
 	      				refd=open(fileName.c_str(),O_RDWR,S_IRWXU|S_IWUSR);
					if(refd!= -1)
						dup2(refd,fileno(stdin));
				}
				else // <<
				{
					inin_redirect(fileName);
					refd=open("temp.txt",O_RDWR,S_IRWXU|S_IWUSR);
					
					if(refd!= -1)
						dup2(refd,fileno(stdin));

				}
				
			}
			
			//history
 	    		if(strcmp(command, "history") == 0)
 			{
 				histo();
 				exit(0);
 			}
 			else if(strcmp(command, "fg") == 0)
 			{
 				
 				exit(0);
 			}		  		
 	      		else if(execvp(command, cstr)<0)
 	      		{
 	      			cout<<strerror(errno)<<endl;
 	             		exit(127);
 	      		} 
 	      	             	
 	        }
		return;
	}
	/*************** If Pipe is Present*********************/
	else
	{
	
	is_pipe=0;

	if (np>0)
	{
		//Parse the input and store in array of strings 
		//separating the input in accodance with the pipe
		
		string *p_arr=new string[np+1];
		int start=0,max=0;
		int p_count=np,j=0;
		for(int i=0;i<input.length()-1;)
		{
			if(input.at(i)== '|')
			{
				p_arr[j]=input.substr(start,i-start);
				j++;
				p_count--;
				while(i<input.length()-1 && (input.at(++i)== ' ' || input.at(i)=='\t') );
				start=i;
		
			}
			if(p_count==0)
			{
				p_arr[j]=input.substr(i);
	//			if(max<p_arr[i].length())
	//				max=p_arr[i].length();
				j++;
				break;
			}
			i++;
		}
		
		//Convert all the input data to required char* to pass execvp
		char*** p_cstr=new char**[j];
		for(int i=0;i<j;i++)
		{
			//Create char* array for execvp
			char ** cstr=new char*[p_arr[i].length()+1];
			char * tmpstr=new char[p_arr[i].length()+1];
			std::strcpy (tmpstr, p_arr[i].c_str());
		
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
 			p_cstr[i]=cstr;	
		}
		
		int ipipe[100][2];		
 		for (int i=0;i<j;i++)
 		{
 			//Create a child process for executing the command
		
			if(pipe(ipipe[i])<0)
			{
				perror("Pipes");
				return;
			}
		
			pid = fork();
	 	
		 	if(pid<0)
		 	{
		 		cout<<"Process ccreation failed ! "<<endl;
		 		exit(-1);
		 	}
	 	
		 	//wait for the child process to terminate
		       	if(pid==0)
		       	{	
		       		
		       		if(i==0)
		       		{  
		       			close(ipipe[i][0]);
		       			//close input of pipe
		       			if(out_redi)
 	      				{
 	      					if(outout_redi)// >>
 	      						refd=open(fileName.c_str(),O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);

 	      					else // >
 	      						refd=open(fileName.c_str(),O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU);
					
						if(refd!= -1)
							dup2(refd,fileno(stdout));
					}
					dup2(ipipe[i][1], fileno(stdout));
					close(ipipe[i][1]);
					
				}
				else if(i==j-1) 
			       	{
					dup2(ipipe[i-1][0],fileno(stdin));
					close(ipipe[i-1][0]);
					if(out_redi)
 	      				{
 	      					if(outout_redi)// >>
 	      						refd=open(fileName.c_str(),O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);

 	      					else // >
 	      						refd=open(fileName.c_str(),O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU);

						if(refd!= -1)
							dup2(refd,fileno(stdout));
					}
			
			        }
			        else
			        {
			        	dup2(ipipe[i-1][0],fileno(stdin));
					close(ipipe[i-1][0]);
					if(out_redi)
 	      				{
 	      					if(outout_redi)// >>
 	      						refd=open(fileName.c_str(),O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);

 	      					else // >
 	      						refd=open(fileName.c_str(),O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU);
				
						if(refd!= -1)
							dup2(refd,fileno(stdout));
					}
					dup2(ipipe[i][1],fileno(stdout));
					close(ipipe[i][1]);
						//cout<<i;
			        }
			        
			        // for echo
			        if(strcmp(p_cstr[i][0], "echo") == 0)
 				{
 					myecho(envp,p_cstr[i]);
 					exit(0);
				}
				else if(strcmp(p_cstr[i][0], "history") == 0)
 				{
 					histo();
 					exit(0);
				}
				else if(strcmp(command, "fg") == 0)
 				{
 					exit(0);
 				}				
				else if(execvp(p_cstr[i][0], p_cstr[i])<0)
				{
	 	      			cout<<strerror(errno)<<endl;
	 	        	     	exit(127);
 	      			} 
 	      		//pid = wait(status);
		      	} 
	       	else  
	       	{	
	       		//wait for process 1 to finish
	       		//if(bg_pos<=0)
 	      		  	waitpid(pid,status,0);
 	      	/*	else
 	      		{
 	      			procid.push_back(pid);
 	      			cout<<"["<<++bg_cnt<<"]\t"<<pid<<endl;	
 	      			kill(pid, SIGTSTP);	
			}
		*/	close(ipipe[i][1]);
	       	}
		
 		
 		}			
	}}
	close(refd);
}

