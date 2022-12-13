#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include "LineParser.h"
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

#define INPUT_BUFF_SIZE 2048
#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

typedef struct process{
	cmdLine* cmd; // the command line
	pid_t pid; // process id 
	int status; // status can be: RUNNING/SUSPENDED/TERMINATED 
	struct process *next; // next process in the list
} process;

process* process_list = NULL;



void execute(cmdLine *curr_cmd);
void wake(cmdLine *curr_cmd);
void suspend(cmdLine *curr_cmd);
void my_kill(cmdLine *curr_cmd);
void freeProcessList(process* proc_list);
void freeEachCmdLine(cmdLine *curr_cmd);
void addProcess(process** process_list, cmdLine* cmd, pid_t pid);
void printProcessList(process** process_list);
void updateProcessStatus(process* process_list, int pid, int status);
void updateProcessList(process** proc_list);
int isExit = 0;
int debug_mode = 0;

int main(int argc, char **argv)
{
    cmdLine *command_lines, *curr_cmd;
	printf("Starting the program\n");
	int i = 0;
	for(i = 1; i < argc; i++)
	{
		if(strncmp(argv[i], "-d", 2) == 0){
			debug_mode = 1;
		}
	}

    char input_buff[INPUT_BUFF_SIZE]; //check the size 
    char directory_buff[PATH_MAX];
	while (1)
	{
        getcwd(directory_buff, PATH_MAX);
		printf("\n%s:$ ",directory_buff); 
        fgets(input_buff, INPUT_BUFF_SIZE, stdin);
        command_lines = parseCmdLines(input_buff);
        curr_cmd = command_lines;
        if (curr_cmd != NULL)
		{
			execute(curr_cmd);
		}
        if(isExit)
        {
				return 0;
		}
		// free(command_lines);
	}

	return 0;
}


void execute(cmdLine *curr_cmd)
{
	int ret_val = 0;
	pid_t pid = -1;
	if(curr_cmd != NULL){
		if(strcmp(curr_cmd->arguments[0], "quit") == 0)
		{
			if(debug_mode)
			{
				fprintf(stderr, "PID: %d, command: quit\n", getpid());
			}
			isExit = 1; //if command "quit" is entered, we need to end the infinite loop
			freeProcessList(process_list);

			return;
		}

		if(strcmp(curr_cmd->arguments[0], "cd") == 0)
		{
			ret_val = chdir(curr_cmd->arguments[1]); //changes the current working directory of the calling process
			if(debug_mode)
			{
				fprintf(stderr, "PID: %d, command: cd\n", getpid());
			}
			if(ret_val == -1)
			{
				char * path = curr_cmd->arguments[0];
				perror("failed in file "+*path);
				exit(1);
			}
			return;
		}

		if(strcmp(curr_cmd->arguments[0], "procs") == 0)
		{
			printProcessList(&process_list);  //prints current processes
			if(debug_mode)
			{
				fprintf(stderr, "PID: %d, command: procs\n", getpid());
			}
			return;
		}

		if(strcmp(curr_cmd->arguments[0], "wake") == 0)
		{
			wake(curr_cmd);	
			return;
		}
		

		if(strcmp(curr_cmd->arguments[0], "suspend") == 0){
			suspend(curr_cmd);					
			return;
		}

		if(strcmp(curr_cmd->arguments[0], "kill") == 0)
		{
			my_kill(curr_cmd);	
			return;
		}


		pid = fork();
		if(pid == 0) 
		{
			ret_val = execvp(curr_cmd->arguments[0],curr_cmd->arguments); //executes the program pointed to by filename(arg[0])
			if(ret_val == -1)
			{
				char * path = curr_cmd->arguments[0];
				perror("failed in file "+*path);
				exit(1);
			}
			exit(ret_val);
		}

		if(debug_mode)
		{
			fprintf(stderr, "PID: %d, command: %s\n", pid, curr_cmd->arguments[0]);
		}

		if(curr_cmd->blocking) //if blocking, so wait for the process to end. blocking if there is no &. 
		{
			pid = waitpid(pid, &ret_val,0);
		}
		if(ret_val == -1)
		{
			char * path = curr_cmd->arguments[0];
			perror("failed in file "+*path);
			exit(1);
		}
		else
		{
            addProcess(&process_list, curr_cmd, pid);
        }
	}
}

void wake(cmdLine *curr_cmd)
{
	int procPid = -1;
	int pid = -1;
	if(curr_cmd->arguments[1] != NULL)
		procPid = atoi(curr_cmd->arguments[1]);
	else
	{
		perror("<process id> is missing");
		return;
	}
	if(debug_mode)
		fprintf(stderr, "PID: %d, command: suspend\n", getpid());
	pid = fork();
	if(pid == 0)
	{
		if(kill(procPid,SIGCONT) == -1) //send continue signal to the process.
				perror("kill");
		_exit(0);                
	}
}

void suspend(cmdLine *curr_cmd)
{
	int procPid = -1;
	int pid = -1;
	if(curr_cmd->arguments[1] != NULL)
		procPid = atoi(curr_cmd->arguments[1]);
	else
	{
		perror("<process id> is missing");
		return;
	}
	if(debug_mode)
		fprintf(stderr, "PID: %d, command: suspend\n", getpid());
	pid = fork();
	if(pid == 0)
	{ 
		if(kill(procPid,SIGTSTP)==-1) //send stop signal to the process.
				perror("kill");
		_exit(0);                
	}
}

void my_kill(cmdLine *curr_cmd)
{
	int procPid = -1;
	int pid = -1;		
	if((curr_cmd->arguments[1] != NULL)){
		procPid = atoi(curr_cmd->arguments[1]);
	}
	else
	{
		perror("<process id> is missing");
		return;
	}
	
	if(debug_mode){
		fprintf(stderr, "PID: %d, command: stop\n", getpid());
	}
	
	pid = fork();
	if(pid == 0)
	{
		if(kill(procPid,SIGINT) == -1) //send terminate signal to the process.
				perror("kill");
		_exit(0);                
	}
}

void freeProcessList(process* proc_list)
{          
    while(proc_list != NULL)
	{
        freeEachCmdLine(proc_list->cmd); 
        proc_list = proc_list->next;
    }
    return;
}

void freeEachCmdLine(cmdLine *curr_cmd)
{
    if (curr_cmd != NULL)
	{
		int i;
		for (i = 0; i < curr_cmd->argCount; i++)
		{
			if(curr_cmd->arguments[i] != NULL) 
				free(curr_cmd->arguments[i]);
		}
		if (curr_cmd->next != NULL) 
			freeEachCmdLine(curr_cmd->next);
	} 
		return;

}

void addProcess(process** process_list, cmdLine* cmd, pid_t pid)
{
	process* newProc = (malloc(sizeof(process)));
	newProc->cmd = cmd;
	newProc->pid = pid;
	newProc->status = RUNNING; 
	newProc->next = NULL;
	if(*process_list != NULL)
	{
		(*process_list)->next = newProc;
	}
	else
	{
		*process_list = newProc;
	}
}

void printProcessList(process** proc_list){
    if(proc_list == NULL) return;
    updateProcessList(proc_list);
    printf("#\t PID\t Command\t Status (0 is Suspended, -1 is Terminated, 1 is Running)\n");
	process *curr_proc = *proc_list;
    process *previous = NULL, *delete_proc;
	int proc_index = 0;

	while(curr_proc != NULL){
		printf("%d\t, %d\t, %s\t, %d\t\n", proc_index, curr_proc->pid, curr_proc->cmd->arguments[0], curr_proc->status);
		if(curr_proc->status == TERMINATED)
		{
            if(previous != NULL){
                previous->next = curr_proc->next;
            }
			else
			{
                process_list = curr_proc->next;
            }            
            delete_proc = curr_proc;
            curr_proc = curr_proc->next;
            delete_proc->next = NULL;
            freeProcessList(delete_proc);
        }
		else
		{
            previous = curr_proc;
            curr_proc = curr_proc->next;
        }
		proc_index++;
        delete_proc = NULL;
	}	
}

void updateProcessStatus(process* process_list, int pid, int status)
{
    if((process_list != NULL) && (process_list->pid == pid))
	{
        process_list->status = status;
    }
    return;
}


void updateProcessList(process** proc_list)
{
    if(proc_list == NULL) return;
    int ret_id = 0, status;
    pid_t curr_pid = -1;
    process *curr_proc = *proc_list;
    while(curr_proc != NULL){
        curr_pid = curr_proc->pid;
        ret_id = waitpid(curr_pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
        if(ret_id != 0)
		{ //child died - update its status
            if(WIFEXITED(status) || WIFSIGNALED(status) || (ret_id == -1)){ // child exited normally || child terminated over received signal || error occured
                updateProcessStatus(curr_proc, curr_pid, TERMINATED);
            }
			else if(WIFSTOPPED(status)){ // child stopped (paused)
                updateProcessStatus(curr_proc, curr_pid, SUSPENDED);
                    
            }
			else if(WIFCONTINUED(status)){ // child continued after suspention over signal
                updateProcessStatus(curr_proc, curr_pid, RUNNING);
            }
        }
        status = 0;
        curr_proc = curr_proc->next;
    }
}