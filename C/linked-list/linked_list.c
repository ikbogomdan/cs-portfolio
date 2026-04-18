#include "linked_list.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

struct ListNode* read_linked_list(const char *filepath) { 
    int fd = open(filepath, O_RDONLY);  
    if (fd == -1) { 
        return NULL;
    }
    struct ListNode* ret = (struct ListNode*)malloc(sizeof(struct ListNode)); 
    int dif;
    int art;
    char cd[2];  
    if (read(fd, &dif, 4) != 4 || read(fd, &cd, 2) != 2 || read(fd, &art, 4) != 4) {
        free(ret);
        close(fd);
        return NULL;
    } 
    struct ListNode* last; 
    ret->country_code[0] = cd[0];
    ret->country_code[1] = cd[1];
    ret->part_number = art;
    lseek(fd, dif, SEEK_SET);
    last = ret; 
    while (1) {
        if (read(fd, &dif, 4) != 4 || read(fd, &cd, 2) != 2 || read(fd, &art, 4) != 4) {
            break;
        } 
        struct ListNode* another_last = (struct ListNode*)malloc(sizeof(struct ListNode)); 
        another_last->country_code[0] = cd[0];
        another_last->country_code[1] = cd[1];
        another_last->part_number = art; 
        last->next = another_last;  
        last = another_last;
        if (dif == -1) {
            break;
        }
        lseek(fd, dif, SEEK_SET);
    }
    last->next = NULL;
    close(fd);
    return ret;
}
