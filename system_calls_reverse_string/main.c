/*
Roll No.: 201305549
Name    : Gangasagar Patil

*/

#include "def.h"

int main(int argc, char *argv[])
{
	int result = -1;
	if (argc != 2)
	{
		printf("No input File specified \n");
	}
	
	if(argv[1] != NULL)
		result = reverseFile(argv[1]);
		
	return 0;
}
