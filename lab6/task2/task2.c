#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "LineParser.h"
#include <linux/limits.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0
int debug = 0;
int indx = 0;

char* status_str(int status) {
    switch (status) {
        case -1 :
            return "Terminated";
        case 0 :
            return "Suspended";
        case 1 :
            return "Running";    
        default :
            return "Not valid status";
    }
}

typedef struct process{
    cmdLine* cmd; /* the parsed command line*/
    pid_t pid; /* the process id that is
        running the command*/
    int status; /* status of the process:
    RUNNING/SUSPENDED/TERMINATED */
    struct process *next; /* next process in chain */
    int indx;
} process;

process* process_list = NULL;

process* newProcess(cmdLine* cmd, pid_t pid) {
	process *proc=(process*)(malloc(sizeof(process)));
	proc -> status = RUNNING;
	proc -> next = NULL;
	proc -> cmd = cmd;
	proc -> pid = pid;
    proc -> indx = indx++;
	return proc;
}

void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
//Receive a process list (process_list), a command (cmd), and the process id (pid)
//of the process running the command.

  process *myProcess = newProcess(cmd,pid);
    if (*process_list == NULL) {
        *process_list = myProcess;
    }	
    else {
        process* list = *process_list;
        while (list -> next !=NULL){
            list = list -> next;
        }
        list -> next = myProcess;
    }
}

void printProcess(pid_t pid, cmdLine* cmd, int status, int indx){
    printf("%d %s %s %d\n",pid, cmd->arguments[0], status_str(status), indx);
}

void updateProcessStatus(process** process_list, pid_t pid, int status){
    process * list = * process_list;
    while(list!= NULL){
        if (list->pid == pid){
            list->status = status;
            return;
        }
        list = list->next;
    }
}

void updateProcessList(process** process_list){
    int status;
    process* list = *process_list;
    while(list!= NULL){
        int pid = waitpid(list->pid, &status, WNOHANG | WUNTRACED | WCONTINUED);// pid = 0 if no change in status, and the process's pid otherwise
        if (pid > 0){
            if (WIFCONTINUED(status)) {
                list -> status = RUNNING;
            }
            else if (WIFSTOPPED(status)) {
                list -> status = SUSPENDED;
            }
            else {
                list -> status = TERMINATED;
            }
        }
        else if (pid == -1) {
            list -> status = TERMINATED;
        }
        list = list -> next;
    }
}


void printProcesses(process** process_list){
    updateProcessList(process_list);
    process* list = *process_list;
    process* prev = NULL;
    process* curr = NULL;
    while(list != NULL){
        printProcess(list->pid, list->cmd, list->status, list->indx);
        if (list->status == TERMINATED){
            curr = list;
            list  = list->next;
            if (prev!=NULL){    
                prev->next = list;
            }
            else{
                *process_list = list;
            }
            freeCmdLines(curr->cmd);
            free(curr);
            continue;
        }
        prev = list;
        list  = list->next;

    }
}

void freeProcessList(process** process_list){
    process* list = *process_list;
    while(list != NULL)
    {
        freeCmdLines(list->cmd);
        // free(list->cmd);
        process* tmp = list;
        list = list->next;
        free(tmp);

    }
    
}

void nap(cmdLine* pCmdLine){
    kill(atoi(pCmdLine->arguments[2]), 20);
    updateProcessStatus(&process_list, atoi(pCmdLine->arguments[2]), SUSPENDED);
    sleep(atoi(pCmdLine->arguments[1]));
    kill(atoi(pCmdLine->arguments[2]), 18);
    updateProcessStatus(&process_list, atoi(pCmdLine->arguments[2]), RUNNING);
}

void stop(cmdLine* pCmdLine){
    kill(atoi(pCmdLine->arguments[1]), 2);
    updateProcessStatus(&process_list, atoi(pCmdLine->arguments[1]), TERMINATED);
}

void execute(cmdLine *pCmdLine){
    //execvp replaces the current process with the new process, keeping some of the file handles open.
    //If we want to launch a new process, you must use fork() and call execvp() in the child process.
    
    // quit command need to execute by parent process
    if (!strcmp("quit",pCmdLine->arguments[0])){
           fprintf(stdout,"%s\n","Program terminated ...");
           freeCmdLines(pCmdLine);
           freeProcessList(&process_list);
           exit (0);
        }
    // cd command need to execute by parent process
    else if (!strcmp("cd",pCmdLine->arguments[0]) ){
        if(chdir(pCmdLine->arguments[1])==-1){
           fprintf(stderr,"failed to execute cd command with error number : %d\n",errno);     
        }
        freeCmdLines(pCmdLine);
        return;
    }
    else if (!strcmp("showProcs",pCmdLine->arguments[0]) ){
        printProcesses(&process_list);
        freeCmdLines(pCmdLine);
        return;
    }

    int status;
    pid_t pid = fork();

    if (pid < 0) { // The call to fork failed for some reason. You must take evasive action. A system dependent error message will be waiting in msg.
        _exit(1);
    }
    else if (pid == 0 ){ // You are in the child process. You can call exec to start another process. If that fails, you should probably call exit.
            if (debug){
                fprintf(stderr, "The command was executed as part of the child process with id :%d\n",pid);
            }
            if (!strcmp("nap",pCmdLine->arguments[0]) ) {
                nap(pCmdLine);
                _exit(0);
            }
            else if (!strcmp("stop",pCmdLine->arguments[0]) ) {
                stop(pCmdLine);
                _exit(0);
            }
            if (execvp(pCmdLine->arguments[0],pCmdLine->arguments) == -1) {
                    perror("An error has occured");
                    _exit(1); // need to exit cause we dont want the process to continue (end with process of the command)
            }
        }
    else if (pid > 0) { // You are in the parent process. The value returned from fork is the process id of the child process. You should probably arrange to wait for any child processes to exit.
            addProcess(&process_list, pCmdLine, pid);
            if (debug){
             fprintf(stderr, "the parent process with id: %d is waiting for child process\n",pid);
            }
            
            if (strcmp(pCmdLine->arguments[0], "nap") && pCmdLine->blocking){ // when command finished with & (blocking set to 0) the process isnt blocking the shell process
                waitpid(pid,&status,0); // waitpid() functions shall obtain status information of child when available , return pid of the child and stored the status at the adress given
            }
                                    // 0 <==> (exit(0) or main end or process terminated)
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