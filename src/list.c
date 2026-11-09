/* list.c — adapted from the group's May 2025 Operating Systems project. */
#include <unistd.h> 
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    DIR *d;
    struct dirent *dir;
    struct stat st;
    char path[512];
    char *folder = ".";

    if (argc == 2) {
        folder = argv[1];
    } else if (argc > 2) {
        write(2, "Usage: os-list [directory]\n", sizeof("Usage: os-list [directory]\n") - 1);
        exit(1);
    }

    d = opendir(folder);
    if (!d) {
        perror("Could not open directory");
        exit(2);
    }

    while ((dir = readdir(d)) != NULL) {
        if (snprintf(path, sizeof(path), "%s/%s", folder, dir->d_name) >= (int)sizeof(path)) {
            fprintf(stderr, "Directory entry path is too long.\n");
            closedir(d);
            return 3;
        }
        if (stat(path, &st) == 0) {
            if (S_ISDIR(st.st_mode))
                printf("%s\t[directory]\n", dir->d_name);
            else if (S_ISREG(st.st_mode))
                printf("%s\t[file]\n", dir->d_name);
        }
    }

    closedir(d);
    return 0;
}

