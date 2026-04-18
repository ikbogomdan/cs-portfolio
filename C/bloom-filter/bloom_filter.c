#include "bloom_filter.h" 
#include <stdlib.h>

const uint64_t cPrime = 1099511628211ULL; 

uint64_t calc_hash(const char* str, uint64_t modulus, uint64_t seed) {
    uint64_t hash = 435345544457;  
    for (int i = 0; i < seed; ++i) {
        hash = hash * hash % modulus;
    } 
    uint64_t prime = 1;
    for (const char* p = str; *p != '\0'; ++p) {
        prime = (cPrime * prime) % modulus; 
        hash = (hash + prime * (uint64_t)(*p)) % modulus;
    } 
    return hash % modulus;
}

void bloom_init(struct BloomFilter* bloom_filter, uint64_t set_size, 
                hash_fn_t hash_fn, uint64_t hash_fn_count) {
    bloom_filter->set_size = set_size;
    uint64_t set_sz = (set_size + 63) / 64;
    bloom_filter->set = (uint64_t*)calloc(set_sz, sizeof(uint64_t));
    bloom_filter->hash_fn = hash_fn;
    bloom_filter->hash_fn_count = hash_fn_count; 
}

void bloom_destroy(struct BloomFilter* bloom_filter) {
    if (bloom_filter != NULL) {
        free(bloom_filter->set);
    }
    bloom_filter->set = NULL;
}

void bloom_insert(struct BloomFilter* bloom_filter, Key key) {
    for (uint64_t i = 0; i < bloom_filter->hash_fn_count; ++i) {
        uint64_t hash = bloom_filter->hash_fn(key, bloom_filter->set_size, i);
        uint64_t bit_pos = hash % bloom_filter->set_size;
        uint64_t arr_pos = bit_pos / 64;
        uint64_t bit_mask = 1ULL << (bit_pos % 64);
        if (bloom_filter->set != NULL) {
            bloom_filter->set[arr_pos] |= bit_mask;
        }
    }
}

bool bloom_check(struct BloomFilter* bloom_filter, Key key) {
    for (uint64_t i = 0; i < bloom_filter->hash_fn_count; ++i) {
        uint64_t hash = bloom_filter->hash_fn(key, bloom_filter->set_size, i);
        uint64_t bit_pos = hash % bloom_filter->set_size;
        uint64_t arr_pos = bit_pos / 64;
        uint64_t bit_mask = 1ULL << (bit_pos % 64); 
        if (bloom_filter->set == NULL ||  !(bloom_filter->set[arr_pos] & bit_mask)) {
            return false;
        }
    }
    return true;
}
