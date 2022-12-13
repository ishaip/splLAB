#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include "LineParser.h"
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

#define USER_LINE_BUFFER_SIZE 2048
#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0

int main(int argc, char **argv){
	int pipefd[2];		// file decriptor is of type int https://man7.org/linux/man-pages/man3/fileno.3.html
	pid_t child_pid;
	char buf;
	int ret_val;

	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	child_pid = fork();
	if (child_pid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (child_pid == 0) {    /* Child reads from pipe */
		//close(pipefd[0]); closing a File that isnt used, is this needed?
		write(pipefd[1],"hello",strlen("hello"));
		close(pipefd[1]);          /* Close unused write end */
		exit(EXIT_SUCCESS);
	} else {            /* Parent writes argv[1] to pipe */
		waitpid(child_pid,&ret_val,0);		/* Wait for child */
		close(pipefd[1]);          /* Close unused write end */
		while (read(pipefd[0], &buf, 1) != '\0')
			putchar(buf);
		printf("\n");
		close(pipefd[0]);          /* Reader will see EOF */               
		exit(EXIT_SUCCESS);
	}
}