/* remove.c — adapted from the group's May 2025 Operating Systems project. */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        write(2, "Usage: os-remove <file>\n", sizeof("Usage: os-remove <file>\n") - 1);
        exit(1);
    }

    if (unlink(argv[1]) == -1) {
        dprintf(2, "Could not remove file: %s\n", strerror(errno));
        exit(2);
    }

    write(1, "File removed successfully\n", sizeof("File removed successfully\n") - 1);
    return 0;
}
