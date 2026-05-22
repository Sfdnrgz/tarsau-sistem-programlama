#include "tarsau.h"

#include <stdio.h>
#include <string.h>

static void print_usage(void) {
    printf("Kullanim:\n");
    printf("  tarsau -b dosya1 dosya2 ... [-o arsiv.sau]\n");
    printf("  tarsau -a arsiv.sau [dizin]\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    if (strcmp(argv[1], "-b") == 0) {
        char *files[MAX_FILES];
        int file_count = 0;
        const char *archive_name = DEFAULT_ARCHIVE;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-o") == 0) {
                if (i + 1 >= argc) {
                    print_usage();
                    return 1;
                }
                archive_name = argv[++i];
            } else {
                if (file_count >= MAX_FILES) {
                    fprintf(stderr, "Giris dosyasi sayisi en fazla 32 olabilir!\n");
                    return 1;
                }
                files[file_count++] = argv[i];
            }
        }

        if (file_count == 0) {
            print_usage();
            return 1;
        }
        return create_archive(file_count, files, archive_name);
    }

    if (strcmp(argv[1], "-a") == 0) {
        if (argc < 3 || argc > 4) {
            print_usage();
            return 1;
        }
        const char *archive_name = argv[2];
        const char *output_dir = (argc == 4) ? argv[3] : ".";
        return extract_archive(archive_name, output_dir);
    }

    print_usage();
    return 1;
}
