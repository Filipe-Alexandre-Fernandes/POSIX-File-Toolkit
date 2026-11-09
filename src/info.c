/* info.c — adapted from the group's May 2025 Operating Systems project. */
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

void print_info(struct stat *info) {
    char buffer[256];
    char date_string[100];
    struct tm *tm;

    
    if (S_ISREG(info->st_mode)) {
        write(STDOUT_FILENO, "Type: Regular file\n", sizeof("Type: Regular file\n") - 1);
    } else if (S_ISDIR(info->st_mode)) {
        write(STDOUT_FILENO, "Type: Directory\n", sizeof("Type: Directory\n") - 1);
    } else if (S_ISLNK(info->st_mode)) {
        write(STDOUT_FILENO, "Type: Symbolic link\n", sizeof("Type: Symbolic link\n") - 1);
    } else {
        write(STDOUT_FILENO, "Type: Other\n", sizeof("Type: Other\n") - 1);
    }

    
    snprintf(buffer, sizeof(buffer), "i-node: %ld\n", (long)info->st_ino);
    write(STDOUT_FILENO, buffer, strlen(buffer));

    
    snprintf(buffer, sizeof(buffer), "Owner UID: %d\n", info->st_uid);
    write(STDOUT_FILENO, buffer, strlen(buffer));

    
    tm = localtime(&info->st_ctime);
    strftime(date_string, sizeof(date_string), "%Y-%m-%d %H:%M:%S", tm);
    snprintf(buffer, sizeof(buffer), "Status changed: %s\n", date_string);
    write(STDOUT_FILENO, buffer, strlen(buffer));

    
    tm = localtime(&info->st_atime);
    strftime(date_string, sizeof(date_string), "%Y-%m-%d %H:%M:%S", tm);
    snprintf(buffer, sizeof(buffer), "Last accessed: %s\n", date_string);
    write(STDOUT_FILENO, buffer, strlen(buffer));

    
    tm = localtime(&info->st_mtime);
    strftime(date_string, sizeof(date_string), "%Y-%m-%d %H:%M:%S", tm);
    snprintf(buffer, sizeof(buffer), "Last modified: %s\n", date_string);
    write(STDOUT_FILENO, buffer, strlen(buffer));
}

int file_info(const char *filename) {
    struct stat info;

    if (lstat(filename, &info) < 0) {
        write(STDERR_FILENO, "Could not stat file\n", sizeof("Could not stat file\n") - 1);
        return 1;
    }

    print_info(&info);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        write(STDERR_FILENO, "Usage: os-info <file>\n", sizeof("Usage: os-info <file>\n") - 1);
        return 1;
    }
    return file_info(argv[1]);
}
