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
    char encoder = 0;
    char incoderChar =0;
    char* output = "";
    int lettersChaged = 0;



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
        // else if (strncmp(argv[i], "-o", 2) == 0){
        //     outputFile = fopen(argv[i] + 2, "w");
        // }

        else if (strncmp(argv[i], "-i", 2) == 0){
            inputFile = fopen(argv[i] + 2, "r");
        }
    }
    if (runningDebug == true){
        for(int i = 1; i>argc; i++){
            fprintf(stderr, "%s", argv[i]);
        }
        fprintf (stderr, " ");
    }


    while((c = fgetc(inputFile)) != EOF && encoderFlag){        //not adding first chares encoder amount of times
        if (encoderFlag && plusMinusCase == -1 && encoder >0)
                encoder = encoder -1;
        else
            strcat(output, c);
    }

    while (encoderFlag && plusMinusCase ==1 && encoder >0)      //adding the fisrt char encoder amount of times
    {
        strcat(output, output[0]);
        encoder--;
    }
    

    while((c = fgetc(inputFile)) != EOF && !encoderFlag){
        if(runningDebug)
             printf("%d",c);
        if( 'A' <= c && c <= 'Z'){
            c = 46; 
            lettersChaged ++;
        }
        if(runningDebug)
            printf(" %d\n" ,c );
        strcat(output , c);
     }
    if (runningDebug)
        printf("%d", lettersChaged);
    printf("%s", output);
}