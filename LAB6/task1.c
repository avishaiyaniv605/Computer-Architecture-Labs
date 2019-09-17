#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "LineParser.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h> 
#include <fcntl.h>

#define INPUT_MAX 2048
int isDebug = 0, shouldTerminate = 0;
int execute(cmdLine *pCmdLine);
void freeCmdLines(cmdLine *pCmdLine);
void freeCmdLines(cmdLine *pCmdLine);		

int main(int argc, char **argv){
  char inputCommand[INPUT_MAX];
  char path[PATH_MAX];
  cmdLine *pCmdLine;


  for (int i = 0; i < argc; ++i){
    if(strncmp(argv[i], "-d", 2) == 0)
      isDebug = 1;
  }

  while (!shouldTerminate){
	if(getcwd(path, PATH_MAX) == NULL)  {    // retreiving curr directory
      perror("Error: Entered invalid command (length exceeds max)");     
      break;           
    }
    printf("%s\n", path);                   // printing working directory as unix shell would do.
    fgets(inputCommand, INPUT_MAX, stdin);  //retreiving command from user.
    pCmdLine = parseCmdLines(inputCommand); // parsing command string using given method.
    if(!pCmdLine)  {                      
      perror("Error: Parsing went wrong.");  
      break;           
    }
    execute(pCmdLine);
    freeCmdLines(pCmdLine);
  }
  return 0;
}

int execute(cmdLine *pCmdLine){
  int pid, wait, wstatus, c;
  if(strcmp(pCmdLine->arguments[0], "quit") == 0){ //quit command
    if(isDebug == 1){
      fprintf(stderr, "fork\n %d \n", pid);
      fprintf(stderr, "execvp\n %d \n", pid);
    }
    shouldTerminate = 1;                //break while loop
    freeCmdLines(pCmdLine);
    exit(0);
  }

  else if(strcmp(pCmdLine->arguments[0], "cd") == 0){ //cd command
    if((c = chdir(pCmdLine->arguments[1])) < 0){
      fprintf(stderr, "chaging directoy failed \n");
    }
    if(isDebug == 1){
      fprintf(stderr, "chdir\n %d \n", pid);
    }
    return 0;
  }

  // user did not ask to quit.
  pid = fork();
  if(pid > 0 && pCmdLine->blocking == 1){
    int wait = waitpid(pid, &wstatus, WUNTRACED);
    if(wait < 0){  // error has occured
      freeCmdLines(pCmdLine);
      exit(0);
    }
    if(isDebug == 1)
     fprintf(stderr, "waitpid\n %d \n", pid);
  }

  if(pid == 0){
    int in = 0, out = 0;
    if(pCmdLine->inputRedirect){
      in = open(pCmdLine->inputRedirect, O_RDONLY, 0);
      if(in < 0){
        printf("Error loading file\n");
        freeCmdLines(pCmdLine);
        close(in);
        exit(0);
      }
      else{
	      close(0);
        dup(in);
        close(in);
	    }
    }
    if(pCmdLine->outputRedirect){
      out = open(pCmdLine->outputRedirect, O_CREAT | O_RDWR, 0777);
      if(out < 0){
        printf("Error loading file\n");
        freeCmdLines(pCmdLine);
        close(out);
        exit(0);
      }
      else{
	      close(1);
        dup(out);
        close(out);
	    }
    }
  }

  if(pid < 0 || (pid == 0 && (execvp(pCmdLine->arguments[0], pCmdLine->arguments)) < 0)){ //if fork fails or its the son procces and! execution fails
    freeCmdLines(pCmdLine);
    perror("Error: ");
    _exit(0);           
  }
  // execution and fork went seccssesfully.

  if(isDebug == 1){
     fprintf(stderr, "fork\n %d \n", pid);
     fprintf(stderr, "execvp\n %d \n", pid);
  }
  return 0;
}

void comboExecute(cmdLine *pCmdLine){
    cmdLine *next = pCmdLine->next;
    int pid1, pid2;
    int pipefd[2];
    pipe(pipefd);
    pid1 = fork();
    if(pid1 < 0){
        exit(0);
    }

    if(pid1 == 0){ //first child
        fclose(stdout);
        dup(pipefd[1]);
        close(pipefd[1]);
        execvp(pCmdLine->arguments[0], pCmdLine->arguments);
        return 0;
    }
    else if (pid1 > 0){ // parent
        int s1, s2;
        waitpid(pid2, &s2, WUNTRACED);
        close(pipefd[1]);

        pid2 = fork();
        if(pid2 < 0)
            exit(0);
        else if(pid2 == 0){ //second child
            fclose(stdin);
            dup(pipefd[0]);
            close(pipefd[0]);
            execvp(next->arguments[0], next->arguments);
            return 0;
        }
        else if(pid2 > 0){ // parent
            waitpid(pid2, &s2, WUNTRACED);
        }
    }
}
