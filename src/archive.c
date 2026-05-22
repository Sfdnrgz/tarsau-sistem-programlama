#include "tarsau.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int create_archive(int file_count, char **files, const char *archive_name) {
    if (file_count <= 0 || file_count > MAX_FILES) {
        fprintf(stderr, "Giris dosyasi sayisi 1 ile 32 arasinda olmalidir!\n");
        return 1;
    }

    FileInfo infos[MAX_FILES];
    off_t total_size = 0;

    for (int i = 0; i < file_count; i++) {
        struct stat st;

        if (stat(files[i], &st) == -1 || !S_ISREG(st.st_mode)) {
            fprintf(stderr, "%s giris dosyasinin formati uyumsuzdur!\n", files[i]);
            return 1;
        }

        if (!is_ascii_text_file(files[i])) {
            fprintf(stderr, "%s giris dosyasinin formati uyumsuzdur!\n", files[i]);
            return 1;
        }

        total_size += st.st_size;
        if (total_size > MAX_TOTAL_SIZE) {
            fprintf(stderr, "Giris dosyalarinin toplam boyutu 200 MB'i gecemez!\n");
            return 1;
        }

        memset(&infos[i], 0, sizeof(FileInfo));
        snprintf(infos[i].name, sizeof(infos[i].name), "%s", get_base_name(files[i]));
        snprintf(infos[i].source_path, sizeof(infos[i].source_path), "%s", files[i]);
        infos[i].permissions = st.st_mode & 0777;
        infos[i].size = st.st_size;
    }

    char metadata[16384];
    metadata[0] = '\0';

    for (int i = 0; i < file_count; i++) {
        char record[512];
        snprintf(record, sizeof(record), "|%s,%o,%lld|", infos[i].name,
                 infos[i].permissions, (long long)infos[i].size);
        if (strlen(metadata) + strlen(record) >= sizeof(metadata)) {
            fprintf(stderr, "Organizasyon bilgisi cok buyuk!\n");
            return 1;
        }
        strcat(metadata, record);
    }

    int out_fd = open(archive_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd < 0) {
        perror("Arsiv dosyasi olusturulamadi");
        return 1;
    }

    char header[HEADER_SIZE + 1];
    long metadata_len = (long)strlen(metadata);
    snprintf(header, sizeof(header), "%010ld", metadata_len);

    if (write(out_fd, header, HEADER_SIZE) != HEADER_SIZE ||
        write(out_fd, metadata, strlen(metadata)) != (ssize_t)strlen(metadata)) {
        perror("Arsiv basligi yazilamadi");
        close(out_fd);
        return 1;
    }

    for (int i = 0; i < file_count; i++) {
        int in_fd = open(infos[i].source_path, O_RDONLY);
        if (in_fd < 0) {
            perror("Giris dosyasi acilamadi");
            close(out_fd);
            return 1;
        }

        if (safe_copy_n_bytes(in_fd, out_fd, infos[i].size) == -1) {
            perror("Dosya arsive kopyalanamadi");
            close(in_fd);
            close(out_fd);
            return 1;
        }
        close(in_fd);
    }

    close(out_fd);
    printf("Dosyalar birlestirildi.\n");
    return 0;
}
