#include "io.h"
/* show.c — adapted from the group's May 2025 Operating Systems project. */
#include <stdio.h>
#include <fcntl.h>      
#include <unistd.h>     
#include <stdlib.h>     
#include <errno.h>      
#include <string.h>     

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    int fd;
    ssize_t bytes_read;
    char buffer[BUF_SIZE];

    
    if (argc != 2) {
        write(2, "Usage: os-show <file>\n", sizeof("Usage: os-show <file>\n") - 1);
        exit(1);
    }

    
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        
        dprintf(2, "Could not open file '%s': %s\n", argv[1], strerror(errno));
        exit(2);
    }

    
    while ((bytes_read = read_retry(fd, buffer, BUF_SIZE)) > 0) {
        if (write_all(1, buffer, (size_t)bytes_read) < 0) {
            dprintf(2, "Could not write stdout\n");
            close(fd);
            exit(3);
        }
    }

    if (bytes_read < 0) {
        dprintf(2, "Could not read file '%s'\n", argv[1]);
        close(fd);
        exit(4);
    }

    close(fd);
    return 0;
}
