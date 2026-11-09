#include <sys/stat.h>
#include "io.h"
/* copy.c — adapted from the group's May 2025 Operating Systems project. */
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
    char dest_name[256];

    if (argc != 2) {
        write(2, "Usage: os-copy <file>\n", sizeof("Usage: os-copy <file>\n") - 1);
        exit(1);
    }

    if (snprintf(dest_name, sizeof(dest_name), "%s.copy", argv[1]) >= (int)sizeof(dest_name)) {
        fprintf(stderr, "Destination path is too long.\n");
        return 1;
    }

    fd_src = open(argv[1], O_RDONLY);
    if (fd_src == -1) {
        dprintf(2, "Could not open source file: %s\n", strerror(errno));
        exit(2);
    }

    fd_dest = open(dest_name, O_WRONLY | O_CREAT, 0644);
    if (fd_dest == -1) {
        dprintf(2, "Could not create destination file: %s\n", strerror(errno));
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
    if (ftruncate(fd_dest, 0) < 0) {
        perror("Could not truncate destination");
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
