#include "utils.h"

#include <stdio.h>
#include <time.h>

void printWithTime(const char *message) {
    time_t now;
    struct tm local_tm;
    char time_buf[32];

    time(&now);
    localtime_r(&now, &local_tm);
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", &local_tm);

    printf("[%s] %s", time_buf, message);
    fflush(stdout);
}
