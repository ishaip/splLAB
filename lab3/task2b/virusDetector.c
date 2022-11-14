#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct virus {
unsigned short SigSize;
char virusName[16];
unsigned char* sig;
} virus; 

typedef struct link link;

struct link {
link *nextVirus;
virus *vir;
};

void kill_virus(char *fileName, int signitureOffset, int signitureSize); 
void detect_virus(char *buffer, unsigned int size, link *virus_list);
link *print_signatures(link* virus_list);
link *alloc_scan(link* virus_list);
link* load_signatures(link * virus_list);
virus *readVirus(FILE *);
void print_virus(virus* virus, FILE* output);
void Print_hex(unsigned char * buffer, int length, FILE *output);
void list_print(link *virus_list, FILE *output); 
link* list_append(link* virus_list, virus* data);
void list_free(link *virus_list); 
link* quit(link* virus_list);
link* fix_file(link* virus_list);

typedef struct fun_desc{
  char *name;
  link* (*fun)(link*);
}FunDesc;


FunDesc fun_desc[]= {
  {"Load signatures",load_signatures},
  {"Print signatures",print_signatures},
  {"Detect viruses",alloc_scan},
  {"Fix file",fix_file},
  {"Quit", quit},
  //{NULL,NULL}
};


//very similar to what was used in lab2
int main(int argc, char **argv){
  //the string we are going to be using
  link *carray_vir = NULL;
  //we arn't keeping the array size explicite
  int num_of_func = 0;
  char buffer[10];
  for( int i = 0; fun_desc[i].name != NULL; i++)
    num_of_func++;

  while(1){
    printf("\nPlease choose a function: \n");
    //print all the avalable options
    for( int i = 0; fun_desc[i].name != NULL; i++)
      printf("%d)  %s\n", i + 1, fun_desc[i].name);

    printf("Option : ");
    fflush(stdin);
    scanf("%s",buffer);

    //normalize option to a decimal number
    int option = atoi(buffer);

    if( 1 > option || option > num_of_func ){
      //there is a need to make sure the user used a legal input
      printf("\nNot within bounds\n");
      return -1;
    }
    printf("\nWithin bounds\n");
    carray_vir = fun_desc[option - 1].fun(carray_vir);
    printf("DONE.\n");
  }
}

//task 1 a:
virus* readVirus(FILE* input){
    virus *vir = malloc(sizeof(virus));
    // sigSize value
    fread(&vir -> SigSize,sizeof(unsigned short),1,input);
    // virus name
    fread(vir -> virusName, sizeof( * (vir -> virusName)), 16, input);
    // sig value
    vir -> sig = (unsigned char*)(malloc(sizeof(char)*(vir -> SigSize)));
    fread(vir -> sig, sizeof(unsigned char),vir -> SigSize, input);
    return vir;
}

void print_virus(virus* virus, FILE* output){
    fprintf(output, "Virus name: %s\n", virus->virusName);
    fprintf(output, "Virus size: %d\n", virus->SigSize);
    fprintf(output, "signature:\n");
    Print_hex(virus->sig,virus->SigSize,output);
    fprintf(output, "\n\n");
}

//an edited version of the PrintHex from assinment1 to spport outputs file
void Print_hex(unsigned char *buffer, int length, FILE *output){
    for (int i=0; i < length; i++){
        fprintf(output, "%02X ",buffer[i]);
    }
}

//task 1 b:
void list_print(link *virus_list, FILE *output){
    while(virus_list != NULL){
        print_virus(virus_list -> vir,output);
        fprintf(output, "\n");
        virus_list = virus_list -> nextVirus;
    }
}

//adding at the start of the list
link* list_append(link* virus_list, virus *data){
    if(virus_list == NULL)
        virus_list -> vir = data;
    else{
        virus_list -> nextVirus = virus_list;
        virus_list -> vir = data;
    }
    return virus_list;
}

void list_free(link *virus_list){
    while(virus_list != NULL){
        free(virus_list->vir->sig);
        free(virus_list->vir);
        free(virus_list);
        virus_list = virus_list -> nextVirus;
    }
}


link* load_signatures(link * virus_list){
    FILE* signature_fp;
    char input[30];
    list_free(virus_list);
    virus_list = NULL;
    printf("Please enter signature file name:\n");
    //flush stdin before reading new data
    fflush(stdin);
    scanf("%s", input);
    signature_fp = fopen(input , "rb");
    if(signature_fp == NULL){
        printf("Wrong name\n");
        exit(-1); 
    }

    // same as in task 0
    fseek(signature_fp,0,SEEK_END);
    long file_length_in_bytes = ftell(signature_fp);
    fseek(signature_fp,0,SEEK_SET);
    char *ignore = malloc(sizeof(unsigned long));
    fread(ignore, sizeof(unsigned long),1,signature_fp);

    while( ftell(signature_fp) < file_length_in_bytes){
        virus *vir = malloc(sizeof(virus));
        vir = readVirus(signature_fp);
        virus_list = list_append(virus_list,vir);
    }
    fclose(signature_fp);
    return virus_list;
}

link *print_signatures(link* virus_list){
    if(virus_list !=NULL)
        list_print(virus_list, stdout);
    return virus_list;
}

//same as lab2
link* quit(link* virus_list){
    list_free(virus_list);
    printf("\n");
    exit(0);
  return virus_list;
}

//1c:
link *alloc_scan(link* virus_list){
    // similar to task 0
    char str[30];
    char * file_buffer = (char *)(malloc(10000*sizeof(char)));
    unsigned int size = 10000;
    printf("Please enter suspect file name:\n");
    //flush stdin before reading new data
    fflush(stdin);
    scanf("%s", str);
    FILE* suspect_fp = fopen(str , "rb");
    if(suspect_fp == NULL)
        exit(-1);

    fseek(suspect_fp,0,SEEK_END);
    long file_length_in_bytes = ftell(suspect_fp);
    fseek(suspect_fp,0,SEEK_SET);  

    fread(file_buffer,sizeof(*file_buffer),10000,suspect_fp);
    if(file_length_in_bytes<10000)
        size = (unsigned int)(10000 - file_length_in_bytes);
    
    detect_virus(file_buffer,size,virus_list);
    fclose(suspect_fp);
    free(file_buffer);
    return virus_list;
}



void detect_virus(char *buffer, unsigned int size, link *virus_list){
    while(virus_list!=NULL){
        unsigned int cur_sigSize = (unsigned int)(virus_list->vir->SigSize);
        unsigned int last_index = size - cur_sigSize;
        for(unsigned int byte_location=0; byte_location<=last_index; byte_location++){
            if(memcmp(buffer+byte_location, virus_list->vir->sig, cur_sigSize)==0)
                fprintf(stdout,"The starting byte locatio: %d\nThe virus name: %s\nThe size of the virus signature: %d\n",byte_location,virus_list->vir->virusName,cur_sigSize);
        virus_list = virus_list->nextVirus;
        }
    }
}

link* fix_file(link* virus_list){
    char fileName[30], byte_location[30];
    int signitureOffset, signitureSize;
    printf("File name: \n");
    fflush(stdin);
    scanf("%s", fileName);

    printf("Byte location:\n");
    fflush(stdin);
    scanf("%s", byte_location);
    signitureOffset = atoi(byte_location);
    //padd byte_loacation with 0's
    memset(byte_location,0,30);

    printf("Signature size:\n");
    fflush(stdin);
    scanf("%s", byte_location);
    signitureSize = atoi(byte_location);

    kill_virus(fileName,signitureOffset,signitureSize);

    return virus_list;
}

void kill_virus(char *fileName, int signitureOffset, int signitureSize) {
    FILE* suspected_file;
    char * NOP_replacment_segmant = (char *)(malloc(signitureSize*sizeof(char)));
    memset(NOP_replacment_segmant,0x90,signitureSize*sizeof(char)); //0x90 is NOP
    suspected_file = fopen(fileName, "r+");
    if(suspected_file == NULL){
        printf("Wrong file name");
        exit(-1);
    }
    fseek(suspected_file,signitureOffset,SEEK_SET);
    fwrite(NOP_replacment_segmant,signitureSize,1,suspected_file);
    free(NOP_replacment_segmant);
    fclose(suspected_file);
}