/****************************************************
*Purpose: Piping Simulator                          *
*Author: Hussain Mehdi                              *
*Last Edited: 27/07/2021			      *
*****************************************************/

#define _GNU_SOURCE		
#include <sched.h>		//Included preprocessor directives
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "linkedList.h"	//Contains struct for Linked List
#define FIBER_STACK 8192

int pipefd[2];		//Pipe File Descriptor. Used to avoid magic numbers (e.g: 0,1)

/****************************************************
*Purpose: First execution from linked list          *
*						      *
*****************************************************/
int execFirst(void *arg)	
{
	node *i = arg;		//Linked list data 
	node *ptr;
	int pid;
	close(pipefd[0]);	//Closing read end of pipe
	char* order= strtok(i->data, " ");	//break up string stored in linked list
	char *bin_file = order;		//parameters for execlp
	char *var1 = order;
	order = strtok(NULL, " ");
	char *var2 = order; 
	char *var3 = NULL;
	
	if(order != NULL)
	{
	order = strtok(NULL, " ");
	var3 = order;
	}
		dup2(pipefd[1], STDOUT_FILENO);	//using dup2 to change standard output to write into pipe
	pid = execlp(bin_file, var1, var2, NULL);	// executing the process using execlp
        if (pid == -1)
		{
			fprintf(stderr, "Error executing %s\n", bin_file);
		}
	exit(0);	
}
/****************************************************
*Purpose: Second execution from linked list         *
*						      *
*****************************************************/
int execSecond(void *arg)
{
	close(pipefd[1]);			//Closing read end of pipe
	dup2(pipefd[0], STDIN_FILENO); 	//using dup2 to change standard output to read from pipe
	
	node *i = arg;
	node *ptr;
	int pid;
	
	char* order= strtok(i->data, " ");	
	char *bin_file = order;		//parameters for execlp
	char *var1 = order;
	order = strtok(NULL, " ");
	char *var2 = order; 
	char *var3 = NULL;
	
	if(order != NULL)
	{
	order = strtok(NULL, " ");
	var3 = order;
	}
	pid = execlp(bin_file, var1, var2, var3, NULL); 	// executing the process using execlp
        if (pid == -1)
		{
			fprintf(stderr, "Error executing %s\n", bin_file);
		}
	exit(0);	
}
/*********************************************************
*Purpose: File reader function, to populate linked list  *
*						      	   *
**********************************************************/
node *fileReader(FILE *file)	
{
    char *info = (char*)malloc(sizeof(int));		 //using malloc to allocate memory space for read-in data
    int fileRead = fscanf(file,"%[^\n] ", info);	 //using fscanf to parse input file
    node *i = malloc(sizeof(node));
     if(fileRead == EOF) 
    {
        i = NULL;
    }
  
    if (i != NULL) 
    {
        i->data = strdup(info);
        if (i->data != NULL) 
        {
            i->next = fileReader(file);	//populating linked list with read data
        }
        else 
        {   
            free(info);       	//Freeing allocated memory space from malloc
            free(i);
            i = NULL;
        }
    }
    return i;
}

int main(int argc, char** argv)
{
	int status;
    if(argc<2)				//Checking to see input from user on console
    {
        printf("\nPlease use the following format when compiling: ./pipesim inputfile [outputfile] <-optional\n");
    }
    else if(argc<=3)		// if 2 or 3 commands entered, it proceeds to read
    {
        char *FILE_NAME = argv[1];
	FILE *fp = fopen(FILE_NAME,"r");	//File pointer created to read file
	void * stack;
      	stack = malloc(FIBER_STACK);

	if(!fp)			//Catch exception/error thrown while reading
    	{
        	printf("Error reading from file, make sure to use '.txt' extension\n\n");   
    	}
	else
	{
		char *FILE_NAME = argv[1];
		FILE *fp = fopen(FILE_NAME,"r");	

			node *i = fileReader(fp);

		pipe(pipefd);		//Declaring pipe 
		
		pid_t cpid;		//clone process id
	cpid= clone(execFirst, (char *)stack + FIBER_STACK, CLONE_VM|CLONE_VFORK, i);    //using clone() to create first process
	
	if(cpid == -1)			//catch errors during clone
	{
		perror("clone");
	}
	i = i->next;
	
	cpid=clone(execSecond, (char *)stack + FIBER_STACK, CLONE_VM|CLONE_VFORK, i);    //using clone(0 to create second process
	
	if(argc == 3)		//printing output to file if entered by user at first
	{	
		close(pipefd[1]);		
		int redirect_fd = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY);
		dup2(redirect_fd, STDIN_FILENO);
		close(redirect_fd);
	}
	}

    	close(pipefd[0]);	//closing the pipe ends
	close(pipefd[1]);
	
	waitpid(-1,&status, 0);	//Children processes exit
	waitpid(-1,&status, 0);
	
	free(stack);			//Freeing stack used during cloning
    }

    return 1;
}
