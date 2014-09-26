/*
Roll No.: 201305549
Name    : Gangasagar Patil

*/

#include "def.h"

//Reverse the content of the file
int reverseFile(char *iFilePath)
{
	int iDescriptor = -1, readRes = -1, oDescriptor = -1, i = 0, fileSize = 0;
	int j=0, counter=0;
	int charSize=strlen(iFilePath);
	char* oFile = (char*)malloc(256*sizeof(char));//Output file name
	char* iFile = (char*)malloc(256*sizeof(char));//Input file name
	char* tempArr=(char*)malloc(256*sizeof(char));//Temperary buffer for reading the data from file
	char ch;
	
	//Separate filename from file path
	for(i=charSize-1; i>=0; i--)
	{
		if(iFilePath[i]=='/')
		{
			for(j=0;j<charSize-i;j++)
			  iFile[j]= iFilePath[i+j+1];
			break;		
		}
		
		else if(i==0)
			strcpy(iFile,iFilePath);		
	}
	
	//Create output file name
	snprintf(oFile, 255, "%s%s", "reverse_", iFile);
	
	//Open both input and output file. If output file not present, then create new one.
	iDescriptor = open(iFilePath,O_RDONLY );
	oDescriptor = open(oFile,O_CREAT|O_RDWR,S_IRWXU|S_IWUSR);

	if(iDescriptor == -1)
	{
		printf("Unable to open the input file. Please try again\n");
		return -1;
	}
	if(oDescriptor == -1)
	{
		printf("Unable to open the output file. Please try again\n");
		return -1;
	}
	
	//Check for file size 
	fileSize = lseek(iDescriptor,0, SEEK_END);
	if(fileSize > 0)
	{
		counter = fileSize/256;
		for(i = counter; i >= 0 ; i--)
		{
			//Go to offset where we want to read the character
			lseek(iDescriptor, (long int) (256*i),SEEK_SET);
					
			readRes = read(iDescriptor,tempArr,256);
			if(readRes < 0) //If no data is read
			{
				printf("Error while reading data \n");	
				continue;
			}	
		
			//Reversal of tempArr				
			for(j=0; j<(readRes/2); j++)
			{	
				//Swap characters
				ch=tempArr[j];
				tempArr[j]=tempArr[readRes-1-j];
				tempArr[readRes-1-j]=ch;
			}
			//Write the swapped buffer to output file
			write(oDescriptor,tempArr,readRes);
		}
	}
	
	//Free all the memory allocated and close all open files
	close(iDescriptor);
	close(oDescriptor);
	free(oFile);
	free(tempArr);
	free(iFile);
	
	return 0;
}

