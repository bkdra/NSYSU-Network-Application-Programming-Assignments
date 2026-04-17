#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int parent_exists(pid_t ppid)
{
    char path[64];

    if (ppid <= 0)
        return 0;

    snprintf(path, sizeof(path), "/proc/%ld", (long)ppid);
    return access(path, F_OK) == 0;
}

int main(void)
{
    int all_passed = 1;
    pid_t my_parent = getppid();

    struct {
        const char *name;
        pid_t ppid;
        int expected;
    } tests[] = {
        {"PID 0 is invalid", 0, 0},
        {"Negative PID is invalid", -1, 0},
        {"init/systemd PID 1 should exist", 1, 1},
        {"Current process parent should exist", my_parent, 1},
        {"Very large PID should not exist", 999999, 0}
    };

    size_t ntests = sizeof(tests) / sizeof(tests[0]);

    for (size_t i = 0; i < ntests; i++) {
        int got = parent_exists(tests[i].ppid);
        const char *result = (got == tests[i].expected) ? "PASS" : "FAIL";

        printf("[%s] %-35s ppid=%ld expected=%d got=%d\n",
               result,
               tests[i].name,
               (long)tests[i].ppid,
               tests[i].expected,
               got);

        if (got != tests[i].expected)
            all_passed = 0;
    }

    if (!all_passed) {
        fprintf(stderr, "Some tests failed.\n");
        return EXIT_FAILURE;
    }

    printf("All tests passed.\n");
    return EXIT_SUCCESS;
}
