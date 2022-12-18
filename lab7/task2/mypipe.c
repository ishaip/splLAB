#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

#define STDIN 0
#define STDOUT 1
#define STDERROR 2



int main(int argc, char **argv){
	int pipefd[2];
	pid_t child_pid1,child_pid2;
	int ret_val;

	if (pipe(pipefd) == -1) {
		perror("pipe error");
		exit(EXIT_FAILURE);
	}

	child_pid1 = fork();
	if (child_pid1 == -1) {
		perror("fork error");
		exit(EXIT_FAILURE);
	}

	if (child_pid1 == 0) {    /* Child reads from pipe */
		close(stdout);
		dup(pipefd[1]);  
		close(pipefd[1]);
		char* my_argv[3] = {"ls", "-l", 0};
		execvp(my_argv[0], my_argv);
        _exit(0);
	} 
	if(child_pid1>0) {            /* Parent writes argv[1] to pipe */
		
		child_pid2 = fork();
		if (child_pid2 == -1) {
			perror("fork error");
			exit(EXIT_FAILURE);
		}

		if (child_pid2 == 0) {    /* Child reads from pipe */
			close(stdin);
			dup(pipefd[0]);
			close(pipefd[0]);
			char* my_argv[5] = {"tail", "-n","2","in.txt" , 0};		//do we need this to be an adress?
			execvp(my_argv[0], my_argv);
        	_exit(0);
		} 
		
		if(child_pid2>0) { 
			close(pipefd[0]);
			waitpid(child_pid1,&ret_val,0);	
			waitpid(child_pid2,&ret_val,0);	/* ret val isn't relevent we just want both childern to finish */
			exit(0);
		}
	}
	return 0;
}