#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h> 
#include <linux/limits.h>

int _dir_rm(char* path) {
  DIR *dir = opendir(path);
  if (!dir) return 1;
  struct dirent* cur_file;

  int result_global = 0;

  while ((cur_file = readdir(dir)) != NULL) {  
        if (strcmp(cur_file->d_name, ".") == 0 || strcmp(cur_file->d_name, "..") == 0) continue; 
        char new_path[PATH_MAX];
        strcpy(new_path, path);
        strcat(new_path, "/");
        strcat(new_path, cur_file->d_name);  
        if (cur_file->d_type == DT_DIR) {
          if (_dir_rm(new_path) != 0) {
            result_global = 1;
          } 
          continue;
        }
        if (cur_file->d_type == DT_REG || cur_file->d_type == DT_LNK) {
          if (unlink(new_path) != 0) {
            result_global = 1;
          } 
          continue;
        } 
        result_global = 1;
    } 
  closedir(dir);
  if (result_global != 1) {
    result_global = rmdir(path) == 0 ? 0 : 1;
  }
  return result_global;
}

int _rm_dir(char* path, int r_mode) {
    struct stat st;
    if (lstat(path, &st) != 0) {
        return 1; 
    } 
    if (S_ISLNK(st.st_mode)) { 
        return unlink(path) == 0 ? 0 : 1;
    }
    else if (S_ISDIR(st.st_mode)) {
        if (r_mode == 0) {
            return 1;  
        } else { 
            return _dir_rm(path);
        }
    }
    else { 
        return unlink(path) == 0 ? 0 : 1;
    }
}

int _rm(int argc, char** argv) {

    int opt;    
    int r_flag = 0;
    while((opt = getopt(argc, argv, "r")) != -1) {
        switch (opt) {
            case 'r': 
                r_flag = 1;
                break; 
        }
    }     
    for (int i = optind; i < argc; ++i) {   
        if (_rm_dir(argv[i], r_flag) != 0) {
          return 1;
        }
    }  
    return 0;
}


int main(int argc, char* argv[]) {
    return _rm(argc, argv);
}

