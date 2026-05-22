#include "tarsau.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>

static int read_exact(int fd, char *buffer, size_t n) {
    size_t total = 0;
    while (total < n) {
        ssize_t r = read(fd, buffer + total, n - total);
        if (r <= 0) return -1;
        total += (size_t)r;
    }
    return 0;
}

static int parse_metadata(char *metadata, FileInfo *infos, int *count) {
    int idx = 0;
    char *p = metadata;

    while (*p != '\0') {
        if (*p != '|') return -1;
        p++;

        char *end = strchr(p, '|');
        if (!end) return -1;
        *end = '\0';

        char *name = strtok(p, ",");
        char *perm = strtok(NULL, ",");
        char *size = strtok(NULL, ",");
        char *extra = strtok(NULL, ",");

        if (!name || !perm || !size || extra || idx >= MAX_FILES) return -1;
        if (strstr(name, "../") || strchr(name, '/')) return -1;

        memset(&infos[idx], 0, sizeof(FileInfo));
        snprintf(infos[idx].name, sizeof(infos[idx].name), "%s", name);
        infos[idx].permissions = (mode_t)strtol(perm, NULL, 8);
        infos[idx].size = (off_t)atoll(size);
        if (infos[idx].size < 0) return -1;

        idx++;
        p = end + 1;
    }

    *count = idx;
    return idx > 0 ? 0 : -1;
}

int extract_archive(const char *archive_name, const char *output_dir) {
    if (!has_sau_extension(archive_name)) {
        fprintf(stderr, "Arsiv dosyasi uygunsuz veya bozuk!\n");
        return 1;
    }

    int in_fd = open(archive_name, O_RDONLY);
    if (in_fd < 0) {
        fprintf(stderr, "Arsiv dosyasi uygunsuz veya bozuk!\n");
        return 1;
    }

    char header[HEADER_SIZE + 1];
    if (read_exact(in_fd, header, HEADER_SIZE) == -1) {
        fprintf(stderr, "Arsiv dosyasi uygunsuz veya bozuk!\n");
        close(in_fd);
        return 1;
    }
    header[HEADER_SIZE] = '\0';

    for (int i = 0; i < HEADER_SIZE; i++) {
        if (!isdigit((unsigned char)header[i])) {
            fprintf(stderr, "Arsiv dosyasi uygunsuz veya bozuk!\n");
            close(in_fd);
            return 1;
        }
    }

    long metadata_size = atol(header);
    if (metadata_size <= 0 || metadata_size > 16384) {
        fprintf(stderr, "Arsiv dosyasi uygunsuz veya bozuk!\n");
        close(in_fd);
        return 1;
    }

    char *metadata = (char *)malloc((size_t)metadata_size + 1);
    if (!metadata) {
        close(in_fd);
        return 1;
    }

    if (read_exact(in_fd, metadata, (size_t)metadata_size) == -1) {
        fprintf(stderr, "Arsiv dosyasi uygunsuz veya bozuk!\n");
        free(metadata);
        close(in_fd);
        return 1;
    }
    metadata[metadata_size] = '\0';

    FileInfo infos[MAX_FILES];
    int file_count = 0;
    if (parse_metadata(metadata, infos, &file_count) == -1) {
        fprintf(stderr, "Arsiv dosyasi uygunsuz veya bozuk!\n");
        free(metadata);
        close(in_fd);
        return 1;
    }
    free(metadata);

    const char *dir = output_dir ? output_dir : ".";
    if (make_directory_if_needed(dir) == -1) {
        perror("Dizin olusturulamadi");
        close(in_fd);
        return 1;
    }

    for (int i = 0; i < file_count; i++) {
        char out_path[768];
        if (strcmp(dir, ".") == 0) {
            snprintf(out_path, sizeof(out_path), "%s", infos[i].name);
        } else {
            snprintf(out_path, sizeof(out_path), "%s/%s", dir, infos[i].name);
        }

        int out_fd = open(out_path, O_WRONLY | O_CREAT | O_TRUNC, infos[i].permissions);
        if (out_fd < 0) {
            perror("Cikis dosyasi olusturulamadi");
            close(in_fd);
            return 1;
        }

        if (safe_copy_n_bytes(in_fd, out_fd, infos[i].size) == -1) {
            fprintf(stderr, "Arsiv dosyasi uygunsuz veya bozuk!\n");
            close(out_fd);
            close(in_fd);
            return 1;
        }

        close(out_fd);
        chmod(out_path, infos[i].permissions);
    }

    close(in_fd);

    if (strcmp(dir, ".") == 0) {
        printf("Dosyalar gecerli dizinde acildi.\n");
    } else {
        printf("%s dizininde dosyalar acildi.\n", dir);
    }
    return 0;
}
