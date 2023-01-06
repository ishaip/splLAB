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
char* convertType(int);
char* flagToReadWrite(int);
int flagToPermissions(int flg);



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
    Elf32_Ehdr *header = (Elf32_Ehdr *) map_start;
    for (size_t i = 0; i < header->e_phnum; i++)
    {
    		Elf32_Phdr* entry = map_start+header->e_phoff+(i* header->e_phentsize);
            func(entry, arg);
    }
}

void load_phdr(Elf32_Phdr *phdr, int fd)
{

    printf("%s\t\t%#08x\t%#08x\t%#10.08x\t%#07x\t%#07x\t%s\t%#-6.01x\n",
        convertType(phdr->p_type),phdr->p_offset,phdr->p_vaddr,phdr->p_paddr,phdr->p_filesz,phdr->p_memsz,flagToReadWrite(phdr->p_flags),phdr->p_align);
    if(phdr->p_type == PT_LOAD)
    {  
        void *vadd = (void *)(phdr -> p_vaddr & 0xfffff000);
        int offset = phdr -> p_offset & 0xfffff000;
        int padding = phdr -> p_vaddr & 0xfff;
        int convertedFlag = flagToPermissions(phdr -> p_flags);
        void* temp;
        if ((temp = mmap(vadd, phdr -> p_memsz+padding, convertedFlag, MAP_FIXED | MAP_PRIVATE, fd, offset)) == MAP_FAILED ) {
            perror("mmap failed1");
            exit(-4);
        }
    }
}

char* convertType(int type){
    switch (type){
        case PT_NULL: return "NULL";
        case PT_LOAD: return "LOAD";
        case PT_DYNAMIC: return "DYNAMIC";
        case PT_INTERP: return "INTERP";
        case PT_NOTE: return "NOTE";
        case PT_SHLIB: return "SHLIB";
        case PT_PHDR: return "PHDR";
        case PT_TLS: return "TLS";
        case PT_NUM: return "NUM";
        case PT_GNU_EH_FRAME: return "GNU_EH_FRAME";
        case PT_GNU_STACK: return "GNU_STACK";
        case PT_GNU_RELRO: return "GNU_RELRO";
        case PT_SUNWBSS: return "SUNWBSS";
        case PT_SUNWSTACK: return "SUNWSTACK";
        case PT_HIOS: return "HIOS";
        case PT_LOPROC: return "LOPROC";
        case PT_HIPROC: return "HIPROC"; 
        default:return "Unknown";
    }
}

char* flagToReadWrite(int flg){
    switch (flg){
        case 0x000: return "";
        case 0x001: return "E";
        case 0x002: return "W";
        case 0x003: return "WE";
        case 0x004: return "R";
        case 0x005: return "RE";
        case 0x006: return "RW";
        case 0x007: return "RWE";
        default:return "Unknown";
    }
}

int flagToPermissions(int flg){
    switch (flg){
        case 0x000: return 0;
        case 0x001: return PROT_EXEC;
        case 0x002: return PROT_WRITE;
        case 0x003: return PROT_EXEC | PROT_EXEC;
        case 0x004: return PROT_READ;
        case 0x005: return PROT_READ | PROT_EXEC;
        case 0x006: return PROT_READ | PROT_WRITE;
        case 0x007: return PROT_READ | PROT_WRITE | PROT_EXEC;
        default:return -1;
    }
}

int main(int argc, char **argv){
    if(LoadFile(argv[1])==-1)
    {
        exit(EXIT_FAILURE);
    }
    header = (Elf32_Ehdr *) map_start;
    if(strncmp((char*)header->e_ident,(char*)ELFMAG, 4)==0) //chech if ELF file
    {    
        foreach_phdr(map_start, load_phdr, currFD);
        startup(argc-1, argv+1, (void *)(header->e_entry));
    }
     else
     {
        printf("This is not ELF file\n");
        munmap(map_start, fd_stat.st_size); 
        close(currFD); 
        currFD=-1;
     }
    return 0;
}