#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define PATH_SIZE 4096

int main()
{
    char* cwd;
    if ((cwd = getcwd(NULL, 0)) != NULL) {
        printf("%s\n", cwd);
        free(cwd);
    } else {
        perror("getcwd");
        return 1;
    }
    return 0;
}