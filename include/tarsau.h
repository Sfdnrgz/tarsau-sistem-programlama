#ifndef TARSAU_H
#define TARSAU_H

#include <sys/types.h>
#include <sys/stat.h>

#define MAX_FILES 32
#define MAX_TOTAL_SIZE (200LL * 1024LL * 1024LL)
#define DEFAULT_ARCHIVE "a.sau"
#define HEADER_SIZE 10

/* Her dosya icin arsiv icinde tutulacak bilgiler */
typedef struct {
    char name[256];
    char source_path[512];
    mode_t permissions;
    off_t size;
} FileInfo;

int create_archive(int file_count, char **files, const char *archive_name);
int extract_archive(const char *archive_name, const char *output_dir);

int is_ascii_text_file(const char *path);
int has_sau_extension(const char *name);
const char *get_base_name(const char *path);
int make_directory_if_needed(const char *dir);
int safe_copy_n_bytes(int in_fd, int out_fd, off_t n);

#endif
