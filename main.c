#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>

void process_file(const char *file_path, const char *word) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        fprintf(stderr, "Error opening file %s: %s\n", file_path, strerror(errno));
        return;
    }

    char line[1024];
    int line_number = 0;
    while (fgets(line, sizeof(line), file)) {
        line_number++;
        if (strstr(line, word) != NULL) {
            printf("%s:%d:%s", file_path, line_number, line);
        }
    }

    fclose(file);
}

void traverse_directory(const char *dir_path, const char *word) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        fprintf(stderr, "Error opening directory %s: %s\n", dir_path, strerror(errno));
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat statbuf;
        if (lstat(full_path, &statbuf) == -1) {
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
