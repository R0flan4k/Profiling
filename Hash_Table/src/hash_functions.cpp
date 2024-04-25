#include <string.h>

#include "hash_functions.h"


size_t hash_table_function_fl(Elem_t object)
{
    return (size_t) *((char *) object);
}


size_t hash_table_function_len(Elem_t object)
{
    return strlen((char *) object);
}


size_t hash_table_function_sum(Elem_t object)
{
    size_t sum = 0;

    char * word_ptr = (char *) object;
    while (*word_ptr != '\0')
    {
        sum += *word_ptr;
        word_ptr++;
    }

    return sum;
}


size_t hash_table_function_const(Elem_t)
{
    return 0;
}