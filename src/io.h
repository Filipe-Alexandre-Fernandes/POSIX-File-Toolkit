#ifndef TOOLKIT_IO_H
#define TOOLKIT_IO_H
#include <errno.h>
#include <unistd.h>

/* Retry interrupted reads and complete short writes. */
static inline ssize_t read_retry(int fd, void *buffer, size_t size) {
    ssize_t result;
    do { result = read(fd, buffer, size); } while (result < 0 && errno == EINTR);
    return result;
}
static inline int write_all(int fd, const void *buffer, size_t size) {
    const char *cursor = buffer;
    while (size > 0) {
        ssize_t written = write(fd, cursor, size);
        if (written < 0 && errno == EINTR) continue;
        if (written <= 0) return -1;
        cursor += written;
        size -= (size_t)written;
    }
    return 0;
}
#endif
