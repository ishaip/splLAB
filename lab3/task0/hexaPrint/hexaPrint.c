#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char **argv) {
    FILE *input;

    if(argc == 1){
        printf("Please provide a file\n");
        return 1;
    }
    for(int i = 1; i < argc; i++){
        input = fopen(argv[i], "rb");
        fseek(input,0,SEEK_END);
        long file_length = ftell(input);
        fseek(input,0,SEEK_SET);
        unsigned char buffer[1];

        for(long file_index = 0; file_index < file_length; file_index++){
            fread(buffer,1,1,input);
            printf("%02X ",buffer[0]);
        }
        
        printf("\n");
        fclose(input);
    }
}