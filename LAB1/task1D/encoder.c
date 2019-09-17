#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {

    int j = 0, mode = 0, isDebug = 0, sizeOfKeys = 0, fromFile = 0;
    char c, copy, *encKeys, *fileName;
    FILE * input = stdin;
    FILE * output = stdout;

    for (int i = 1; i < argc; i++) { //checking specific mode.
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
            fileName = strdup(argv[i] + 2);
        }
    }

    if (mode == 2 || mode == 3) { //if encryption keys are given.
        size_t size = strlen(encKeys);
        sizeOfKeys = size;
    }

    if (mode == 2 && isDebug == 1) mode = 4; 
    else if (mode == 3 && isDebug == 1) mode = 5;
    else if (isDebug == 1) mode = 1;
    if (fromFile == 1){
        FILE * toOpen = fopen(fileName, "r");
        if (toOpen == NULL) mode = -1;
        else input = toOpen;
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
    } else if (mode == 1) { // -D
        while ((c = fgetc(stdin)) != EOF) {
            fprintf(stderr, "%#02x\t", c);
            if (c > 64 && c < 97) c = (c + 32);
            fprintf(stderr, "%#02x\n", c);
            fprintf(stdout, "%c", c);
        }

            /*
            This is an effect of buffering. When outputting to a terminal, stdio uses line based buffering,
            which will give the expected effect. When outputting to a file, it uses block buffering (typically 4k or 8k blocks),
            hence it will only flush on program end.
            The difference you're observing is because STDERR is unbuffered, unlike most other handles.
            In addition, stdout uses linBuffer so newline character flushes the buffer.
            */ 

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
    } else if (mode == 4 || mode == 5) { // +e -D || -e -D
        while ((c = fgetc(input)) != EOF) {
            copy = c;
            if (mode == 4) {
                fprintf(stderr, "%#02x\t", (c + encKeys[j]));
                fprintf(stderr, "%#02x\n", (c + encKeys[j]));
                fprintf(output, "%c", c + encKeys[j]);
            } else {
                if(c - encKeys[j] < 0) c = -(c - encKeys[j]);
                else c = c - encKeys[j];
                fprintf(stderr, "%#02x\t", c);
                fprintf(stderr, "%#02x\n", c);
                fprintf(output, "%c", copy + encKeys[j]);
            }
            if (copy == '\n') {
                fputc(copy, output);
                j = -1;
            }
            j = (j + 1) % sizeOfKeys;
        }
    }
}




