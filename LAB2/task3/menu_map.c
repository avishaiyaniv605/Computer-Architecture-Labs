#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char censor(char c)
{
    if (c == '!')
        return '.';
    else
        return c;
}

char encrypt(char c)
{
    if (c < 0x20 || c > 0x7E)
        return c;
    return c + 3;
}

char decrypt(char c)
{
    if (c < 0x20 || c > 0x7E)
        return c;
    return c - 3;
}

char xprt(char c)
{
    fprintf(stdout, "%#02x\n", c);
    return c;
}

char cprt(char c)
{
    if (c >= 0x20 && c <= 0x7E)
        fprintf(stdout, "%c\n", c);
    else
        printf(".\n");
    return c;
}

char my_get(char c)
{
    return (char)fgetc(stdin);
}

char quit(char c)
{
    exit(0);
}

struct fun_desc
{
    char *name;
    char (*fun)(char);
};

char *map(char *array, int array_length, char (*f)(char))
{
    char *mapped_array = (char *)(malloc(array_length * sizeof(char)));
    for (size_t i = 0; i < array_length; i++)
    {
        mapped_array[i] = f(array[i]);
    }
    return mapped_array;
}

int main(int argc, char **argv)
{
    int base_len = 5, i = 0, sizeOfMenu = 0, numInput = -1;
    char *carray = (char *)(malloc(base_len * sizeof(char)));
    strcpy(carray, "");
    char input[256];
    struct fun_desc menu[] = {
        {"Censor", censor},
        {"Encrypt", encrypt},
        {"Decrypt", decrypt},
        {"Print hex", xprt},
        {"Print String", cprt},
        {"Get string", my_get},
        {"quit", quit},
        {NULL, NULL}
    };
    while(menu[sizeOfMenu].name != NULL)
        sizeOfMenu++;

    while (1)
    {
        fprintf(stdout, "Please choose a function: \n");
        for (i = 0; i < sizeOfMenu; i++)
            fprintf(stdout, "%i) %s\n", i, menu[i].name);
        fgets(input, 256, stdin);
        int res = sscanf(input,"%d", &numInput);
        if(res == EOF || input == NULL)
        {
            fprintf(stdout, "Never program again. you suck!\n");
            break;
        }
        if (numInput >= 0 && numInput <= sizeOfMenu - 1)
        {
            fprintf(stdout, "Within bounds \nOption: %d\n", numInput);
            carray = map(carray, base_len, menu[numInput].fun);
            fprintf(stdout, "Done.\n\n");
        }
        else
        {
            fprintf(stdout, "Not within bounds\n");
            break;
        }
    }
    free(carray);
}