#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#define KB(i) ((i)*1<<10)
#define INT sizeof(int)
#define SHORT sizeof(short)
#define BYTE sizeof(char)
int unit_size = BYTE;

char* unit_to_format(int unit) {
    static char* formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
    return formats[unit_size-1];
}  

typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
} state;

struct fun_desc{
    char *name;
    void (*fun)(state*);
};

void toggleDebugMode(state* s);
void setUnitSize(state* s);
void setFileName(state* s);
void quit(state* s);
void freeAll(state* s);
void initState(state* s);
void loadIntoMemory(state* s);
void read_units_to_memory(FILE* input, char* buffer, int count);
void memoryDisplay(state* s);
void print_units(FILE* output, char* buffer, int count);
void saveIntoFile(state* s);
void write_units(FILE* output, char* buffer, int count);
void fileModify(state* s);

int main(int argc, char **argv){
    int sizeOfMenu = 0, numInput = -1;
    char input[256];

    struct fun_desc menu[] = {
        {"Toggle Debug Mode", toggleDebugMode},
        {"Set File Name", setFileName},
        {"Set Unit Size", setUnitSize},
        {"Load into memory", loadIntoMemory},
        {"Memory Display", memoryDisplay},
        {"Save Into File", saveIntoFile},
        {"File Modify", fileModify},
        {"Quit", quit},
        {NULL, NULL}
    };

    state* s = (state*)malloc(sizeof(state));  
    initState(s);    

    while(menu[sizeOfMenu].name != NULL)
        sizeOfMenu++;

    while (1){

        if(s->debug_mode){
            fprintf(stderr, "\n%s%d\n", "Unit Size: ", s->unit_size);
            fprintf(stderr, "%s%s\n", "File Name: ", s->file_name);
            fprintf(stderr, "%s%d\n\n", "Mem Count: ", s->mem_count);
        }
        
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
            menu[numInput].fun(s);
        }
        
    }
}

void toggleDebugMode(state* s){

    if (s->debug_mode == 0){
        fprintf(stderr, "\n%s\n", "debug flag is now on");
        s->debug_mode = 1;
    }

    else{
        fprintf(stderr, "%s\n", "debug flag is now off");
        s->debug_mode = 0;
    }
}

void setUnitSize(state* s){
    char size[100];
    fgets(size, 100, stdin);

    if(strcmp(size, "4\n") == 0){
        s->unit_size = INT;
        unit_size = INT;
    }  

    else if(strcmp(size, "2\n") == 0){
        s->unit_size = SHORT;
        unit_size = SHORT;
    }  

    else if(strcmp(size, "1\n") == 0){
        s->unit_size = BYTE;
        unit_size = BYTE;
    }  

    else{
        fprintf(stderr, "%s\n", "Error: Fuck off! ");
        return;
    } 

    if(s->debug_mode){
        fprintf(stderr, "%s%d\n", "Debug: set size to: ", s->unit_size);
    }
}

void setFileName(state* s){
    char newName[100];
    fgets(newName, 100, stdin);
    newName[strlen(newName)-1] = 0;
    strcpy(s->file_name, newName);

    if(s->debug_mode){
        fprintf(stderr, "%s%s\n", "Debug: file name set to: ", s->file_name);
    }
}

void quit(state* s){
    if(s->debug_mode){
        fprintf(stderr, "%s\n", "quitting...");
    }

    freeAll(s);
    exit(0);
}

void freeAll(state* s){
    free(s);
}

void initState(state* s){
    s->debug_mode = 0;
    strcpy(s->file_name, "");
    memcpy(s->mem_buf, "", 128);
    s->mem_count = 0;
    s->unit_size = BYTE;
}

void loadIntoMemory(state* s){
    int location, length;
    FILE* fin;
    char location_length[128];

    if(!(s->file_name)){
        fprintf(stderr, "%s\n", "Error: never load you dushbag");
        return;
    }

    fin = fopen(s->file_name, "r");

    if (fin == NULL){
        fprintf(stderr, "%s\n", "Error: never open a file you dushbag");
        return;
    }

    fprintf(stderr, "%s\n", "Please enter <location> <length>");
    fgets(location_length, 128, stdin);
    int res = sscanf(location_length, "%X %d", &location, &length);

    if(res == EOF){
        fprintf(stderr, "%s\n", "Error: sscanf faild.");
        return;
    }    

    if(s->debug_mode){
        fprintf(stderr, "%s %X %d\n", s->file_name, location, length);    
    }

    fseek(fin, (long)location, SEEK_SET);
    read_units_to_memory(fin, (char*)s->mem_buf, length);
    fprintf(stderr, "%s %d %s\n", "Loaded", length*s->unit_size, "units into memory");
    fclose(fin);    
}   

void read_units_to_memory(FILE* input, char* buffer, int count) {
    fread(buffer, unit_size, count, input);
}

void memoryDisplay(state* s) {
    int address, u;
    char addr_u[128];
    fprintf(stderr, "%s\n", "Please enter <u> <addr>");
    fgets(addr_u, 128, stdin);
    int res = sscanf(addr_u, "%d %X", &u, &address);

    if(res == EOF){
        fprintf(stderr, "%s\n", "Error: sscanf faild.");
        return;
    }    

    if(address == 0x00){
        print_units(stdout, (char*)s->mem_buf, u);
    }

    else{
        print_units(stdout, (char*)address, u);
    }
}

void print_units(FILE* output, char* buffer, int count) {
    char* end = buffer + unit_size*count;
    fprintf(output, "%s", "Decimal\t\tHexadecimal\n============================\n");
    while (buffer < end) {
        //print ints
        if(unit_size == INT){
            int int_num = *((int*)(buffer));
            fprintf(output, "%d\t\t", int_num);
            fprintf(output, unit_to_format(unit_size), int_num);
        }
        //print short
        else if(unit_size == SHORT){
            short short_num = *((short*)(buffer));
            fprintf(output, "%d\t\t", short_num);
            fprintf(output, unit_to_format(unit_size), short_num);
        }
        //print char
        else if(unit_size == BYTE){
            char char_num = *((char*)(buffer));
            fprintf(output, "%d\t\t", char_num);
            fprintf(output, unit_to_format(unit_size), char_num);
        }
        buffer += unit_size;
    }
}

void saveIntoFile(state* s){
    int source_address, target_location, length;
    char vars[128];
    FILE* fin;
    char* buffer;
    fprintf(stderr, "%s\n", "Please enter <source-address> <target-location> <length>");
    fgets(vars, 128, stdin);
    int res = sscanf(vars, "%X %X %d", &source_address, &target_location, &length);

    if(res == EOF){
        fprintf(stderr, "%s\n", "Error: sscanf faild.");
        return;
    }  

    //https://pubs.opengroup.org/onlinepubs/9699919799/functions/fopen.html
    fin = fopen(s->file_name, "rb+");
    fseek(fin, 0, SEEK_END);
    int fSize = ftell(fin);
    rewind(fin);

    if(fSize < target_location + length*unit_size){
        fprintf(stderr, "%s\n", "Error: target is wrong.");
        return;
    }

    else if(source_address == 0x00){
        buffer = (char*)s->mem_buf;
    }

    else{

        buffer = (char*)source_address;
    }

    fseek(fin, target_location, SEEK_SET);
    write_units(fin, buffer, length);
    fclose(fin);
}

/* Writes buffer to file without converting it to text with write */
void write_units(FILE* output, char* buffer, int count) {
    fwrite(buffer, unit_size, count, output);
}

void fileModify(state* s){
    FILE* fin;
    char loc_val[128];
    int location, val;
    fprintf(stderr, "%s\n", "Please enter <location> <val>");
    fgets(loc_val, 128, stdin);
    int res = sscanf(loc_val, "%X %X", &location, &val);

    if(res == EOF){
        fprintf(stderr, "%s\n", "Error: sscanf faild.");
        return;
    }  

    if(s->debug_mode){
        fprintf(stderr, "%s %x %x\n", "<Location, val>:", location, val);
    }  

    fin = fopen(s->file_name, "rb+");
    fseek(fin, location, SEEK_SET);
    write_units(fin, (char*)&val, 1);
    fclose(fin);
}
