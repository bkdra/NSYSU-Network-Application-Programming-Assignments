#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main()
{
    char* str = strdup("test");
    int num = 123;
    char num_str[10];
    snprintf(num_str, sizeof(num_str), "%d", num);
    strcat(str, num_str);

    printf("%s\n", str);
    free(str);
}