#include <stdio.h>
#include <unistd.h>
#include <sys/utsname.h>
#include<stdlib.h>

int main(void) {
    struct utsname uts;
    if(uname(&uts) == -1)
    {
        perror("myuname.c:main:uname");
        exit(1);
    }

    printf("hostname: %s\n", uts.nodename);
    printf("%s\n", uts.release);
    printf("hostid: %ld\n", gethostid());

    return 0;
}