#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {

    int j = 0, mode = 0, isDebug = 0, sizeOfKeys = 0, fromFile = 0, toFile = 0;
    char c, copy, *encKeys, *inFileName, *outFileName;
    FILE * input = stdin;
    FILE * output = stdout;
    FILE * forCheck;

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "+e", 2) == 0 || strncmp(argv[i], "-e", 2) == 0) {
            if (strncmp(argv[i], "+e", 2) == 0) {
                printf("+e\n");
                encKeys = strdup(argv[i] + 2);
                mode = 2;
            } else {
                printf("-e\n");
                encKeys = strdup(argv[i] + 2);
                mode = 3;
            }
        } else if (strncmp(argv[i], "-D", 2) == 0) {
            printf("-D\n");
            isDebug = 1;
        }
        else if(strncmp(argv[i], "-i", 2) == 0){
            fromFile = 1;
            inFileName = strdup(argv[i] + 2);
        }
        else if(strncmp(argv[i], "-o", 2) == 0){
            toFile = 1;
            outFileName = strdup(argv[i] + 2);
        }
    }

    if (mode == 2 || mode == 3) {
        size_t size = strlen(encKeys);
        sizeOfKeys = size;
    }

    if (mode == 2 && isDebug == 1) mode = 4;
    else if (mode == 3 && isDebug == 1) mode = 5;
    else if (isDebug == 1) mode = 1;
    if (fromFile == 1){
        FILE * toOpen = fopen(inFileName, "r");
        if (toOpen == NULL) mode = -1;
        else input = toOpen;
    }
    if (toFile == 1){
        forCheck = fopen(outFileName, "w");
        if (forCheck == NULL) mode = -1;
        else output = forCheck;
    }


    if (mode == -1){
        printf("File does not exist or corrupted");
    }
    else if (mode == 0) { // nothing.
        while ((c = fgetc(input)) != EOF) {
            if (c > 64 && c < 91)
                fprintf(output, "%c", (c + 32));
            else
                fprintf(output, "%c", c);
        }
        if(input != NULL){
            fclose(input);
        }
        if(output != NULL){
            fclose(output);
        }

    } else if (mode == 1) { // -D
        while ((c = fgetc(input)) != EOF) {
            fprintf(stderr, "%#02x\t", c);
            if (c > 64 && c < 97)
                c = (c + 32);
            fprintf(stderr, "%#02x\n", c);
            fprintf(output, "%c", c);
        }
        if(input != NULL){
            fclose(input);
        }
        if(output != NULL){
            fclose(output);
        }
    } else if (mode == 2 || mode == 3) { // +e || -e
        while ((c = fgetc(input)) != EOF) {
            copy = c;
            if (mode == 2)
                fputc((c + encKeys[j]), output);
            else {
                if(c - encKeys[j] < 0)  c = -(c - encKeys[j]);
                else c = c - encKeys[j];
                fputc(c , output);
            }
            if (copy == '\n') {
                fputc(copy, output);
                j = -1;
            }
            j = (j + 1) % sizeOfKeys;
        }
        if(input != NULL){
            fclose(input);
        }
        if(output != NULL){
            fclose(output);
        }
    } else if (mode == 4 || mode == 5) { // +e -D || -e -D
        while ((c = fgetc(input)) != EOF) {
            copy = c;
            if (mode == 4) {
                fputc(c + encKeys[j] , output);
                fprintf(stderr, "%#02x\t", (c + encKeys[j]));
                fprintf(stderr, "%#02x\n", (c + encKeys[j]));
            } else {
                if(c - encKeys[j] < 0) c = -(c - encKeys[j]);
                else c = c - encKeys[j];
                fputc(copy + encKeys[j] , output);
                fprintf(stderr, "%#02x\t", c);
                fprintf(stderr, "%#02x\n", c);
            }
            if (copy == '\n') {
                fputc(copy, output);
                j = -1;
            }
            j = (j + 1) % sizeOfKeys;
        }
        if(input != NULL){
            fclose(input);
        }
        if(output != NULL){
            fclose(output);
        }

    }
}





