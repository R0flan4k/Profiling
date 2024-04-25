#ifndef HASH_TABLE_H
    #define HASH_TABLE_H

    #include <stddef.h>

    #include "list.h"

    typedef int HashTError_t;
    typedef size_t (*HashFunction_t)(Elem_t);

    struct HashTable {
        LinkedList * hash_sets;
        HashFunction_t hash_function;
        size_t size;
        float load_factor;
        float dispersion;
    };

    enum HashTableErrorsMasks {
        HASH_TABLE_ERROR_CANT_CREATE                      = 1 << 0,
        HASH_TABLE_ERROR_CANT_FILL                        = 1 << 1,
        HASH_TABLE_ERROR_CANT_CLEAR                       = 1 << 2,
        HASH_TABLE_ERROR_CANT_OPEN_LOAD_FACTOR_FILE       = 1 << 3,
        HASH_TABLE_ERROR_EMPTY_HASH_FUNCTION              = 1 << 4,
        HASH_TABLE_ERROR_CANT_PUSH_OBJECT                 = 1 << 5,
        HASH_TABLE_ERROR_VECTOR_OVERFLOW                  = 1 << 6,
        HASH_TABLE_ERROR_CALCULATING_EMPTY_HT_LOAD_FACTOR = 1 << 7,
    };

    HashTError_t hash_table_ctor(HashTable * hash_table, const size_t start_size, HashFunction_t hash_function);
    HashTError_t hash_table_recreate(HashTable * hash_table, HashFunction_t hash_function);
    HashTError_t hash_table_fill(HashTable * hash_table, Elem_t objects, size_t objects_num);
    HashTError_t hash_table_dtor(HashTable * hash_table);
    HashTError_t hash_table_load_hist(const HashTable * hash_table);
    HashTError_t get_word_vectors(const char * buffer, __m256i word_vectors[], const size_t words_num);


#endif // HASH_TABLE_H