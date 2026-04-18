#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int tailf_(const char* path) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        return -1;
    } 
    char* buf = (char*)calloc(300, sizeof(char));
    ssize_t byte_read; 
    while ((byte_read = read(fd, buf, sizeof(buf))) > 0) {
        write(STDOUT_FILENO, buf, byte_read); 
    } 
    while (1) {
        byte_read = read(fd, buf, sizeof(buf)); 
        if (byte_read > 0) { 
            write(STDOUT_FILENO, buf, byte_read);
        } else if (byte_read == 0) { 
            usleep(300);
        } else { 
            break;
        }
    } 
    close(fd);
    return 0;
}
int main(int argc, char* argv[]) {
    if (argc != 2) {
        return -1;
    } 
    return tailf_(argv[1]); 
}
