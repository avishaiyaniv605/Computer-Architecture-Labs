#include "util.h"
#define _GNU_SOURCE
#include <dirent.h>
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_LSEEK 19
#define SYS_GETDENTS 141
#define STDOUT 1
#define STDIN 0
#define STDERR 2
#define SEEK_CUR 1
#define BUF_SIZE 8192

struct linux_dirent {
    unsigned long  d_ino;     
    unsigned long  d_off;
    unsigned short d_reclen;  
    char           d_name[];  
};

void printStdErr(int SysCallId, int returnedCode){
    system_call(SYS_WRITE, STDERR, "\n- S_C ID: ", 11);
    system_call(SYS_WRITE, STDERR, itoa(SysCallId), 1);
    system_call(SYS_WRITE, STDERR, "\n- Return Value: ", 17);
    system_call(SYS_WRITE, STDERR, itoa(returnedCode), 1);
    system_call(SYS_WRITE, STDERR, "\n\n", 1);
}


void printErrDirName(char* name, int length){
    system_call(SYS_WRITE, STDERR, "\n- Dirent Name: ", 16);
    system_call(SYS_WRITE, STDERR, name, strlen(name));
    system_call(SYS_WRITE, STDERR, "\n- Dirent Length: ", 18);
    system_call(SYS_WRITE, STDERR, itoa(length), 1);
    system_call(SYS_WRITE, STDERR, "\n\n", 1);
}

void printGaps(){
    system_call(SYS_WRITE, STDERR, "\n--------------------\n\n", 23);
}

void typeChecker(char d_type){
    if(d_type == DT_REG){
        system_call(SYS_WRITE, STDOUT, "\n", 1);
        system_call(SYS_WRITE, STDOUT, "Regualr", 7);
    }
    else if(d_type == DT_CHR){
        system_call(SYS_WRITE, STDOUT, "\n", 1);
        system_call(SYS_WRITE, STDOUT, "Char dev", 8);
    }
    else if(d_type == DT_DIR){
        system_call(SYS_WRITE, STDOUT, "\n", 1);
        system_call(SYS_WRITE, STDOUT, "Directory", 9);
    }
    else if(d_type == DT_FIFO){
        system_call(SYS_WRITE, STDOUT, "\n", 1);
        system_call(SYS_WRITE, STDOUT, "FIFO", 4);
    }
    else if(d_type == DT_LNK){
        system_call(SYS_WRITE, STDOUT, "\n", 1);
        system_call(SYS_WRITE, STDOUT, "Symlink", 7);
    }
    else if(d_type == DT_BLK){
        system_call(SYS_WRITE, STDOUT, "\n", 1);
        system_call(SYS_WRITE, STDOUT, "Block dev", 9);
    }
    else if(d_type == DT_SOCK){
        system_call(SYS_WRITE, STDOUT, "\n", 1);
        system_call(SYS_WRITE, STDOUT, "Socket", 6);
    }
    else if(d_type == DT_CHR){
        system_call(SYS_WRITE, STDOUT, "\n", 1);
        system_call(SYS_WRITE, STDOUT, "Char dev", 8);
    }
    else{
        system_call(SYS_WRITE, STDOUT, "\n", 1);
        system_call(SYS_WRITE, STDOUT, "????", 8);
    }
    system_call(SYS_WRITE, STDOUT, "\n", 1);
}

int main (int argc , char* argv[], char* envp[])
{
    system_call(SYS_WRITE, STDERR, "\nFlame 2 strikes!.\n\n", 20);
    int  isDebug = 0, nread, outFile, bpos, fDesc, isPrefix = 0, aPrefix = 0;
    char buf[BUF_SIZE];
    char* prefix;
    struct linux_dirent *d;
    char d_type;

    int i = 0;
    for (i = 1; i < argc; ++i)
    {
        if(strcmp(argv[i], "-D") == 0){
            isDebug = 1;
        }
        else if(strncmp(argv[i], "-p", 2) == 0){
            isPrefix = 1;
            prefix = &argv[i][2];
            if (strlen(prefix) > 10)
                system_call(1, 0x55, 1, 1);    
        }
        else if (strncmp(argv[i], "-a", 2) == 0){
            isPrefix = 1;
            aPrefix = 1;
            prefix = &argv[i][2];
        }
    }

    fDesc = system_call(SYS_OPEN, ".", 0, 0777);
    if(isDebug == 1){
        printStdErr(SYS_OPEN, fDesc);
        printGaps();
    }

    if (fDesc < 0){
        system_call(1, 0x55, 1, 1);
    }

    for( ; ; ){
        nread = system_call(SYS_GETDENTS, fDesc, buf, BUF_SIZE);
        if (nread < 0)
            system_call(1, 0x55, 1, 1);
        if (nread == 0)
            break;
        for(bpos = 0; bpos < nread; bpos += d->d_reclen){
            d = (struct linux_dirent *) (buf + bpos);
            d_type = *(buf + bpos + d->d_reclen - 1);
            if(isPrefix == 1 && strncmp(prefix, d->d_name, strlen(prefix)) == 0){
                outFile = system_call(SYS_WRITE, STDOUT, d->d_name, strlen(d->d_name));
                if (outFile < 0)
                    system_call(1, 0x55, 1, 1);
                typeChecker(d_type);
                system_call(SYS_WRITE, STDOUT, "\n", 1);
                if(isDebug == 1){
                    printStdErr(SYS_GETDENTS, nread);
                    printStdErr(SYS_WRITE, outFile);
                    printErrDirName(d->d_name, d->d_reclen);
                    printGaps();
                }
            }
            else if(isPrefix == 0){
                outFile = system_call(SYS_WRITE, STDOUT, d->d_name, strlen(d->d_name));
                if (outFile < 0)
                    system_call(1, 0x55, 1, 1);
                typeChecker(d_type);
                if(isDebug == 1){
                    printStdErr(SYS_GETDENTS, nread);
                    printStdErr(SYS_WRITE, outFile);
                    printErrDirName(d->d_name, d->d_reclen);
                    printGaps();
                }
            }
            if(aPrefix == 1 && isPrefix == 1 && strncmp(prefix, d->d_name, strlen(prefix)) == 0){
                infector("in");
                if(isDebug == 1){
                    printStdErr(SYS_GETDENTS, nread);
                    printStdErr(SYS_WRITE, outFile);
                    printErrDirName(d->d_name, d->d_reclen);
                    printGaps();
                }
            }
            if (isDebug == 0 && isPrefix == 0){
                system_call(SYS_WRITE, STDOUT, "\n", 1);
            }
        }
    }
    return 0;
}

