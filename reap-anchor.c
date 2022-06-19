#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <sysexits.h>

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s command [ args ]\n", argv[0]);
        return EX_USAGE;
    }

    unsigned long subreaper = 1;
    if (prctl(PR_SET_CHILD_SUBREAPER, &subreaper, 0, 0, 0) == -1) {
        perror("prctl(PR_SET_CHILD_SUBREAPER)");
        return EX_OSERR;
    }

    pid_t child = fork();
    if (child == -1) {
        perror("fork");
        return EX_OSERR;
    }
    if (child == 0) {
        execvp(argv[1], &argv[1]);
        fprintf(stderr, "exec(%s): %s\n", argv[1], strerror(errno));
        return EX_OSERR;
    }

    int child_status = EX_SOFTWARE;
    for (;;) {
        int wstatus;
        pid_t pid = wait(&wstatus);
        if (pid == -1) {
            if (errno == ECHILD) {
                break;
            }
            perror("wait");
            return EX_OSERR;
        }
        if (pid == child) {
            child_status = WEXITSTATUS(wstatus);
        }
    }

    return child_status;
}
