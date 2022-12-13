#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

void handler_sigtstp(int signum);
void handler_sigint(int signum);
void handler_sigcont(int signum);

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
	signal(SIGCONT, handler_sigint);
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
	signal(SIGTSTP, handler_sigcont);
}
