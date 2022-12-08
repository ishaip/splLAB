#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include "LineParser.h"
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

#define USER_LINE_BUFFER_SIZE 2048

void execute(cmdLine *pCmdLine);

int exit_flag = 0;

int main(int argc, char **argv){
	char user_line_buff[USER_LINE_BUFFER_SIZE], directory_path_buff[PATH_MAX];
	cmdLine * cmd_lines, * cmd_line;

	printf("Runing shell now\n");
    printf("the cahr & will simbolize that we are in the shell\n");

	while(1) {
		/* print cur dir and get user line input */
		getcwd(directory_path_buff,PATH_MAX);
		printf("\n%s:& ",directory_path_buff); /* cwd print */
		fgets(user_line_buff,USER_LINE_BUFFER_SIZE,stdin);
		/* parse and eceute */
		cmd_lines = parseCmdLines(user_line_buff);
		cmd_line = cmd_lines;
		while (cmd_line != NULL)
		{
			execute(cmd_line);
			if(exit_flag){
				break;
			}
			cmd_line = cmd_line->next;
		}
		freeCmdLines(cmd_lines);
		if(exit_flag){
				return 0;
		}
		sleep(2);
	}
	return 0;
}

void execute(cmdLine *pCmdLine){
	int ret_val = 0;
	pid_t pid = -1;
	if(pCmdLine != NULL){
		if(strcmp(pCmdLine->arguments[0], "quit") == 0){
			exit_flag = 1;
			return;
		}

		if(strcmp(pCmdLine->arguments[0], "cd") == 0){
			/*char * const *args = &(pCmdLine->arguments[1]);*/
			ret_val = chdir(pCmdLine->arguments[1]);
			if(ret_val == -1){
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
			if(ret_val == -1){
				char * path = pCmdLine->arguments[0];
				perror("failed in"+*path);
				exit(1);
			}
			exit(ret_val);
		}else{
			/*if parent -> wait for child execution to end*/
			pid = waitpid(pid, &ret_val,0);
			if(ret_val == -1){
				char * path = pCmdLine->arguments[0];
				perror("failed in"+*path);
				exit(1);
			}
		}
	}
}