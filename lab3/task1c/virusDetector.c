#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct virus {
unsigned short SigSize;
char virusName[16];
unsigned char* sig;
} virus; 



virus* readVirus(FILE*);
void printVirus(virus* virus, FILE* output);


int main(int argc, char **argv) {
    return 0;
}

virus* readVirus(FILE* input){
    virus *vir = malloc(sizeof(virus));
    // sigSize value
    unsigned char sigSize_buff[2];
    fread(sigSize_buff,2,2,input);
    vir -> SigSize = (((short)sigSize_buff[1]) << 8) | sigSize_buff[0];
    // virus name
    fread(vir -> virusName, sizeof( * (vir -> virusName)), 16, input);
    // sig value
    vir -> sig = (unsigned char*)(malloc(sizeof(char)*(vir -> SigSize)));
    fread(vir -> sig, sizeof(*(vir -> sig)),vir -> SigSize, input);
    return vir;
}

void printVirus(virus* virus, FILE* output){

}