#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include "LineParser.h"
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

/*t0b*/

void handler_sigtstp(int sig);
void handler_sigint(int sig);
void handler_sigcont(int sig);

int main(int argc, char **argv){
	printf("Starting the program\n");
	signal(SIGTSTP, handler_sigtstp);
	signal(SIGINT, handler_sigint);
	signal(SIGCONT, handler_sigcont);
	while(1) {
		sleep(2);
	}

	return 0;
}

void handler_sigtstp(int signum){
	char* signame = strsignal(signum);
	printf("received signal: %s\n", signame);
	signal(signum, SIG_DFL);
	raise(signum);
	signal(signum, handler_sigcont);
}

void handler_sigint(int signum){
	char* signame = strsignal(signum);
	printf("received signal: %s\n", signame);
	signal(signum, SIG_DFL);
	raise(signum);
}

void handler_sigcont(int signum){
	char* signame = strsignal(signum);
	printf("received signal: %s\n", signame);
	signal(signum, SIG_DFL);
	raise(signum);
	signal(signum, handler_sigtstp);
}