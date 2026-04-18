#include "utf8_file.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>     
#include <stdlib.h>   
#include <errno.h> 


int count_(uint8_t byte) {
    int result = 0;
    uint8_t lst = (1 << 7); 
    while (byte & lst) {
        ++result;
        lst /= 2;
    }  
    return result;
}

uint32_t read_utf(utf8_file_t* f) {
    uint8_t byte; 

    ssize_t bytes_read = read(f->fd, &byte, 1);
    if (bytes_read <= 0) {
        if (bytes_read == 0) {
            errno = 0;  
        }
        return (uint32_t)-1;  
    }

    int count_bytes = count_(byte);

    if (count_bytes == 0) {
        return byte;
    }
 
    if (count_bytes == 1 || count_bytes > 6) {
        errno = EILSEQ;
        return (uint32_t)-1;
    }
    
    uint32_t result = byte & (0x3F >> (count_bytes - 1));

    for (int i = 0; i < count_bytes - 1; ++i) {
        bytes_read = read(f->fd, &byte, 1); 
        if (bytes_read <= 0) {
            errno = EILSEQ;  
            return (uint32_t)-1;
        } 
        if ((byte & 0xC0) != 0x80) {
            errno = EILSEQ;
            return (uint32_t)-1;
        } 
        result = (result << 6) | (byte & 0x3F);  
    }
    return result; 

}

int utf8_read(utf8_file_t* f, uint32_t* res, size_t count) {
    if (f == NULL || res == NULL) {
        errno = EINVAL;
        return -1;
    } 
    int i = 0;
    for (i = 0; i < count; ++i) {
        uint32_t tmp = read_utf(f); 
        if (tmp == (uint32_t)-1) {
            if (errno != 0) {
                return -1;  
            } 
            if (i == 0) {
                return 0;
            }
            break;
        }
        res[i] = tmp;
    } 
    return i;
}


int utf8_helper(uint32_t t, uint8_t* buf) { 
    if (t >= (1 << 31)) { 
        errno = EILSEQ;  
        return -1;
    }
    if (t < (1 << 7)) {
        buf[0] = t;
        return 1;
    }
    uint8_t count = 1;
    if (t < (1 << 11)) {
        count = 2;
    } else if (t < (1 << 16)) {
        count = 3;
    } else if (t < (1 << 21)) {
        count = 4;
    } else if (t < (1 << 26)) {
        count = 5;
    } else if (t < (1 << 31)) {
        count = 6;
    }

    uint32_t cur_pos = 0;
    for (int i = count - 1; i > 0; --i) {
        buf[i] = (1ULL << 7);
        for (int j = 0; j < 6; ++j) {
            buf[i] += ((t & (1ULL << cur_pos)) >> (cur_pos - j));
            ++cur_pos;
        }
    }

    uint8_t byte_codes[5] = {
        0b11000000, 
        0b11100000, 
        0b11110000, 
        0b11111000,
        0b11111100
    };

    buf[0] = byte_codes[count - 2];
    uint8_t new_pref = 0;

    while ((1ULL << cur_pos) <= t) {
        if ((1ULL << cur_pos) & t) {
            buf[0] += (1ULL << new_pref);
        }
        ++new_pref;
        ++cur_pos;
    }
    return count;
}


int utf8_write(utf8_file_t* f, const uint32_t* str, size_t count) { 
    int i = 0;
    for (; i < count; ++i) {
        uint32_t t = str[i];
        uint8_t buf[6];  
        int cnt = utf8_helper(t, buf);
        if (cnt == -1) {
            if (i == 0) return -1;
            return i;
        }
        ssize_t byte_done = write(f->fd, buf, cnt);
        if (byte_done != cnt) {
            if (i == 0) return -1;
            return i;
        }
    }
    return i;
}

utf8_file_t* utf8_fromfd(int fd) {
    utf8_file_t* file = (utf8_file_t*)malloc(sizeof(utf8_file_t));
    file->fd = fd; 
    return file; 
}

