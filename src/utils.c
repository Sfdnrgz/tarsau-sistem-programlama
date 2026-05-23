#include "tarsau.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

const char *get_base_name(const char *path) {
    const char *p = strrchr(path, '/');
    return p ? p + 1 : path;
}

int has_sau_extension(const char *name) {
    size_t len = strlen(name);
    if (len < 5) return 0;
    return strcmp(name + len - 4, ".sau") == 0;
}

int make_directory_if_needed(const char *dir) {
    struct stat st;

    if (dir == NULL || strlen(dir) == 0 || strcmp(dir, ".") == 0) {
        return 0;
    }

    if (stat(dir, &st) == 0) {
        return S_ISDIR(st.st_mode) ? 0 : -1;
    }

    if (mkdir(dir, 0755) == -1) {
        return -1;
    }
    return 0;
}

/* ASCII metin kontrolu: 1 baytlik ASCII karakterler kabul edilir. */
int is_ascii_text_file(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) return 0;

    unsigned char buffer[4096];
    ssize_t r;
    while ((r = read(fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < r; i++) {
            unsigned char c = buffer[i];
            if (c == 9 || c == 10 || c == 13) continue;       /* tab, LF, CR */
            if (c < 32 || c > 126) {
                close(fd);
                return 0;
            }
        }
    }

    close(fd);
    return r == 0;
}

int safe_copy_n_bytes(int in_fd, int out_fd, off_t n) {
    char buffer[8192];
    off_t remaining = n;

    while (remaining > 0) {
        ssize_t want = remaining > (off_t)sizeof(buffer) ? (ssize_t)sizeof(buffer) : (ssize_t)remaining;
        ssize_t r = read(in_fd, buffer, want);
        if (r <= 0) return -1;

        ssize_t written_total = 0;
        while (written_total < r) {
            ssize_t w = write(out_fd, buffer + written_total, (size_t)(r - written_total));
            if (w <= 0) return -1;
            written_total += w;
        }
        remaining -= r;
    }
    return 0;
}

// Utility functions updated for file validation
