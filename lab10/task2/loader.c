#include <stdio.h>
#include <unistd.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

extern int startup(int argc, char **argv, void (*start)());
#define buffer_length 100

/*Global variables*/
int currFD=-1;
void* map_start; /* will point to the start of the memory mapped file */
struct stat fd_stat; /* this is needed to  the size of the file */
Elf32_Ehdr *header; /* this will point to the header structure */

/*function decleration*/
int LoadFile(char*);
void examineFile(char*);
void printProgramHeaders();
char* mapping_type_to_string(int);
char* mapping_to_writeRead(int);
int mapping_to_protection(int flg);



int LoadFile(char* filename){     //reading material
    int fd;
    if((fd = open(filename, O_RDWR)) < 0) {
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
   if(currFD!=-1){close(currFD);}
    currFD=fd;
    return currFD;
}

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *,int), int arg)
{
    printf("%s\t\t%s\t\t%s\t%s\t%s\t%s\t%s\t%s\n",
    "type", "offset","virtAddr","physAddr","filesiz","memsiz","flg","align");
    Elf32_Ehdr *header = (Elf32_Ehdr *) map_start;
    for (size_t i = 0; i < header->e_phnum; i++)
    {
    		Elf32_Phdr* entry = map_start+header->e_phoff+(i* header->e_phentsize);
            func(entry, arg);
    }
    return 1;
}

void load_phdr(Elf32_Phdr *phdr, int fd)
{
    printf("%s\t\t%#08x\t%#08x\t%#10.08x\t%#07x\t%#07x\t%s\t%#-6.01x\n",
        mapping_type_to_string(phdr->p_type),phdr->p_offset,phdr->p_vaddr,phdr->p_paddr,phdr->p_filesz,phdr->p_memsz,mapping_to_writeRead(phdr->p_flags),phdr->p_align);
    if(phdr->p_type == PT_LOAD)
    {  //from reading material
        void *vadd = (void *)(phdr -> p_vaddr & 0xfffff000);
        int offset = phdr -> p_offset & 0xfffff000;
        int padding = phdr -> p_vaddr & 0xfff;
        int convertedFlag = mapping_to_protection(phdr -> p_flags);
        void* temp;
        if ((temp = mmap(vadd, phdr -> p_memsz+padding, convertedFlag, MAP_FIXED | MAP_PRIVATE, fd, offset)) == MAP_FAILED ) {
            perror("mmap failed1");
            exit(-4);
        }
    }
}

//mapping functions in order to ordnize the other functions
char* mapping_type_to_string(int flag){
    if (flag == PT_NULL) return "NULL";
    if (flag == PT_LOAD) return "LOAD";
    if (flag == PT_DYNAMIC) return "DYNAMIC";
    if (flag == PT_INTERP) return "INTERP";
    if (flag == PT_NOTE) return "NOTE";
    if (flag == PT_SHLIB) return "SHLIB";
    if (flag == PT_PHDR) return "PHDR";
    if (flag == PT_TLS) return "TLS";
    if (flag == PT_NUM) return "NUM";
    if (flag == PT_GNU_EH_FRAME) return "GNU_EH_FRAME";
    if (flag == PT_GNU_STACK) return "GNU_STACK";
    if (flag == PT_GNU_RELRO) return "GNU_RELRO";
    if (flag == PT_SUNWBSS) return "SUNWBSS";
    if (flag == PT_SUNWSTACK) return "SUNWSTACK";
    if (flag == PT_HIOS) return "HIOS";
    if (flag == PT_LOPROC) return "LOPROC";
    if (flag == PT_HIPROC) return "HIPROC"; 
    return "ERROR";

}

char* mapping_to_writeRead(int flag){
    if(flag == 0) return "";
    if(flag == 1) return "E";
    if(flag == 2) return "W";
    if(flag == 3) return "WE";
    if(flag == 4) return "R";
    if(flag == 5) return "RE";
    if(flag == 6) return "RW";
    if(flag == 7) return "RWE";
    return "ERROR";
}

int mapping_to_protection(int flag){
    if(flag == 0) return 0;
    if(flag == 1) return PROT_EXEC;
    if(flag == 2) return PROT_WRITE;
    if(flag == 3) return PROT_EXEC | PROT_EXEC;
    if(flag == 4) return PROT_READ;
    if(flag == 5) return PROT_READ | PROT_EXEC;
    if(flag == 6) return PROT_READ | PROT_WRITE;
    if(flag == 7) return PROT_READ | PROT_WRITE | PROT_EXEC;
    return -1;
}

int main(int argc, char **argv){
    if(LoadFile(argv[1])==-1)
    {
        exit(EXIT_FAILURE);
    }
    header = (Elf32_Ehdr *) map_start;
    if(strncmp((char*)header->e_ident,(char*)ELFMAG, 4)==0) //check if ELF file
    {    
        foreach_phdr(map_start, load_phdr, currFD);
        startup(argc-1, argv+1, (void *)(header->e_entry));
    }
    else
    {
        printf("This is not ELF file\n");
        //clear the map function allocated memory "un-maping"
        munmap(map_start, fd_stat.st_size); 
        close(currFD); 
        currFD=-1;
    }
    return 0;
}