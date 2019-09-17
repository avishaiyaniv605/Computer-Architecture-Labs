#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "LineParser.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

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
  int pid, wait, wstatus;
  if(strcmp(pCmdLine->arguments[0], "quit") == 0){ //quit command
	if(isDebug == 1){
	   fprintf(stderr, "fork\n %d \n", pid);
	   fprintf(stderr, "execvp\n %d \n", pid);
  	}
    shouldTerminate = 1;                //break while loop
    freeCmdLines(pCmdLine);
    exit(0);
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

  if(pid < 0 || (pid == 0 && execvp(pCmdLine->arguments[0], pCmdLine->arguments) < 0)){ //if fork fails or its the son procces and! execution fails
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
