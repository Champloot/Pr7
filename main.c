#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>  // Добавлено для isalnum()

void process_file(const char *file_path, const char *word) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        fprintf(stderr, "Error opening file %s: %s\n", file_path, strerror(errno));
        return;
    }

    char line[1024];
    int line_number = 0;
    size_t word_len = strlen(word);

    while (fgets(line, sizeof(line), file)) {
        line_number++;
        char *found = line;
        int found_in_line = 0;

        while ((found = strstr(found, word)) != NULL) {
            // Проверка границ слова
            int start_ok = (found == line) || !isalnum((unsigned char)*(found - 1);
            int end_ok = !isalnum((unsigned char)*(found + word_len));

            if (start_ok && end_ok) {
                found_in_line = 1;
                break;
            }
            found++;
        }

        if (found_in_line) {
            printf("%s:%d:%s", file_path, line_number, line);
        }
    }

    fclose(file);
}

char *expand_path(const char *path) {
    if (path[0] == '~' && (path[1] == '/' || path[1] == '\0')) {
        const char *home = getenv("HOME");
        if (!home) {
            fprintf(stderr, "HOME environment variable not set\n");
            return NULL;
        }

        size_t len = strlen(home) + strlen(path + 1) + 1;
        char *expanded = malloc(len);
        if (!expanded) {
            fprintf(stderr, "Memory allocation failed\n");
            return NULL;
        }

        snprintf(expanded, len, "%s%s", home, path + 1);
        return expanded;
    } else {
        return strdup(path);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s [directory] <word>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *dir_arg = (argc == 3) ? argv[1] : "~/files";
    const char *word = (argc == 3) ? argv[2] : argv[1];

    char *expanded_dir = expand_path(dir_arg);
    if (!expanded_dir) {
        return EXIT_FAILURE;
    }

    traverse_directory(expanded_dir, word);

    free(expanded_dir);
    return EXIT_SUCCESS;
}
