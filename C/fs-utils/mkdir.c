#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>


int _touch_dir(char* path, int cur_dir, int p_mode, mode_t mode) {

    struct stat st;
    int is_exist = 0; 

    char* touch_path = (char*)calloc(cur_dir + 1, sizeof(char));
    for (int i = 0; i < cur_dir; ++i) {
        touch_path[i] = path[i];
    }  

    if (stat(touch_path, &st) != -1) {
        if (S_ISDIR(st.st_mode)) {
            is_exist = 1; 
        }
    } 
    if (is_exist == 1) {
        free(touch_path);
        return 0;
    }

    int is_last = (cur_dir == (int)strlen(path) - 1 ? 1 : 0); 

    if ((p_mode == 0) && (is_exist == 0) && (is_last == 0)) { 
        free(touch_path);
        return 1;
    }

    mode_t actual_mode = mode;
    if (!is_last) { 
        actual_mode = 0755;
    }

    int tmp = mkdir(touch_path, actual_mode);
    free(touch_path);
    return tmp;
}

int _mkdir(int argc, char** argv) {
    int opt;  
    struct option long_options[] = {
        {"mode", required_argument, NULL, 'm'}, 
        {0, 0, 0, 0}
    }; 
    
    int p_mode = 0;
    char* m_mode = "0755"; 
    
    while((opt = getopt_long(argc, argv, "m:p", long_options, NULL)) != -1) {
        switch (opt) {
            case 'p':
                p_mode = 1; 
                break;
            case 'm':
                m_mode = optarg; 
                break; 
            default:
                return -1;
        }
    }  
    
    int mode = strtol(m_mode, NULL, 8);
    //printf("%d\n", mode);
    int return_code = 0; 
    int return_global = 0;
    for (int i = optind; i < argc; ++i) {
        char *path = argv[i];
        char *path_copy = (char*)calloc(strlen(path) + 2, sizeof(char));
        strcpy(path_copy, argv[i]);
        path_copy[strlen(path)] = '/';
        path_copy[strlen(path) + 1] = '\0'; 
        int _path_size = strlen(path_copy); 
        int cur_dir = 1; 
        return_code = 0; 
        for (int j = 1; j < _path_size; ++j) { 
            if (path_copy[j] == '/') { 
                return_code = _touch_dir(path_copy, cur_dir, p_mode, mode);
            } 
            if (return_code == 1) {
                return_global |= return_code;
                break;
            }
            ++cur_dir; 
        }
        free(path_copy);
    } 
    
    return return_global;
}


int main(int argc, char* argv[]) {
    return _mkdir(argc, argv);
}
