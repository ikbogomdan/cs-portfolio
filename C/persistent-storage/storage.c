#include "storage.h"
#include <sys/stat.h>  
#include <sys/types.h>  
#include <stdio.h>     
#include <errno.h>     
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
int directory_exists(const char* path) {
    struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

void touch_file(const char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        fd = open(filename, O_CREAT, 0755);
        if (fd != -1) {
            close(fd);
        }
    } else {
        close(fd);
    }
}

uint64_t get_version(const char* path) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        return 0;
    }
    int file_size = lseek(fd, 0, SEEK_END);
    if (file_size == 0) {
        close(fd);
        return 0;
    }

    lseek(fd, 0, SEEK_SET); 
    while (1) { 
        uint64_t version;
        uint64_t str_length; 

        read(fd, &version, sizeof(uint64_t)) != sizeof(uint64_t);
        read(fd, &str_length, sizeof(uint64_t)) != sizeof(uint64_t);

        int current_pos = lseek(fd, 0, SEEK_CUR); 

        if (current_pos + (off_t)str_length == file_size) {
            close(fd);
            return version;
        } 

        lseek(fd, str_length, SEEK_CUR);
    }  
    close(fd);
    return 0;
}

uint64_t write_(const char* path, const char* value) {
    uint64_t version = get_version(path) + 1;
    int fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0755);   
    if (fd == -1) { 
        return 0;
    }  
    write(fd, &version, sizeof(uint64_t));
    uint64_t str_length = strlen(value);
    write(fd, &str_length, sizeof(uint64_t));
    write(fd, value, str_length);
    close(fd);  
    return version;
}
char* partition(const char* path, const char* add_path) { 
    char* another = (char*)calloc((strlen(path) % 2 == 0 ? strlen(path) / 2 + 1 : strlen(path) / 2) + 10
    + strlen(add_path), sizeof(char)); 
    strcpy(another, add_path);
    int cur_pos = strlen(add_path);
    another[cur_pos++] = '/';  
    for (int i = 0; i < strlen(path) + 1; ++i) {
        if (i % 2 == 0 && i != 0) {
            int check = directory_exists(another); 
            if (check == 0) { 
                mkdir(another, 0777);
            } 
            another[cur_pos++] = '/';
        }
        if (i < strlen(path)) {
            another[cur_pos++] = path[i]; 
        }
    }
    {
        int check = directory_exists(another);
        if (another[cur_pos - 1] == '/') { 
            another[cur_pos++] = '@';
        } 
        another[cur_pos] = '\0';
    }  
    return another;
}

uint64_t get_by_version(const char* path, uint64_t key, char* return_string) { 
    int fd = open(path, O_RDONLY);
    if (fd == -1) { 
        return 0;
    } 
    while (1) { 
        uint64_t version; 
        read(fd, &version, sizeof(uint64_t));
        uint64_t str_length;
        read(fd, &str_length, sizeof(uint64_t)); 
        if (version == key) {  
            char* str_tmp = (char*)calloc(str_length + 1, sizeof(char));
            read(fd, str_tmp, str_length); 
            strcpy(return_string, str_tmp);
            free(str_tmp);
            break;
        }
        lseek(fd, str_length, SEEK_CUR); 
    } 
    close(fd);
    return 0;
}

uint64_t get_last_version(const char* path, char* return_string) { 
    int fd = open(path, O_RDONLY);
    if (fd == -1) { 
        return 0;
    } 
    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);  
    uint64_t version; 
    while (1) {  
        read(fd, &version, sizeof(uint64_t));
        uint64_t str_length;
        read(fd, &str_length, sizeof(uint64_t)); 
        off_t current_pos = lseek(fd, 0, SEEK_CUR);
        if (current_pos + str_length == file_size) {  
            char* str_tmp = (char*)calloc(str_length + 1, sizeof(char));
            read(fd, str_tmp, str_length); 
            strcpy(return_string, str_tmp);
            free(str_tmp);
            break;
        } else { 
            lseek(fd, str_length, SEEK_CUR); 
        }
    } 
    close(fd);
    return version;
}

void storage_init(storage_t* storage, const char* root_path) { 
    storage->root_path = (char*)calloc(strlen(root_path) + 1, sizeof(char));
    strcpy(storage->root_path, root_path); 
}

void storage_destroy(storage_t* storage) {
    free(storage->root_path);
}  

version_t storage_set(storage_t* storage, storage_key_t key, storage_value_t value) {
    int add_ = 0;
    char* part_key = partition(key, storage->root_path);
    touch_file(part_key);
    version_t tmp = write_(part_key, value);
    free(part_key);
    return tmp;
}

version_t storage_get(storage_t* storage, storage_key_t key, returned_value_t returned_value) {
    char* part_key = partition(key, storage->root_path);
    version_t tmp = get_last_version(part_key, returned_value);
    free(part_key);
    return tmp;
}

version_t storage_get_by_version(storage_t* storage, storage_key_t key, version_t version, returned_value_t returned_value) {
    char* part_key = partition(key, storage->root_path);
    get_by_version(part_key, version, returned_value) + 1;
    free(part_key);
    return version;
}  
