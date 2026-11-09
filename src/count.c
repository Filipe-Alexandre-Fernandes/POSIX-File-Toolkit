#include "io.h"
/* count.c — adapted from the group's May 2025 Operating Systems project. */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    int fd;
    ssize_t bytes;
    char buffer[BUF_SIZE];
    int lines = 0;

    if (argc != 2) {
        write(2, "Usage: os-count <file>\n", sizeof("Usage: os-count <file>\n") - 1);
        exit(1);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Could not open file");
        exit(2);
    }

    while ((bytes = read_retry(fd, buffer, BUF_SIZE)) > 0) {
        for (ssize_t i = 0; i < bytes; i++) {
            if (buffer[i] == '\n') lines++;
        }
    }

    if (bytes < 0) {
        perror("Could not read file");
        close(fd);
        return 3;
    }
    dprintf(1, "Newline count: %d\n", lines);

    close(fd);
    return 0;
}
