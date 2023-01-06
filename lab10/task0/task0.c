#include <stdio.h>
#include <unistd.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#define buffer_length 100


int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *,int), int arg)
{
    Elf32_Ehdr *header = (Elf32_Ehdr *) map_start;
    for (size_t i = 0; i < header->e_phnum; i++){
    		Elf32_Phdr* entry = map_start+header->e_phoff+(i* header->e_phentsize);
            func(entry, arg);
    	}
}

void func(Elf32_Phdr *entry,int s)
{
    printf("Program header is ok \n");
}

int main(int argc, char **argv){
    int fd;
    int currFD=-1;
    void* map_start;
    struct stat fd_stat; 
    if((fd = open("a.out", O_RDWR)) < 0) {
      perror("error in open");
      exit(-1);
   }
    if(fstat(fd, &fd_stat) != 0 ) {
      perror("stat failed");
      exit(-1);
   }
    if ((map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0)) == MAP_FAILED ) {
      perror("mmap failed");
      exit(-4);
   }
   printf("size is: %d", ((Elf32_Ehdr *) map_start)->e_phnum);
   if(currFD!=-1){close(currFD);}
    currFD=fd;
    int num = foreach_phdr(map_start, func, 5);
    return num;
}