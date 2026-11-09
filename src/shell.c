/* Adapted from the group's May 2025 fork/exec/wait command interpreter. */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CMD_LEN 1024
#define MAX_ARGS 64

int main(void) {
    char line[MAX_CMD_LEN];
    while (1) {
        printf("%% ");
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            return ferror(stdin) ? 1 : 0;
        }
        if (!strchr(line, '\n') && strlen(line) == sizeof(line) - 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            fprintf(stderr, "Command line is too long.\n");
            continue;
        }
        char *args[MAX_ARGS], *state = NULL;
        int count = 0;
        char *token = strtok_r(line, " \t\r\n", &state);
        while (token && count < MAX_ARGS - 1) {
            args[count++] = token;
            token = strtok_r(NULL, " \t\r\n", &state);
        }
        if (token) {
            fprintf(stderr, "Too many command arguments.\n");
            continue;
        }
        args[count] = NULL;
        if (!count) continue;
        if (!strcmp(args[0], "exit") && count == 1) return 0;
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            continue;
        }
        if (pid == 0) {
            execvp(args[0], args);
            perror("Could not execute command");
            _exit(127);
        }
        int status;
        pid_t result;
        do { result = waitpid(pid, &status, 0); } while (result < 0 && errno == EINTR);
        if (result < 0) perror("waitpid failed");
        else if (WIFEXITED(status))
            printf("Command %s exited with status %d\n", args[0], WEXITSTATUS(status));
        else
            printf("Command %s terminated abnormally\n", args[0]);
    }
}
