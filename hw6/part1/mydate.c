#include<sys/types.h>
#include<time.h>
#include<stdio.h>
#include<stdlib.h>

int main()
{
    time_t t;
    struct tm *tmptr;
    char strtm[1024];

    time(&t);
    tmptr = localtime(&t);
    
    strftime(strtm, 1024, "%b %d(%a), %Y", tmptr);

    switch(tmptr->tm_hour)
    {
        case 0 ... 11:
            printf("%s %d:%d AM\n", strtm, tmptr->tm_hour, tmptr->tm_min);
            break;
        case 12:
            printf("%s %d:%d PM\n", strtm, tmptr->tm_hour, tmptr->tm_min);
            break;
        case 13 ... 23:
            printf("%s %d:%d PM\n", strtm, (tmptr->tm_hour) - 12, tmptr->tm_min);
            break;
    }

}
