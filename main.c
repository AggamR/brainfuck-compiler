#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define COLOR_RED     "\x1b[31m"
#define COLOR_RESET   "\x1b[0m"
#define COLOR_GREEN   "\x1b[32m"

char* toLower(char* s) {
  for(char *p=s; *p; p++) *p=tolower(*p);
  return s;
}

void errmsg(char msg[]) {
    printf(COLOR_RED "%s\n" COLOR_RESET, msg);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: `bf FILE [OPTIONS]`");
        return 1;
    }

    int memp = 0; // memory pointer
    int memSize = 64;

    FILE * fpsource;
    fpsource = fopen(argv[1], "r");

    if (fpsource == NULL) {
        errmsg("file can't be opened");
        return 1;
    }

    char line[128];
    char code[512];

    while (!feof(fpsource)) {
        fgets(line, 128, fpsource);
        strcat(code, line);
    }

    fclose(fpsource); 

    char asmFileName[128] = { 0 };

    for (int i = 2; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strlen(argv[i]) > 2) {
                errmsg("only one option at a time");
                return 1;
            }

            switch (argv[i][1]) {
                case 'c':  // c file output
                    if (i+1 < argc) {
                        if (strlen(argv[i+1]) < 128)
                            strcpy(asmFileName, argv[i+1]);
                        else 
                            errmsg("file name too long");
                    }
                    else
                        errmsg("please enter a file name");
                    break;
            }

            continue;
        }

        if (strcmp(toLower(argv[i]), "help") == 0) {
            printf(
                "\tthis is the brainfuck compiler. it is to be used to compile brainfuck to c.\n"
                "\tyou can use the options:\n"
                "\t\"-c <FILENAME>\" to define the c output file name.\n"
                "\t\"help\" to print this text.\n"
            );
            continue;
        }
            
    }

    FILE * fpasm;
    if (asmFileName[0] == 0) {
        strcpy(asmFileName, argv[1]);
        strcat(asmFileName, ".c");
    }
    fpasm = fopen(asmFileName, "w");

    
    // THIS CODE IS A PART OF A FEATURE THAT CURRENTLY DOESN'T WORKED - TO BE FIXED 
    //int makex = 0; // make executable

    fprintf(fpasm, "#include <stdio.h>\n#include <stdlib.h>\nint main() {\n\tint* p = malloc(%d);\n", memSize);

    int stack[memSize/4];
    int sp = memSize/4 - 1; // stack pointer

    int toadd = 0;

    // reset arrays
    for (int i = 0; i < memSize; i++) {
        stack[i/4] = 0;  // must be a netter way of doing this...
    }

    // loop chars in code
    for (int i = 0; i < strlen(code); i++) {
        if (code[i] == '+') {
            toadd++;
            continue;
        }
        else if (code[i] == '-') {
            toadd--;
            continue;
        }
        else if (toadd != 0){
            fprintf(fpasm, "\t*(p+%d) = *(p+%d) + %d;\n", memp, memp, toadd);
            toadd = 0;
        }

        switch (code[i]) {
            case '>':
                if (memp < memSize - 1) memp++;
                break;
            case '<':
                if (memp > 0) memp--;
                break;
            case '.':
                fprintf(fpasm, "\tputchar(*(p+%d));\n", memp);
                break;
            case ',':
                fprintf(fpasm, "\t*(p+%d) = getchar();\n", memp);
                break;
            case '[':
                fprintf(fpasm, "\twhile (*(p+%d) != 0) {\n", memp);
                break;
            case ']':
                fprintf(fpasm, "\t}\n");
                break;
        }
    }

    fprintf(fpasm, "\treturn 0;\n}", memSize);
    printf(COLOR_GREEN "output file: %s" COLOR_RESET, asmFileName);
    fclose(fpasm);
    /*
    // THIS CODE CURRENTLY DOESN'T WORK - IT IS TO BE FIXED
    if (makex == 1) {
        #ifdef _WIN32 // Includes both 32 bit and 64 bit
            #ifdef _WIN64
                printf("this feature is not supported in windows\n");
                return 3;
            #else 
                printf("this feature is not supported in windows\n");
                return 3;
            #endif
        #endif
        
        char cmpcmnd[strlen(asmFileName)];
        strcat(cmpcmnd, "gcc ");
        strcat(cmpcmnd, asmFileName);
        //strcat(cmpcmnd, " -c");

        printf("%s", cmpcmnd);

        if (system("command -v gcc > /dev/null") == 0)
            system(cmpcmnd);
    }
    */

    return 0;
}