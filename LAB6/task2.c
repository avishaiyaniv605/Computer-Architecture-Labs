#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h> 
#include <fcntl.h>

int isDebug = 0;
int fd[2];

int main(int argc, char **argv) {   
    int i, pid1 = 0, pid2 = 0;
    for (i = 0; i < argc; ++i){
        if(strncmp(argv[i], "-d", 2) == 0)
            isDebug = 1;
    }  
    pipe(fd);
    pid1 = fork();
    if(pid1 < 0){
        exit(0);
    }

    if(pid1 == 0){ //first child
        fprintf(stderr, "%s","(child1>redirecting stdout to the write end of the pipe…)"); 
        close(1);
        dup(fd[1]);
        close(fd[1]);
        char* const ls[] = {"ls", "-l", NULL};
        fprintf(stderr, "%s", "(child1>going to execute cmd: …)"); 
         
        execvp(ls[0], ls);
        return 0;
    }
    else if (pid1 > 0){ // parent
        int s1, s2;
        fprintf(stderr, "%s", "(parent_process>waiting for child processes to terminate…)"); 
        waitpid(pid1, &s2, WUNTRACED);
        fprintf(stderr, "%s", "(parent_process>closing the write end of the pipe…)\n"); 
        close(fd[1]);

        fprintf(stderr, "%s", "(parent_process>forking…)\n"); 
        pid2 = fork();
        fprintf(stderr, "%s%d", "(parent_process>created process with id: )", getpid()); 
        fprintf(stderr, "%s", "\n"); 

        
        if(pid2 < 0)
            exit(0);

        else if(pid2 == 0){ //second child
            fprintf(stderr, "%s%d", "(child2>redirecting stdin to the read end of the pipe…)"); 
            close(0);
            dup(fd[0]);  
            close(fd[0]);
            char* const tail[] = {"tail", "-n", "2", NULL};
            fprintf(stderr, "%s%d", "(child2>going to execute cmd: …)"); 
            execvp(tail[0], tail);
            return 0;
        }
        else if(pid2 > 0){ // parent
            fprintf(stderr, "%s", "(parent_process>waiting for child processes to terminate…)"); 
            waitpid(pid2,&s2, WUNTRACED);
            fprintf(stderr, "%s", "(parent_process>closing the read end of the pipe…)"); 
            close(fd[0]);
            fprintf(stderr, "%s", "(parent_process>exiting…)"); 
        }
    }
}
