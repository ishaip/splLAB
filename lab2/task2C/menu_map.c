#include <stdlib.h>
#include <stdio.h>
#include <string.h>


char censor(char c);
char* map(char *array, int array_length, char (*f) (char));
char my_get(char c);
char cprt(char c);
char encrypt(char c);
char decrypt(char c);
char xprt(char c);
char quit(char c);

typedef struct fun_desc{
  char *name;
  char (*fun)(char);
}FunDesc;

 FunDesc fun_desc[]= {
  {"Get string", my_get},
  {"print string", cprt},
  {"print hex", xprt},
  {"Censor",censor},
  {"Encrypt",encrypt},
  {"Decrypt", decrypt},
  {"Quit", quit},
  {NULL,NULL}
};


char censor(char c) {
  if(c == '!')
    return '.';
  else
    return c;
}

//2.a a map function that takes the operator and uses it on all element in the array
char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  for(int i = 0; i < array_length; i++ ){
    mapped_array[i] = f(array[i]);
  }
  free(array);
  return mapped_array;
}

//2.b 
char my_get(char c){
  return fgetc(stdin);
}

char cprt(char c){
  if(0x20 <=c && c <= 0x7E)
    printf("%c", c);
  else
    printf(".");

  printf("\n");
  return c;
}

char encrypt(char c){
  if(0x20 <=c && c <= 0x7E)
    return c + 3;

  else 
    return c;
}

char decrypt(char c){
  if(0x20 <=c && c <= 0x7E)
    return c - 3;

  else 
    return c;
}

char xprt(char c){
  if( c == '\n')
    printf(".");
  else
    printf("%x", c);
  printf("\n");
  return c;
}

char quit(char c){
  if(c == 'q')
  printf("\n");
    exit(0);
  return c;
}

//3.a
int main(int argc, char **argv){
  //the string we are going to be using
  char *carray = malloc(5 * sizeof(char));
  //we arn't keeping the array size explicite
  int num_of_func = -1;
  for( int i = 0; fun_desc[i].name != NULL; i++)
    num_of_func++;

  while(1){
    printf("\nPlease choose a function: \n");
    //print all the avalable options
    for( int i = 0; fun_desc[i].name != NULL; i++)
      printf("%d)  %s\n", i, fun_desc[i].name);

    printf("Option : ");
    int option = fgetc(stdin);
    fgetc(stdin); //getting rid of the \n typed by pressing enter

    //normalize option to a decimal number
    option = option -'0';

    if( 0 > option || option > num_of_func){
      //there is a need to make sure the user used a legal input
      printf("\nNot within bounds\n");
      return 0;
    }
    printf("\nWithin bounds\n");
    carray = map(carray, 5, fun_desc[option].fun);
    printf("DONE.\n");
  }

  return 0;
}


