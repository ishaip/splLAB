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
int debug_mode = 0;

int main(int argc, char **argv)
{
    cmdLine *command_lines, *curr_cmd;
	printf("Starting the program\n");
	int i = 0;
	for(i = 1; i < argc; i++)
	{
		if(strncmp(argv[i], "-d", 2) == 0){
			debug_mode = 1;
		}
	}

    char input_buff[INPUT_BUFF_SIZE]; //check the size 
    char directory_buff[PATH_MAX];
	while (1)
	{
        getcwd(directory_buff, PATH_MAX);
		printf("\n%s:$ ",directory_buff); 
        fgets(input_buff, INPUT_BUFF_SIZE, stdin);
        command_lines = parseCmdLines(input_buff);
        curr_cmd = command_lines;
        while (curr_cmd != NULL)
		{
			execute(curr_cmd);
			if(isExit)
            {
				break;
			}
			curr_cmd = curr_cmd->next;
		}
        
        freeCmdLines(command_lines);
		if(isExit)
        {
				return 0;
		}
	}

	return 0;
}


void execute(cmdLine *curr_cmd){
	int ret_val = 0;
	pid_t pid = -1;
	if(curr_cmd != NULL){
		if(strcmp(curr_cmd->arguments[0], "quit") == 0){
			if(debug_mode)
			{
				fprintf(stderr, "PID: %d, command: quit\n", getpid());
			}
			isExit = 1;
			return;
		}

		if(strcmp(curr_cmd->arguments[0], "cd") == 0){
			ret_val = chdir(curr_cmd->arguments[1]);
			if(debug_mode)
			{
				fprintf(stderr, "PID: %d, command: cd\n", getpid());
			}
			if(ret_val == -1)
			{
				char * path = curr_cmd->arguments[0];
				perror("failed in file "+*path);
				exit(1);
			}
			return;
		}

		// if cmd require forking
		pid = fork();
		// if forked so only child will execute 
		if(pid == 0)
		{
			//if child -> execute
			ret_val = execvp(curr_cmd->arguments[0],curr_cmd->arguments);
			if(ret_val == -1)
			{
				char * path = curr_cmd->arguments[0];
				perror("failed in file "+*path);
				exit(1);
			}
			exit(ret_val);
		}

		if(debug_mode)
		{
			fprintf(stderr, "PID: %d, command: %s\n", pid, curr_cmd->arguments[0]);
		}

		if(curr_cmd->blocking)
		{
			pid = waitpid(pid, &ret_val,0);
		}
		if(ret_val == -1)
		{
			char * path = curr_cmd->arguments[0];
			perror("failed in file "+*path);
			exit(1);
		}
	}
}