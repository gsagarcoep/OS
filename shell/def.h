/*
Roll No.: 201305549
Name    : Gangasagar Patil

Myshell
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <string.h>
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <cstdio>
#include <vector>
#include <sys/wait.h>
#include <fcntl.h>
#include <algorithm>
#include <signal.h>

using namespace std;

void printPrompt();
void hist(std::vector<string>& history);
void parseArgs(string input,string &command, string &args);
void histo();
void executeComm(const char* command, char* const* cstr,string input,string args,char** envp);
void sigproc(int inp);
void banghis(string input,string &args);
void myecho(char** envp,char* const* args);
void set_var(string input);
void add_histo(string input);
void myexport(char ** cstr);
