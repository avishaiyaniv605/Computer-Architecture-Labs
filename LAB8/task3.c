#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#define ON 1
#define BASE_OP 0
#define SEC_OP 1
#define SMBL_OP 2
#define REL_OP 3

struct fun_desc{
    char *name;
    void (*fun)();
};

int isDebug = 0;
int fd, num_of_section_headers, opCode = -1;
void *map_start; /* will point to the start of the memory mapped file */
struct stat fd_stat; /* this is needed to  the size of the file */
Elf32_Ehdr *header; /* this will point to the header structure */

void examineELF();
void examineSections();
void examineSymbols();
void map(char* fileName);
void actOnPrint(int opCode);
void printBasic();
void printSections();
void printSymbols();
void toggleDebugMode();
void examineRelocTabs();
void quit();

int main(int argc, char **argv){
    int sizeOfMenu = 0, numInput = -1;
    char input[256];
    struct fun_desc menu[] = {
        {"Toggle Debug Mode", toggleDebugMode},
        {"Examine ELF File", examineELF},
        {"Print Section Names", examineSections},
        {"Print Symbols", examineSymbols},
        {"Relocation Tables", examineRelocTabs},
        {"Quit", quit},
        {NULL, NULL}
    };


    while(menu[sizeOfMenu].name != NULL)
        sizeOfMenu++;

    while (1){        
        int i = 0 ;
        fprintf(stdout, "Please choose a function: \n");
        for (i = 0; i < sizeOfMenu; i++)
            fprintf(stdout, "%d) %s\n", i, menu[i].name);
        fgets(input, 256, stdin);
        int res = sscanf(input, "%d", &numInput);
        if(res == EOF || input == NULL){
            fprintf(stdout, "Never program again. you suck!\n");
            break;
        }
        else{
            menu[numInput].fun();
        }
        
    }
}

void quit(){
    exit(0);
}

void toggleDebugMode(){
    isDebug = ON - isDebug;
    if(!isDebug){
        fprintf(stderr, "%s\n\n", "debug flag is now off");
        return;
    }
    fprintf(stderr, "%s\n\n", "debug flag is now on");
}

void examineELF(){
    opCode = BASE_OP;
    char buff[100];
    char fName[100];
    fprintf(stderr, "Which file?\n");
    fgets(buff, 100, stdin);
    buff[strlen(buff)-1] = 0;
    strcpy(fName, buff);
    if(isDebug){
        fprintf(stderr, "%s%s\n", "Debug info: File loaded: ", fName);
    }
    map(fName);
}

void examineSections(){
    opCode = SEC_OP;
    char buff[100];
    char fName[100];
    fprintf(stderr, "Which file?\n");
    fgets(buff, 100, stdin);
    buff[strlen(buff)-1] = 0;
    strcpy(fName, buff);
    if(isDebug){
        fprintf(stderr, "%s%s\n", "Debug info: File loaded: ", fName);
    }
    map(fName);
}

void examineSymbols(){
    opCode = SMBL_OP;
    char buff[100];
    char fName[100];
    fprintf(stderr, "Which file?\n");
    fgets(buff, 100, stdin);
    buff[strlen(buff)-1] = 0;
    strcpy(fName, buff);
    if(isDebug){
        fprintf(stderr, "%s%s\n", "Debug info: File loaded: ", fName);
    }
    map(fName);
}

void map(char* fileName){
    if((fd = open(fileName, O_RDWR)) < 0 ) {
      perror("error in open");
      exit(-1);
    }

    if(fstat(fd, &fd_stat) != 0 ) {
      perror("stat failed");
      exit(-1);
    }

    if((map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0)) == MAP_FAILED ) {
      perror("mmap failed");
      exit(-4);
    }

   /* now, the file is mapped starting at map_start.
    * all we need to do is tell *header to point at the same address:
    */

    header = (Elf32_Ehdr *) map_start;
    if((header->e_ident[0] != 0x7f) || (header->e_ident[1] != 0x45) || (header->e_ident[2] != 0x4c)){
        printf("File is not in elf format.");
        close(fd);
        munmap(map_start, fd_stat.st_size);
        fd = -1;
        return;
    }

   /* now we can do whatever we want with header!!!!
    * for example, the number of section header can be obtained like this:
    */
   actOnPrint(opCode);
   /* now, we unmap the file */
   munmap(map_start, fd_stat.st_size);
}

void actOnPrint(int opCode){
    if(opCode == BASE_OP){
        printBasic();
    }
    else if(opCode == SEC_OP){
        printSections();
    }
    else if(opCode == SMBL_OP){
        printSymbols();
    }
    else{
        printReloc();
    }
    opCode = -1; 
}

void printBasic(){
    fprintf(stderr, "----------------- DETAILS -----------------\n");
    fprintf(stderr, "Magic number: %.3s\n", header->e_ident + 1);
    if(header->e_type == ELFDATA2MSB){
        fprintf(stderr, "The data encoding scheme of the object file: big endian\n");
    }else{
        fprintf(stderr, "The data encoding scheme of the object file: little endian\n");
    } 
    fprintf(stderr, "Entry point (hexadecimal address): %X\n", header->e_entry);
    fprintf(stderr, "The file offset in which the section header table resides: %d\n", header->e_shoff);
    fprintf(stderr, "The number of section header entries: %d\n", header->e_shnum);
    fprintf(stderr, "The size of each section header entry: %d\n", header->e_shentsize);
    fprintf(stderr, "The file offset in which the program header table resides: %d\n", header->e_phoff);
    fprintf(stderr, "The number of program header entries: %d\n", header->e_phnum);
    fprintf(stderr, "The size of each program header entry: %d\n\n", header->e_phentsize);
}

void printSections(){
    //http://www.sco.com/developers/gabi/latest/ch4.sheader.html#sh_type    num to type convertion
    int i = 0;
    Elf32_Shdr* sHeaders = (Elf32_Shdr*) (map_start + header->e_shoff);
    char* sectionStr = (char*) (map_start + sHeaders[header->e_shstrndx].sh_offset);
    fprintf(stderr, "There are %d section headers, starting at offset %#0x:\n\n\n", header->e_shnum, header->e_shoff);
    fprintf(stderr, "[Nr] %-20s%-20s%-20s%-20s%-20s\n", "Name", "Addr", "Off", "Size", "Type");
    for (i = 0; i < header->e_shnum; i++){
        if(i < 10){
            fprintf(stderr, "[ %d] %-20s%-20x%-20x%-20x%-20d\n", i,  (sectionStr + sHeaders[i].sh_name), sHeaders[i].sh_addr, sHeaders[i].sh_offset, sHeaders[i].sh_size, sHeaders[i].sh_type);
        }
        else{
            fprintf(stderr, "[%d] %-20s%-20x%-20x%-20x%-20d\n", i,  (sectionStr + sHeaders[i].sh_name), sHeaders[i].sh_addr, sHeaders[i].sh_offset, sHeaders[i].sh_size, sHeaders[i].sh_type);
        }
    }
    fprintf(stderr, "\n");
}

void printSymbols(){
    //[index] value section_index section_name symbol_name 
    int i = 0, j = 0;
    Elf32_Shdr* sHeaders = (Elf32_Shdr *)(map_start + header->e_shoff);
    for(i = 0; i < header->e_shnum; i++){      
            if (sHeaders[i].sh_type == SHT_DYNSYM){
                Elf32_Sym* symtab = (Elf32_Sym *)(map_start + sHeaders[i].sh_offset);
                int index = sHeaders[i].sh_link;
                int offset = (map_start + sHeaders[index].sh_offset);
                int size = sHeaders[i].sh_size / sizeof(Elf32_Sym);
                Elf32_Shdr* sectionNdx = sHeaders + (header->e_shstrndx);    
                char* sectionStr = (char*)(map_start + sectionNdx->sh_offset);
                char* currSectionName = (char *)(sectionStr + sHeaders[symtab[j].st_shndx].sh_name);
                fprintf(stderr, "Symbol table '.dynsym' contains %d entries:\n\n", size);
                fprintf(stderr, "num: %-20s%-20s%-20s%-20s\n", "value", "section_index", "section_name", "symbol_name");
                for (j = 0; j < size; j++){
                    char* currSymbolName = (char *)(offset + symtab[j].st_name);
                    currSectionName = (char *)(sectionStr + sHeaders[symtab[j].st_shndx].sh_name);
                    fprintf(stderr, "%d:   %-20x%-20d%-20s%-20s\n", j, symtab[j].st_value, symtab[j].st_shndx, currSectionName, currSymbolName);
                }
            }
    }
    
    for (i = 0; i < header->e_shnum; i++){
        if (sHeaders[i].sh_type == SHT_SYMTAB) {
                Elf32_Sym* symtab = (Elf32_Sym *)(map_start + sHeaders[i].sh_offset);
                int index = sHeaders[i].sh_link;
                int offset = (map_start + sHeaders[index].sh_offset);
                int size = sHeaders[i].sh_size / sizeof(Elf32_Sym);
                fprintf(stderr, "Symbol table '.symtab' contains %d entries:\n\n", size);
                fprintf(stderr, "num: %-20s%-20s%-20s%-20s\n", "value", "section_index", "section_name", "symbol_name");
                for (j = 0; j < size; j++){
                    char* currSymbolName, *currSectionName;
                    if(symtab[j].st_shndx == 0xfff1){
                        currSectionName = ".abs";
                    }
                    else if(symtab[j].st_shndx == 0xfff2){
                        currSectionName = ".common";
                    }
                    else if(symtab[j].st_shndx == 0xffff){
                        currSectionName = ".hireserve";
                    }
                    else {
                        Elf32_Shdr* sectionNdx = sHeaders + (header->e_shstrndx);    
                        char* sectionStr = (char*) (map_start + sectionNdx->sh_offset);
                        currSectionName = (char *)(sectionStr + sHeaders[symtab[j].st_shndx].sh_name);
                    }
                    currSymbolName = (char *)(offset + symtab[j].st_name);
                    if(j < 10){
                        fprintf(stderr, "%d:    %-20x%-20d%-20s%-20s\n", j, symtab[j].st_value, symtab[j].st_shndx, currSectionName, currSymbolName);
                    }
                    else{
                        fprintf(stderr, "%d:   %-20x%-20d%-20s%-20s\n", j, symtab[j].st_value, symtab[j].st_shndx, currSectionName, currSymbolName);                    
                    }
                }
            
            }
    }
}



void examineRelocTabs(){
    opCode = REL_OP;
    char buff[100];
    char fName[100];
    fprintf(stderr, "Which file?\n");
    fgets(buff, 100, stdin);
    buff[strlen(buff)-1] = 0;
    strcpy(fName, buff);
    if(isDebug){
        fprintf(stderr, "%s%s\n", "Debug info: File loaded: ", fName);
    }
    map(fName);
}

void printReloc(){
    int i = 0, j = 0;
    Elf32_Shdr* sHeaders = (Elf32_Shdr *)(map_start + header->e_shoff);
    for(i = 0; i < header->e_shnum; i++){      
            if (sHeaders[i].sh_type == SHT_REL){
                Elf32_Rel* reloctab = (Elf32_Rel *)(map_start + sHeaders[i].sh_offset);
                int size = sHeaders[i].sh_size / sizeof(Elf32_Rel);
                int index = sHeaders[i].sh_link;
                Elf32_Shdr* sectionNdx = sHeaders + (sHeaders[index].sh_link);  
                char* strTab = (char*)(map_start + sectionNdx->sh_offset);
                Elf32_Sym *symtab = (Elf32_Sym*)(map_start + sHeaders[index].sh_offset);
                //ELF32_R_TYPE[reloctab.r_info]
                fprintf(stderr, "%s%-20s%-20s%-20s%-20s\n", "offset", "info", "Type", "Sym.value", "Sym.name");

                for (j = 0; j < size; j++){
                    int ndx = ELF32_R_SYM(reloctab[j].r_info);
                    char *currSymbolName = (char *)(strTab + symtab[ndx].st_name);
                    int type = ELF32_R_TYPE(reloctab[j].r_info);
                    int val = symtab[ndx].st_value;
                    fprintf(stderr, "%x%-20x%-20d%-20d%-20s\n", reloctab[j].r_offset, reloctab[j].r_info, type, val, currSymbolName);
                }
            }
    }

}
