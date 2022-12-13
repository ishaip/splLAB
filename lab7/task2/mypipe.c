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
	pid_t cpid1,cpid2;
	int ret_val;

	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	cpid1 = fork();
	if (cpid1 == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (cpid1 == 0) {    /* Child reads from pipe */
		close(stdout);
		dup(pipefd[1]);  /* dup copy fd to lowest fd avaliable and its stdout since it has been closed */
		/* now stdout is the write end of the pipe */
		close(pipefd[1]);
		char* argv[3] = {"ls", "-l", 0};
		execvp(argv[0], argv);
        _exit(0);
	} 
	if(cpid1>0) {            /* Parent writes argv[1] to pipe */
		
		cpid2 = fork();
		if (cpid2 == -1) {
			perror("fork");
			exit(EXIT_FAILURE);
		}

		if (cpid2 == 0) {    /* Child reads from pipe */
			close(stdin);
			dup(pipefd[0]);
			close(pipefd[0]);
			char* argv[4] = {"tail", "-n","2", 0};
			execvp(argv[0], argv);
        	_exit(0);
		} 
		
		if(cpid2>0) { 
			close(pipefd[0]);
			waitpid(cpid1,&ret_val,0);		/* Wait for child */
			waitpid(cpid2,&ret_val,0);		/* Wait for child */
			exit(EXIT_SUCCESS);
		}
	}
}