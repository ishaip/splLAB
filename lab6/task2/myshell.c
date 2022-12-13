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

typedef struct process{
	cmdLine* cmd; /* the parsed command line*/
	pid_t pid; /* the process id that is
	running the command*/
	int status; /* status of the process:
	RUNNING/SUSPENDED/TERMINATED */
	struct process *next; /* next process in chain */
} process;

void execute(cmdLine *pCmdLine);
void updateProcessList(process **process_list);
void updateProcessStatus(process* process_list, int pid, int status);
void addProcess(process** process_list, cmdLine* cmd, pid_t pid);
void printProcessList(process** process_list);
void freeProcessList(process* process_list);
void myFreeCmdLines(cmdLine *pCmdLine); 

int exit_flag = 0;
int debug_flag = 0;
process* process_list = NULL;

int main(int argc, char **argv){
	char user_line_buff[USER_LINE_BUFFER_SIZE], directory_path_buff[PATH_MAX];
	int i;
	cmdLine * cmd_lines, * cmd_line;
	printf("Starting the program\n");
	for(i = 1; i < argc; i++){
		if(strncmp(argv[i], "-d", 2) == 0){
			debug_flag = 1;
		}
	}
	while(1) {
		/* print cur dir and get user line input */
		getcwd(directory_path_buff,PATH_MAX);
		printf("\n%s:$ ",directory_path_buff); /* cwd print */
		fgets(user_line_buff,USER_LINE_BUFFER_SIZE,stdin);
        if(strncmp(user_line_buff, "\n", 1) == 0){continue;}
		/* parse and eceute */
		cmd_lines = parseCmdLines(user_line_buff);
		cmd_line = cmd_lines;
        
        if(cmd_line != NULL){
            execute(cmd_line);
        }

		/*if((cmd_line != NULL) && (cmd_line->arguments[0] != NULL))freeCmdLines(cmd_lines);*/    
		if(exit_flag){
				return 0;
		}
	}
	return 0;
}

void execute(cmdLine *pCmdLine){
	int ret_val = 0, seconds_to_nap = 0;
	pid_t pid = -1, pid_to_care = -1;
	char* name = pCmdLine->arguments[0];
	if(pCmdLine != NULL){
		if(strcmp(pCmdLine->arguments[0], "quit") == 0){
			if(debug_flag){
				fprintf(stderr, "PID: %d, command: quit\n", getpid());
			}
			exit_flag = 1;
			return;
		}

		if(strcmp(pCmdLine->arguments[0], "cd") == 0){
			/*char * const *args = &(pCmdLine->arguments[1]);*/
			
			ret_val = chdir(pCmdLine->arguments[1]);
			if(debug_flag){
				fprintf(stderr, "PID: %d, command: cd\n", getpid());
			}
			if(ret_val == -1){
				char * path = pCmdLine->arguments[0];
				perror("failed in"+*path);
				_exit(1);
			}
			return;
		}

		if(strcmp(pCmdLine->arguments[0], "showprocs") == 0){
			/*char * const *args = &(pCmdLine->arguments[1]);*/
			printProcessList(&process_list);
			if(debug_flag){
				fprintf(stderr, "PID: %d, command: showprocs\n", getpid());
			}
			return;
		}
		
        if(strcmp(pCmdLine->arguments[0], "nap") == 0){
			/*char * const *args = &(pCmdLine->arguments[1]);*/
			
            if((pCmdLine->arguments[1] != NULL) && (pCmdLine->arguments[2] != NULL)){
                seconds_to_nap = atoi(pCmdLine->arguments[1]);
                pid_to_care = atoi(pCmdLine->arguments[2]);
            } else {
                perror("nap missing its arguments: nap <time in seconds> <process id>");
                return;
            }
            
            if(debug_flag){
				fprintf(stderr, "PID: %d, command: nap\n", getpid());
			}
			
            pid = fork();
            if(pid == 0){
                if(kill(pid_to_care,SIGTSTP)==-1)
                        perror("kill");
                sleep(seconds_to_nap);
                if(kill(pid_to_care,SIGCONT)==-1)
                        perror("kill");
                _exit(0);                
            } /* else{
                addProcess(process_list, pCmdLine, pid);
            }*/
			
			return;
		}
		
		if(strcmp(pCmdLine->arguments[0], "stop") == 0){
			/*char * const *args = &(pCmdLine->arguments[1]);*/
			
            if((pCmdLine->arguments[1] != NULL)){
                pid_to_care = atoi(pCmdLine->arguments[1]);
            } else {
                perror("stop missing its arguments: stop <process id>");
                return;
            }
            
            if(debug_flag){
				fprintf(stderr, "PID: %d, command: stop\n", getpid());
			}
			
            pid = fork();
            if(pid == 0){
                if(kill(pid_to_care,SIGINT)==-1)
                        perror("kill");
                _exit(0);                
            } /*else{
                addProcess(process_list, pCmdLine, pid);
            }	*/		
			return;
		}

		/* if cmd require forking */
		pid = fork();
		/* if forked so only child will execute */
		if(pid == 0){
			/*if child -> execute*/
			ret_val = execvp(pCmdLine->arguments[0], pCmdLine->arguments);
			if(ret_val == -1){
				char * path = pCmdLine->arguments[0];
				perror("failed in"+*path);
				_exit(1);
			}	
			exit(ret_val);
		}else{
		/*if parent -> wait for child execution to end*/
		if(debug_flag){
			fprintf(stderr, "PID: %d, command: %s\n", pid, name);
		}
		if(pCmdLine->blocking){
			pid = waitpid(pid, &ret_val,0);
		}
		if(ret_val == -1){
			char * path = pCmdLine->arguments[0];
			perror("failed in"+*path);
			_exit(1);
		}else{
            printf("adding process %s\n", pCmdLine->arguments[0]);
            addProcess(&process_list, pCmdLine, pid);
            printf("added: %d\t, %d\t, %s\t, %d\t\n", 0, process_list->pid, process_list->cmd->arguments[0], process_list->status);
        }
      }
	}
}

// cmd, pid, status, next
void updateProcessList(process** proc_list){
    //printf("updateProcessList1\n");
    if(proc_list == NULL) return;
    //printf("updateProcessList2\n");
    int ret_id = 0, status;
    pid_t curr_pid = -1;
    process *curr_proc = *proc_list;
    //printf("updateProcessList3\n");
    while(curr_proc != NULL){
        //printf("updateProcessList1 - while\n");
        curr_pid = curr_proc->pid;
        ret_id = waitpid(curr_pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
        //printf("updateProcessList2-while\n");
        if(ret_id != 0){ //child died - update its status
            if(WIFEXITED(status) || WIFSIGNALED(status) || (ret_id == -1)){ // child exited normally || child terminated over received signal || error occured
                updateProcessStatus(curr_proc, curr_pid, TERMINATED);
            } else if(WIFSTOPPED(status)){ // child stopped (paused)
                updateProcessStatus(curr_proc, curr_pid, SUSPENDED);
                    
            } else if(WIFCONTINUED(status)){ // child continued after suspention over signal
                updateProcessStatus(curr_proc, curr_pid, RUNNING);
            }
        }
        status = 0;
        curr_proc = curr_proc->next;
        //printf("updateProcessList3 - while\n");
    }
    //("updateProcessList4\n");   
}

void updateProcessStatus(process* proc, pid_t pid, int status){
    if((proc != NULL) && (proc->pid == pid)){
        proc->status = status;
    }
    return;
}


void addProcess(process** proc_list, cmdLine* cmd, pid_t pid){
	process* procToAdd = (malloc(sizeof(process)));
	procToAdd->cmd = cmd;
	procToAdd->pid = pid;
	procToAdd->status = RUNNING;
	procToAdd->next = (*proc_list);
    process_list = procToAdd;
}

void printProcessList(process** proc_list){
    if(proc_list == NULL) return;
    updateProcessList(proc_list);
    printf("#\t PID\t Command\t Status(-1=Terminated, 0=Suspended, 1=Running)\n");
	process *temp_proc = *proc_list;
    process *prev_proc = NULL, *delete_proc;
	int proc_index = 0;

	while(temp_proc != NULL){
		printf("%d\t, %d\t, %s\t, %d\t\n", proc_index, temp_proc->pid, temp_proc->cmd->arguments[0], temp_proc->status);
		if(temp_proc->status == TERMINATED){
            if(prev_proc != NULL){
                prev_proc->next = temp_proc->next;
            }else{
                process_list = temp_proc->next;
            }            
            delete_proc = temp_proc;
            temp_proc = temp_proc->next;
            delete_proc->next = NULL;
            freeProcessList(delete_proc);
        } else {
            prev_proc = temp_proc;
            temp_proc = temp_proc->next;
        }
		proc_index++;
        delete_proc = NULL;
	}	
}

void freeProcessList(process* proc_list){          
    while(proc_list != NULL){
        myFreeCmdLines(proc_list->cmd);
        proc_list = proc_list->next;
    }
    return;
}


void myFreeCmdLines(cmdLine *pCmdLine)
{
    int i;
    if (!pCmdLine) return;
    
    for (i = 0; i < pCmdLine->argCount; i++){
        if(pCmdLine->arguments[i]) free(pCmdLine->arguments[i]);
    }
    
    
    if (pCmdLine->next != NULL) myFreeCmdLines(pCmdLine->next);
}
