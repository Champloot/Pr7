#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>

// Прототипы функций
void process_file(const char *file_path, const char *word);
void traverse_directory(const char *dir_path, const char *word);
char *expand_path(const char *path);

// Обработка файла
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

        while ((found = strstr(found, word)) {
            // Проверка границ слова
            int start_ok = (found == line) || !isalnum((unsigned char)*(found - 1));
            int end_ok = (found + word_len >= line + strlen(line)) 
                        || !isalnum((unsigned char)*(found + word_len));

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

// Рекурсивный обход директорий
void traverse_directory(const char *dir_path, const char *word) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        fprintf(stderr, "Error opening directory %s: %s\n", dir_path, strerror(errno));
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat statbuf;
        if (lstat(full_path, &statbuf) {
            fprintf(stderr, "Error stating %s: %s\n", full_path, strerror(errno));
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            traverse_directory(full_path, word);
        } else if (S_ISREG(statbuf.st_mode)) {
            process_file(full_path, word);
        }
    }

    closedir(dir);
}

// Расширение пути с ~
char *expand_path(const char *path) {
    if (path[0] == '~' && (path[1] == '/' || path[1] == '\0')) {
        const char *home = getenv("HOME");
        if (!home) {
            fprintf(stderr, "HOME environment variable not set\n");
            return NULL;
        }

        char *expanded = malloc(strlen(home) + strlen(path) + 1);
        snprintf(expanded, strlen(home) + strlen(path) + 1, "%s%s", home, path + 1);
        return expanded;
    }
    return strdup(path);
}

// Главная функция
int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s [directory] <word>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *dir_arg = (argc == 3) ? argv[1] : "~/files";
    const char *word = (argc == 3) ? argv[2] : argv[1];

    char *expanded_dir = expand_path(dir_arg);
    if (!expanded_dir) return EXIT_FAILURE;

    traverse_directory(expanded_dir, word);
    free(expanded_dir);
    return EXIT_SUCCESS;
}
