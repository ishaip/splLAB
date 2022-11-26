#include "util.h"

#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_ISEEK 19
#define SYS_GETDENTS 141
#define SYS_GETCWD 0xb7

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define O_RD 0
#define O_WR 1
#define O_RDRW 2
#define O_CREAT 64
#define O_DIRECTORY 4
#define O_APPEND 0x400



#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define EOF 4

#define BUFFER_SIZE 8192

typedef struct linux_dirent {
    unsigned long  d_ino;     /* Inode number */
    unsigned long  d_off;     /* Offset to next linux_dirent */
    unsigned short d_reclen;  /* Length of this linux_dirent */
    char          d_name[1];  /* Filename (null-terminated) */
                        /* length is actually (d_reclen - 2 -
                           offsetof(struct linuxma_dirent, d_name) */
} linux_dirent;

typedef struct link link;
struct link {
    link *next_dirent;
    linux_dirent *dir;
};

extern int system_call(int op_code, ...); 

int sys_write(int fd, const char *output_buff, int number_of_bytes_to_write);
void sys_error_exit(const char * err_msg);

/* given file descributor - return the size of the file in bytes. */
extern void infection(int fd);
extern void infector(char * str);

int main (int argc , char* argv[], char* envp[]){

  int buff_size = 20000;
  char buffer[buff_size];
  int  curr_pos, curr_dir, dir_size, a_flag = -1, i = 1;
  struct linux_dirent *dirp;
      
  for(i = 1; i < argc; i++){   
    if(strncmp("-a", argv[i], 2) == 0) {a_flag = i;}  /*making sure there is a -a*/
  }

  /*opening current dir*/
  curr_dir = system_call(SYS_OPEN,".", O_RD, 0777); 
  /*getting num of bytes readed in curr dir by sys_getdents*/
  dir_size = system_call(SYS_GETDENTS,curr_dir, buffer, buff_size); 

  if(dir_size < 0 || curr_dir < 0)
    sys_error_exit("Directory Opening Fail"); 

  for(curr_pos = 0; curr_pos < dir_size;){
    dirp = (struct linux_dirent *)(buffer + curr_pos);

    sys_write(STDOUT, dirp->d_name, strlen(dirp->d_name));  /*write the file name*/
    sys_write(STDOUT, "\n", 1);

    if(a_flag != 0){
        if(strncmp(dirp->d_name, argv[a_flag] + 2, strlen(argv[a_flag] + 2)) == 0){
            infector(dirp->d_name);
            infection(0);
        }
    }
    curr_pos += dirp->d_reclen;
  }
  return 0;  
}


/*sys write gets the number of bytes its spposed to write and an output*/
int sys_write(int fd, const char *output_buff, int number_of_bytes){
    int ret=-1;
    ret = system_call(SYS_WRITE,fd,output_buff,number_of_bytes);
    if (ret < 0 || (ret==0 && number_of_bytes>0)){
      sys_error_exit("Fail to write to file");
    }
    return ret;
}


void sys_error_exit(const char * err_msg){
  sys_write(STDOUT,err_msg,strlen(err_msg));
  system_call(SYS_EXIT,0x55);
}