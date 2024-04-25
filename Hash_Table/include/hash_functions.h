#ifndef HASH_FUNCTIONS_H
    #define HASH_FUNCTIONS_H

    #include "hash_table.h"

    size_t hash_table_function_const(Elem_t object);
    size_t hash_table_function_fl(Elem_t object);
    size_t hash_table_function_sum(Elem_t object);
    size_t hash_table_function_len(Elem_t object);

#endif // HASH_FUNCITONS_H