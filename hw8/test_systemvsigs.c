#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "SystemVsigs.h"

static volatile sig_atomic_t usr1_count = 0;
static volatile sig_atomic_t alrm_count = 0;

static void usr1_handler(int sig)
{
    (void)sig;
    usr1_count++;
}

static void alrm_handler(int sig)
{
    (void)sig;
    alrm_count++;
}

static int print_result(const char *name, int ok)
{
    printf("[%s] %s\n", ok ? "PASS" : "FAIL", name);
    return ok ? 0 : 1;
}

int main(void)
{
    int failures = 0;
    sigset_t pending;

    // 1. test sigset by self-designating a handler
    sigset(SIGUSR1, SIG_DFL);
    sigrelse(SIGUSR1);

    if (sigset(SIGUSR1, usr1_handler) == SIG_ERR) {
        perror("sigset(SIGUSR1, usr1_handler)");
        return 1;
    }

    raise(SIGUSR1);
    printf("1. test sigset by self-designating a handler\n");
    failures += print_result("sigset installs handler", usr1_count == 1);
    printf("\n");

    // 2. test sighold (check that signal is blocked)
    if (sighold(SIGUSR1) == -1) {
        perror("sighold(SIGUSR1)");
        return 1;
    }

    raise(SIGUSR1);
    sigpending(&pending);
    printf("2. test sighold (check that signal is blocked)\n");
    failures += print_result("sighold blocks delivery", usr1_count == 1);
    failures += print_result("signal is pending while held", sigismember(&pending, SIGUSR1) == 1);
    printf("\n");

    // 3. test sigrelse (check that signal is unblocked and delivered)
    if (sigrelse(SIGUSR1) == -1) {
        perror("sigrelse(SIGUSR1)");
        return 1;
    }

    /* Pending SIGUSR1 should be delivered when unblocked. */
    printf("3. test sigrelse (check that signal is unblocked and delivered)\n");
    failures += print_result("sigrelse unblocks and delivers pending", usr1_count >= 2);
    printf("\n");

    // 4. test sigignore (check that signal is ignored)
    if (sigignore(SIGUSR1) == -1) {
        perror("sigignore(SIGUSR1)");
        return 1;
    }
    printf("4. test sigignore (check that signal is ignored)\n");
    {
        sig_atomic_t before = usr1_count;
        raise(SIGUSR1);
        failures += print_result("sigignore ignores signal", usr1_count == before);
    }
    printf("\n");

    // 5. test sigset with SIG_HOLD (check that signal is blocked and previous handler is returned)
    if (sigset(SIGUSR1, usr1_handler) == SIG_ERR) {
        perror("sigset(SIGUSR1, usr1_handler)");
        return 1;
    }

    printf("5. test sigset with SIG_HOLD (check that signal is blocked and previous handler is returned)\n");
    {
        void (*prev)(int);
        prev = sigset(SIGUSR1, SIG_HOLD);
        failures += print_result("sigset(SIG_HOLD) succeeds", prev != SIG_ERR);

        // The signal should be blocked by SIG_HOLD, so the handler should not be called and the count should remain the same.
        raise(SIGUSR1);
        failures += print_result("sigset(SIG_HOLD) blocks delivery", usr1_count == 2);

        // The previous handler is returned, and this signal is blocked by SIG_HOLD before, so it should return SIG_HOLD.
        prev = sigset(SIGUSR1, usr1_handler);
        failures += print_result("sigset returns SIG_HOLD if previously blocked", prev == SIG_HOLD);
    }
    printf("\n");

    // 6. test sigpause (check that it waits for signal delivery and returns -1 with EINTR)
    if (sigset(SIGALRM, alrm_handler) == SIG_ERR) {
        perror("sigset(SIGALRM, alrm_handler)");
        return 1;
    }

    if (sighold(SIGALRM) == -1) {
        perror("sighold(SIGALRM)");
        return 1;
    }

    alarm(1);
    errno = 0;
    printf("6. test sigpause (check that it waits for signal delivery and returns -1 with EINTR)\n");
    printf("waiting for SIGALRM...\n");
    if (sigpause(SIGALRM) != -1) { // means that sigpause returned without being interrupted by the signal, which is a failure.
        failures += print_result("sigpause returns -1", 0);
    } else {
        failures += print_result("sigpause interrupted with EINTR", errno == EINTR);
    }
    failures += print_result("sigpause allowed SIGALRM delivery", alrm_count == 1);
    printf("\n");

    if (failures == 0) {
        printf("\nAll tests passed.\n");
        return 0;
    }

    printf("\n%d test(s) failed.\n", failures);
    return 1;
}
