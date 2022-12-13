#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include "LineParser.h"
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

#define INPUT_BUFF_SIZE 2048

void execute(cmdLine *pCmdLine);
int isExit = 0;

int main(int argc, char **argv)
{
    cmdLine *command_lines, *command_line;
	printf("Starting the program\n");
    char input_buff[INPUT_BUFF_SIZE]; //check the size 
    char directory_buff[PATH_MAX];
	while (1)
	{
        getcwd(directory_buff, PATH_MAX);
        fgets(input_buff, INPUT_BUFF_SIZE, stdin);
        command_lines = parseCmdLines(input_buff);
        command_line = command_lines;
        while (command_line != NULL)
		{
			execute(command_line);
			if(isExit)
            {
				break;
			}
			command_line = command_line->next;
		}
        
        freeCmdLines(command_lines);
		if(isExit)
        {
				return 0;
		}
	}

	return 0;
}


void execute(cmdLine *pCmdLine){
	int ret_val = 0;
	pid_t pid = -1;
	if(pCmdLine != NULL){
		if(strcmp(pCmdLine->arguments[0], "quit") == 0){
			isExit = 1;
			return;
		}

		if(strcmp(pCmdLine->arguments[0], "cd") == 0){
			/*char * const *args = &(pCmdLine->arguments[1]);*/
			ret_val = chdir(pCmdLine->arguments[1]);
			if(ret_val == -1)
			{
				char * path = pCmdLine->arguments[0];
				perror("failed in"+*path);
				exit(1);
			}
			return;
		}

		/* if cmd require forking */
		pid = fork();
		/* if forked so only child will execute */
		if(pid == 0){
			/*if child -> execute*/
			ret_val = execvp(pCmdLine->arguments[0],pCmdLine->arguments);
			if(ret_val == -1)
			{
				char * path = pCmdLine->arguments[0];
				perror("failed in"+*path);
				exit(1);
			}
			exit(ret_val);
		}
		// else
		// {
		// 	/*if parent -> wait for child execution to end*/
		// 	pid = waitpid(pid, &ret_val,0);
		// 	if(ret_val == -1)
		// 	{
		// 		char * path = pCmdLine->arguments[0];
		// 		perror("failed in"+*path);
		// 		exit(1);
		// 	}
		// }
	}
}