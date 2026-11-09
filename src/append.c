#include <sys/stat.h>
#include "io.h"
/* append.c — adapted from the group's May 2025 Operating Systems project. */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    int fd_src, fd_dest;
    ssize_t bytes;
    char buffer[BUF_SIZE];

    if (argc != 3) {
        write(2, "Usage: os-append <source> <destination>\n", sizeof("Usage: os-append <source> <destination>\n") - 1);
        exit(1);
    }

    fd_src = open(argv[1], O_RDONLY);
    if (fd_src == -1) {
        dprintf(2, "Could not open source: %s\n", strerror(errno));
        exit(2);
    }

    fd_dest = open(argv[2], O_WRONLY | O_APPEND);
    if (fd_dest == -1) {
        dprintf(2, "Could not open destination: %s\n", strerror(errno));
        close(fd_src);
        exit(3);
    }

    struct stat source_info, destination_info;
    if (fstat(fd_src, &source_info) < 0 || fstat(fd_dest, &destination_info) < 0 ||
        (source_info.st_dev == destination_info.st_dev &&
         source_info.st_ino == destination_info.st_ino)) {
        fprintf(stderr, "Source and destination must be distinct readable files.\n");
        close(fd_src);
        close(fd_dest);
        return 3;
    }
    while ((bytes = read_retry(fd_src, buffer, BUF_SIZE)) > 0) {
        if (write_all(fd_dest, buffer, (size_t)bytes) < 0) {
            dprintf(2, "Could not write destination\n");
            close(fd_src);
            close(fd_dest);
            exit(4);
        }
    }

    int failed = bytes < 0;
    if (failed) perror("Could not read source");
    if (close(fd_src) < 0) failed = 1;
    if (close(fd_dest) < 0) failed = 1;
    return failed ? 4 : 0;
}
