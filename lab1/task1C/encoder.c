#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    //FILE *outputFile = stdout;
    FILE *inputFile = stdin;
    char c =0;
    bool runningDebug = false;
    bool encoderFlag = false;
    int offset = 1;
    char encoder[256] = "";
    int lettersChaged = 0;
    int encoderIndex = 0;
    int encoderLength = 1;




    for (int i =1; i < argc;i++){
        if(strncmp(argv[i], "-D",2) ==0)
            runningDebug = true;
        
        else if(strncmp(argv[i], "-e",2) ==0){
            strncpy(encoder, argv[i] + 2, strlen(argv[i]) - 2);
            encoderFlag = true;
            offset = -1;
            encoderLength = strlen(argv[i]) - 2;
        }
        
        else if(strncmp(argv[i], "+e",2) ==0){
            strncpy(encoder, argv[i] + 2, strlen(argv[i]) - 2);
            encoderFlag = true;
            encoderLength = strlen(argv[i]) - 2;
        }

        // else if (strncmp(argv[i], "-o", 2) == 0){
        //     outputFile = fopen(argv[i] + 2, "w");
        // }

        else if (strncmp(argv[i], "-i", 2) == 0){
            inputFile = fopen(argv[i] + 2, "r");
        }
    }

    while((c = fgetc(inputFile)) != EOF ){

        if(runningDebug)
             fprintf(stderr, "%02X" ,c );
             
        if(!encoderFlag && 'a' <= c && c <= 'z'){
            c = c + 'A' - 'a'; 
            lettersChaged ++;
        }

        if(runningDebug)
            fprintf(stderr, " %02X\n" ,c );

        if(encoderFlag){
            c = c + (offset * (encoder[encoderIndex] - 48));
            encoderIndex ++;
            encoderIndex = encoderIndex % encoderLength;
        }

        printf("c");

    }

    printf("\n");
    return 0;
}