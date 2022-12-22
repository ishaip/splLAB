#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "LineParser.h"
#include <linux/limits.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>

#define HISTLEN 4

char* history[HISTLEN];
int oldest = -1;

typedef struct commands{
	char* command; // the command line
	struct commands *next; // next process in the list
} commands;

void excuteLastCommand(int n);


void freeHistory(){
    for (int i=0 ;i <= oldest; i++){
        free(history[i]);
    }
}

void printHistory(){
    for (int i=0 ;i <= oldest && i < HISTLEN - 1; i++){
        printf("%d: %s\n",i ,history[i]);
    }
} 

int debug_mode =0;
void executePipe(cmdLine * pCmdLine){
    int pipefd[2];
    int status;
    int pid;

    if (pipe(pipefd) ==-1 ){
        perror("pipe");
        exit(1);
    }

    for ( int i = 0; i<2; i++){
        pid = fork();
        if (pid == 0){
            close(1-i); //  first iteration close stdout and duplicate ( pipefd[1] = stdout ) second close stdin and dup (pipfd[0] = stdin)
            dup(pipefd[1-i]);
          //  close(pipefd[1-i]); // why we need to close here
            if (pCmdLine->inputRedirect != NULL){
                int fd = open(pCmdLine->inputRedirect, O_RDONLY);
                if(fd == -1 ){
                    perror("invalid input redirection");
                    _exit(1);
                }
                close(0);   //replace stdin instead of fb
                dup(fd);
                close(fd);
            }
            if (pCmdLine->outputRedirect != NULL){
                int fd = open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT, 0644);
                if(fd == -1 ){
                    perror("invalid output redirection");
                    _exit(1);
                }
                close(1);
                dup(fd);
                close(fd);
            }
            if (execvp(pCmdLine->arguments[0],pCmdLine->arguments) == -1) {
                perror("An error has occured");
                _exit(1); // need to exit cause we dont want the process to continue (end with process of the command)
            }
            _exit(0);
        }
        pCmdLine = pCmdLine->next;
        close(pipefd[1-i]);     //we used dup to change pipe[1] and pipe[0]
    }
    close(pipefd[0]);
    waitpid(pid, &status, 0);
    // waitpid(pid2, &status, 0);
    
}

void execute(cmdLine *pCmdLine){
    //execvp replaces the current process with the new process, keeping some of the file handles open.
    //If we want to launch a new process, you must use fork() and call execvp() in the child process.
    
    // quit command need to execute by parent process
    if (!strcmp("quit",pCmdLine->arguments[0])){
           fprintf(stdout,"%s\n","quiting");
           freeCmdLines(pCmdLine);
           freeHistory();
           exit (0);
        }
    // cd command need to execute by parent process
    if (!strcmp("cd",pCmdLine->arguments[0]) ){
        if(chdir(pCmdLine->arguments[1])==-1){
           fprintf(stderr,"cd lacks args or invalid args \n");     
        }
        freeCmdLines(pCmdLine);
        return;
    }
    // history command need to be excuted
    if (!strcmp("history",pCmdLine->arguments[0]) ){
        printHistory();
        freeCmdLines(pCmdLine);
        return;
    }
    if (!strcmp("!!",pCmdLine->arguments[0])){
        if(!oldest){
           fprintf(stderr,"first command, cannot excute previous %d\n",errno);     
        }
        else{
            excuteLastCommand(1);
            freeCmdLines(pCmdLine);
            return;
        }
    }
    if (!strncmp("!",pCmdLine->arguments[0],1)){
        if(!oldest){
            fprintf(stderr,"first command, cannot excute previous %d\n",errno);     
        }
        else{
            char temp[256];
            //get all the string apprat from the first letter
            strncpy(temp, pCmdLine->arguments[0] + 1, strlen(pCmdLine->arguments[0]) - 1);
            int n = atoi(temp);
            if(n >= oldest || n < 1 || n > HISTLEN)
                fprintf(stderr,"didnt invoke that amount of commands %d\n",errno);
            else{
                excuteLastCommand(n);
                freeCmdLines(pCmdLine);
                return;
            }
        }
    }
    if (pCmdLine->next != NULL){
        executePipe(pCmdLine);
        freeCmdLines(pCmdLine);
        return;
    }
    int status;
    int pid = fork();

    if (pid < 0) { // The call to fork failed for some reason. You must take evasive action. A system dependent error message will be waiting in msg.
        _exit(1);
    }
    else if (pid == 0 ){ // You are in the child process. You can call exec to start another process. If that fails, you should probably call exit.
            if (debug_mode){
                fprintf(stderr, "child process id :%d\n",pid);
            }
            if (pCmdLine->inputRedirect != NULL){
                int fd = open(pCmdLine->inputRedirect, O_RDONLY);
                if(fd == -1 ){
                    perror("invalid input");
                    _exit(1);
                }
                close(0);
                dup(fd);
                close(fd);
            }
            if (pCmdLine->outputRedirect != NULL){
                int fd = open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT, 0644);
                if(fd == -1 ){
                    perror("invalid output");
                    _exit(1);
                }
                close(1);
                dup(fd);
                close(fd);
            }
            if (execvp(pCmdLine->arguments[0],pCmdLine->arguments) == -1) {
                    perror("execvp error");
                    _exit(1); // need to exit cause we dont want the process to continue (end with process of the command)
            }
        }
    else if (pid > 0) { // You are in the parent process. The value returned from fork is the process id of the child process. You should probably arrange to wait for any child processes to exit.
            if (debug_mode){
             fprintf(stderr, "parnet process  id %d is waiting\n",pid);
            }
            if (pCmdLine->blocking){ // when command finished with & (blocking set to 0) the process isnt blocking the shell process
            waitpid(pid,&status,0); // waitpid() functions shall obtain status information of child when available , return pid of the child and stored the status at the adress given
            }                        // 0 <==> (exit(0) or main end or process terminated)
            freeCmdLines(pCmdLine);
    }
}

void excuteLastCommand(int n){
    cmdLine* cmdLine;
    free(history[0]);
    history[0] = malloc(sizeof(history[n]));
    strcpy(history[0], history[n]);    
    fprintf(stderr,"%s",history[0]);
    if ((cmdLine = parseCmdLines(history[0]))!= NULL ){
        execute(cmdLine);
    }
    else{
        fprintf(stdout,"%s\n","command issues");
    }
}


int main(int argc, char** argv) {

    int i = 0;
    char buf[PATH_MAX]="";
    char command[2048]="";
    cmdLine* cmdLine;
    for (i = 0; i<argc;i++){
        if (!strncmp("-d",argv[i],2)){
            debug_mode=1;
        }
    }
        
    while(1){
        getcwd(buf,PATH_MAX); // get current working directory
        printf("%s>",buf);
        char* ret_val = fgets(command,2048,stdin);

        free(history[HISTLEN - 1]);
        for (int i = HISTLEN - 2; i >= 0; i--){
            history[i + 1] = history[i];
        }
        if(oldest < HISTLEN - 1)
            oldest = oldest + 1;        //make sure oldest is a pointer to the last element
        history[0] = malloc(sizeof(ret_val));
        strcpy(history[0], ret_val);

        // NULL on error or when end of file occurs while no characters have been read.
        
        if (debug_mode){
            fprintf(stderr,"the command is: %s\n",command);
        }
        
        if (ret_val!= NULL&& (cmdLine = parseCmdLines(command))!= NULL ){
        execute(cmdLine);
        }
        else{
            fprintf(stdout,"%s\n","cannot parse command");
        }
    }
    return 0;
}