#include "util.h"

#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_LSEEK 19
#define STDOUT 1
#define STDIN 0
#define STDERR 2
#define SEEK_CUR 1
#define Read_Only 0
#define Write_Only 1 
#define Create_File 64 


int main (int argc , char* argv[], char* envp[])
{
    char scanner;
    int gap = 'a' - 'A', isDebug = 0, i, bytesRead, fdo = STDOUT, fdi = STDIN;
    char buffer[256];
    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-D", 2) == 0) 
            isDebug = 1;
        else if(strncmp(argv[i], "-i", 2) == 0){
            if ((fdi = system_call(SYS_OPEN, &argv[i][2], Read_Only, 0777)) < 0){
                system_call(SYS_WRITE, STDERR, "\nError loading file: ",21);
                system_call(1, 0x55, 1, 1);      
            }
         }
        else if(strncmp(argv[i], "-o", 2) == 0){
            if((system_call(SYS_OPEN, &argv[i][2], Create_File, 0777)) < 0 ||
                (fdo = system_call(SYS_OPEN, &argv[i][2], Write_Only, 0777)) < 0){
                system_call(SYS_WRITE, STDERR, "\nError loading file: ",21);
                system_call(1, 0x55, 1, 1); 
            }      
        }
    }


    buffer[0] = '\n';
    i = 1;
    while((bytesRead = system_call(SYS_READ, fdi, &scanner, 1)) > 0){
        if (scanner < 'a' && scanner >= 'A'){
            scanner += gap;
        }
        buffer[i] = scanner;
        i++;
        if(isDebug == 1){
            system_call(SYS_WRITE, STDERR, "\nS_C ID: ",9);
            system_call(SYS_WRITE, STDERR, itoa(SYS_READ), 1);
            system_call(SYS_WRITE, STDERR, "\nReturn Value: ", 14);
            system_call(SYS_WRITE, STDERR, itoa(bytesRead), 1);
            system_call(SYS_WRITE, STDERR, "\n\n-------------------------", 27);

        }
        if(buffer[i-1] == '\n'){
            system_call(SYS_WRITE, fdo, buffer, i);
            i = 1;
        }
    }

    fdo = system_call(SYS_CLOSE, fdo);
    fdi = system_call(SYS_CLOSE, fdi);
    if(isDebug == 1){
        system_call(SYS_WRITE, STDERR, "\nS_C ID: ",9);
        system_call(SYS_WRITE, STDERR, itoa(SYS_CLOSE), 1);
        system_call(SYS_WRITE, STDERR, "\nReturn Value: ", 14);
        system_call(SYS_WRITE, STDERR, itoa(fdi), 1);
        system_call(SYS_WRITE, STDERR, "\n\n-------------------------", 27);
        system_call(SYS_WRITE, STDERR, "\nS_C ID: ",9);
        system_call(SYS_WRITE, STDERR, itoa(SYS_CLOSE), 1);
        system_call(SYS_WRITE, STDERR, "\nReturn Value: ", 14);
        system_call(SYS_WRITE, STDERR, itoa(fdo), 1);
    }

  return 0;
}