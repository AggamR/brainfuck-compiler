#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
        printf("file can't be opened\n");
        exit(1);
    }

    char line[128];
    char code[512];

    while (!feof(fpsource)) {
        fgets(line, 128, fpsource);
        strcat(code, line);
    }

    fclose(fpsource); 

    FILE * fpasm;
    char asmFileName[strlen(argv[1]) + 2]; 
    strcpy(asmFileName, argv[1]);
    strcat(asmFileName, ".c");
    fpasm = fopen(asmFileName, "w");

    fprintf(fpasm, "#include <stdio.h>\n#include <stdlib.h>\nint main() {\nint* p = malloc(%d);\n", memSize);

    int stack[memSize/4];
    int sp = memSize/4 - 1; // stack pointer

    // reset arrays
    for (int i = 0; i < memSize; i++) {
        stack[i/4] = 0;  // must be a netter way of doing this...
    }

    // loop chars in code
    for (int i = 0; i < strlen(code); i++) {
        //printf("\nmem[memp]: %d\ni: %d", mem[memp], i);
        switch (code[i]) {
            case '+':
                fprintf(fpasm, "*(p+%d) = *(p+%d) + 1;\n", memp, memp);
                break;
            case '-':
                fprintf(fpasm, "*(p+%d) = *(p+%d) - 1;\n", memp, memp);
                break;
            case '>':
                if (memp < memSize - 1) memp++;
                break;
            case '<':
                if (memp > 0) memp--;
                break;
            case '.':
                fprintf(fpasm, "putchar(*(p+%d));\n", memp);
                break;
            case ',':
                fprintf(fpasm, "*(p+%d) = getchar();\n", memp);
                break;
            case '[':
                fprintf(fpasm, "while (*(p+%d) != 0) {\n", memp);
                //stack[sp] = i;
                //sp++;
                break;
            case ']':
                fprintf(fpasm, "}\n");
                //if (mem[memp] != 0) i = stack[sp-1];
                //else sp--;
                break;
        }
    }

    fprintf(fpasm, "}", memSize);

    return 0;
}