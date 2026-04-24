#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include "popen_close.h"

void test_read_mode() 
{
    printf("=== Test 1: mode = r ===\n");
    FILE *fp = mypopen("echo hello_from_child", "r");
    if (fp == NULL) {
        perror("mypopen(r) failed");
        return;
    }

    char buf[256];
    if (fgets(buf, sizeof(buf), fp) != NULL) {
        printf("Parent received: %s", buf);
    } else {
        printf("No data read from child\n");
    }

    int st = mypclose(fp);
    printf("mypclose(r) returned: %d\n\n", st);
}

void test_write_mode() 
{
    printf("=== Test 2: mode = w ===\n");
    printf("Expected child output: ABC XYZ\n");

    FILE *fp = mypopen("tr a-z A-Z", "w");
    if (fp == NULL) {
        perror("mypopen(w) failed");
        return;
    }

    fprintf(fp, "abc xyz\n");
    fflush(fp);

    int st = mypclose(fp);
    printf("mypclose(w) returned: %d\n\n", st);
}

void test_multiple_children() 
{
    printf("=== Test 3: multiple mypopen children ===\n");

    FILE *a = mypopen("sleep 1; echo first_done", "r");
    FILE *b = mypopen("sleep 2; echo second_done", "r");

    if (a == NULL || b == NULL) {
        perror("mypopen(multi) failed");
        if (a) mypclose(a);
        if (b) mypclose(b);
        return;
    }

    char buf[256];

    if (fgets(buf, sizeof(buf), a)) {
        printf("A says: %s", buf);
    }
    if (fgets(buf, sizeof(buf), b)) {
        printf("B says: %s", buf);
    }

    int sa = mypclose(a);
    int sb = mypclose(b);

    printf("mypclose(A) = %d, mypclose(B) = %d\n\n", sa, sb);
}

int main(void) {
    test_read_mode();
    test_write_mode();
    test_multiple_children();
    return 0;
}