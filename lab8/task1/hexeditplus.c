#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>
#include <fcntl.h> 
#include <errno.h> 
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#define SEEK_START  0

typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
  char display_mode;
} state;

typedef struct fun_desc{
  char *name;
  void (*fun)(state*);
}FunDesc;

// void printDisplay(state* s, char c);
void debugMode (state* s);
void setName (state* s);
void setSize (state* s);
void loadMem (state* s);
void displayMode (state* s);
void memoryDesplay (state* s);
void saveFiles (state* s);
void memoryModify (state* s);
void quit(state* s);



FunDesc fun_desc[]= {
  {"Toggle Debug Mode", debugMode},
  {"Set File Name", setName},
  {"Set Unit Size", setSize},
  {"Load Into Memory",loadMem},
  {"Toggle Display Mode",displayMode},
  {"Memory Display", memoryDesplay},
  {"Save Into File",saveFiles},
  {"Memory Modify",memoryModify},
  {"Quit", quit},
  {NULL,NULL}
};

// void printDisplay(state* s, int i){
//   if(s->display_mode)
//     printf("\n%d", c);
//   else
//     printf("\n%X", c);
// }

void debugMode (state* s){
    if(s-> debug_mode)
        printf("debug flag now off\n");
    else
        printf("debug flag now on\n");
    s-> debug_mode = !s-> debug_mode;
}

void setName (state* s){
    printf("enter file name: \n");
    scanf("%s", s->file_name);
    int c;
    while((c = getchar()) != '\n' && c!=EOF);
    if(s-> debug_mode)
        printf("Debug: file name set to %s \n", s->file_name);
}

void setSize (state* s){
  printf("enter a number any number: \n");
  int option = fgetc(stdin);
  fgetc(stdin);
  option = option -'0';
  if(option == 1 || option == 2 || option == 4){
      s->unit_size = option;
      if(s-> debug_mode)
          printf("Debug: set size to %d \n", option);
  }
  else
      printf("dude this isn't 1 or 2 or 4 \n");
}


void loadMem (state* s){ //todo
  if(s->file_name[0] == 0){
    printf("file name has no value\n");
    return;
  }
  FILE* myfile = fopen(s->file_name, "r+");
  if(myfile == NULL){
    printf("Wrong file name\n");
    return;
  }
  printf("enter location, length \n");    //assuming location is decimal
  char buffer[10000];
  fgets(buffer,100000,stdin);

  int location;
  int length;
  sscanf(buffer,"%X %d", &location, &length);
  // int c;
  // while((c = getchar()) != '\n' && c!=EOF);

  fseek(myfile,location, SEEK_START);
  fread(s->mem_buf, s->unit_size, length, myfile);
  fclose(myfile);
  printf("Loaded %d units into memory", length);
}

void displayMode (state* s){
  if(s->display_mode)
      printf("display mode now off\n");
  else
      printf("display mode now on\n");
  s-> display_mode = !s-> display_mode;
}



void memoryDesplay (state* s){
  printf("enter address, u \n");    
  char buffer[10000];
  fgets(buffer,100000,stdin);

  int address;
  int u;
  sscanf(buffer,"%X %d", &address, &u);
  // int c;
  // while((c = getchar()) != '\n' && c!=EOF);

  if(s->file_name[0] == 0){
    printf("file name has no value\n");
    return;
  }
  int currentfd = -2;
  currentfd = open(s->file_name, O_RDWR);

  if(address == 0)
    for(int i = 0; i < u; i++)
    {
      for(int j = s->unit_size-1; j>=0 ; j--)
      {
        if(s->display_mode)
          printf("%d", s->mem_buf[j+(i*s->unit_size)]);
        else
          printf("%X", s->mem_buf[j+(i*s->unit_size)]);
      }
      printf("\n");
    }
  else
  {
    char *map_start;
    map_start = mmap((void*)(address), (u*s->unit_size),PROT_READ|PROT_WRITE, MAP_PRIVATE, currentfd, 0);
    for(int i = 0; i<(u*s->unit_size); i++)
    {
      printf("\n %s", map_start+(i*4));
    }
  }
}

void saveFiles (state* s){
  int source_add = 0;
  int target_loc = 0;
  int length =0;
  printf("please entry source-address, target, length: \n");
  char buffer[10000];
  fgets(buffer,100000,stdin);
  sscanf(buffer,"%X %X %d", &source_add, &target_loc, &length);

  FILE* myfile = fopen(s->file_name, "w+");
  if(myfile == NULL){
    printf("Wrong file name\n");
    return;
  }
  if(source_add == 0)
  {
    fseek(myfile,target_loc,SEEK_SET);
    fwrite(s->mem_buf,s->unit_size,length,myfile);
    fclose(myfile);
  }
  else
  {
    fseek(myfile,target_loc,SEEK_SET);
    fwrite(s->mem_buf,s->unit_size,length,myfile);
    fclose(myfile);
  }

  printf("Loaded %d units into memory", length);
}
void memoryModify (state* s){
  int location = 0;
  // int val = 0;
  char val[s->unit_size];
  printf("please entry location, val: \n");
  char buffer[10000];
  fgets(buffer,100000,stdin);
  sscanf(buffer,"%X %s", &location, val);
  if(s->debug_mode)
    printf("the location is: %X\n the value is: %s \n", location, val);

  for(int i=0; i<s->unit_size; i++)
  {
    s->mem_buf[location+i] = val[i]- '0';
  }
  // s->mem_buf[location] = val;
}


void quit(state* s){
    free(s);
    printf("qutting\n");
    exit(0);
}

int main(int argc, char **argv){
  //the state we are going to be using
  state *s = malloc(sizeof(state));
  s->debug_mode = 0;
  s->unit_size = 1;
  s->display_mode = 0;
  int num_of_func = 0;

  for( int i = 0; fun_desc[i].name != NULL; i++)
    num_of_func++;

  while(1){
    if(s->debug_mode)
    {
        printf("\nunit_size is: %d \n", s->unit_size);
        printf("file_name: %s \n", s->file_name);
        printf("mem_count is: %d \n", s->mem_count);
    }
    printf("\nPlease choose a function: \n");
    //print all the avalable options
    for( int i = 0; fun_desc[i].name != NULL; i++)
      printf("%d)  %s\n", i, fun_desc[i].name);

    printf("Option : ");
    int option = fgetc(stdin);
    fgetc(stdin); //getting rid of the \n typed by pressing enter
    //normalize option to a decimal number
    option = option -'0';

    //printf("Option : %X ",option);

    if( 0 > option || option > num_of_func - 1){
      //there is a need to make sure the user used a legal input
      printf("Not within bounds\n");
      return 0;
    }
    printf("Within bounds\n");
    fun_desc[option].fun(s);
    //printf("DONE.\n");
  }

  return 0;
}