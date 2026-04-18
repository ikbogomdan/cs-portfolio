#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

bool is_same_file(const char* lhs_path, const char* rhs_path) {
    struct stat stat1, stat2; 
    if (stat(lhs_path, &stat1) || stat(rhs_path, &stat2)) return false;  
    return (stat1.st_dev == stat2.st_dev && stat1.st_ino == stat2.st_ino);
}

int main(int argc, const char* argv[]) { 
    if (argc != 3) return -1;
    if (is_same_file(argv[1], argv[2])) printf("yes\n");
    else printf("no\n");
    return 0;
}
