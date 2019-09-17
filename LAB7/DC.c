#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>


int digit_cnt(char* str);

int main(int argc, char **argv){
	return 0;
}

int digit_cnt(char* str){
    int counter = 0;
    for (size_t i = 0; str[i] != '\0' ; i++){
       if(*(str + i) <= '9' && *(str + i) >= '0'){
           counter++;
       }
    }
    return counter;
}
