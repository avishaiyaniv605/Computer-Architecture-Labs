#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char sig[];
} virus;

typedef struct link link;

struct link {
    link *nextVirus;
    virus *vir;
};

struct fun_desc {
    char *name;
    void (*fun)(link*);
};

void list_print(link *virus_list);
/* Print the data of every link in list. Each item followed by a newline character. */

link* list_append(link* virus_list, virus* data);
/* Add a new link with the given data to the list
   (either at the end or the beginning, depending on what your TA tells you),
   and return a pointer to the list (i.e., the first link in the list).
   If the list is null - create a new entry and return a pointer to the entry. */

void list_free(link *virus_list); /* Free the memory allocated by the list. */

void PrintHex(unsigned char* buffer, int length);

void load_signatures(link* virus_list);

void on_my_way_to_detect(link* virus_list);

void detect_virus(unsigned char *buffer, link *virus_list,unsigned int size);

void kill_virus(char *fileName, int signitureOffset, int signitureSize);

void on_my_way_to_kill(link* virus_list);

int main(int argc, char **argv) {
    int i, bounds, input, inputTest;
    link *virus_list = (link*) malloc(sizeof (struct link));
	virus_list-> vir = NULL;   // first link flag
    struct fun_desc menu[] = {{"Load signatures", load_signatures},
                              {"Print signatures", list_print},
                              {"Detect viruses", on_my_way_to_detect},
                              {"Fix file", on_my_way_to_kill},
                              {"Quit", list_free},
                              {NULL, NULL}};

    bounds = sizeof(menu) / sizeof((menu)[0]) -1 ;
    while (1){
        fprintf(stdout, "Please choose a function:\n");
        for (i = 0 ; i < bounds; i++)
            printf("%i. %s\n",i + 1,menu[i].name);

        inputTest = scanf("%d", &input);
        if (inputTest < 0)
        {
            printf("%s\n", "Leave my program alone!");
            exit(1);
        }
        if (input > 0 && input <= bounds){
            menu[input - 1].fun(virus_list);   
            printf("DONE.\n");
        }
        else {
            printf("Not within bounds");
            break;
        }
    }
    list_free(virus_list);
    return 0;
}


link* list_append(link* virus_list, virus* data){
    if(virus_list->vir == NULL){    //empty list
       virus_list->vir = data;
        virus_list->nextVirus = NULL;
    }
    else{                           // inserts new virus and new link
        link* currLink = virus_list;
        while(currLink->nextVirus){
            currLink = currLink->nextVirus;
        }
        currLink->nextVirus = (link*) malloc(sizeof (struct link));
        if (currLink->nextVirus == NULL){
            printf("memory allocation failed.\n");
            exit(1);
        }
        currLink->nextVirus-> vir = data;
        currLink->nextVirus->nextVirus = NULL;
    }
    return virus_list;
}

void load_signatures(link *virus_list){
    char* fileNameFromUser;
    int inputTest;
    printf("%s\n", "please enter file name.");
    inputTest = scanf("%s", fileNameFromUser);
    if (inputTest < 0)
    {
        printf("%s\n", "Leave my program alone!");
        exit(1);
    }
    FILE *inputFile = fopen(fileNameFromUser, "r");

    unsigned short currLength = 0;
    int finsihPosition, bytesFromFile, currSignSize, currPos = 0;
    if (!inputFile) {
        printf("problems loading file.\n");
        return;
    }
    else{
        fseek (inputFile , 0L , SEEK_END);
        finsihPosition = ftell(inputFile);
        rewind (inputFile);
    }
    while(currPos != finsihPosition){
        bytesFromFile = fread(&currLength, sizeof(unsigned short), 1, inputFile);
        if(bytesFromFile == 0) {
            fprintf(stdout, "failed.\n" );
        }
        else{

            virus* currVirus = (virus*) malloc(currLength);
            currSignSize = currLength - 18;
            currVirus->SigSize = currSignSize;
            fread(currVirus->virusName, 1, currLength - 2, inputFile);
            //fread(currVirus->sig, 1, currSignSize, inputFile);

            virus_list = list_append(virus_list, currVirus);
            if (virus_list == NULL){
                printf("Cannot allocate memory.\n");
                exit(1);
            }
            currPos = ftell(inputFile);
        }
    }
    fclose(inputFile);
}

void list_print(link *virus_list){
    while(virus_list && virus_list->vir){
        fprintf(stdout, "Virus name: %s\n",virus_list->vir->virusName);
        fprintf(stdout, "Virus size: %d\n",virus_list->vir->SigSize);
        fprintf(stdout, "Virus signature:\n");
        PrintHex(virus_list->vir->sig,virus_list->vir->SigSize);
        fprintf(stdout,"\n\n");
        virus_list = virus_list->nextVirus;
    }
}

void list_free(link *virus_list) {/* Free the memory allocated by the list. */
    link *linForFreeing;
    if (!virus_list -> vir){ 
        free(virus_list);
        exit(0);
    }
    while(virus_list){
        linForFreeing = virus_list;
        virus_list = virus_list -> nextVirus;
        if (linForFreeing->vir)
            free(linForFreeing->vir);
        free(linForFreeing);
    }
    exit(0);
}

void PrintHex(unsigned char* buffer, int length){
    int j;
    for (j = 0; j < length ; j++){
        fprintf(stdout, "%02X ", buffer[j]);
    }
}

void on_my_way_to_detect(link* virus_list){
    if (virus_list->vir == NULL){
        fprintf(stdout, "Something went wrong.\n");
        return;
    }
    FILE * inputFile; 
    unsigned char buffer[10000]; // as expected in task
    char nameOfFile[32];
    unsigned int fileLength = 0;
    printf("Enter the name of file you want to clean \n");
    fgetc(stdin);
    if(fgets(nameOfFile, 32, stdin) != NULL){
        size_t nameLen = strlen(nameOfFile);
        if(nameLen > 0 && nameOfFile[nameLen-1] == '\n'){
            nameOfFile[--nameLen] = '\0';
        }
    }
    inputFile = fopen(nameOfFile, "r");
    if (!inputFile){
        fprintf(stdout, "File not found.\n");
        return;
    }
    fileLength = fread(buffer, 1, 10000, inputFile);
    if (fileLength > 10000)
        fileLength = 10000;
    else if (fileLength == 0)
        fprintf(stdout, "error loading file.\n");
    else
        detect_virus(buffer,virus_list,fileLength);            

    fclose(inputFile);
}

void detect_virus(unsigned char *buffer,link *virus_list, unsigned int size){
    link* currLink = virus_list;
    int i, hasFoundVirus = 0, currVirusLength, compareRes;
    while(currLink){
        currVirusLength = currLink->vir->SigSize;
        i = 0;
        while(i < size - currVirusLength + 1) {
            compareRes = memcmp(currLink->vir->sig, &buffer[i], currVirusLength);
            if(compareRes == 0){
            printf("Starting from byte: %d\n", i);
            printf("Virus name: %s\n",currLink->vir->virusName);
            printf("Virus size: %d\n\n",currVirusLength);
            hasFoundVirus = 1;
        }
        i++;
    }
    currLink = currLink->nextVirus;
  }
  if(hasFoundVirus == 0){
    printf("\n\n\n%s\n\n\n", "Your file is free from viruses! --- GO TO THE BEACH");
  }
}

void on_my_way_to_kill(link* virus_list){
    char nameOfFile[32], buffer[32];
    int filePos,size;
    printf("Enter file name you want to free from viruses: \n");
    fgetc(stdin);
    if(fgets(nameOfFile, 32, stdin) != NULL){
        size_t nameLen = strlen(nameOfFile);
        if(nameLen > 0 && nameOfFile[nameLen - 1] == '\n'){
            nameOfFile[--nameLen] = '\0';
        }
    }
    printf("Enter position in file where the virus starts: \n");
    if(fgets(buffer, 32, stdin) != NULL)
        filePos = atoi(buffer);
    printf("Enter the virus's size: \n");
    if(fgets(buffer, 32, stdin) != NULL)
        size = atoi(buffer);
    kill_virus(nameOfFile,filePos,size);
}


void kill_virus(char *fileName, int signatureOffset, int signatureSize) {
    FILE * infectedFile = fopen(fileName, "rw+"); 
    char toReplaceInFile [signatureSize];
    int i;

    if (infectedFile == NULL)
        fprintf(stdout, "error loading file.\n");
    else{
        rewind(infectedFile);
        fseek(infectedFile, signatureOffset, SEEK_CUR);
        for (i = 0; i< signatureSize; i++)
            toReplaceInFile[i] = (char)0x90;
        fwrite(toReplaceInFile, 1, signatureSize, infectedFile);
        fclose(infectedFile);
    }
}







	
	
	
