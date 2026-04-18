#include "lca.h"

#include <stdio.h>
#include <string.h>

enum { FILENAME_SIZE = 512, MAX_PROC_LINE = 2048 };

pid_t parent(pid_t id) {
  char filename[FILENAME_SIZE];
  sprintf(filename, "/proc/%d/status", id);

  FILE* process_info_file = fopen(filename, "r");

  pid_t result = 0;

  char row[MAX_PROC_LINE];
  while (fgets(row, MAX_PROC_LINE, process_info_file) != NULL) {
    if (strncmp(row, "PPid", 4) == 0) {
      sscanf(row, "PPid:    %d", &result);
    }
  }

  fclose(process_info_file);

  return result;
}

pid_t find_lca(pid_t x, pid_t y) {
  pid_t x_ancestors[MAX_TREE_DEPTH];
  pid_t y_ancestors[MAX_TREE_DEPTH];

  pid_t* x_pointer = x_ancestors;

  for (; x != 0; ++x_pointer) {
    *x_pointer = x;
    x = parent(x);
  }

  pid_t* y_pointer = y_ancestors;

  for (; y != 0; ++y_pointer) {
    *y_pointer = y;
    y = parent(y);
  }

  --x_pointer;
  --y_pointer;

  for (; x_pointer >= x_ancestors && y_pointer >= y_ancestors &&
         *x_pointer == *y_pointer;
       --x_pointer, --y_pointer) {
  }

  ++x_pointer;

  return *x_pointer;
}
