#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
  /*
   .
   .
   Any additional fields you deem necessary
  */
} state;

typedef struct fun_desc{
  char *name;
  void (*fun)(state*);
}FunDesc;

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
    if(s-> debug_mode)
        printf("Debug: file name set to %s \n", s->file_name);
}

void setSize (state* s){
    printf("enter a number any number: \n");
    int option = fgetc(stdin);
    fgetc(stdin);
    option = option -'0';
    if(option == 1 || option == 2 || option == 3){
        s->unit_size = option;
        if(s-> debug_mode)
            printf("Debug: set size to %d \n", option);
    }
    else
        printf("dude this isn't 1 or 2 or 4 ");
}


void loadMem (state* s){
    printf("not emplemented");
}
void displayMode (state* s){
    printf("not emplemented");
}
void memoryDesplay (state* s){
    printf("not emplemented");
}
void saveFiles (state* s){
    printf("not emplemented");
}
void memoryModify (state* s){
    printf("not emplemented");
}


void quit(state* s){
    free(s);
    printf("qutting\n");
    exit(0);
}



// char* map(char *array, int array_length, void (*f) (state*)){
//   char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
//   for(int i = 0; i < array_length; i++ ){
//     mapped_array[i] = f(array[i]);
//   }
//   free(array);
//   return mapped_array;
// }


int main(int argc, char **argv){
  //the state we are going to be using
  state *s = malloc(sizeof(state));
  s->debug_mode = 0;
  s->unit_size = 1;

  int num_of_func = 0;

  for( int i = 0; fun_desc[i].name != NULL; i++)
    num_of_func++;

  while(1){
    if(s->debug_mode)
        printf("unit_size, file_name, and mem_count\n");

    printf("Please choose a function: \n");
    //print all the avalable options
    for( int i = 0; fun_desc[i].name != NULL; i++)
      printf("%d)  %s\n", i, fun_desc[i].name);

    printf("Option : ");
    int option = fgetc(stdin);
    fgetc(stdin); //getting rid of the \n typed by pressing enter

    //normalize option to a decimal number
    option = option -'0';

    if( 0 > option || option > num_of_func - 1){
      //there is a need to make sure the user used a legal input
      printf("\nNot within bounds\n");
      return 0;
    }
    printf("\nWithin bounds\n");
    fun_desc[option].fun(s);
    //printf("DONE.\n");
  }

  return 0;
}