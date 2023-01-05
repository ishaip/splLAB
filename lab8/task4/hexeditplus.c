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



int digit_cnt(char* c)
{
  int counter = 0;
  for(int i=0; i<strlen(c); i++)
  {
    if((c[i] - '0') >= 0 && (c[i]- '0') <= 9)
      counter++;
  }
  printf("The number of digits in the string is: %d/n", counter);
  return counter;
}

int main(int argc, char** argv){

  digit_cnt(argv[1]);
  return 0;
}