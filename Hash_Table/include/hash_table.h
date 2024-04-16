#ifndef HASH_TABLE_H
    #define HASH_TABLE_H

    #include "list.h"

    typedef int HashTError_t;

    struct HashTable {
        LinkedList * hash_sets;
        size_t (*hash_function)(Elem_t object);
        size_t size;
    };

    enum HashTableErrorsMasks {
        HASH_TABLE_ERROR_CANT_CREATE                = 1 << 0,
        HASH_TABLE_ERROR_CANT_FILL                  = 1 << 1,
        HASH_TABLE_ERROR_CANT_CLEAR                 = 1 << 2,
        HASH_TABLE_ERROR_CANT_OPEN_LOAD_FACTOR_FILE = 1 << 3,
        HASH_TABLE_ERROR_EMPTY_HASH_FUNCTION        = 1 << 4,
        HASH_TABLE_ERROR_CANT_PUSH_OBJECT           = 1 << 5,
    };

    HashTError_t hash_table_ctor(HashTable * hash_table, const size_t start_size);
    HashTError_t hash_table_set_function(HashTable * hash_table, size_t (*hash_function)(Elem_t object));
    HashTError_t hash_table_clear(HashTable * hash_table);
    HashTError_t hash_table_fill(HashTable * hash_table, Elem_t * objects, size_t objects_num);
    HashTError_t hash_table_dtor(HashTable * hash_table);
    HashTError_t hash_table_load_hist(HashTable * hash_table);

    size_t hash_table_function_const(Elem_t object);
    size_t hash_table_function_fl(Elem_t object);
    size_t hash_table_function_sum(Elem_t object);
    size_t hash_table_function_len(Elem_t object);

    const size_t HASH_TABLE_SIZE = 100;

#endif // HASH_TABLE_H