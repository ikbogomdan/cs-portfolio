#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pcre.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

const size_t kBufferSize = 1ULL << 14;

void search(pcre* pattern, char* path) {
  struct stat path_stat;

  if (lstat(path, &path_stat) == -1) {
    return;
  }

  if (S_ISDIR(path_stat.st_mode)) {
    DIR* directory = opendir(path);

    if (directory == NULL) {
      return;
    }

    struct dirent* current_file = readdir(directory);

    while (current_file != NULL) {
      char* new_path =
          calloc(strlen(path) + strlen(current_file->d_name) + 2, 1);

      strcat(new_path, path);
      strcat(new_path, "/");
      strcat(new_path, current_file->d_name);

      if (strcmp(current_file->d_name, ".") != 0 &&
          strcmp(current_file->d_name, "..") != 0) {
        search(pattern, new_path);
      }

      free(new_path);
      current_file = readdir(directory);
    }

    if (closedir(directory) == -1) {
      return;
    }
  } else {
    int file_descriptor = open(path, O_RDONLY);

    struct stat file_stat;

    if (fstat(file_descriptor, &file_stat) == -1) {
      return;
    }

    char* file_mapping = mmap(NULL, file_stat.st_size, PROT_READ, MAP_PRIVATE,
                              file_descriptor, 0);

    if (file_mapping == MAP_FAILED) {
      close(file_descriptor);
      return;
    }

    unsigned int line_no = 1;

    size_t line_start = 0;
    for (size_t index = 0; index < file_stat.st_size; ++index) {
      if (file_mapping[index] == '\n') {
        char* partial_copy =
            strndup(file_mapping + line_start, index - line_start);

        int options[30];
        memset(options, 0, 30 * sizeof(int));

        int search_result = pcre_exec(pattern, NULL, partial_copy,
                                      index - line_start, 0, 0, options, 30);

        if (search_result > 0) {
          printf("%s:%d: %s\n", path, line_no, partial_copy);
        }

        free(partial_copy);

        ++line_no;
        line_start = index + 1;
      }
    }

    char* partial_copy =
        strndup(file_mapping + line_start, file_stat.st_size - line_start);

    int options[30];
    memset(options, 0, 30 * sizeof(int));

    int search_result =
        pcre_exec(pattern, NULL, partial_copy, file_stat.st_size - line_start,
                  0, 0, options, 30);

    if (search_result > 0) {
      printf("%s:%d: %s\n", path, line_no, file_mapping + line_start);
    }

    free(partial_copy);

    munmap(file_mapping, file_stat.st_size);
  }
}

int main(int argc, char* argv[]) {
  const char* regex = argv[1];
  char* dir = argv[2];

  const char* error;
  int erroffset;
  pcre* pattern = pcre_compile(regex, PCRE_UTF8, &error, &erroffset, NULL);

  if (pattern == NULL) {
    fprintf(stderr, "error: %s\n", error);

    return -1;
  }

  search(pattern, dir);

  pcre_free(pattern);

  return 0;
}
