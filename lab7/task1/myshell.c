#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "LineParser.h"
#include <linux/limits.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>

int debug =0;

void execute(cmdLine *pCmdLine){
    //execvp replaces the current process with the new process, keeping some of the file handles open.
    //If we want to launch a new process, you must use fork() and call execvp() in the child process.
    
    // quit command need to execute by parent process
    if (!strcmp("quit",pCmdLine->arguments[0])){
           fprintf(stdout,"%s\n","Program terminated");
           freeCmdLines(pCmdLine);
           exit (0);
        }
    // cd command need to execute by parent process
    if (!strcmp("cd",pCmdLine->arguments[0]) ){
        if(chdir(pCmdLine->arguments[1])==-1){
           fprintf(stderr,"faild to execute cd command with error number : %d\n",errno);     
        }
        freeCmdLines(pCmdLine);
        return;
    }
    int status;
    int pid = fork();

    if (pid < 0) { // The call to fork failed for some reason. You must take evasive action. A system dependent error message will be waiting in msg.
        _exit(1);
    }
    else if (pid == 0 ){ // You are in the child process. You can call exec to start another process. If that fails, you should probably call exit.
            if (debug){
                fprintf(stderr, "The command was executed as part of the child process with id :%d\n",pid);
            }
            if (pCmdLine->inputRedirect != NULL){
                int fd = open(pCmdLine->inputRedirect, O_RDONLY);
                if(fd == -1 ){
                    perror("invalid input redirection");
                    _exit(1);
                }
                close(0);
                dup(fd);	// update file desriptor to be fb	
                close(fd);	// the old fb is still open
            }
            if (pCmdLine->outputRedirect != NULL){
                int fd = open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT, 0644);
                if(fd == -1 ){
                    perror("invalid output redirection");
                    _exit(1);
                }
                close(1);
                dup(fd);	// update file desriptor to be fb	
                close(fd);	// the old fb is still open
            }
            if (execvp(pCmdLine->arguments[0],pCmdLine->arguments) == -1) {
                    perror("An error has occured");
                    _exit(1); // need to exit cause we dont want the process to continue (end with process of the command)
            }
        }
    else if (pid > 0) { // You are in the parent process. The value returned from fork is the process id of the child process. You should probably arrange to wait for any child processes to exit.
            if (debug){
             fprintf(stderr, "the parnet process with id: %d is waiting for child process\n",pid);
            }
            if (pCmdLine->blocking){ // when command finished with & (blocking set to 0) the process isnt blocking the shell process
            waitpid(pid,&status,0); // waitpid() functions shall obtain status information of child when available , return pid of the child and stored the status at the adress given
            }                        // 0 <==> (exit(0) or main end or process terminated)
            freeCmdLines(pCmdLine);
    }
}


int main(int argc, char** argv) {
    int i = 0;
    char buf[PATH_MAX]="";
    char command[2048]="";
    cmdLine* cmdLine;
    for (i = 0; i<argc;i++){
        if (!strncmp("-d",argv[i],2)){
            debug=1;
        }
    }
        
    while(1){
        getcwd(buf,PATH_MAX); // get current working directory
        printf("%s>",buf);
     

        char* ret_val = fgets(command,2048,stdin);
        // NULL on error or when end of file occurs while no characters have been read.
        
        if (debug){
            fprintf(stderr,"the command is: %s\n",command);
        }
        
        if (ret_val!= NULL&& (cmdLine = parseCmdLines(command))!= NULL ){
        execute(cmdLine);
        }
        else{
                fprintf(stdout,"%s\n","command issues");
        }
    }
    return 0;
}