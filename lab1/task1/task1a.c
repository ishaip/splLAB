#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char **argv) {
    FILE *outputFile = stdout;
    FILE *inputFile = stdin;
    char c =0;
    bool  runningDebug = false;
    bool encoderFlag = false;
    int plusMinusCase = 1;
    int encoder = 0;



    for (int i =1; i < argc;i++){
        if(strncmp(argv[i], "-D",2) ==0)
            runningDebug = true;

        else if(strncmp(argv[i], "-e",2) ==0){
            encoder =  argv[i] + 2;
            encoderFlag = true;
            plusMinusCase = -1;
        }

        else if(strncmp(argv[i], "+e",2) ==0){
            encoder =  argv[i] + 2;
            encoderFlag = true;
        }
        else if (strncmp(argv[i], "-o", 2) == 0){
            outputFile = fopen(argv[i] + 2, "w");
        }

        else if (strncmp(argv[i], "-i", 2) == 0){
            inputFile = fopen(argv[i] + 2, "r");
        }
    }
    // if (runningDebug == true){
    //     for(int i = 1; i>arg)
    // }


    while((c = fgetc(inputFile)) != EOF){
        if( 'A' <= c && c >= 'Z')
            c = 46; 
        fputs(outputFile , c);
    }
    fclose(outputFile);
    fclose(inputFile);

}